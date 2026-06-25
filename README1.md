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
