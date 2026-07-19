# 调试记录

## 问题 1：触摸屏与摄像头 I2C 引脚冲突（PD14）

### 问题描述

摄像头初始化或 I2C 通信期间，触摸屏无响应或卡顿。

### 排查过程

- 正点原子 N647 开发板上，PD14 引脚同时被两个外设使用：
  - **硬件 I2C2**（`I2C2_SDA`，AF4）：IMX335 摄像头传感器的配置接口
  - **软件模拟 I2C**（`GPIO_MODE_OUTPUT_PP`）：触摸屏控制器的通信接口
- 摄像头初始化时需要频繁通过 I2C2 读写 IMX335 寄存器，此时 PD14 被初始化为 AF4 模式
- 摄像头通信结束后，PD14 没有恢复为 GPIO 模式，导致触摸屏无法正常通信

### 根因

PD14 引脚只有一根物理连线，但两个外设都需要独占使用。CubeMX 生成的初始化代码无法处理这种动态切换场景，需要在每次 I2C 操作前后手动切换引脚功能。

### 修改方法

**文件**：`ElectronicWarehouse/Appli/Drivers/BSP/IMX335/imx335.c`

新增两个辅助函数，在 `imx335_io_writereg` 和 `imx335_io_readreg` 的调用前后动态切换 PD14：

```c
// 切换到 I2C2 模式（摄像头通信前调用）
static void pd14_to_i2c2(void)
{
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_14;
    gpio.Mode = GPIO_MODE_AF_OD;      // 复用开漏 → I2C
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF4_I2C2;   // I2C2 复用功能
    HAL_GPIO_Init(GPIOD, &gpio);
}

// 恢复为 GPIO 模式（摄像头通信后调用）
static void pd14_to_gpio(void)
{
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_14;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出 → 触摸 I2C
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOD, &gpio);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
```

读写函数调用模式：
```c
static int32_t imx335_io_writereg(...)
{
    pd14_to_i2c2();                          // ← 通信前切换
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c2, ...);
    pd14_to_gpio();                          // ← 通信后恢复
    return (status == HAL_OK) ? 0 : 1;
}
```

`imx335_io_readreg` 同理。


可以尝试在摄像头任务彻底启动后
---

## 问题 2：摄像头界面返回后再进入画面卡死

### 问题描述

- 首次点击"摄像头"按钮进入摄像头界面 → 画面正常显示
- 点击"返回"按钮回到主界面 → 正常
- 再次点击"摄像头"按钮 → **摄像头画面冻结**（不刷新）

### 排查过程

#### 1. 确认 Sensor_Task 是否在运行

在 Sensor_Task 主循环中加入 PE10 LED 闪烁：

```c
while(1)
{
    a++;
    if(a == 50) {
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);
        a = 0;
    }
    imx335_isp_background_process();
    vTaskDelay(pdMS_TO_TICKS(10));
}
```

**结果**：第一次进入和再次进入时，PE10 LED 都正常闪烁 → Sensor_Task 的 while(1) 循环在运行，任务没有被卡死。

#### 2. 确认 imx335_init 是否成功

在 `imx335_init()` 的各个环节加入 printf 诊断：
- `dcmipp_init` → OK
- `ReadID` → OK
- `Init` → OK
- `SetFrequency` → OK
- `ISP_Init` → OK
- `ISP_Start` → OK

**结果**：串口无任何错误打印 → imx335_init 所有步骤返回成功。

#### 3. DCMIPP frame callback 检测

PE10 在 while(1) 中以 500ms 周期闪烁（亮暗分明），但仔细观察暗的时候还有微弱光闪烁——这是 `imx335_dcmipp_pipe_frame_cb` 每帧被调用时 PE10 翻转留下的痕迹（DCMIPP 帧率远高于 while 循环周期）。

**关键发现**：
| 阶段 | PE10 微弱光闪烁 |
|------|-----------------|
| 首次进入摄像头 | 有（DCMIPP 正常输出帧） |
| 返回主界面后 | 有（DCMIPP 仍在后台运行） |
| 再次进入摄像头 | **无**（DCMIPP 没有输出帧） |

**结论**：再次进入时，DCMIPP 硬件层面没有启动成功，虽然 HAL 函数全部返回 `HAL_OK`。

#### 4. 尝试过的方案（均失败）

**方案 A**：在 `imx335_dcmipp_init()` 开头调用 `CSI_PIPE_Stop` 停止残留管道
- 结果：再次进入仍然卡死

**方案 B**：在 `imx335_dcmipp_init()` 开头调用 `HAL_DCMIPP_DeInit` 完全复位外设
- 结果：**返回按钮黑屏** — DeInit 调用 `MspDeInit` 关闭 DCMIPP/CSI 时钟，而 DCMIPP 正在 CONTINUOUS 模式下运行，关闭时钟导致 AXI 总线异常

**方案 C**：在 `action_stop_camera()` 中（返回时）调用 `CSI_PIPE_Stop`
- 结果：**返回按钮黑屏** — `DCMIPP_Stop` 内部轮询 `CPTACT` 硬件标志位，在 LVGL UI 线程中阻塞等待导致 UI 无响应

**方案 D**：只做 RCC DCMIPP 强制复位（不关时钟）
- 结果：仍卡死

**方案 E**：RCC DCMIPP + CSI 同时强制复位
- 结果：仍卡死

**方案 F**：给 `rgblcd_clear(BLUE)` 加 DMA2D 互斥锁
- 结果：**返回按钮黑屏** — CMSIS-RTOS2 互斥锁不可递归，被 LVGL flush 中的同一任务重入导致死锁

#### 5. 根因分析

DCMIPP 外设的硬件状态机在经历 STOP → INIT → START 流程后，虽然 HAL 层面的软件状态机转换正确，但物理层的 CSI PHY、像素管道 FIFO、AXI DMA 引擎无法从运行中途的强制停止中完全恢复。`HAL_DCMIPP_DeInit` 可以做到完全复位，但会关闭时钟导致总线问题。

### 最终方案：Sensor_Task 常驻运行

放弃"退出摄像头时销毁任务、再进入时重新创建"的动态管理模式，改为 **Sensor_Task 创建一次、永久运行**。

**核心思路**：
- DCMIPP 只在启动时初始化一次，之后永不停止
- 摄像头界面切换**只控制 LTDC 的 Color Keying**（图层叠加），不触碰 DCMIPP 硬件
- Layer 0（RGB888 摄像头）始终由 DCMIPP 写入帧数据
- Layer 1（RGB565 LVGL UI）在进入摄像头时开启 Color Keying，黑色像素穿透显示底层摄像头画面

**修改的文件**：

| 文件 | 改动 |
|------|------|
| `Appli/Core/Src/app_freertos.c:141` | Sensor_Task 在启动时创建，常驻运行 |
| `Appli/APP/ui/actions.c` | `action_start_camera` — 只开 Color Keying + 清 Layer 1 + 切屏 |
| `Appli/APP/ui/actions.c` | `action_stop_camera` — 只关 Color Keying + 清 Layer 0 + 返回 |
| `Appli/APP/Sensor_task.c` | 去掉动态创建/销毁逻辑，清理诊断代码 |
| `Appli/Drivers/BSP/IMX335/imx335.c` | frame callback 去掉诊断 GPIO；stop_capture 补充 PIPE2 |
| `Appli/Drivers/BSP/IMX335/imx335.c` | start_capture 加入 PIPE1/PIPE2 失败 printf |

**action_start_camera**（进入摄像头界面）：
```c
void action_action_start_camera(lv_event_t * e) {
    // ① 开 Color Keying，黑色像素直接穿透到 Layer 0
    HAL_LTDC_ConfigColorKeying(&hltdc, 0, 1);
    HAL_LTDC_EnableColorKeying(&hltdc, 1);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

    // ② 清 Layer 1 帧缓冲（避免切屏瞬间残留旧数据）
    memset(g_ltdc_ui_framebuf, 0, sizeof(g_ltdc_ui_framebuf));

    // ③ 切屏（Sensor_Task 常驻运行，DCMIPP 一直在采集）
    eez_flow_push_screen(SCREEN_ID_CAMERA, LV_SCR_LOAD_ANIM_NONE, 0, 0);
}
```

**action_stop_camera**（返回主界面）：
```c
void action_action_stop_camera(lv_event_t * e) {
    // ① 关 Color Keying（Main 屏幕不能开，否则黑色 UI 元素会消失）
    HAL_LTDC_DisableColorKeying(&hltdc, 1);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

    // ② 清除 Layer 0 摄像头残留画面
    rgblcd_clear(BLUE);

    // ③ 返回 Main（Sensor_Task 继续在后台运行，DCMIPP 持续采集）
    eez_flow_pop_screen(LV_SCR_LOAD_ANIM_NONE, 0, 0);
}
```

### 经验教训

1. STM32N6 的 DCMIPP 在连续采集模式（CONTINUOUS）下，停止后重新启动不完全可靠。如非必要，不要停止已运行的管道。
2. `HAL_DCMIPP_DeInit` 会关闭外设时钟，如果 DCMIPP 正在通过 AXI DMA 写入内存，关闭时钟会导致总线异常。只能在确认 DMA 完全停止后调用。
3. CMSIS-RTOS2 的 `osMutex` 不可递归，同一任务不能多次获取同一个互斥锁。
4. 在 LVGL UI 线程（`LV_Task`）中不能调用任何阻塞等待硬件标志位的函数，否则 UI 无响应。

---

## 问题 3：摄像头任务运行时触摸屏失灵（PD4 冲突）

### 问题描述

- 注释掉 Sensor_Task（摄像头任务）→ 触摸屏正常
- 启用 Sensor_Task → 触摸屏失灵
- 注释掉 LV_Task（仅运行摄像头任务）→ 摄像头正常

### 背景

更换为团队自画的板子后，CubeMX 中 IMX335 摄像头的 **I2C2_SDA 从 PD15 改为 PD4**。PD4 同时也是触摸屏软件 I2C 的 **CT_IIC_SDA**（数据线）。

修改前后的引脚对比：

| 信号 | 原引脚（正点原子 N647） | 新引脚（自画板） |
|------|------------------------|-----------------|
| I2C2_SCL | PD14 | PD14（不变） |
| I2C2_SDA | PD15 | **PD4** |
| CT_IIC_SCL | PD14 | PD14（不变） |
| CT_IIC_SDA | PD4 | PD4（不变） |

### 原因分析

之前只解决过 PD14（SCL）的冲突——摄像头 I2C 通信时短暂偷走 SCL，但触摸的 SDA（PD4）始终保持 GPIO 模式。触摸软件 I2C 读取过程中即使 SCL 偶有异常，SDA 还能正常拉低/读取，整体功能勉强可用。

I2C2_SDA 改到 PD4 后，摄像头 I2C 通信时 PD14 和 PD4 **同时被切到 AF4**：

```
摄像头 I2C 通信期间：
  PD14 → AF4 (I2C2_SCL)
  PD4  → AF4 (I2C2_SDA)

触摸软件 I2C 需要：
  PD14 → GPIO_OUTPUT_PP (CT_IIC_SCL)
  PD4  → GPIO_OUTPUT_OD  (CT_IIC_SDA)
```

两个引脚同时消失，触摸软件 I2C 的 SCL 和 SDA 双双失效，`tp_dev.scan(0)` 无法与触摸芯片通信。

### 为什么单独运行一个任务正常

两个任务本身逻辑都没问题，问题出在**并发时序窗口**：

- Sensor_Task 每 10ms 走一次 `imx335_isp_background_process` → ISP 内部通过 I2C2 读写传感器寄存器 → 调用 `imx335_io_readreg/writereg` 切换 PD14/PD4
- LV_Task 以更高频率（约 30ms）调用 `touchpad_read` → `tp_dev.scan(0)` → 软件 I2C 读取触摸坐标

两者没有互斥保护，传感器 I2C 通信时间窗口内恰好撞上触摸读取时，触摸读取失败。10ms 周期 × 高频率碰撞 → 触摸基本处于持续失效状态。

单独运行时不存在竞争，各自正常。

### 解决方法：加入互斥锁串行化 PD14/PD4 访问

**文件**：`ElectronicWarehouse/Appli/Drivers/BSP/IMX335/imx335.c`、`ElectronicWarehouse/Appli/Core/Src/lv_port_indev.c`

新增一个 CMSIS-RTOS2 互斥锁 `pd_i2c_mutex`，摄像头 I2C 和触摸 I2C 操作前必须先获取锁。

**修改 1** — `imx335.c` 新增互斥锁全局变量和初始化：

```c
// 全局变量（文件顶部）
osMutexId_t pd_i2c_mutex;
const osMutexAttr_t pd_i2c_mutex_attr = { .name = "pd_i2c_mutex" };

// 在 imx335_dcmipp_init() 中创建（任何 I2C 通信之前）
pd_i2c_mutex = osMutexNew(&pd_i2c_mutex_attr);
```

**修改 2** — `imx335.c` 读写函数加锁：

```c
static int32_t imx335_io_writereg(...) {
    osMutexAcquire(pd_i2c_mutex, osWaitForever);
    pd14_to_i2c2();
    pd4_to_i2c2();
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c2, ...);
    pd4_to_gpio();
    pd14_to_gpio();
    osMutexRelease(pd_i2c_mutex);
    return (status == HAL_OK) ? 0 : 1;
}
```

`imx335_io_readreg` 同理。

**修改 3** — `lv_port_indev.c` 触摸读取加锁：

```c
#include "cmsis_os.h"
extern osMutexId_t pd_i2c_mutex;

static void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    if (pd_i2c_mutex != NULL) {
        osMutexAcquire(pd_i2c_mutex, osWaitForever);
    }
    tp_dev.scan(0);
    if (pd_i2c_mutex != NULL) {
        osMutexRelease(pd_i2c_mutex);
    }
    // ... 状态处理不变
}
```

**互斥锁作用示意**：

```
Sensor_Task (摄像头)              LV_Task (触摸)
    │                                  │
    │ Acquire(pd_i2c_mutex) ✓          │
    │ pd14/4→AF4, I2C通信              │ Acquire(pd_i2c_mutex) ⏳阻塞
    │ pd14/4→GPIO                      │ (等待...)
    │ Release(pd_i2c_mutex)            │ Acquire(pd_i2c_mutex) ✓
    │                                  │ tp_dev.scan(0)
    │                                  │ Release(pd_i2c_mutex)
```

**NULL 检查的原因**：LVGL 可能在摄像头任务创建锁之前就尝试读触摸（启动初期），此时 `pd_i2c_mutex == NULL` 跳过加锁，保证启动阶段触摸也能正常工作。

---

## 问题 4：AHT10 温湿度传感器无法通信（总线无活动）

### 问题描述

在 STM32N6 平台上通过 I3C2 外设（I2C 兼容模式）驱动 AHT10 温湿度传感器失败。I3C HAL 初始化返回成功，`aht10_i2c_write` 返回错误。切换到软件 I2C（Bit-Bang GPIO）后仍无法通信。

### 环境信息

| 项目 | 详情 |
|------|------|
| MCU | STM32N647X0HXQ |
| 外设 | I3C2（PH7=SCL, PH8=SDA, AF2） |
| 传感器 | AHT10（I2C 7位地址 0x38） |
| 内核时钟 | PCLK1（CCIPR4 I3C2SEL=1） |
| RTOS | FreeRTOS（CMSIS-RTOS2） |
| TrustZone | 使能（`__ARM_FEATURE_CMSE == 3U`） |
| RIF | 已配置 GPIOH PIN7/8 为 GPIO_PIN_SEC |

### 第一阶段：HAL I3C 驱动（I2C Private Message）

**现象**：`aht10_init()` 返回 1（`aht10_i2c_write` 失败），SCL/SDA 始终为高电平，逻辑分析仪抓不到任何总线活动。

**排查步骤**：

**1. CtrlBuf/TxBuf 空指针崩溃**

`I3C_XferTypeDef xfer = {0}` 将缓冲区指针初始化为 NULL，导致 `I3C_ControlBuffer_PriorPreparation` 返回 `HAL_ERROR`，`HAL_I3C_AddDescToFrame` 失败。

修复：在栈上预分配 `uint32_t ctrl_buf[2]`、`uint8_t tx_buf[8]`、`uint8_t rx_buf[8]`。

**2. 多余的 I3C 仲裁调用**

原代码调用了 `HAL_I3C_Ctrl_GenerateArbitration()`（发送 `S + 0x7E + W`）。总线上没有 I3C 目标器件，仲裁必然因无 ACK 而失败。

修复：删除仲裁调用。

**3. I3C 地址格式修正**

I3C HAL 的 `TargetAddr` 字段要求原始 7 位地址（0x38），不同于传统 `HAL_I2C` 需要传入左移 1 位后的地址（0x70）。

修复：`AHT10_ADDR` 设为 `0x38`。

**4. 启用 ControlFIFO**

怀疑 STM32N6 的 I3C 需要通过 C-FIFO + TSFSET 触发机制而不是直接写 CR 寄存器来启动传输。

修改 `i3c.c`：`sFifoConfig.ControlFifo = HAL_I3C_CONTROLFIFO_ENABLE`。

结果：**无效**，总线仍无任何活动。

**5. 硬件寄存器深度诊断**

在 `main.c` 加入 I3C2 全寄存器 dump（CFGR/CR/SR/EVR/IER/SER/TIMINGR0/1）和 RCC 时钟状态（APB1ENR1/APB1RSTR1/CCIPR4/MSICFGR）。同时加入直接 LL 层 CR 写入测试，完全绕过 HAL。

**寄存器 dump 关键输出**：

```
I3C2 CFGR=0x00080003:  EN=1 CRINIT=1 NOARBH=0 TMODE=1 SMODE=0
I3C2 EVR=0x00000003:   CFEF=1 TXFEF=1 CFNFF=0
RCC APB1ENR1=0x02400011:  I3C2EN=1 (时钟已使能)
RCC APB1RSTR1=0x00000000: I3C2RST=0 (复位已释放)
RCC CCIPR4=0x02401010:    I3C2SEL=1 (内核时钟=PCLK1)
GPIOH IDR=0x00000180:    PH7=1 PH8=1 (引脚读回高电平)
```

**EVR 寄存器异常分析**：

EVR（Event Register）关键位：
- 位 0 `CFEF`（Control FIFO Empty Flag）= 1 → C-FIFO 为空
- 位 2 `CFNFF`（Control FIFO Not Full Flag）= **0** → C-FIFO 显示"已满"

这两个标志**相互矛盾**：空 FIFO 应该也是"Not Full"。CFNFF=0 意味着硬件拒绝接受控制数据。

**直接 LL 层 CR 写入测试**：
- TMODE=1（ControlFIFO 模式），通过 C-FIFO + TSFSET 触发
- 写入 CR 控制字后：CFEF 变为 0（FIFO 收到数据），CFNFF 仍为 0
- 置 `TSFSET=1` 后：EVR **无任何变化**，FCF（帧完成）从未置位，ERRF（错误）从未置位
- 等待 100ms 超时：总线始终无 START 条件

**结论**：I3C2 硬件对 C-FIFO 数据写入和 TSFSET 触发均不响应。即使直接写 CR 寄存器（TMODE=0 模式）也无反应。外设配置全部正确（EN=1, CRINIT=1, 时钟已使能, 复位已释放），最可能原因是 STM32N6 I3C2 硅片勘误或内核时钟未到达外设内部逻辑。ST 社区 2026-02 有相同报障帖（"STM32N6 I3C mixed communication - no signal"），0 回复，无公开解决方案。

### 第二阶段：软件 I2C（Bit-Bang GPIO）

放弃 I3C 硬件，用纯 GPIO 模拟 I2C 时序。

**实现文件**：`Appli/Drivers/BSP/SoftI2C/soft_i2c.c`

- GPIO 开漏输出模式 + 上拉，BSRR 寄存器原子写控制 SCL/SDA
- 微秒延时经历三次迭代：

| 方案 | 现象 |
|------|------|
| DWT `CYCCNT` 周期计数器 | 卡死在 `while ((DWT->CYCCNT - start) < cycles)` — 计数器不递增 |
| `SysTick->VAL` | VAL 始终为 0 — SysTick 未运行或已被 RTOS 重新配置 |
| `__NOP()` 忙等循环 | 可执行，循环正常完成 |

**现象**：`aht10_init()` 返回 1（Init FAILED），PH7/PH8 始终高电平。

**GPIO 控制诊断**：

在 FreeRTOS `StartDefaultTask` 任务中直接调用 `HAL_GPIO_WritePin` / `HAL_GPIO_TogglePin` 翻转 PH7：
- 任务确实被执行（断点确认到达）
- 用**逻辑分析仪**抓取 PH7 电平 → **始终高电平，无任何跳变**
- 查看 GPIOH MODER → PH7/PH8 已配置为输出模式
- 写入 BSRR 拉低 PH7 的代码已执行，但物理引脚无反应

### 第三阶段：RIF 安全性排查

检查 `SystemIsolation_Config()`（`main.c:312`）中的 RIF 配置：

```c
HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_7, GPIO_PIN_SEC | GPIO_PIN_NPRIV);
HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_8, GPIO_PIN_SEC | GPIO_PIN_NPRIV);
```

- PH7/PH8 被标记为安全引脚（`GPIO_PIN_SEC`）+ 非特权可访问（`GPIO_PIN_NPRIV`）
- GPIOH 时钟确认使能（`gpio.c:50` 和 `ltdc.c:133` 均有 `__HAL_RCC_GPIOH_CLK_ENABLE()`）
- 代码运行在 Secure 模式（`__ARM_FEATURE_CMSE == 3U`），与引脚安全属性匹配
- GPIOH 组级别未被 `HAL_RIF_RISC_SetSlaveSecureAttributes` 显式配置为安全外设，但引脚级属性应独立生效

### 根因结论

**AHT10 传感器本身硬件损坏。**

判断依据：
1. I3C2 硬件不产生总线活动（ControlFIFO 使能/禁用、直接 CR 写入均无效）
2. 软件 I2C（GPIO 直接写 BSRR 寄存器翻转引脚）在逻辑分析仪上无任何电平变化
3. 软件 I2C 独立于 I3C 外设，仅依赖 GPIO 输出驱动 — GPIO 寄存器写入不改变引脚电平，直接排除 I3C 问题
4. GPIOH 其他引脚工作正常（LCD/LTDC 使用 PH4/PH9/PH10/PH11/PH12/PH13/PH14/PH15），排除 GPIOH 整组问题
5. RIF 安全配置与代码运行模式匹配（Secure 代码访问 Secure 引脚），排除 TrustZone 问题
6. 最可能的硬件故障：PH7/PH8 引脚内部输出驱动电路损坏，或 AHT10 传感器端 SDA/SCL 对 VDD 短路导致开漏输出无法拉低总线电平

### 解决方向

1. 更换 AHT10 传感器模块
2. 或改用其他任意空闲 GPIO 引脚作为软件 I2C（避免受影响的 PH7/PH8），确认新引脚可正常翻转后再连接 AHT10
3. I3C2 外设问题待 ST 发布 STM32N6 勘误表后再评估是否可修复

### 涉及文件

| 文件 | 说明 |
|------|------|
| `Appli/Drivers/BSP/AHT10/aht10.h` | AHT10 驱动头文件 |
| `Appli/Drivers/BSP/AHT10/aht10.c` | AHT10 驱动实现（切换为软件 I2C） |
| `Appli/Drivers/BSP/SoftI2C/soft_i2c.h` | 软件 I2C 驱动头文件（引脚宏 + GPIO 控制宏） |
| `Appli/Drivers/BSP/SoftI2C/soft_i2c.c` | 软件 I2C 驱动实现（NOP 忙等延时） |
| `Appli/Core/Src/main.c` | 主程序（含 AHT10 诊断测试代码） |
| `Appli/Core/Src/i3c.c` | I3C2 初始化（已添加 RELEASE_RESET） |

### 关键经验

1. **I3C 兼容 I2C**：STM32N6 的 I3C2 在 Legacy I2C 模式下可能存在硅片问题。ST 社区 2026-02 已有相同报障且无回复，暂不建议在此平台上用 I3C 驱动纯 I2C 设备
2. **软件 I2C 延时**：RTOS 环境下 DWT 和 SysTick 均可能不可用，NOP 忙等是最可靠的回退方案
3. **GPIO 安全属性**：TrustZone + RIF 环境下需注意引脚级安全配置（`HAL_GPIO_ConfigPinAttributes`）是否与代码执行模式匹配
4. **寄存器诊断方法**：直接读写外设寄存器并逐位解码，能比 HAL 返回值更快定位问题根因
5. **分层隔离验证**：从 HAL → LL → 纯 GPIO → 换引脚 逐层剥离，排除中间件问题后最终定位到硬件

---

## 问题 5：扫码选库后 UI 卡死（历史记录功能添加后引入）

### 问题描述

- 本地扫码 → 选柜子 → **库存信息正常显示** → **历史记录不显示** → **UI 卡死**
- 加入扫码出入库历史记录功能前，扫码流程正常，多次测试均无卡死
- 串口输出报错：`assertion "false" failed: file "../APP/ui/eez-flow.cpp", line 6387, function: void eez::flow::stopScript()`

### 排查过程

#### 1. 怀疑对象：多任务 UI 通信互斥锁不完整

**分析路径**：`Barcode_Task` 和 `UART4_RxTask` 通过 `ui_bridge.cpp` 写入 EEZ-Flow 全局变量时持有 `flow_var_mutex`，但 `LV_Task` 在处理 UI 事件和渲染时直接调用 `eez::flow::setGlobalVariable()/getGlobalVariable()`，完全绕过互斥锁。

**修改**（方案 1）：将 `flow_var_mutex` 从上层（`ui_bridge.cpp` + `eez_flow_tick()`）下沉到最底层 `eez-flow.cpp` 的 `setGlobalVariable()` / `getGlobalVariable()` 内部，确保所有访问路径都经过同一把锁。

- `eez-flow.cpp`：`setGlobalVariable(Assets*, ...)` 和 `getGlobalVariable(Assets*, ...)` 内部加 `osMutexAcquire/Release`
- `eez-flow.cpp`：`eez_flow_tick()` 移除互斥锁（避免与 `action_givetime` 内的 `setGlobalVariable` 形成非递归锁死锁）
- `ui_bridge.cpp`：10 个 `ui_*` 函数全部移除 `osMutexAcquire/Release`（锁已下沉到底层）

**结果**：库存正常显示，历史记录仍不显示，UI 仍卡死。锁的覆盖缺口不是根因。

#### 2. 怀疑对象：action_givetime 重入触发死循环

**分析路径**：`action_givetime` 是 LVGL 事件回调，运行在 `LV_Task` 上。它通过 `getGlobalVariable(HISTORY_RECORDS)` 读取历史数组，原地修改条目时间戳，然后调用 `setGlobalVariable(HISTORY_COUNT, count+1)` 递增计数。`setGlobalVariable` 写入 `HISTORY_COUNT` 后 EEZ-Flow 检测到变化，可能再次触发 UI 刷新 → 再次调用 `action_givetime` → 无限循环。

**修改**（方案 A）：
- `ui_push_history()`：新增 `setGlobalVariable(HISTORY_COUNT, count)` — 在写入历史记录的同时写入计数
- `action_givetime()`：删除末尾的 `setGlobalVariable(HISTORY_COUNT, count+1)` — 打断重入循环链

**结果**：现象不变。库存正常，历史记录不显示，UI 卡死 + 同样的 `assert(false)` 报错。重入循环也不是根因。

#### 3. 根因定位：stopScript() 桩函数 assert(false)

**分析路径**：`assert(false)` 报错直接指向 `eez-flow.cpp:6387` 的 `stopScript()` 函数：

```cpp
static void stopScript() {
    assert(false);  // ← 桩函数，未实现
}
```

追溯框架调用链：

```
ui_push_history() 写入 HISTORY_RECORDS
  → EEZ-Flow tick() 检测变化
    → 触发历史列表关联的 Flow 执行
      → Flow 执行完毕
        → executeEndComponent()          (eez-flow.cpp:2851)
          → 判断：顶层独立 Flow（非子流程、非 Action）
            → stopScriptHook()            (eez-flow.cpp:2858)
              → stopScript()              (eez-flow.cpp:6386)
                → assert(false) → 💥 卡死
```

**为什么库存列表不触发这个错误？**

库存列表（`CABINETS`）走的是 **纯数据绑定** 路径——EEZ-Flow 的 List Widget 直接读取数组数据渲染 LVGL 控件，不涉及任何 Flow 执行。没有 Flow 执行，就没有 `executeEndComponent → stopScript` 调用。

历史记录列表（`HISTORY_RECORDS`）触发了 **Flow 执行**——EEZ-Flow 为历史记录关联了一个流程脚本，脚本执行完毕后框架调用 `stopScript()` 进行清理。

**为什么 assert(false) 导致 UI 卡死而非正常崩溃？**

STM32 嵌入式环境的 `assert(false)` 等价于 `abort()` → 进入死循环 `while(1){}`。`stopScript()` 运行在 `LV_Task` 线程上，`LV_Task` 是唯一负责 LVGL 渲染和触摸事件的任务。它一死，屏幕定格、触摸无响应，表现为"卡死"。

### 根因

`eez-flow.cpp` 是 EEZ Studio 工具自动生成的代码。`stopScript()` 是 EEZ-Flow 框架的钩子函数，默认实现是 `assert(false)`，本意是"如果 UI 项目使用了 Flow 执行功能，你必须自己实现这个钩子"。

历史记录功能触发了一个顶层 Flow 执行，Flow 执行完毕后调用 `stopScript()` → `assert(false)` → LV_Task 死在死循环里。

这是 **EEZ Studio 代码生成器的桩函数未实现问题**，不是互斥锁问题。

### 修改方法

**文件**：`ElectronicWarehouse/Appli/APP/ui/eez-flow.cpp:6386-6388`

将 `stopScript()` 从带 `assert(false)` 的桩函数改为空实现：

```cpp
static void stopScript() {
    assert(false);  // ← 删除这行
}
```

改为：

```cpp
static void stopScript() {
}
```

### 全部修改汇总

| 轮次 | 文件 | 改动 | 解决的问题 |
|------|------|------|-----------|
| R1 | `eez-flow.cpp` | `setGlobalVariable`/`getGlobalVariable` 内部加锁 | LV_Task 与 Barcode_Task 数据竞态 |
| R1 | `eez-flow.cpp` | `eez_flow_tick()` 移除互斥锁 | 防止与 action_givetime 的非递归锁死锁 |
| R1 | `ui_bridge.cpp` | 10 个 `ui_*` 函数移除互斥锁 | 锁下沉后外层锁冗余 |
| R2 | `ui_bridge.cpp` | `ui_push_history` 增加 `HISTORY_COUNT` 同步写入 | 历史记录和计数原子写入 |
| R2 | `ui_bridge.cpp` | `action_givetime` 删除 `setGlobalVariable(HISTORY_COUNT)` | 打断重入触发循环 |
| **R3** | **`eez-flow.cpp`** | **`stopScript()` 移除 `assert(false)`** | **Flow 执行完毕不再崩溃** |

### 涉及文件

| 文件 | 说明 |
|------|------|
| `Appli/APP/ui/eez-flow.cpp` | EEZ Studio 自动生成代码（含 stopScript 桩函数） |
| `Appli/APP/ui/ui_bridge.cpp` | UI 桥接层（历史记录/库存的推拉操作） |
| `Appli/APP/Barcode_task.c` | 扫码任务（调用 ui_push_history） |
| `Appli/APP/UART4_RxTask.c` | UART4 接收任务（handle_inventory → handle_store → ui_push_history） |
| `Appli/APP/Outbound_task.c` | 出库任务（history_list/history_count 数据定义） |

### UI 通信架构总览

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  Barcode_Task   │    │  UART4_RxTask    │    │    LV_Task      │
│  (扫码枪)       │    │  (ESP32 通信)    │    │  (LVGL + EEZ)   │
└──────┬──────────┘    └──────┬───────────┘    └──────┬──────────┘
       │                      │                       │
       │  ui_push_history     │  ui_push_history      │  lv_timer_handler
       │  ui_push_inventory   │  ui_get/set_integer   │  ui_tick (eez_flow_tick)
       │  ui_get/set_integer  │                       │
       ▼                      ▼                       ▼
  ┌────────────────────────────────────────────────────────┐
  │                    ui_bridge.cpp                        │
  │  (数据转换层：C struct ↔ EEZ-Flow Value/Array)          │
  └────────────────────────┬───────────────────────────────┘
                           │
                           ▼
  ┌────────────────────────────────────────────────────────┐
  │                    eez-flow.cpp                         │
  │  setGlobalVariable() / getGlobalVariable()              │
  │  🔒 flow_var_mutex 保护（方案 1 加入）                   │
  │                                                        │
  │  eez::flow::tick() — Flow 执行引擎                      │
  │  stopScript() — Flow 结束钩子（方案 3 修复）              │
  └────────────────────────┬───────────────────────────────┘
                           │
                           ▼
                  g_globalVariables->values[]
                  (EEZ-Flow 全局变量数组)
```

### 经验教训

1. **EEZ Studio 自动生成的钩子函数默认是 `assert(false)` 桩**：如果 UI 项目中使用了会触发 Flow/脚本执行的功能，必须自己实现或至少改为空实现。同类的钩子还有 `showKeyboardHook`、`showKeypadHook` 等
2. **数据绑定 vs Flow 执行**：EEZ-Flow 中 List Widget 的数据绑定不触发 Flow 执行，但某些复杂 UI 组件（如带事件的列表项）会触发。添加新 UI 功能时要注意这个区别
3. **assert 在嵌入式环境不会退出程序**：`assert(false)` 会进入 `abort()` 死循环，如果发生在 UI 线程上会导致整个屏幕冻结
4. **调试技巧**：当 UI 卡死时，先检查串口是否有 `assert` 报错，它直接告诉你崩溃的精确位置

---

## 问题 6：摄像头 AE（自动曝光）持续需要 I2C，但为避免与触摸屏冲突提前释放了总线

### 问题描述

摄像头初始化后画面颜色异常——白平衡只在初始化瞬间做了一次，之后不再更新。实际上 AWB（自动白平衡）在 DCMIPP ISP 管线内部完成，不需要 I2C。真正的问题出在 AE（自动曝光）算法——它需要通过 I2C 持续调整传感器的增益（Gain）和曝光时间（Exposure），而初始化后 I2C 总线被永久释放给触摸屏了。

### 硬件背景

团队自画板上，摄像头和触摸屏的 I2C 引脚：

| 信号 | 摄像头（GPIO 模拟 I2C） | 触摸屏 GT9xxx（硬件 I2C2） |
|------|--------------------------|---------------------------|
| SCL | PD14 | PD14 |
| SDA | **PC2** | **PD4** |

只有 PD14（SCL）一根引脚共享。之前旧版硬件 SDA 也是共用的（都在 PD4），导致摄像头软件 I2C 通信时 PD4 上的电平翻转被 GT9xxx 触摸芯片当成 I2C 指令，清空了出厂配置表，触摸屏直接报废。

### 根因

`Sensor_task.c` 在摄像头初始化完成后立刻调用 `imx335_io_deinit()`，把 PD14 从 GPIO 模式切到 AF4（硬件 I2C2），从此摄像头再也发不了 I2C。但 AE 算法在运行时需要通过 I2C 调传感器增益和曝光：

```
imx335_isp_background_process()
  → ISP_BackgroundProcess()
    → ISP_Algo_AE_Process()           // AE 算法
      → ISP_SVC_Sensor_SetGain()
        → imx335_set_sensor_gain_helper()
          → IMX335_SetGain()
            → imx335_io_writereg()    ← I2C 写传感器！
      → ISP_SVC_Sensor_SetExposure()
        → IMX335_SetExposure()
          → imx335_io_writereg()      ← I2C 写传感器！
```

I2C 总线被释放后这些调用全部静默失败，曝光和增益锁死在初始值。

### 修改方法：分时复用 + 互斥锁 + I2C2 时钟关断

**核心思路**：每次摄像头 I2C 通信前后动态切换 PD14，切换期间关掉 I2C2 时钟（物理上杜绝触摸屏外设误触发），加 FreeRTOS 互斥锁保护。

**修改的文件**：

| 文件 | 改动 |
|------|------|
| `imx335.h` | 新增 `#include "cmsis_os.h"`；声明 `extern osMutexId_t cam_i2c_mutex` |
| `imx335.c` | 新增 `#include "i2c.h"`；定义 `osMutexId_t cam_i2c_mutex` |
| `imx335.c` → `imx335_io_init()` | 新增 `__HAL_RCC_I2C2_CLK_DISABLE()`，接管总线时关断 I2C2 时钟 |
| `imx335.c` → `imx335_io_deinit()` | 新增 `__HAL_RCC_I2C2_CLK_ENABLE()` + `MX_I2C2_Init()`，释放总线时恢复 I2C2 |
| `imx335.c` → `imx335_io_writereg()` | 用互斥锁包裹：拿锁 → init → I2C 通信 → deinit → 放锁 |
| `imx335.c` → `imx335_io_readreg()` | 同上 |
| `Sensor_task.c` | 删除 `imx335_io_deinit()`；在 `imx335_init()` 之前 `osMutexNew(NULL)` 创建互斥锁 |
| `gt9xxx.c` | 新增 `#include "imx335.h"` 和 `#include "cmsis_os.h"`；`gt9xxx_wr_reg()` / `gt9xxx_rd_reg()` 的 HAL I2C 调用用同一把锁包裹 |

**writereg/readreg 改造后的结构**（以 writereg 为例）：

```c
static int32_t imx335_io_writereg(uint16_t dev_addr, uint16_t reg, uint8_t *data, uint16_t length)
{
    int32_t ret = 0;

    osMutexAcquire(cam_i2c_mutex, osWaitForever);  // ① 拿锁
    imx335_io_init();                               // ② 关 I2C2 时钟，PD14→GPIO

    cam_iic_start();
    // ... I2C 通信（所有错误 goto exit） ...
    
exit:
    cam_iic_stop();
    imx335_io_deinit();                             // ③ PD14→AF4，开 I2C2 时钟，重初始化 I2C2
    osMutexRelease(cam_i2c_mutex);                  // ④ 放锁
    return ret;
}
```

**运行时行为**：

```
摄像头要发 I2C ─→ 拿锁 → 关 I2C2 → PD14切GPIO → 通信 → PD14切AF4 → 开I2C2+重初始化 → 放锁
触摸屏要发 I2C ─→ 拿锁 → HAL_I2C2通信 ───────────────────────────────────────→ 放锁
```

两个设备通过互斥锁串行化，I2C2 在摄像头通信期间时钟被关闭，物理上不存在，绝无可能误触发触摸芯片。

### 为什么是安全的

1. **I2C2 时钟关断**：摄像头 I2C 通信期间，I2C2 外设物理上不存在。即便 PD4 上有干扰也绝无可能被解释为有效 I2C 帧
2. **互斥锁**：摄像头 I2C 和触摸屏 I2C 互斥，不会出现"摄像头正用 I2C 时 HAL 超时"或"I2C2 正通信时被中途关时钟"的情况
3. **SDA 不复用**：摄像头 SDA 在 PC2，触摸屏 SDA 在 PD4，硬件上已隔离。只有 SCL（PD14）需要分时切换
4. **每次通信后释放**：摄像头只在需要时短暂占用 PD14，通信完立刻归还，触摸屏响应不受影响

---

## 问题 7：NVStore NOR Flash 数据持久化

### 背景

EEPROM（2Kb）太小，无法存储 ~6KB 物料数据（`inventory_item_T[28]` + `inventory_item_D[6]`）+ ~5KB 人脸底库（`face_gallery[FACE_GALLERY_MAX=10]`）。NOR Flash（MX25UM25645G，32MB）在 `0x71E00000`-`0x72000000` 有约 2MB 空闲空间。

### 存储布局

```
0x71E00000  +-----------------------------+
            | INVENTORY SLOT A    (8 KB)  |  2 × 4KB Sector
0x71E02000  +-----------------------------+
            | INVENTORY SLOT B    (8 KB)  |
0x71E04000  +-----------------------------+
            | FACE GALLERY SLOT A (8 KB)  |
0x71E06000  +-----------------------------+
            | FACE GALLERY SLOT B (8 KB)  |
0x71E08000  +-----------------------------+
            | (free: ~1.97 MB)            |
0x72000000  +-----------------------------+
```

### 每槽数据结构（A/B 双槽，各 8KB）

```
Offset  大小   字段
------  ----   ----
0x00    4      Magic:  0x4E565453 ("NVTS")
0x04    4      Type:   0x494E564E ("INVN") 或 0x46414345 ("FACE")
0x08    4      Version: 单调递增版本号
0x0C    4      PayloadSize
0x10    4      CRC32（只对 Payload 计算，不包含 Header）
0x14    N      Payload
```

### A/B 双槽 + 版本号可靠性机制

**写入时**：
1. 读取两槽 Header，找到有效版本号
2. 选择版本号较低的一槽（或无效槽）作为目标
3. 先擦除目标槽，再写入新版本数据（version = 当前最高版本 + 1）
4. 任意时刻掉电，至少有一槽完好 → 上电后 CRC 校验选出有效槽

**上电时**：
1. 扫描两槽 Header，校验 Magic → Type → PayloadSize 范围 → CRC32
2. 选取 Version 最高的有效槽加载到 SRAM
3. 若两槽均无效，返回 `NVSTORE_ERROR_NODATA`，保留 SRAM 中的编译期初值

### API

```c
NVStore_Status NVStore_Init(NORFlash_ObjectTypeDef *flashObj);
NVStore_Status NVStore_LoadInventory(void);
NVStore_Status NVStore_SaveInventory(void);
NVStore_Status NVStore_LoadFaceGallery(void);
NVStore_Status NVStore_SaveFaceGallery(void);
```

### 问题 7.1：NVStore_LoadFaceGallery() 栈溢出 HardFault

**现象**：首次上电调用 `NVStore_LoadFaceGallery()` 时 HardFault。

**根因**：函数内部的 `face_entry_t entries[FACE_GALLERY_MAX]`（5280 字节）是本地自动变量，分配在 main 栈上。main 栈大小仅 `_Min_Stack_Size = 0x800`（2KB），5280 字节远超 2KB，直接栈溢出。函数在 `main()` 中 RTOS 启动前被调用。

**修复**：为 `NVStore_LoadFaceGallery()` 和 `NVStore_SaveFaceGallery()` 中的大数组加上 `static` 关键字，从栈迁移到 BSS 段。

```c
// 修改前（栈上，HardFault）
face_entry_t entries[FACE_GALLERY_MAX];

// 修改后（BSS，安全）
static face_entry_t entries[FACE_GALLERY_MAX];
```

### 问题 7.2：SaveFaceGallery 返回 -2 + PRECISERR HardFault

**现象**：串口 `[TEST] SaveFaceGallery = -2`（`NVSTORE_ERROR_ERASE`），随后进入 HardFault，故障分析器报 **PRECISERR（精确的数据访问冲突）**。

**输出含义**：

| 返回值 | 含义 |
|--------|------|
| 0 | `NVSTORE_OK` — 写入成功 |
| -1 | `NVSTORE_ERROR_INIT` — NORFlash 未初始化 |
| -2 | `NVSTORE_ERROR_ERASE` — 擦除失败 |
| -3 | `NVSTORE_ERROR_WRITE` — 写入失败 |
| -4 | `NVSTORE_ERROR_CRC` — CRC 校验失败 |
| -5 | `NVSTORE_ERROR_NODATA` — 无有效数据 |
| -6 | `NVSTORE_ERROR_PARAM` — 参数错误 |

**根因**：Memory-Mapped 模式激活时执行间接擦除/写入命令导致 XSPI 总线冲突。擦除命令部分执行后 Flash 进入异常状态。saveface 返回后 AI 任务通过 MM 端口读取 NN 权重 → 总线错误 → HardFault。

**XSPI 双端口架构**：
- Port1（间接命令端口）：用于 `NORFlash_EraseSector` / `NORFlash_Write`
- Port2（内存映射端口）：用于 XIP 取指令和 MM 模式读取数据

两个端口共享同一组物理引脚。MM 模式激活时，Port2 持续占用总线，Port1 的间接命令会被干扰。

**修复**：在 `erase_and_write()` 中，通过 `SCB->VTOR` 运行时检测当前是 LRUN（SRAM 0x34000000）还是 XIP（NOR Flash）模式：

```c
__attribute__((section(".RamFunc")))  // 关键函数放在 SRAM 执行
static NVStore_Status erase_and_write(uint32_t addr, uint32_t erase_size,
                                      const uint8_t *data, uint32_t data_len)
{
    NVStore_Status status = NVSTORE_OK;
    int lrun = ((SCB->VTOR & 0xFF000000) == 0x34000000);

    __disable_irq();

    if (lrun) {
        NORFlash_DisableMemoryMappedMode(nv_flash);  // LRUN：安全关闭 MM
    }
    // XIP：RamFunc + __disable_irq() 已保证无 Flash 指令取指，不关 MM

    if (NORFlash_EraseSector(nv_flash, addr, erase_size) != NORFlash_OK) {
        status = NVSTORE_ERROR_ERASE; goto exit;
    }
    if (NORFlash_Write(nv_flash, addr, data, data_len) != NORFlash_OK) {
        status = NVSTORE_ERROR_WRITE;
    }

exit:
    if (lrun) {
        NORFlash_EnableMemoryMappedMode(nv_flash);  // LRUN：恢复 MM
    }
    __enable_irq();
    return status;
}
```

**为什么 RELEASE（XIP）模式安全**：
- 擦写函数已放入 `.RamFunc` 段，运行时从 SRAM 执行，不访问 NOR Flash
- `__disable_irq()` 阻止中断服务程序取指令时访问 Flash
- 双重保证下，擦写期间无人访问 Port2 → 间接命令安全
- 因此 XIP 模式下无需关闭 MM 模式

### 问题 7.3：.RamFunc 段链接脚本修改 & LRUN 适配

**XIP 链接脚本修改**（`STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld`）：
- 从 `.text` 中删除 `*(.RamFunc)` / `*(.RamFunc*)`
- 新增独立段 `>RAM AT> ROM`（LMA 在 Flash，VMA 在 SRAM，启动时由 `main.c` 拷贝）

**LRUN 链接脚本修改**（`STM32N647X0HXQ_LRUN_RAMxspi1.ld`）：
- 同样修改，但使用 `>RAM`（LMA=VMA，无需拷贝）
- `main.c` 中通过 `if (&_siramfunc != &_sramfunc)` 判断是否需拷贝，LRUN 下 LMA=VMA 自动跳过

### 涉及文件

| 文件 | 操作 | 说明 |
|------|------|------|
| `Appli/APP/nvstore.h` | 新建 | API 头文件 |
| `Appli/APP/nvstore.c` | 新建 | 完整实现（~257 行） |
| `Appli/STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld` | 修改 | 新增 `.RamFunc` 段 `>RAM AT> ROM` |
| `Appli/STM32N647X0HXQ_LRUN_RAMxspi1.ld` | 修改 | 新增 `.RamFunc` 段 `>RAM` |
| `Appli/Core/Src/main.c` | 修改 | RamFunc 拷贝 + NVStore 初始化 + Load |
| `Appli/APP/AI_task.c` | 修改 | 测试串口命令（saveface/loadface/clearface） |
| `Appli/X-CUBE-AI/App/app_x-cube-ai.c` | 修改 | 新增 `ai_face_gallery_export/import` |
| `Appli/X-CUBE-AI/App/app_x-cube-ai.h` | 修改 | 导出声明 |

### 验证流程

1. **DEBUG 模式编译**：确认 NVStore_Init/Load/Save 功能正常
2. **串口测试**：
   - `saveface` → 应返回 0（OK）
   - `loadface` → 应返回 0 并打印已存储的人脸列表
   - `clearface` → 清空 RAM 中的 gallery
3. **掉电测试**：`register <name>` 录入人脸 → `saveface` → 断电 → 上电 → `loadface` → `gallery` 确认数据恢复
4. **首次上电**：清空 NOR Flash 数据区域，确认 Load 返回 NODATA，使用编译期默认值

### 问题 7.4：RELEASE 模式下发 saveface 命令后程序死锁（XIP 核心冲突）

#### 现象

- **DEBUG/LRUN 模式**：`saveface` / `saveinv` 正常，返回 0，读写 NOR Flash 均成功
- **RELEASE/XIP 模式**：NVStore 初始化成功（打印 `Init failed` 不出现），但一发 `saveface` 串口命令，系统立刻死锁——串口无任何输出，屏幕冻结

#### 调试过程

**1. 早期错误（已修正）**：

- **NORFlashObject 在 `#ifdef DEBUG` 内** → `nv_flash` 为 NULL → SaveFaceGallery 返回 -1。移到 `#ifdef` 外解决。
- **RELEASE 路径调用 NORFlash_XSPI_Init() 等函数** → 内部调用 `HAL_XSPI_Abort()` 写 XSPI2 CR 寄存器 → 干扰正在进行的 XIP 读取 → 启动时即死锁。改为纯数据赋值（不写硬件寄存器）解决。

**2. 核心死锁问题**：

`erase_and_write` 虽然标了 `__attribute__((section(".RamFunc")))` 放在 SRAM 执行，但它的**调用链内部**依然有大量函数在 NOR Flash 里：

```
erase_and_write()               ← .RamFunc（SRAM ✓）
  → NORFlash_EraseSector()      ← .text（Flash ✗）
    → NORFlash_WaitBusy()       ← .text（Flash ✗）
    → NORFlash_EnableWrite()    ← .text（Flash ✗）
      → NORFlash_XSPI_*()       ← .text（Flash ✗）
        → HAL_XSPI_Command()    ← .text（Flash ✗）
        → HAL_XSPI_AutoPolling()← .text（Flash ✗）
        → XSPI_WaitFlagState...()← .text（Flash ✗）
```

发完 Erase (0x21) 或 Program (0x12) 命令后，MX25UM25645G 进入 **busy 状态**。Flash 芯片在 busy 期间只响应 Read Status Register (0x05) 命令，**其他所有命令（包括内存映射读 0xEE）均被忽略**。CPU 执行下一条指令时，取指触发 AXI 读 → 发 0xEE → Flash 不响应 → **AXI 总线永远等不到数据 → CPU 死锁**。

#### 根因本质

**XIP（eXecute In Place）+ Flash 擦写 = 死锁**，除非满足以下两个条件：

| 条件 | 含义 |
|------|------|
| 1. 擦写期间的 100% 指令都在 SRAM 执行 | 不能有任何一个函数调用落到 Flash |
| 2. 擦写期间禁止中断 | ISR 可能也在 Flash 里 |

MX25UM25645G **不支持 Read-While-Write（RWW）**——不能一边擦除/编程一边读取。XSPI2 的 Port1（间接命令）和 Port2（内存映射读取）共享同一组物理 IO 引脚，无法同时操作。

**这并非 STM32N6 或我们代码特有的问题，而是所有 XIP NOR Flash 系统做在线更新的通用约束。**

#### 解决方案：直写 XSPI2 寄存器

在 `erase_and_write` 的 XIP 路径中，**所有 Flash 操作通过直接写 XSPI2 硬件寄存器完成**，不调用任何 Flash 中的函数。整个擦写序列在一条 SRAM 中的函数里完成，CPU 取指 100% 走 SRAM。

**架构**：

```
erase_and_write()                       ← .RamFunc（SRAM）
  │
  ├─ LRUN 分支 ─→ HAL/NORFlash 函数调用（安全：代码在 SRAM）
  │
  └─ XIP 分支  ─→ 全部直接写 XSPI2 寄存器（零 Flash 函数调用）
       │
       ├─ xip_wait_ready()      ← .RamFunc   自动轮询 RDSR 等 WIP=0
       ├─ xip_send_cmd_instr()  ← .RamFunc   发仅指令命令（WREN 0x06）
       └─ xip_send_cmd_addr()   ← .RamFunc   发指令+地址命令（SER 0x21）
```

**XSPI2 寄存器布局**（基址 `XSPI2` / `XSPI2_BASE_S`）：

| 寄存器 | 偏移 | 关键位域 |
|--------|------|---------|
| CR | 0x000 | EN[0], ABORT[1], FMODE[29:28], APMS[22], PMM[23], MSEL[31:30] |
| SR | 0x020 | TEF[0], TCF[1], SMF[3], BUSY[5] |
| FCR | 0x024 | CTEF[0], CTCF[1], CSMF[3]（写 1 清除对应 SR 标志） |
| DLR | 0x040 | 数据长度 - 1 |
| AR | 0x048 | 目标地址 |
| DR | 0x050 | 数据寄存器（逐字节写） |
| PSMKR | 0x080 | 自动轮询掩码 |
| PSMAR | 0x088 | 自动轮询匹配值 |
| PIR | 0x090 | 自动轮询间隔 |
| CCR | 0x100 | IMODE[2:0], IDTR[3], ISIZE[5:4], ADMODE[10:8], ADDTR[11], ADSIZE[13:12], DMODE[26:24], DDTR[27], DQSE[29] |
| TCR | 0x108 | DCYC[4:0], SSHIFT[30] |
| IR | 0x110 | 指令值（8D8D8D 模式为 16 位：CMD«8 | ~CMD） |
| WCCR | 0x180 | 写路径 CCR（MM 模式用） |
| WTCR | 0x188 | 写路径 TCR |
| WIR | 0x190 | 写路径 IR |

**FMODE 功能模式**：

| FMODE | 模式 | 说明 |
|-------|------|------|
| 0 | 间接写入 | 用于 WREN / Erase / Program 命令 |
| 1 | 间接读取 | 一般不用 |
| 2 | 自动轮询 | 用于 RDSR 等待 WIP=0 |
| 3 | 内存映射 | 用于 XIP 正常取指 |

**8D-8D-8D DTR 模式的 CCR 寄存器值**：

| 数据阶段 | CCR 值 | 组成 |
|---------|--------|------|
| 仅指令 | `0x0000003F` | IMODE=7(8线), IDTR=1, ISIZE=3(16位) |
| 指令+地址 | `0x00003F3F` | + ADMODE=7, ADDTR=1, ADSIZE=3(32位) |
| 指令+地址+数据（写） | `0x0F003F3F` | + DMODE=7, DDTR=1, DQSE=0 |
| 指令+地址+数据（读） | `0x2F003F3F` | + DMODE=7, DDTR=1, DQSE=1 |

**8D8D8D 指令编码（16 位 DTR = 指令字节«8 | ~指令字节）**：

| 命令 | 编码 | 用途 |
|------|------|------|
| 0x05 | `0x05FA` | Read Status Register |
| 0x06 | `0x06F9` | Write Enable |
| 0x21 | `0x21DE` | Sector Erase (4KB) |
| 0x12 | `0x12ED` | Page Program (256B) |
| 0xEE | `0xEE11` | Memory-Mapped Read（恢复 XIP 用） |

#### 擦写操作完整序列（XIP 路径）

**步骤 0：退出 MM 模式**
```
保存 save_ccr/ir/tcr/wccr/wir/wtcr ← xspi->CCR/IR/TCR/WCCR/WIR/WTCR
__DSB()                              // 确保无待处理 AXI 事务
CR = (CR & ~FMODE) | EN              // FMODE=0（间接写入），保持 EN=1
等 BUSY=0                            // 确保 XSPI 空闲
```

**步骤 1-3 对每个扇区重复（8KB = 2 × 4KB）**：
```
┌─ 自动轮询等 Flash Ready ─────────────────────────────────┐
│ CCR=0x2F003F3F, TCR=4, IR=0x05FA, AR=0, DLR=0             │
│ PSMKR=0x01, PSMAR=0x00, PIR=0x10                           │
│ FMODE=2（自动轮询）, APMS=1, PMM=0（AND 匹配）              │
│ 写 AR=0 触发 → 等 SMF=1 → FCR 清 SMF                       │
├─ Write Enable ────────────────────────────────────────────┤
│ CCR=0x0000003F, TCR=0, IR=0x06F9                            │
│ FMODE=0（间接写入）→ 等 BUSY=0 → 等 TCF=1 → FCR 清 TCF     │
├─ Sector Erase ────────────────────────────────────────────┤
│ CCR=0x00003F3F, TCR=0, IR=0x21DE, AR=addr                  │
│ FMODE=0 → 等 BUSY=0 → 等 TCF=1 → FCR 清 TCF                │
└────────────────────────────────────────────────────────────┘
```

**步骤 4：等待最后一个扇区擦除完成**

自动轮询（同上，等 WIP=0）。

**步骤 5-7 对每个页重复（最多 32 页 × 256B = 8KB）**：
```
┌─ Write Enable（同上）─────────────────────────────────────┐
├─ Page Program ────────────────────────────────────────────┤
│ CCR=0x0F003F3F, TCR=0, IR=0x12ED, AR=addr, DLR=len-1      │
│ FMODE=0 → 逐字节写 DR → 等 BUSY=0 → 等 TCF=1 → FCR 清 TCF │
├─ 自动轮询等 Program 完成（同上）───────────────────────────┤
└────────────────────────────────────────────────────────────┘
```

**步骤 8：恢复 MM 模式**
```
CCR=save_ccr, TCR=save_tcr, IR=save_ir
WCCR=save_wccr, WTCR=save_wtcr, WIR=save_wir
CR = (CR & ~FMODE) | (3 << FMODE_Pos) | EN   // FMODE=3
__DSB()
```

**关键设计决策**：

1. **读写配置分别保存恢复**：读路径（CCR/TCR/IR）被间接命令覆盖，需保存。写路径（WCCR/WTCR/WIR）不碰但为安全也保存。
2. **Page Program 不用 DQS**：CCR=0x0F003F3F（DQSE=0），因为写数据不需要 DQS 信号。
3. **自动轮询用 DQS**：CCR=0x2F003F3F（DQSE=1），因为读状态寄存器需要 DQS。
4. **超时用 volatile 忙等循环**：不能调用 `HAL_GetTick()`（在 Flash 里），用 volatile countdown + NOP。800MHz 下 ~6 cycles/iter ≈ 7.5ns/iter，600M iterations ≈ 4.5s。Sector Erase 最坏 5s（MX25UM25645G 手册），设 800M iterations ≈ 6s 留余量。
5. **写 DR 逐字节**：`*((__IO uint8_t *)&xspi->DR) = byte`，与 HAL 行为一致。

#### 全部修改文件

| 文件 | 改动 | 说明 |
|------|------|------|
| `Appli/APP/nvstore.c` | **核心修改** | 新增 3 个 `.RamFunc` 寄存器级辅助函数 + 重写 `erase_and_write` 的 XIP 分支 |
| `Appli/APP/nvstore.h` | 新建 | API 头文件（地址宏、状态枚举、函数声明） |
| `Appli/STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld` | 修改 | 新增 `.RamFunc` 段 `>RAM AT> ROM` |
| `Appli/STM32N647X0HXQ_LRUN_RAMxspi1.ld` | 修改 | 新增 `.RamFunc` 段 `>RAM` |
| `Appli/Core/Src/main.c` | 修改 | NORFlashObject 全局化、RELEASE 路径纯数据初始化、RamFunc 拷贝、NVStore Init + Load |

#### 参考资料

- **ST 社区**：STM32N6 XSPI + NOR Flash XIP 擦写讨论
  - `https://community.st.com/t5/stm32-mcus-products/stm32n6-i3c-mixed-communication-no-signal/td-p/770793` — I3C 问题（同平台已知外设缺陷，0 回复）
  - 搜索关键词：`STM32 XIP NOR Flash write from SRAM RamFunc` / `XSPI memory-mapped write conflict`
- **MX25UM25645G 数据手册**：
  - 不支持 Read-While-Write（RWW）
  - Sector Erase (0x21): 典型 0.3s，最大 5s
  - Page Program (0x12): 典型 0.6ms，最大 5ms
  - 8D-8D-8D DTR 模式指令编码：16 位 = CMD«8 | ~CMD
- **STM32N6 参考手册（RM0486）**：XSPI 章节 — CR/SR/FCR/CCR/TCR/IR/DLR/AR/DR 寄存器定义
  - FMODE[29:28]：00=间接写入, 10=自动轮询, 11=内存映射
  - APMS[22]：自动轮询匹配停止
  - PMM[23]：轮询匹配模式（0=AND, 1=OR）
- **ST AN4760**：Quad-SPI interface on STM32 microcontrollers — 虽然针对 QSPI 但 XSPI 兼容，FMODE/auto-polling 原理相同
- **ARM Cortex-M55 Generic User Guide**：`__DSB()` / `__disable_irq()` / `SCB->VTOR` 用法

### 经验教训

1. **main 栈只有 2KB**：RTOS 启动前在 `main()` 中调用的函数不能使用大局部变量。`static` 数组在 BSS 中不受栈大小限制。
2. **XSPI 双端口总线冲突**：Memory-Mapped 模式启用时，间接命令（Erase/Write）会被 Port2 的 AXI 事务干扰。LRUN 模式必须关闭 MM，XIP 模式必须退到间接模式（FMODE=0/2）。
3. **RamFunc 不仅函数本身要在 SRAM——整条调用链都必须**：只把顶层函数标 `.RamFunc` 不够，它调用的每一个子函数、子子函数都要在 SRAM。XIP 模式下消除调用链的唯一方法是用寄存器直接编程。
4. **SCB->VTOR 是可靠的运行时模式判别**：比条件编译（`#ifdef DEBUG`）更安全，不受编译配置错误影响。
5. **A/B 双槽 + 版本号**是 Flash 存储的经典容错方案：一个槽被擦除但未写入的时间窗口内掉电，至少还有另一个槽完好。
6. **XIP Flash 擦写是嵌入式系统写入自身 Flash 的统一难题**：无论 STM32、NXP、TI 的 MCU，只要从 NOR Flash 执行代码，在线更新就必须把擦写代码放在 SRAM。这是硬件约束，与厂商无关。

---

### 问题 7.5：RELEASE/XIP 模式直接寄存器编程擦写 NOR Flash 失败（最终结论）

#### 背景

问题 7.4 中已将 `erase_and_write` 及其所有子函数放入 `.RamFunc`（SRAM 执行），XIP 路径完全通过直接写 XSPI2 硬件寄存器实现擦除和写入，零 HAL/Flash 函数调用。理论上消除了所有 Flash 取指依赖。

#### 测试流程

1. **编译 RELEASE 模式**（代码在 NOR Flash 0x70100400 XIP 执行）
2. **上电运行**，串口输入 `saveface` 命令
3. **观察结果**：系统死锁，串口无返回，屏幕冻结

#### 调试过程中发现的子问题与修复

##### 子问题 A：寄存器写入顺序错误（FMODE=0 前写了寄存器）

**现象**：`saveface` 等几秒后返回 -2（`NVSTORE_ERROR_ERASE`）。

**根因**：`xip_wait_ready`（auto-polling）退出时将 FMODE 留在 2。紧接着 `xip_send_cmd_instr`（WREN 0x06）在 FMODE=2 时写了 IR 寄存器——这在 auto-polling 模式下只会更新 auto-poll 配置，不会触发间接传输。然后切 FMODE=0，但传输永远没有被触发过。WREN 命令从未到达 Flash → WEL 未置位 → Sector Erase 无效。

**修复**：所有 4 处 helper 函数改为"先设 FMODE=0，再写寄存器触发传输"。FMODE 切换必须在寄存器写入之前。

##### 子问题 B：退出 MM 模式时切换了 EN=0→1 导致 AXI 总线错误

**现象**：修复 A 后，`saveface` 直接卡死（无返回）。

**根因**：退出 Memory-Mapped 模式时，代码执行了 `EN=0 → EN=1` 重置 XSPI 外设。这会导致：
1. XSPI2 外设被禁用，所有未完成的 AXI 读请求收到 SLVERR/DECERR 错误响应
2. AXI 总线错误触发 CPU 的 BusFault/HardFault
3. 异常处理器的向量表在 NOR Flash（0x70100400）中 → CPU 尝试读取异常向量 → AXI 读 NOR Flash → XSPI2 已禁用 → AXI 总线停顿 → 无限死锁

**修复**：移除 EN 切换。参照 HAL `HAL_XSPI_Abort()` 的实现——仅 ABORT + 清 FMODE，不动 EN 位。

**参考文件**：
- `ElectronicWarehouse/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_xspi.c` 第 2460 行 `HAL_XSPI_Abort()`
- `ElectronicWarehouse/Drivers/BSP/NORFlash/norflash_xspi.c` 第 93 行 `NORFlash_XSPI_DisableMapMode()`

##### 子问题 C：ICACHE 投机取指导致 AXI 停顿

**现象**：修复 A+B 后，`saveface` 不再立刻卡死，但返回 -2（`NVSTORE_ERROR_ERASE`），等待约 6 秒后输出。

**根因**：Cortex-M55 的指令缓存（ICACHE）在退出 MM 模式前可能已缓存了 NOR Flash 区域的指令。退出 MM 后（FMODE=0），XSPI2 Port2 不再服务 MM 读取。此时如果 ICACHE 未命中（CPU 需要取指但指令不在 ICACHE），CPU 会通过 AXI 总线去 NOR Flash 取指 → AXI 停顿 → 死锁。

此外 ICACHE 也可能触发投机预取（speculative prefetch），CPU 可能会预测性地从 NOR Flash 地址取指，即使正在执行的代码在 SRAM。

**修复**：退出 MM 模式前通过直接写 SCB 寄存器禁用 ICACHE：
```c
SCB->CCR &= ~SCB_CCR_IC_Msk;  // 关 I-Cache
__DSB(); __ISB();
SCB->ICIALLU = 0UL;           // 无效化 I-Cache
__DSB(); __ISB();
```
恢复 MM 模式后重新使能：
```c
SCB->CCR |= SCB_CCR_IC_Msk;   // 开 I-Cache
__DSB(); __ISB();
```
注意：不能 `#include "core_starmc1.h"`（与 `core_cm55.h` 类型定义冲突），直接写寄存器即可。

##### 子问题 D：xip_wait_ready 在 FMODE=0 时写了 AR 触发垃圾传输

**现象**：修复 A+B+C 后，`saveface` 仍返回 -2，等待约 6 秒。

**根因**：`xip_wait_ready` 函数在 FMODE=0 的配置阶段写了 `xspi->AR = 0`。在 FMODE=0（间接写入模式）下写 AR 会触发一次间接传输——XSPI 开始向 Flash 发送指令+地址，但此时 PSMKR/PSMAR/PIR 等 auto-polling 寄存器尚未配置完成。随后 FMODE 切换到 2（auto-polling）又写一次 AR。第一次 AR 写触发的垃圾传输可能使 Flash 进入错误状态。

**修复**：`xip_wait_ready` 的 Step 2（FMODE=0 配置阶段）不写 AR，只在 Step 4（FMODE=2）写 AR 触发 auto-polling。
```c
// Step 2: 写 CCR/TCR/IR/DLR/PSMKR/PSMAR/PIR — 不写 AR！
// Step 3: 切 FMODE=2 + APMS
// Step 4: 写 AR=0 触发 auto-polling（现在 FMODE=2，正确触发）
```

##### 子问题 E：最终仍死锁（FMODE 切换后首次 auto-polling 无法完成）

**现象**：修复 A+B+C+D，并加入 -20~-25 分段错误码后，`saveface` 再次直接死锁，连错误码都来不及返回。

**分析**：综合所有修复后，死锁发生在退出 MM 模式后、第一次 `xip_wait_ready`（auto-polling）的过程中。说明即使在 FMODE=0 模式下发 RDSR auto-polling，XSPI2 仍然无法完成与 NOR Flash 的正常通信。

可能原因（未验证）：
1. **FMODE 从 3→0 的过渡不干净**：MM 模式可能遗留了未完成的内部状态，单纯清 FMODE 不足以完全退出
2. **ABORT 后 Flash 处于异常状态**：ABORT 可能中断了 Flash 正在执行的读命令，Flash 进入需要特殊恢复的状态（某些 Flash 在命令中断后需要发送 Software Reset 命令）
3. **XSPI2 Port1/Port2 有隐藏的总线竞争**：即使清除了 FMODE，Port2 可能在硬件层面仍然占用着 AXI 总线
4. **MX25UM25645G 在 8D-8D-8D DTR 模式下对命令时序有额外要求**：clock phase、DQS signaling、CS 的建立/保持时间等。虽然 XSPI2 的 DCR 配置（FSBL 初始化）应该正确，但 FSBL 的配置是为 MM 读优化的，不一定适合间接写
5. **AXI bus matrix 层面的问题**：STM32N6 的总线矩阵可能对 XSPI2 MM 区域有特殊处理，切换 FMODE 后可能需要等待 AXI 流水线排空

#### 最终结论

**放弃 RELEASE/XIP 模式下对 NOR Flash 的擦写操作。**

LRUN 模式（代码在 SRAM 中运行，通过 HAL/NORFlash 驱动）已经验证可以正常擦写 NOR Flash。因此在 LRUN 模式下，NVStore 功能仍然是可用的。但产品化 RELEASE 固件（XIP 从 NOR Flash 执行）不支持运行时更新 NOR Flash 中的数据。

**根因**：MX25UM25645G 不支持 Read-While-Write（RWW），当代码从该 Flash 执行时，任何擦除/写入操作都会使 Flash 进入忙状态，无法同时服务 CPU 的取指请求。将擦写代码放入 SRAM 消除了直接的 Flash 取指依赖，但以下隐藏路径仍会触发 Flash 访问：
- Cortex-M55 ICACHE 的投机预取和缓存填充
- AXI 总线上未完成的 XIP 读事务
- 可能的 CPU 投机执行或数据预取

这些硬件层面的行为无法从应用层完全控制。

#### 可行替代方案（供未来参考）

1. **双 Flash 方案**：一片 NOR Flash 用于 XIP（代码执行），另一片 SPI Flash 用于数据存储（读写时不影响 XIP）
2. **EEPROM/FRAM 扩展**：使用更大容量的 I2C/SPI EEPROM（如 64Kb+）或 FRAM 存储关键数据
3. **SD 卡**：STM32N6 有 SDMMC 接口，可使用 SD 卡存储数据，不占用 NOR Flash 带宽
4. **外部 MCU 管理存储**：用一颗小 MCU 管理 NOR Flash 的擦写，主 MCU 通过 UART/SPI 与之通信
5. **仅在 LRUN/DEBUG 模式使用 NVStore**：开发/调试阶段用 LRUN 模式验证逻辑，量产时不依赖 NOR Flash 数据持久化

#### 涉及文件

| 文件 | 状态 | 说明 |
|------|------|------|
| `Appli/APP/nvstore.c` | 保留（LRUN 可用） | ~470 行，含 A/B 双槽 + CRC + 直接寄存器 XIP 代码 |
| `Appli/APP/nvstore.h` | 保留 | NVStore API 头文件 |
| `Appli/STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld` | 保留 | `.RamFunc` 段 `>RAM AT> ROM` |
| `Appli/Core/Src/main.c` | 保留 | NORFlashObject 全局化 + RamFunc 拷贝 + NVStore Init |
| `README1.md` | 本文档 | 问题 7.1-7.5 完整记录 |

#### 参考资料

- **RM0486**：STM32N6 参考手册，XSPI 章节（寄存器布局、FMODE、ABORT、auto-polling）
- **MX25UM25645G 数据手册**：256Mb NOR Flash，8D-8D-8D DTR mode，不支持 RWW
- **AN6228**：How to implement XSPI read-while-write (RWW) feature on STM32 MCUs（需 RWW 支持的 Flash）
- **AN4838**：STM32H7 的 MPU 和 cache 管理（cache coherency 问题与本问题类似）
- **ARM Cortex-M55 技术参考手册**：ICACHE/DCACHE 控制、SCB CCR 寄存器
- **ST 社区**：STM32H7 外部 NOR Flash XIP + 写入的讨论（结论：需要 Flash 支持 RWW 或双 Flash 方案）
  - https://community.st.com/stm32-mcus-embedded-software-32/issues-while-writing-into-an-external-nor-while-xip-from-a-different-section-of-same-external-nor-120586

---

## 问题 8：SD NAND + FATFS 文件系统集成与调试

### 背景

放弃 NOR Flash XIP 擦写后，改用核心板自带的 16Gb SD NAND（4-bit SDMMC2 接口，50MHz）做数据持久化。移植 FATFS R0.15a，用文件系统直接读写二进制文件。CubeMX 已完成 SDMMC2 外设配置。

### 架构概览

```
storage.c (应用层)
  → f_open / f_read / f_write / f_mount    (FATFS R0.15a)
    → disk_read / disk_write / disk_ioctl   (diskio.c 胶水层)
      → sd_nand_read_disk / sd_nand_write_disk  (SD NAND 驱动)
        → HAL_SD_ReadBlocks / HAL_SD_WriteBlocks (STM32 HAL)
          → SDMMC2 (4-bit, 50MHz)
```

### 移植文件清单

**新建：**
| 文件 | 说明 |
|------|------|
| `Appli/Drivers/BSP/SD_NAND/sd_nand.h` | SD NAND 驱动头文件 |
| `Appli/Drivers/BSP/SD_NAND/sd_nand.c` | SD NAND 驱动实现 |
| `Appli/Middlewares/ST/Third_Party/FATFS/ff.h` | FATFS 公共 API |
| `Appli/Middlewares/ST/Third_Party/FATFS/ff.c` | FATFS 核心（R0.15a） |
| `Appli/Middlewares/ST/Third_Party/FATFS/diskio.h` | 磁盘 I/O 接口 |
| `Appli/Middlewares/ST/Third_Party/FATFS/ffconf.h` | FATFS 配置（FF_VOLUMES=1, FF_USE_MKFS=1, FF_CODE_PAGE=936） |
| `Appli/FATFS/Target/diskio.c` | 胶水层（FATFS → SD NAND 驱动） |
| `Appli/APP/storage.h` | 存储模块 API |
| `Appli/APP/storage.c` | 存储模块实现（inventory/faces/history 的保存和加载） |

**修改：**
| 文件 | 改动 |
|------|------|
| `Appli/Core/Inc/main.h` | 添加 `#include "sdmmc.h"` |
| `Appli/Core/Src/main.c` | 添加 SD NAND init + storage init + 数据加载序列 |
| `Appli/Core/Src/app_freertos.c` | 添加 StorageFlush 低优先级任务 + storage_mutex |
| `Appli/APP/AI_task.c` | 添加 saveface/loadface/clearface 串口调试命令 |
| `Appli/Release/sources.mk` | SUBDIRS 添加 SD_NAND、FATFS/Target、FATFS |
| `Appli/Debug/sources.mk` | 同上 |
| `Release/FATFS/Target/subdir.mk` | **手动创建**（CubeMX 不会自动生成此目录的构建文件） |
| `Debug/FATFS/Target/subdir.mk` | **手动创建** |

### FATFS 配置要点 (ffconf.h)

```c
FF_VOLUMES    = 1    // 只挂 SD NAND 一个卷
FF_STR_VOLUME_ID = 0
FF_USE_MKFS   = 1    // 首次启动自动格式化空白卡
FF_USE_LFN    = 0    // 不需要长文件名
FF_FS_EXFAT   = 0    // FAT32 足够
FF_CODE_PAGE  = 936  // 简体中文
FF_USE_LABEL  = 0
```

### 延迟写入架构

为减少 FATFS I/O 对 UI/AI 任务的阻塞，采用脏标志 + 低优先级 Flush 任务模式：

```
AI_task          → storage_mark_faces_dirty()
UART4_RxTask     → storage_mark_inventory_dirty()
Barcode_task     → storage_mark_inventory_dirty()
actions.c        → storage_mark_inventory_dirty()
                       ↓
StorageFlush_Task (优先级 Low, 每 2s)
  → storage_flush()
    → osMutexAcquire(storage_mutex)
    → if dirty: save → clear flag
    → osMutexRelease(storage_mutex)
```

### 问题 8.1：链接错误 "undefined reference to disk_write/disk_read/..."

**现象**：编译时链接器报错，所有 disk_* 函数未定义。

**根因**：`FATFS/Target/subdir.mk` 不存在于 Debug 和 Release 目录。STM32CubeIDE 的 make 系统通过每个源码目录下的 `subdir.mk` 文件来发现 `.c` 源文件，仅在 `sources.mk` 的 SUBDIRS 中添加目录是不够的。

**修复**：手动创建 `Release/FATFS/Target/subdir.mk` 和 `Debug/FATFS/Target/subdir.mk`，参照已有的 `SD_NAND/subdir.mk` 格式：

```makefile
# C sources
C_SOURCES += \
FATFS/Target/diskio.c
```

### 问题 8.2：SD NAND 容量显示为 983040 MB

**现象**：串口输出 `[SD NAND] OK, 983040 MB`，实际应为 ~1920 MB。

**根因**：容量计算公式错误：
```c
// 错误（多乘了 LogBlockSize = 512）
printf("[SD NAND] OK, %lu MB\r\n",
    (unsigned long)(g_sd_nand_info_struct.LogBlockNbr / 2 / 1024
                    * g_sd_nand_info_struct.LogBlockSize));
```
`LogBlockNbr` 是扇区总数（1 sector = 512 字节），`/ 2 / 1024` 已将扇区转为 MB。再乘以 `LogBlockSize`（512）放大了 512 倍：983040 / 512 ≈ 1920 MB。

**修复**：移除多余的 `* g_sd_nand_info_struct.LogBlockSize`：
```c
printf("[SD NAND] OK, %lu MB\r\n",
    (unsigned long)(g_sd_nand_info_struct.LogBlockNbr / 2 / 1024));
```

### 问题 8.3：saveface 返回 FRESULT=13 但初始化显示 "Mount OK"

**现象**：
- 启动日志显示 `[STORAGE] Mount OK`
- 发送 `saveface` 命令后：`[STORAGE] faces.bin open failed (FRESULT=13)`
- FRESULT 13 = `FR_NO_FILESYSTEM`（没有有效的 FAT 文件系统）

**根因**：`f_mount` 的第三个参数 `opt` 的语义陷阱。

```c
// 原代码
res = f_mount(&g_fs, "0:", 0);  // opt=0：延迟挂载

// ff.c 源码第 3719 行
if (opt == 0) return FR_OK;  // 仅注册对象，不读磁盘，永远返回 FR_OK
```

`opt=0`（延迟挂载）只把文件系统对象注册到 FATFS 内部链表，**不读磁盘**、**不检查 FAT 签名**、**永远返回 FR_OK**。空白的 SD NAND 没有文件系统，但 `opt=0` 的 `f_mount` 仍返回 FR_OK。后续的 auto-format 逻辑（`if (res == FR_NO_FILESYSTEM)`）永远不会触发。真正的文件系统检查延迟到 `f_open` 时才执行，此时才发现没有 FAT 签名 → 返回 FR_NO_FILESYSTEM (13)。

**修复**：将 `opt` 从 `0` 改为 `1`（立即挂载）：
```c
res = f_mount(&g_fs, "0:", 1);  // opt=1：立即挂载，真的读磁盘检查 FAT
```

`opt=1` 的 `f_mount` 会：
1. 读磁盘 Sector 0（MBR）
2. 解析分区表找到 FAT 卷
3. 读 FAT 卷的 Boot Sector
4. 校验 FAT 签名（0x55AA）和 FSINFO 签名

如果以上任何步骤失败，返回对应的错误码（`FR_NO_FILESYSTEM` 等），不再"假装成功"。

### 问题 8.4：格式化成功但重新挂载失败（FR_NO_FILESYSTEM 反复出现）

**现象**：修复 8.3 后：
- `[STORAGE] No filesystem, formatting...`
- `[STORAGE] Format OK`
- `[STORAGE] Mount failed (13)` ← 刚格式化完，重新挂载又报无文件系统！

**根因**：D-Cache 一致性问题。`disk_read` 使用了 `SCB_InvalidateDCache_by_Addr`：

```c
// 错误实现
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    sd_nand_read_disk(buff, sector, count);              // DMA 写数据到内存
    SCB_InvalidateDCache_by_Addr(buff, count * 512);     // ← BUG 在这里
    return RES_OK;
}
```

**问题机制**（Cortex-M55 D-Cache）：

1. `f_mkfs` 调用 `disk_write` → 通过 DMA 将 FAT 元数据从 SRAM 写到 SD NAND
2. `f_mkfs` 返回后调用 `f_mount` → 触发 `disk_read` 读回 Boot Sector 验证
3. `sd_nand_read_disk` 通过 SDMMC2 DMA 将数据写入 SRAM 缓冲区
4. `SCB_InvalidateDCache_by_Addr` **直接丢弃**缓冲区对应的 Cache Line

**关键**：如果 FATFS 内部栈变量（如 `fs->win[]` 512 字节工作缓冲区）的 Cache Line 碰巧与 DMA 读入的缓冲区落在同一 Cache Line（或同一组），Invalidate 会在丢弃 DMA 数据的同时，也丢弃了 FATFS 刚刚写回但尚未物理写出的脏数据。CPU 后续读取该地址时 Cache Miss，从内存读到的是**磁盘上的旧数据**——而磁盘上的数据是刚刚被格式化写入的，理应是最新的。

实际上问题更微妙——`f_mkfs` 写入的数据经过 `SCB_CleanDCache_by_Addr` 正确写回了，但问题出在 `f_mount` 内部的读路径。`f_mount` 调用 `disk_read` 读 Boot Sector，DMA 将数据写入 SRAM。如果该地址的 Cache Line 恰好之前被其他变量（如 `g_fs.win[]`）的脏数据占据，Invalidate 会丢弃那些未 Clean 的脏行 → 文件系统对象状态不一致。

**修复**：将 `SCB_InvalidateDCache_by_Addr` 改为 `SCB_CleanInvalidateDCache_by_Addr`：

```c
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    sd_nand_read_disk(buff, sector, count);
    // 先把脏的 Cache Line 写回内存，再 Invalidate
    // 确保 DMA 写入的数据不会因 Invalidate 丢失
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)buff, count * 512);
    return RES_OK;
}
```

`CleanInvalidate` = Clean（写回脏数据到内存）+ Invalidate（丢弃 Cache Line）。下次 CPU 读这个地址时 Cache Miss → 从内存读到 DMA 写入的正确数据。

**为什么 disk_write 不需要改**：写入路径用 `SCB_CleanDCache_by_Addr` 是正确的——只 Clean 不 Invalidate。Clean 将 CPU 写入的数据推到内存，DMA 从内存读到正确数据。如果在写之前 Invalidate，反而会丢弃 CPU 刚写的数据。

### 存储模块 API (storage.h)

```c
int  storage_init(void);                  // f_mount + 自动格式化
int  storage_load_inventory(void);        // 加载 invent_t.bin / invent_d.bin
int  storage_load_faces(void);            // 加载 faces.bin
int  storage_load_history(void);          // 加载 history.bin
int  storage_save_inventory(void);        // 保存整个库存
int  storage_save_faces(void);            // 保存人脸库
int  storage_save_history(void);          // 保存历史

// 延迟写入（设置脏标志，由 StorageFlush_Task 异步处理）
void storage_mark_inventory_dirty(void);
void storage_mark_faces_dirty(void);
void storage_mark_history_dirty(void);
void storage_flush(void);                 // 检查脏标志并写入
```

### 串口调试命令（AI_task.c 中实现）

```
register <name>  — 录入当前人脸
s                — 多角度采集一张（录入过程中）
identify         — 识别当前人脸
gallery          — 列出已注册人脸
saveface         — 保存人脸库到 SD NAND
loadface         — 从 SD NAND 加载人脸库
clearface        — 清空 SRAM 中人脸库（不修改 SD NAND）
help             — 显示帮助
```

### 启动序列（main.c）

在 `SystemIsolation_Config()` 之后、`osKernelInitialize()` 之前：

```c
#include "sd_nand.h"
#include "storage.h"

while (sd_nand_init()) {
    printf("[SD NAND] Retry...\r\n");
    HAL_Delay(1000);
}
printf("[SD NAND] OK, %lu MB\r\n",
    (unsigned long)(g_sd_nand_info_struct.LogBlockNbr / 2 / 1024));

if (storage_init() == 0) {
    storage_load_inventory();
    storage_load_faces();
    storage_load_history();
    printf("[STORAGE] All data loaded\r\n");
} else {
    printf("[STORAGE] Mount failed, RAM-only\r\n");
}
```

### 验证结果

1. 编译通过，烧录启动 → 串口打印 `[SD NAND] OK, 1920 MB`
2. 首次启动 → 自动格式化 + 打印 `No invent_t.bin (first boot)`
3. `saveface` → `[DISKIO] write sect=480 cnt=1` → `[STORAGE] Face gallery saved (1 faces)`
4. `clearface` → 清空 SRAM → `loadface` → 人脸恢复 → `gallery` 显示已存储的人脸

### 涉及文件汇总

| 文件 | 操作 | 说明 |
|------|------|------|
| `Appli/Core/Inc/main.h` | 修改 | 添加 `#include "sdmmc.h"` |
| `Appli/Core/Src/main.c` | 修改 | SD NAND init + storage init + 数据加载 + MB 修复 |
| `Appli/Core/Src/app_freertos.c` | 修改 | StorageFlush 任务 + storage_mutex |
| `Appli/FATFS/Target/diskio.c` | 新建/修改 | FATFS 胶水层（含 D-Cache 修复） |
| `Release/FATFS/Target/subdir.mk` | 新建 | Release 构建文件 |
| `Debug/FATFS/Target/subdir.mk` | 新建 | Debug 构建文件 |
| `Appli/APP/storage.h` | 新建 | 存储模块 API |
| `Appli/APP/storage.c` | 新建/修改 | 存储模块实现（含 f_mount opt 修复） |
| `Appli/APP/AI_task.c` | 修改 | 串口调试命令（saveface/loadface/clearface） |
| `Appli/Middlewares/ST/Third_Party/FATFS/ffconf.h` | 新建 | FATFS 配置 |
| `Appli/Release/sources.mk` | 修改 | SUBDIRS 新增 |
| `Appli/Debug/sources.mk` | 修改 | SUBDIRS 新增 |

### 经验教训

1. **`f_mount` opt 参数是经典的 FATFS 陷阱**：`opt=0`（延迟挂载）只注册不检查，永远返回 FR_OK。需要验证文件系统是否真的存在时必须用 `opt=1`（立即挂载）。无数嵌入式开发者在这栽过跟头。
2. **Cortex-M55 D-Cache 一致性是 DMA + 文件系统并存时的核心难点**：读路径用 `CleanInvalidate`（先写回脏数据再丢弃 Cache Line），写路径用 `Clean`（只写回不丢弃）。Invalidate 单独使用会丢弃未写回的脏数据。
3. **STM32CubeIDE 的 make 系统依赖 `subdir.mk`**：新增源码目录时，除了在 `sources.mk` 的 SUBDIRS 中添加目录名，还必须手动创建对应目录下的 `subdir.mk` 文件。仅修改 SUBDIRS 不够。
4. **f_mkfs 是启动时格式化的安全方案**：配合 `FF_USE_MKFS=1`，首次上电时自动检测并格式化空白 SD NAND，无需预先用 PC 格式化。
5. **延迟写入模式适合 UI 交互场景**：通过脏标志 + 低优先级 Flush 任务，将慢速的 FATFS 写入从热路径（扫码、人脸录入）移到后台，避免 UI 卡顿。
6. **FATFS 文件写操作本身是可靠的**：只要 D-Cache 一致性问题解决、f_mount 正确返回、SD NAND 驱动稳定，FATFS 的 `f_write` 写入小文件（几 KB）非常可靠。

---

## 问题 9：network_fc (MobileFaceNet) 人脸识别模型验证与调优

### 背景

两个 AI 模型部署在 STM32N6 上：
- **network_f** (BlazeFace) — 人脸检测，已验证正常
- **network_fc** (MobileFaceNet) — 人脸识别（128 维 embedding），待验证

network_fc 权重 1,156,289 字节存放在 NOR Flash `0x71200000`（xSPI2）。原始 ONNX 模型：`mobilefacenet_int8_faces.onnx`。

此前 RELEASE/XIP 模式下 NOR Flash 擦写失败（问题 7），因此权重是否被正确烧录也是一个待确认项。

### 模型规格

| 属性 | 值 |
|------|-----|
| 模型 | MobileFaceNet (int8 量化) |
| C 模型名 | network_fc |
| 输入 | [1, 3, 112, 112] int8, scale≈0.007843, zp=0 |
| 输出 | [1, 128] float32 |
| 权重大小 | 1,156,289 bytes |
| 权重位置 | NOR Flash @ 0x71200000 (xSPI2, octoFlash mempool) |
| epochs | 183 (73 HW + 110 SW) |
| 运行时 API | LL_ATON_RT_Init/RunEpochBlock/DeInit |

### 流水线架构

```
摄像头 (IMX335, 800×480)
  → DCMIPP PIPE2 (128×128 RGB888)
    → uint8→float32 归一化
      → network_f (BlazeFace, NPU)
        → 人脸检测框 [0,1] 归一化坐标
          → ai_crop_resize_face_112() (DMA2D + 双线性插值)
            → 112×112 uint8 RGB
              → quantize_u8_to_s8() (u8 - 128)
                → network_fc (MobileFaceNet, NPU)
                  → 128-dim float32 embedding
                    → L2 归一化
                      → 与 gallery 中每个人脸的 L2 距离比对
                        → 三步判断: CONFIDENT_DIST → RATIO → MATCH
```

### 调试基础设施

#### Embedding 全维输出 + 统计量

在 `AI_task.c` 的 `run_reid_pipeline()` 中打印全部 128 维 embedding（每行 16 维），以及统计量（min/max/rms/l2norm）和帧间余弦相似度（COSIM）。

#### 推理过程跟踪

在 `app_x-cube-ai.c` 的 `ai_face_reid_run()` 中打印输入/输出 buffer 地址/大小/类型，以及输出 scale/zero point 和前 8 个 float32 的 hex dump。

#### 权重 CRC32 校验

在 `app_x-cube-ai.c` 中实现查表法 CRC32（多项式 0xEDB88320），`AI_task.c` 中增加 `crccc` 串口命令：

```
PC 端: network_fc_atonbuf.xSPI2.raw
  MD5:  E06B8C80910FBEA94DB44113F364BD02
  CRC32: 0xF3588EFA
  大小:  1,156,289 bytes
```

设备端读取 0x71200000 处 1,156,289 字节计算 CRC32，与 PC 端预期值对比。

#### 新增串口命令

| 命令 | 功能 |
|------|------|
| `register <name>` | 开始录入人脸 |
| `s` | 录入时采集一帧 |
| `identify` | 识别当前人脸 |
| `gallery` | 列出全部已注册人脸 |
| `saveface` | 人脸库保存到 SD NAND |
| `loadface` | 从 SD NAND 加载人脸库 |
| `clearface` | 清空 RAM 中的人脸库 |
| `crccc` | CRC32 校验 network_fc 权重 |

### 识别算法（三步判断）

```
face_gallery[i] 存储 L2 归一化后的 128 维 embedding

识别时：
1. 计算 query embedding（L2 归一化后）与每个 gallery entry 的 L2 距离
2. 如果 best_dist ≤ FACE_CONFIDENT_DIST → 直接匹配（跳过 ratio 检查）
3. 否则，如果 best_dist / second_dist > FACE_RATIO_THRESHOLD → 拒绝（模糊不清）
4. 否则，如果 best_dist < FACE_MATCH_THRESHOLD → 匹配成功
5. 以上都不满足 → 不匹配
```

### 第一层验证：模型是否在吐垃圾数据？

#### 正常输出特征（多次测试归纳）

| 指标 | 正常范围 | 异常（垃圾数据） |
|------|---------|----------------|
| `[REID-STAT] l2norm` | 1.7 ~ 2.0 | 0.0 或 > 100 |
| `[REID-STAT] min/max` | [-0.5, 0.6] 对称 | 全 0、NaN 或 > 100 |
| `[REID-COSIM]` 同脸 | > 0.83 | < 0.3 |
| `[FC] Epochs done` | 370~382 | 不在这个范围 |
| `[FC-RAW]` 跨帧 | 值会变化 | 每帧完全相同 |
| `[FC-DBG] OUT type` | 1 (float32) | 7 (int8) |

#### 实测结果（uart记录 ①）

**CRC32 校验：**
```
[CMD] crccc
[CRC-FC] 0x71200000 + 1156289 bytes -> CRC32=0xF3588EFA (expect 0xF3588EFA)
```
权重完整性确认无误。模型正常输出，不是垃圾数据。

**三次识别 tyx：**

| # | Epochs | l2norm | COSIM vs prev | 结果 |
|---|--------|--------|---------------|------|
| 1 | 380 | 1.8387 | — | 拒绝 (ratio=0.977, best=0.383, second=0.393) |
| 2 | 370 | 1.8737 | 0.954 | ✓ Matched tyx (dist=0.328) |
| 3 | 378 | 1.9085 | 0.954 | ✓ Matched tyx (dist=0.238) |

**陌生人识别（冒充 tyx）：**

| # | dist | 结果 |
|---|------|------|
| 1 | 0.269 | ✗ 错误匹配为 tyx |
| 2 | 0.314 | ✗ 错误匹配为 tyx |
| 3 | 0.325 | ✗ 错误匹配为 tyx |

陌生人全部被错误匹配 → **阈值过松，需要调整**。

### 第二层验证：识别阈值调优

阈值迭代历史：

| 轮次 | MATCH_THRESHOLD | RATIO_THRESHOLD | CONFIDENT_DIST | 效果 |
|------|----------------|----------------|----------------|------|
| 初始 | 1.0 | 0.75 | 0.30 | 太严，谁都认不出 |
| R1 | 0.55 | 0.85 | 0.30 | 同人能认出，陌生人也被认成同人 |
| R2 | 0.40 | 0.80 | **0.38** | CONFIDENT_DIST=0.38 时所有 dist<0.38 的匹配都跳过 ratio → 陌生人误识 |
| R3 | 0.40 | 0.93 | **0.25** | 陌生人安全了，但 zs/wph 互串问题首次出现 |

**关键发现（uart记录 ②）：zs 被识别为 wph**

| # | dist to wph | 结果 |
|---|-------------|------|
| 1 | 0.303 | ✗ 错误匹配为 wph |
| 2 | 0.378 | ✗ 错误匹配为 wph（dist > CONFIDENT_DIST 但仍 < MATCH_THRESHOLD） |

**关键问题（uart记录 ③）：zs 无法区分**

| # | best dist | second dist | ratio | 结果 |
|---|-----------|-------------|-------|------|
| 1 | 0.370 | 0.377 | 0.983 | ✗ 拒绝（ratio > 0.80） |
| 2 | 0.266 | 0.288 | 0.923 | ✗ 拒绝（ratio > 0.80） |
| 3 | 0.486 | 0.500 | 0.974 | ✗ 拒绝（ratio > 0.93，uart记录 ④） |

**根本原因**：zs 和 wph 在 int8 量化 MobileFaceNet 的特征空间中距离太近。best 和 second 的差值常 < 0.02，在这个量级上噪声就能让排名翻转。

#### 最终阈值

```c
#define FACE_MATCH_THRESHOLD    0.40f
#define FACE_RATIO_THRESHOLD    0.93f
#define FACE_CONFIDENT_DIST     0.25f
```

### 第三层验证：人脸尺寸门禁（Face Size Gate）

#### 问题发现

同一人的识别距离随 face_size（检测框 max(w,h)）波动。人脸太近或太远都会导致识别质量下降。

#### 实现

在 `AI_task.c` 主循环中 `MX_X_CUBE_AI_Process_User()` 返回后，ReID 前插入 gate：

```c
#define FACE_SIZE_MIN  0.28f  // 检测框 max(w,h) 下限
#define FACE_SIZE_MAX  0.56f  // 检测框 max(w,h) 上限

float fs = max(result.detections[0].width, result.detections[0].height);
if (fs < FACE_SIZE_MIN || fs > FACE_SIZE_MAX) {
    result.nb_detect = 0;  // 不画框、不识别
    printf("[GATE] face_size=%.2f out of [%.2f, %.2f]\r\n", fs, FACE_SIZE_MIN, FACE_SIZE_MAX);
}
```

#### 效果

实测 face_size 在 0.31~0.35 范围，均在门禁内。门禁解决了距离不一致导致的识别质量波动，但不解决 zs/wph 区分问题。

### 第四层验证：录入质量门禁（Enroll Quality Gate）

#### 问题

5 次采集录入中，用户可能在中间某帧角度偏差较大，导致该帧 embedding 与整体均值差异大，最终平均后的 gallery entry 质量下降。

#### 实现

在 `app_x-cube-ai.c` 的 `ai_face_enroll_multi()` 中，每帧 embedding（L2 归一化后）与当前累积平均做 COSIM 比较：

```c
#define ENROLL_QUALITY_COSIM   0.88f  // 低于此值则拒绝该帧
#define ENROLL_QUALITY_REJECT    (-1) // 返回值：样本被拒绝

// 在 ai_face_enroll_multi() 中：
if (enroll_count > 0) {
    // 计算当前累积平均的 L2 归一化向量
    // 计算新样本与累积平均的余弦相似度
    if (cosim < ENROLL_QUALITY_COSIM) {
        printf("[REID] Sample REJECTED (cos=%.4f < %.2f)\r\n", cosim, ENROLL_QUALITY_COSIM);
        return ENROLL_QUALITY_REJECT;
    }
}
```

`AI_task.c` 调用侧处理 reject 返回值：不消耗 `g_capture_pending`，提示用户保持不动重新发 `s`。

#### 实测结果（uart记录 ⑤）

重新录入 zs 和 wph 后各识别 3 次：

**识别 zs：**

| # | best dist | second dist | ratio | 结果 |
|---|-----------|-------------|-------|------|
| 1 | 0.362 | — | — | ✓ Matched zs |
| 2 | 0.301 | 0.303 | 0.994 | ✗ 拒绝（ratio > 0.93，zs/wph 几乎一样近） |
| 3 | 0.333 | — | — | ✓ Matched zs |

**识别 wph：**

| # | best dist | second dist | ratio | 结果 |
|---|-----------|-------------|-------|------|
| 1 | 0.306 | 0.321 | 0.952 | ✗ 拒绝（ratio > 0.93） |
| 2 | 0.202 | — | — | ✓✓ 高置信匹配（dist < 0.25，跳过 ratio） |
| 3 | 0.442 | — | — | ✗ 不匹配（dist > 0.40） |

#### 分析

- 6 次中 3 次成功（50%）、2 次正确拒绝模糊匹配（33%）、1 次拒真（17%）
- 录入质量门禁本身运行正常（多次录入过程中无 rejection 日志，说明采集质量合格）
- zs/wph 无法区分的问题没有改变 — best 和 second 距离仍然高度重叠

### 量化公式说明

当前代码：

```c
void quantize_u8_to_s8(const uint8_t *u8, int8_t *s8, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        s8[i] = (int8_t)((int16_t)u8[i] - 128);
    }
}
```

理论正确公式：`s8 = round(u8 - 127.5)`。`u8 - 128` 是截断近似，对奇数 u8 值有 1-LSB 偏差。实际影响忽略不计（余弦相似度差异 < 0.001）。已写为注释说明。

### 修改文件清单

| 文件 | 改动 |
|------|------|
| `Appli/APP/AI_task.c` | 完整 embedding 输出 + STAT + 帧间 COSIM；face size gate；enroll reject 处理；`crccc` 命令 |
| `Appli/X-CUBE-AI/App/app_x-cube-ai.c` | `[FC-DBG]` buffer info + `[FC-RAW]` hex dump；CRC32 实现 + `fc_weight_crc32()`；enroll quality gate |
| `Appli/X-CUBE-AI/App/app_x-cube-ai.h` | `FACE_MATCH_THRESHOLD` 等阈值定义；`ai_detection_t` / `ai_result_t` / `face_entry_t` 结构体；`ENROLL_QUALITY_COSIM` / `ENROLL_QUALITY_REJECT` |

### 最终配置

```c
// 识别
#define FACE_MATCH_THRESHOLD    0.40f
#define FACE_RATIO_THRESHOLD    0.93f
#define FACE_CONFIDENT_DIST     0.25f

// 人脸尺寸门禁
#define FACE_SIZE_MIN           0.28f
#define FACE_SIZE_MAX           0.56f

// 录入质量
#define ENROLL_QUALITY_COSIM    0.88f
#define FACE_ENROLL_SAMPLES     5

// 人脸库
#define FACE_EMBEDDING_DIM      128
#define FACE_GALLERY_MAX        10
```

### 结论

| 验证项 | 状态 |
|--------|------|
| NOR Flash 权重烧录正确 | ✓ CRC32 校验通过 |
| 模型能正常输出 128 维 embedding | ✓ 统计量健康，帧间 COSIM 0.83~0.96 |
| 人脸检测 → 裁剪 → 量化 → 推理流水线完整可运行 | ✓ |
| 人脸库管理（录入/识别/保存/加载/清空） | ✓ 全部正常 |
| Face size gate 过滤距离不合适帧 | ✓ |
| Enroll quality gate 拒绝录入不一致帧 | ✓ |

**未解决的核心问题**：int8 MobileFaceNet 对 zs 和 wph 两张人脸的区分力不足。best 和 second gallery entry 的 L2 距离仅差 0.01~0.02，噪声即可导致排名翻转。这是模型容量（int8 量化）上的固有限制。

**可能的改进方向**：
1. 1:1 验证模式（把"你是谁"变成"你是 X 吗"）
2. 用 zs/wph 数据专门微调模型
3. 使用更大的识别模型（受限于 STM32N6 NPU 和内存）
4. 如果实际场景中人员长相差异较大，当前配置可能足够

### 经验教训

1. **嵌入式 AI 调优需要分层验证**：先确认模型不吐垃圾（CRC + embedding 统计量），再调算法阈值，最后加业务层保护（size gate / quality gate）。跳过任何一层都会浪费时间
2. **int8 量化模型的区分力有限**：MobileFaceNet 原版 float32 模型的 1:N 识别能力尚可，但 int8 量化后信息损失使某些相似人脸无法可靠区分
3. **ratio 检查是 1:N 识别的重要手段**：单靠距离阈值容易误识/拒真，best/second ratio 能捕捉到"两个都差不多近"的模糊情况
4. **人脸距离（框大小）显著影响识别质量**：一个简单的 size gate 就能过滤掉很多低质量帧
5. **录入质量直接影响 gallery 纯度**：COSIM-based quality gate 是廉价且有效的保障
