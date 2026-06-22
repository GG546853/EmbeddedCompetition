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
