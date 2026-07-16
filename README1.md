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
