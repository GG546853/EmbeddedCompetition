# 多屏切换 + 摄像头 Layer 叠加调试记录

## 目标

实现 LVGL 多屏导航：
- Main 屏幕点击按钮 → 切换到 Camera 屏幕 + 启动 Sensor_Task 和 AI_Task
- Camera 屏幕两个按钮：Return（返回 Main + 停任务）、Start（触发人脸识别）
- 摄像头画面在 LTDC Layer 0，LVGL UI 在 Layer 1，通过 Color Keying 使 Layer 1 黑色像素透明，露出 Layer 0

---

## 问题 1：点击按钮创建 AI 任务后程序卡死

### 现象

点击 Main 屏幕中间按钮 → 程序完全卡死，屏幕不切换，LED 不再闪烁。

### 排查过程

1. **检查任务创建** — `osThreadNew` 会立即触发 FreeRTOS 调度
2. **检查优先级** — 发现 `Sensor_task.c` 中任务优先级为 `osPriorityNormal + 1`，高于 LV_Task 的 `osPriorityNormal`
3. **检查 Sensor_Task 入口** — 发现 `while(imx335_init()) { __NOP(); }` 死循环无任何阻塞调用

### 根因

```
LV_Task (Normal) → 点击按钮 → action_start_camera → osThreadNew(Sensor_Task)
→ FreeRTOS 立即切换到 Sensor_Task (Normal+1，优先级更高)
→ imx335_init() 失败返回非 0 → while 循环 spin，永不让出 CPU
→ LV_Task 永远得不到运行 → `eez_flow_push_screen` 根本没执行
```

两个因素叠加：**高优先级 + 无 yield 的死循环 = 系统卡死**。

### 修复

`Sensor_task.c` 第 17-21 行：

```c
// 修复前
while (imx335_init()) { __NOP(); }

// 修复后
while (imx335_init()) { vTaskDelay(pdMS_TO_TICKS(100)); }
```

同时降优先级到 `osPriorityNormal`（与 LV_Task 同级），避免不必要抢占。

---

## 问题 2：切到 Camera 屏幕后看不到摄像头画面（白屏/黑屏）

### 现象

屏幕切换成功，但只看到 Camera UI 的按钮，背景不是摄像头画面，而是白屏或黑屏。

### 排查过程（逐层剥离）

#### 第 1 步：确认 Layer 0 硬件是否正常

**测试**：禁用 Layer 1（窗口缩到 1×1），用 `rgblcd_clear(RED)` 填充 Layer 0 为红色。

→ **结果**：全屏红色 ✓ — Layer 0 硬件、帧缓冲、LTDC 配置均正常。

#### 第 2 步：确认 LTDC Blending 硬件是否正常

**测试**：恢复 Layer 1 窗口，设全局 `Alpha = 0`（全部透明，不依赖 per-pixel alpha）。

→ **结果**：全屏红色 ✓ — Blending 硬件正常。

#### 第 3 步：确认 per-pixel Alpha（Backcolor 匹配）是否正常

**测试**：设 `Alpha0 = 0`（黑色像素 → 透明），`memset` Layer 1 帧缓冲为全黑。

→ **结果**：黑色不透明 ✗ — Backcolor 匹配机制没生效。

#### 第 4 步：检查 HAL 层 LTDC 配置

追踪 `HAL_LTDC_ConfigLayer` 源码，发现关键逻辑：

```c
// STM32 HAL 内部逻辑
if (Backcolor.Red != 0 || Backcolor.Green != 0 || Backcolor.Blue != 0) {
    // Backcolor 非零 → 开 COLKEN + 写 CKCR
    LTDC_LAYER->CR |= LTDC_LxCR_COLKEN;
    LTDC_LAYER->CKCR = Backcolor;
} else {
    // Backcolor 全零 → 关 COLKEN !!!
    LTDC_LAYER->CR &= ~LTDC_LxCR_COLKEN;
}
```

### 根因

**STM32 HAL 的 Backcolor 处理逻辑与 RGB565 per-pixel alpha 机制存在冲突**：

- RGB565 格式没有原生 alpha 通道，LTDC 通过比较像素色与 Backcolor 来决定每个像素的 alpha（匹配 → Alpha0，不匹配 → Alpha）
- 此比较机制依赖 **COLKEN**（Color Keying Enable）位和 **CKCR**（Color Key Configuration Register）
- 但当 Backcolor 为黑色 `(0,0,0)` 时，HAL 认为"不需要 Color Keying"而**关闭 COLKEN**
- 结果：Blackcolor 比较机制失效，RGB565 所有像素都使用 Alpha（255 = 不透明）

这是一个 HAL 设计缺陷：将两个独立功能（Color Keying 穿透 vs Backcolor 匹配 alpha）绑在同一个硬件位上，且用一个不准确的判断条件（Backcolor 是否为 0）来开关。

### 修复

`actions.c` 中，`HAL_LTDC_ConfigLayer` 之后手动开启 COLKEN：

```c
// 进入 Camera 模式
HAL_LTDC_ConfigColorKeying(&hltdc, 0, 1);  // Color Key = 黑色
HAL_LTDC_EnableColorKeying(&hltdc, 1);     // 强制开 COLKEN
HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

// 退出 Camera 模式
HAL_LTDC_DisableColorKeying(&hltdc, 1);    // Main 屏幕必须关，否则黑色 UI 元素会消失
```

---

## 最终架构

```
┌─────────────────────────────────────┐
│  LTDC Layer 1 (LVGL UI, RGB565)    │
│  ┌──────┐ ┌──────────┐             │
│  │Return│ │  Start   │  ← 按钮有色   │
│  └──────┘ └──────────┘             │
│  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │
│  ░░░ 黑色背景 ░░░░░░░░░░░░░░░░░░░░  │ ← Color Key = Black → 穿透
│  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │
├─────────────────────────────────────┤
│  LTDC Layer 0 (摄像头, RGB888)     │
│  实时摄像头画面                       │
└─────────────────────────────────────┘
```

- **Main 屏幕**：COLKEN 关，Layer 1 全不透明，正常 UI 显示
- **Camera 屏幕**：COLKEN 开，黑色背景穿透 → Layer 0 摄像头画面可见，按钮浮在画面上方

---

## 涉及文件

| 文件 | 修改内容 |
|------|---------|
| `Appli/APP/ui/actions.c` | 新建。三个 action 函数，管理 COLKEN 开关、任务生命周期、屏幕切换 |
| `Appli/APP/ui/screens.c` | Camera 屏幕背景设黑色+不透明，"Return" 标签文字改白色 |
| `Appli/APP/Sensor_task.c` | `while(imx335_init())` 加 `vTaskDelay(100)`；优先级降为 Normal |
| `Appli/Core/Src/app_freertos.c` | Sensor_Task / AI_Task 改为动态创建（不在初始化时创建） |

## 待解决

- [ ] DMA2D 互斥锁：LVGL flush 与 AI_Task `fill_rect()` 共用 DMA2D，需加 mutex 保护
- [ ] `action_recognize`：需要 `g_trigger_identify` 全局变量对接 AI_task 的人脸识别流程
- [ ] `action_stop_camera` 中 `rgblcd_clear(BLUE)` 与 LVGL flush 可能抢 DMA2D

---

# UI 移植记录：test_E → uip1（2026-07-01）

## 概述

将 `C:\Users\96022\Desktop\EEZ_PC_test\test_E` 生成的 UI 替换为 `C:\Users\96022\Desktop\uip1` 的仓库管理 UI（"Ewhouse"）。

| 项目 | 旧 UI (test_E) | 新 UI (uip1) |
|---|---|---|
| 屏幕数 | 2 (Main, Camera) | 3 (PAGE_LOCK, PAGE_MAIN, PAGE_ERROR) |
| Widget 数 | 6 | 90 |
| 图片资源 | 1 (cat, 3.8MB) | 8 (~2.9MB) |
| 字体 | Montserrat 14 only | 5 个中文字体 (~24MB) + Montserrat 8~48 |
| Actions | 3 (camera 相关) | 0 |
| Assets blob | 1036 bytes | 21180 bytes |
| Flow 变量 | 无 | 17 个 |
| 数据结构 | 无 | Cabinet, LogEntry |

---

## 操作步骤

### 1. 替换 UI 源文件

将 `C:\Users\96022\Desktop\uip1\src\ui\` 下全部 27 个文件复制到 `ElectronicWarehouse/Appli/APP/ui/`，覆盖旧的 15 个文件。删除旧文件 `ui_image_cat.c`。

新增文件：

| 类型 | 文件 | 大小 |
|---|---|---|
| 中文字体 | `ui_font_chinese.c` | 4.5 MB |
| | `ui_font_chinese24.c` | 12 MB |
| | `ui_font_chinese18.c` | 4.5 MB |
| | `ui_font_chinese10.c` | 2.7 MB |
| | `ui_font_chinese8.c` | 41 KB |
| UI 图片 | `ui_image_p_iamge.c` | 2.4 MB |
| | `ui_image_page_wms.c` | 191 KB |
| | `ui_image_page_dashboard.c` | 20 KB |
| | `ui_image_page_inventory.c` | 19 KB |
| | `ui_image_page_activity.c` | 17 KB |
| | `ui_image_page_uimode.c` | 121 KB |
| | `ui_image_page_search.c` | 13 KB |
| | `ui_image_page_printfer.c` | 14 KB |

### 2. 更新构建系统

修改 `Debug/APP/ui/subdir.mk` 和 `Release/APP/ui/subdir.mk`，将 `C_SRCS`、`C_DEPS`、`OBJS`、`clean` 段从旧的 6 个 .c 文件更新为 18 个 .c 文件。

### 3. 重写 actions.c

uip1 无 native C action 回调，将其改为最小桩文件，但保留 `dma2d_mutex` 定义（被 `lv_st_ltdc.c` 和 `AI_task.c` 通过 extern 引用）：

```c
#include "actions.h"
#include "eez-flow.h"
#include "cmsis_os.h"

osMutexId_t dma2d_mutex;
const osMutexAttr_t dma2d_mutex_attr = { .name = "dma2d_mutex" };
```

---

## 遇到的错误与修复

### 错误 1：`unsigned conversion from 'int' to 'unsigned int:20' changes value`

**现象**：编译中文字体文件时大量 `-Woverflow` 警告。

```
../APP/ui/ui_font_chinese24.c:257583:22: warning:
unsigned conversion from 'int' to 'unsigned int:20' changes value from '1969231' to '920655' [-Woverflow]
```

**原因**：LVGL v9.4 的 `lv_font_glyph_dsc_t` 结构体中 `bitmap_index` 是 20 位位域，最大值 1,048,575。中文字体 bitmap 数据超过 1MB，偏移量超出 20 位范围，值被截断后字形渲染会错位。

**修复**：`Core/Inc/lv_conf.h` 第 655 行
```c
#define LV_FONT_FMT_TXT_LARGE 1   // 0 → 1
```
效果：`bitmap_index` 从 20 位位域变为完整 `uint32_t`，支持最大 4GB 字体 bitmap。

---

### 错误 2：`'lv_font_montserrat_XX' undeclared`

**现象**：`screens.c` 引用的 `lv_font_montserrat_8` ~ `_48` 全部报未声明。

**原因**：旧 UI 仅启用了 `lv_font_montserrat_14`，uip1 UI 的 screens.c 在第 3318–3378 行有字体查找表，直接引用了全部 21 个 Montserrat 字号。

**修复**：`Core/Inc/lv_conf.h` 第 606–626 行，将 Montserrat 8~48 全部从 `0` 改为 `1`。

---

### 错误 3：`undefined reference to 'dma2d_mutex'`

**现象**：链接时报错

```
lv_st_ltdc.c:(.text.flush_cb+0x1bc): undefined reference to `dma2d_mutex'
```

**原因**：`dma2d_mutex` 原来定义在旧的 `actions.c` 中，替换为桩文件后丢失。`lv_st_ltdc.c` 和 `AI_task.c` 都通过 `extern` 引用它来保护 DMA2D 的并发访问。

**修复**：在 `APP/ui/actions.c` 中加回定义：
```c
#include "cmsis_os.h"
osMutexId_t dma2d_mutex;
const osMutexAttr_t dma2d_mutex_attr = { .name = "dma2d_mutex" };
```

---

### 错误 4：启动后屏幕蓝屏转黑屏，LVGL 任务死掉（Hard Fault）

**现象**：
- 上电后屏幕闪蓝（Layer 0 背景色）→ 迅速变黑（Layer 1 帧缓冲被 memset 清零）
- LVGL 任务中加的 `HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_10)` 只反转一次即停止
- 结论：任务在 `ui_init()` 或第一次 `lv_timer_handler()` 中崩溃

**根因**：三个内存资源均不足，层层叠加：

| 资源 | 位置 | 旧值 | 为什么不够 |
|---|---|---|---|
| LVGL 任务栈 | `LV_task.c` | 32KB | `create_screen_page_main()` ~3000 行，一口气创建 70+ widget，嵌套调用栈极深 |
| FreeRTOS 堆 | `FreeRTOSConfig.h` | 40KB | LVGL 栈要 96KB 加上其他任务栈，远超出 40KB |
| 系统堆 | 链接脚本 `.ld` | 2KB | `eez-flow.cpp` 用 C++ `new` 解析 21KB assets blob + 创建 90 个 widget 元数据，2KB 秒炸 |
| LVGL 内存池 | `lv_conf.h` | 128KB | 90 个 widget × 几百字节/个，128KB 非常极限 |

**修复**：

① `APP/LV_task.c` — 任务栈
```c
.stack_size = 4096 * 24   // 32KB → 96KB
```

② `Core/Inc/FreeRTOSConfig.h` — FreeRTOS 堆
```c
#define configTOTAL_HEAP_SIZE    ((size_t)131072)   // 40KB → 128KB
```

③ Release 链接脚本 `STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld` — 系统堆
```c
_Min_Heap_Size = 0x10000;   // 0x800 (2KB) → 0x10000 (64KB)
```
（Debug 链接脚本也需同样修改）

④ `Core/Inc/lv_conf.h` — LVGL 内存池
```c
#define LV_MEM_SIZE (256 * 1024U)   // 128KB → 256KB
```

---

## 改动的文件清单

| 文件 | 改动内容 |
|---|---|
| `APP/ui/` (全部文件) | 替换为 uip1 版本，删除 `ui_image_cat.c` |
| `APP/ui/actions.c` | 重写为桩文件，保留 `dma2d_mutex` |
| `Debug/APP/ui/subdir.mk` | C_SRCS/C_DEPS/OBJS/clean 源文件列表更新 |
| `Release/APP/ui/subdir.mk` | 同上 |
| `Core/Inc/lv_conf.h` | `LV_FONT_FMT_TXT_LARGE=1`、Montserrat 8~48 全启用、`LV_MEM_SIZE=256KB` |
| `Core/Inc/FreeRTOSConfig.h` | `configTOTAL_HEAP_SIZE=128KB` |
| `APP/LV_task.c` | `stack_size` 增大到 `4096*24` |
| `STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld` | `_Min_Heap_Size=64KB` |

---

# 注册人脸时屏幕雪花问题（2026-07-07）⚠️ 未解决

## 现象

串口输入 `register zs` 后，屏幕覆盖一层雪花（随机彩色噪点），但雪花后方能看到人影晃动和人脸检测框移动。雪花只在人脸注册（`network_fc` 推理）期间出现，推理完成后恢复。

## 根因分析

`network_fc`（人脸识别模型）NPU 推理时将中间激活数据写入 HyperRAM，起始地址 0x90000000，实际写入量约 1.531 MB（止于 0x90188000）。LCD 显存 `g_ltdc_framebuf`（800×480×3 = 1.15 MB）原本也在 0x90000000，被 NPU 完整覆盖。

```
NPU fc 激活区:  0x90000000 ─────────── 0x90188000 (1.53 MB)
g_ltdc_framebuf: 0x90000000 ───── 0x90119400 (1.15 MB)  ← 被覆盖
crop_temp_buf:          0x90119400 ───── 0x901C2000 (0.69 MB)
```

LTDC 持续从 0x90000000 读取数据显示，NPU 写入的激活数据被 LTDC 当作像素数据显示，呈现为雪花。雪花中能看到人影是因为 DCMIPP PIPE1 持续向同一地址写入新帧，局部覆盖了被破坏的区域。

### 为什么不改 AI 模型

`network_fc` 的 NPU 内存池定义在 `Model/my_mpools/stm32n6_net_fc.mpool`，HyperRAM offset = 0x90000000。重新生成模型需要 NPU 编译器，且会影响模型推理性能。选择改 linker 布局来避开冲突。

## 尝试的修复

### 修改 1：rgblcd.c — 显存换段

`Drivers/BSP/RGBLCD/rgblcd.c:37`：
```c
// 改前
uint8_t g_ltdc_framebuf[480 * 800 * 3] __attribute__((section(".EXTRAM"), aligned(32)));
// 改后
uint8_t g_ltdc_framebuf[480 * 800 * 3] __attribute__((section(".ltdc_fb"), aligned(32)));
```

### 修改 2：linker script — 独立显存区域

`STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld`：

MEMORY 拆分：
```c
// 改前
EXTRAM (rw) : ORIGIN = 0x90000000, LENGTH = 32M

// 改后
EXTRAM    (rw) : ORIGIN = 0x90000000, LENGTH = 2M    // NPU activations + crop buf
EXTRAM_FB (rw) : ORIGIN = 0x90200000, LENGTH = 30M   // LCD framebuffer
```

SECTIONS 增加：
```c
.ltdc_fb (NOLOAD):
{
  . = ALIGN(32);
  *(.ltdc_fb)
  . = ALIGN(32);
} >EXTRAM_FB
```

### 修改 3：imx335.c/h — 固定白平衡函数

`Drivers/BSP/IMX335/imx335.c` 新增 `imx335_set_wb_mode()`：
- 传 0 → AWB 自动模式
- 传 2856/4000/5000/6500 → 固定色温（A / TL84 / D50 / D65）

`Drivers/BSP/IMX335/imx335.h` 添加声明。

### 修改 4：Sensor_task.c — 固定 D50 色温

在 `imx335_init()` 成功后、`imx335_io_deinit()` 前调用 `imx335_set_wb_mode(5000)`，解决 ISP AWB 算法收敛锁死导致的色调不响应问题。

## 编译结果

Release 配置编译通过，map 确认布局正确：

```
.ltdc_fb  0x90200000  0x119400  ./Drivers/BSP/RGBLCD/rgblcd.o
          0x90200000            g_ltdc_framebuf
```

Debug 配置的 `rgblcd.o` 未重新编译（map 仍显示旧布局），需 `make clean` 后重编。

## 测试结果

Release 烧录后，注册人脸时**雪花依旧存在**。显存已确认移到 0x90200000，但问题未消失，说明还有**其他来源**在破坏显存。

## 待排查方向

- [ ] NPU fc 推理是否实际写入范围超出 `fc_analyze.md` 报告的 0x90188000
- [ ] DCMIPP PIPE1 与 DMA2D crop 在同一 HyperRAM 总线上竞争带宽，导致 LTDC 读不到完整帧
- [ ] NPU cache writeback 是否波及 0x90200000 区间
- [ ] `network_f`（人脸检测）运行时是否有未记录的 HyperRAM 写入
- [ ] 尝试将 `.ltdc_fb` 放到更远地址（如 0x90800000）排除 NPU 范围不确定性
