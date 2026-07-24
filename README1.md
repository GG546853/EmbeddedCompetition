# 模型1（BlazeFace 人脸检测）优化记录

## 模型信息

- 模型：`blazeface_front_128_quant_pc_ff_od_wider_face.tflite`
- 输入：128×128 RGB
- 输出：检测框（x_center, y_center, width, height）+ 6 个面部关键点
- 后处理：896 个 anchor（grid0: 512, grid1: 384）→ NMS → EMA 时序平滑

## PC 端测试脚本

- 路径：`STM32N6-GettingStarted-FaceDetection-PCtest/run_blazeface_pc.py`
- 新增 `FaceTracker` 类：IoU 帧间匹配 + EMA 平滑
- CLI 参数：
  - `--filter ema`：启用 EMA 滤波
  - `--alpha 0.6`：EMA 平滑系数（默认 0.6）
  - `--track-iou 0.5`：跟踪 IoU 阈值（默认 0.5）
  - `--hide-raw`：隐藏原始检测框（红色虚线），只显示平滑结果（绿色实线）
- 运行时按键：`f` 切换滤波开关，`q` 退出

---

## 一、参数优化（`app_x-cube-ai.h`）

| 参数 | 原值 | 新值 | 原因 |
|------|------|------|------|
| `AI_FD_CONF_THRESHOLD` | 0.60f | **0.75f** | 0.60 放行低质量检测，导致框和关键点不稳定。ST 官方默认 0.80 |
| `AI_FD_IOU_THRESHOLD` | 0.30f | **0.50f** | NMS 阈值过低导致重复检测。与官方一致 |
| `AI_FD_BOX_SCALE_W` | 0.80f | **1.00f** | 人为缩窄检测框，与模型训练目标不匹配，引入额外抖动 |
| `AI_FD_BOX_SCALE_H` | 1.20f | **1.00f** | 同上，人为拉高检测框 |
| `AI_FD_EMA_ALPHA` | 0.20f | **0.20f**（不变） | 用户确认保持 |
| `AI_FD_TRACK_IOU_THRESH` | 0.50f | **0.50f**（不变） | |

### 文件位置

`ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h` 第 39-46 行

---

## 二、放宽人脸尺寸门限

### 2.1 移除 FACE_SIZE_MIN/MAX 门限（`AI_task.c`）

原代码强制人脸占据画面 46%~56% 才触发识别——用户必须站在极其精确的距离。

```c
// 用 #if 0 ... #endif 包裹
#if 0
    if (result.nb_detect > 0) {
        float fs = ...;
        if (fs < FACE_SIZE_MIN || fs > FACE_SIZE_MAX) {
            result.nb_detect = 0;
        }
    }
#endif
```

宏定义同步放宽（保底值）：

```c
#define FACE_SIZE_MIN  0.25f   // 原 0.46
#define FACE_SIZE_MAX  0.75f   // 原 0.56
```

**文件位置**：`ElectronicWarehouse/Appli/APP/AI_task.c` 第 31-32 行（宏定义）、第 571-589 行（门限检查）

### 2.2 注释后处理中的最小尺寸检查（`app_x-cube-ai.c`）

```c
// if (det.width < 0.30f || det.height < 0.40f) continue;
```

**文件位置**：`ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` 第 403 行

---

## 三、修复 DCMIPP PIPE2 输入管线（`imx335.c`）

### 问题

传感器 2592×1944（4:3 比例）→ PIPE2 非均匀缩放到 1:1 的 128×128 → 人脸被压扁

### 3.1 中心裁剪

在 decimation（1/4）和 downsize 之间添加裁剪：2592×1944 → 1944×1944 正方形，水平居中（HStart=324，左右各裁 12.5%）。decimation 后变为 486×486 正方形。

```c
DCMIPP_CropConfTypeDef pipe2_crop = {0};
pipe2_crop.VStart = 0;
pipe2_crop.HStart = 324;   // (2592 - 1944) / 2
pipe2_crop.VSize  = 1944;
pipe2_crop.HSize  = 1944;
pipe2_crop.PipeArea = DCMIPP_POSITIVE_AREA;
HAL_DCMIPP_PIPE_SetCropConfig(&hdcmipp, DCMIPP_PIPE2, &pipe2_crop);
HAL_DCMIPP_PIPE_EnableCrop(&hdcmipp, DCMIPP_PIPE2);
```

### 3.2 均匀 Downsize

裁剪后 H/V 输入均为 486，downsize 参数统一：

| 参数 | 原值 | 新值 |
|------|------|------|
| HDivFactor | 202（非均匀） | **269**（与 V 一致） |
| HRatio | 41734（非均匀） | **31284**（与 V 一致） |
| VDivFactor | 269 | 269（不变） |
| VRatio | 31284 | 31284（不变） |

**文件位置**：`ElectronicWarehouse/Appli/Drivers/BSP/IMX335/imx335.c` 第 467-514 行

---

## 四、BGR→RGB 通道交换（`AI_task.c`）

### 问题

DCMIPP 输出 BGR 顺序，模型训练时使用 RGB 顺序。

### 修改

归一化循环在 `uint8 → float32` 转换的同时交换 R 和 B 通道：

```c
float *fin = (float *)buffer_in;
for (uint32_t i = 0; i < 128 * 128 * 3; i += 3) {
    fin[i + 0] = (float)nn_input_u8[i + 2] / 255.0f;  // R ← B
    fin[i + 1] = (float)nn_input_u8[i + 1] / 255.0f;  // G ← G
    fin[i + 2] = (float)nn_input_u8[i + 0] / 255.0f;  // B ← R
}
```

**文件位置**：`ElectronicWarehouse/Appli/APP/AI_task.c` 第 555-562 行

---

## 五、移除 DET_BOX_OFFSET 硬编码偏移

`DET_BOX_OFFSET_X/Y` 是对 DCMIPP 偏左问题的临时修补。修复管线后应移除：

- `AI_task.c` — 绘制函数中 `// + DET_BOX_OFFSET_X` 和 `// + DET_BOX_OFFSET_Y` 已注释
- `app_x-cube-ai.c` — 裁剪函数中同样已移除

**文件位置**：
- `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h` 第 82-83 行（宏定义保留但不再使用）
- `ElectronicWarehouse/Appli/APP/AI_task.c` 第 97-98 行（注释掉）
- `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` 第 478-479 行（注释掉）

---

## 六、PIPE2 中心裁剪的坐标映射补偿

### 问题

PIPE2 只看到传感器中间 1944×1944 正方形（列 324~2268），但代码把 NN 的 [0,1] 坐标直接映射到 800×480 全屏，没有补偿裁剪偏移。

已知 PIPE2 处理链：
- 裁剪后：传感器列 [324, 2268]，宽 1944
- 1/4 decimation → 486
- 均匀 downsize 486 → 128

显示（PIPE1）处理链：
- 全传感器列 [0, 2592]
- 直接 downsize 2592 → 800

### 修正公式

```
x_display = (x_nn * 1944 + 324) / 2592 * 800
         = x_nn * 600 + 100
width_display = width_nn * 600
```

Y 轴无裁剪，公式不变。

### 6.1 绘制函数（`AI_task.c`）

```c
#define NN2DISP_SCALE_X  600.0f
#define NN2DISP_OFFSET_X 100.0f

// 框位置和宽度
int bx = (int)(d->x_center * NN2DISP_SCALE_X + NN2DISP_OFFSET_X
               - d->width * NN2DISP_SCALE_X * 0.5f);
int bw = (int)(d->width * NN2DISP_SCALE_X);

// 关键点
int kx = (int)(d->keypoints[k][0] * NN2DISP_SCALE_X + NN2DISP_OFFSET_X);
```

**文件位置**：`ElectronicWarehouse/Appli/APP/AI_task.c` 第 94-117 行

### 6.2 人脸裁剪函数（`app_x-cube-ai.c`）

```c
float dx = det->x_center * 600.0f + 100.0f;
float dw = det->width    * 600.0f;
```

**文件位置**：`ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` 第 477-481 行

### 验证

| 人脸位置 | 修正前误差 | 修正后误差 |
|----------|-----------|-----------|
| 正中（x=0.50） | 0 px | 0 px |
| 偏左（x=0.32） | 偏左 36 px | 0 px |
| 偏右（x=0.68） | 偏右 36 px | 0 px |

---

## 七、裁剪边距增大（`app_x-cube-ai.c`）

```c
#define CROP_MARGIN  0.25f   // 原 0.10f
```

检测框稍微不准时 10% 边距不够用，容易裁掉部分人脸或包含过多背景。

**文件位置**：`ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` 第 464 行

---

## 修改文件清单

| 文件 | 改动概要 |
|------|---------|
| `Appli/X-CUBE-AI/App/app_x-cube-ai.h` | 置信度、IoU、Box Scale、边距等参数 |
| `Appli/X-CUBE-AI/App/app_x-cube-ai.c` | 最小尺寸门限移除、裁剪边距增大、坐标映射补偿、BGR swap |
| `Appli/APP/AI_task.c` | 尺寸门限移除、BGR→RGB 交换、坐标映射补偿 |
| `Appli/Drivers/BSP/IMX335/imx335.c` | DCMIPP PIPE2 中心裁剪 + 均匀 downsize |
| `STM32N6-GettingStarted-FaceDetection-PCtest/run_blazeface_pc.py` | FaceTracker 时序滤波（PC 端验证用） |
