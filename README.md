# BlazeFace 人脸检测 — STM32N6 嵌入式部署

## 项目概述

将 BlazeFace 人脸检测模型（`blazeface_front_128_quant_pc_ff_od_wider_face.tflite`）部署到 STM32N6 NPU 上，配合 IMX335 摄像头实时采集、LTDC 显示屏实时叠加检测结果。

- **推理帧率**: ~10 FPS（每帧约 100ms，含捕获+归一化+NPU推理+后处理+绘图）
- **输入分辨率**: 128×128×3
- **模型编译**: STM32CubeMX X-CUBE-AI，`--native-float = true`
- **最大检测数**: 3 张人脸
- **6 个关键点**: 左眼、右眼、鼻尖、嘴角、左耳、右耳

## 硬件环境

| 组件 | 型号 |
|------|------|
| MCU | STM32N6 (NUCLEO-N657X0-Q) |
| NPU | Neural-ART 加速器 |
| 摄像头 | IMX335 (MIPI CSI) |
| 显示屏 | 800×480 RGB LCD (LTDC) |
| 存储器 | XSPI NOR Flash + AXISRAM |

## 软件环境

- **RTOS**: FreeRTOS (CMSIS-RTOS v2)
- **编译器**: STM32CubeIDE / GCC ARM
- **模型工具链**: STM32CubeMX X-CUBE-AI + ST Edge AI
- **HAL 驱动**: STM32N6 HAL (DCMIPP, LTDC, DMA2D, NPU)

## 架构与数据流

```
IMX335 摄像头
    │
    ├── PIPE1: 800×480 YUV → LTDC Layer 0 (预览)
    │
    └── PIPE2: 128×128 RGB888 → nn_input_u8[49KB]
                                      │
                              uint8→float32 /255.0
                                      │
                              buffer_in[196KB] float32
                                      │
                              NPU 推理 (Neural-ART)
                                      │
                        4 路 float32 输出张量
                                      │
                         BlazeFace 后处理 (CPU)
                         - 输出排序 & 锚点匹配
                         - 边框/关键点解码
                         - NMS 去重
                                      │
                    ┌─────────────────┴─────────────────┐
                    │                                   │
           串口 printf 输出                LTDC Layer 2 叠加绘制
```

## 关键文件

| 文件 | 说明 |
|------|------|
| `ElectronicWarehouse/Appli/APP/AI_task.c` | AI 推理 FreeRTOS 任务 |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` | NPU 推理入口 + 后处理 |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h` | 检测结构体 & 阈值定义 |
| `ElectronicWarehouse/Appli/APP/fd_blazeface_anchors.h` | 896 个锚点坐标 (Grid 0: 512, Grid 1: 384) |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_f.h` | 模型 I/O 规格 |
| `ElectronicWarehouse/Appli/Core/Src/main.c` | NPU RIF 授权 (`SystemIsolation_Config`) |

## 部署步骤

### 1. CubeMX 配置

- 用 X-CUBE-AI 导入 `blazeface_front_128_quant_pc_ff_od_wider_face.tflite`
- 编译选项: `--native-float = true`, `--optimization = 3`
- 输入保持 float32 格式 (196,608 bytes = 128×128×3×4)

### 2. DCMIPP PIPE2 配置

在 `imx335.c` 的 `imx335_dcmipp_init()` 中：
- PIPE2: 128×128 RGB888 输出
- Decimation: 1/4 + downsize
- PixelPacker: RGB888_YUV444_1
- Pitch: 128 × 3 = 384 bytes

### 3. 添加锚点文件

从 STM32N6 官方例程复制锚点常量：
- `g_Anchors_0[1024]` — Grid 0: 512 个锚点 × 2 坐标
- `g_Anchors_1[768]` — Grid 1: 384 个锚点 × 2 坐标

### 4. 实现后处理

在 `app_x-cube-ai.c` 中：

1. 按字节大小排序 4 个输出张量，区分 Score 和 Box
2. 按元素数量匹配 Score ↔ Box（`scnt == bcnt`）
3. 反量化（当前使用 float32 输出，无需反量化）
4. 解码：`x_center = boxes[0] / 128 + anchor_x`, `width = boxes[2] / 128`
5. Sigmoid 置信度
6. NMS with configurable IoU threshold

### 5. 创建 AI 推理任务

FreeRTOS 任务 `AITask`:
- 等待摄像头就绪 (1s)
- 循环: 捕获帧 → 归一化 → NPU 推理 → 后处理 → 绘制显示

### 6. NPU RIF 授权

**关键步骤**: 在 `main.c` 的 `SystemIsolation_Config()` 中添加 NPU RIF 授权，将 NPU 配置为安全特权主设备。缺少此步骤会导致 NPU 输出完全错误。

## 调试记录

### Bug 1: DCMIPP PIPE2 用错 API (导致相机输入全黑)

**现象**: `nn_input_u8` 全为 0，模型输入全黑图，检测结果始终为空。

**原因**: 代码中调用了 `HAL_DCMIPP_PIPE_Start`（并行接口 API），但 IMX335 通过 MIPI CSI 连接，`imx335_dcmipp_init()` 已将 DCMIPP 配置为 `SERIAL_MODE`。函数内部模式检查失败，返回 `HAL_ERROR`，PIPE2 从未启动。

**修复**: 改用 `HAL_DCMIPP_CSI_PIPE_Start` / `HAL_DCMIPP_CSI_PIPE_Stop`。

### Bug 2: DMA 缓冲区在 DTCM 中无法访问

**现象**: DCMIPP DMA 无法写入 `nn_input_u8`。

**原因**: 变量声明在 `.bss` 段（DTCM RAM），DCMIPP DMA 无法访问此区域。

**修复**: 添加 `__attribute__((section(".noncacheable"), aligned(32)))` 将 buffer 放入 `.noncacheable` 段。

### Bug 3: DataType 检测错误 (检测结果始终为空)

**现象**: 后处理解码出异常值，检测结果始终为空。

**原因**: 代码检查 `s_info->type == DataType_FLOAT16`，但实际模型输出是 `DataType_FLOAT32` (enum 值 = 1)，`DataType_FLOAT16` = 14。条件永远为 false，float32 输出被当作 int8 进行反量化，数值完全错误。

**修复**: 改为 `s_info->type == DataType_FLOAT32`。

### Bug 4: 锚点数组选择错误

**现象**: 后处理访问锚点数组越界，部分检测结果异常。

**原因**: 输出张量按字节大小升序排序后，`idx[0]` 是最小 score = Grid 1 (384 元素)，但代码用 `grid == 0` 固定取 `g_Anchors_0` (512 个)。反过来也一样。

**修复**: 改为按实际锚点数选择：`(nb == 512) ? g_Anchors_0 : g_Anchors_1`。

### Bug 5: NPU RIF 授权缺失 (根因 — 导致 NPU 输出值缩小约 100 倍)

**现象**: 相机正常、DCMIPP 正常、cache 正常，但 NPU 输出的 raw box 值仅为 0.2~0.5（正常应在 10~50），导致解码后的边框坐标完全错误。

**原因**: STM32N6 的 RIF (Resource Isolation Framework) 默认禁止 NPU 作为安全主设备访问资源。需要在 `SystemIsolation_Config()` 中显式授权 NPU。

**修复**: 在 `main.c` 的 `SystemIsolation_Config()` 中添加 NPU RIF 配置：
```c
// RIFSC: Authorize NPU as secure privileged master
```

**这是整个调试过程中最关键、最难排查的问题。** NPU 可以正常推理（无异常、无超时），只是输出的数值全部是错误的。直到在 `app_x-cube-ai.c` 中添加了输出张量的 scale/offset debug 打印，发现所有输出都是 float32（scale=NULL, offset=NULL），排除了反量化 bug，才将怀疑转向硬件层面的问题。

### Bug 6: 显示屏坐标方向错误

**现象**: 检测框在屏幕上位置不对，出现不完整的框。

**原因**: 绘制代码用了 `DISP_W=480, DISP_H=800`，但显示屏是 800×480 横屏（`rgblcd_display_dir(1)`）。

**修复**: 改为 `DISP_W=800, DISP_H=480`。

### Bug 7: NMS 无法抑制嵌套框

**现象**: 检测准确，但同一张人脸显示一个大框套一个小框。

**原因**: Grid 0 (16×16) 和 Grid 1 (8×8) 对同一张人脸各自检出，两框 IoU 刚好低于 0.5 阈值，都存活了下来。

**修复**: 将 `AI_FD_IOU_THRESHOLD` 从 `0.5f` 降到 `0.3f`。

## 当前参数

| 参数 | 值 |
|------|-----|
| AI_FD_MAX_DETECTIONS | 3 |
| AI_FD_NUM_KEYPOINTS | 6 |
| AI_FD_IMG_SIZE | 128 |
| AI_FD_CONF_THRESHOLD | 0.55f |
| AI_FD_IOU_THRESHOLD | 0.3f |

## 遗留问题

### 嵌入式精度低于 PC 端

**现象**: 嵌入式端检测置信度在 0.50~0.59 附近，对环境光要求较高。光照稍微不足或角度偏转，检测就容易失败。PC 端在相同场景下表现更稳定。

**可能原因分析**:

1. **IMX335 摄像头的图像质量与 PC 摄像头不同** — 镜头、传感器动态范围、自动曝光/白平衡算法差异导致输入图像分布与训练数据不完全匹配。

2. **128×128 降采样损失** — DCMIPP PIPE2 通过 1/4 decimation + downsize 将原始图像缩小到 128×128，降采样过程中的信息损失可能比 PC 端的图像预处理（通常用更高质量的 resize）更严重。

3. **模型量化误差** — 虽然当前使用 float32 I/O（`--native-float = true`），但模型内部可能仍存在量化层。需要确认 CubeMX 编译时是否对权重/激活值施加了量化。

4. **归一化差异** — 当前使用 `/255.0f` 将 uint8 转为 float32 [0,1]。训练时的归一化参数可能不同（如 BGR→RGB 通道顺序、均值/标准差归一化等），需要与 PC 端训练/推理脚本对齐。

**建议排查方向**:
- 从嵌入式端导出 `nn_input_u8` 原始数据到 PC，用同一段 Python 推理代码对比结果
- 检查模型训练时的输入预处理 pipeline，确认是否需要 BGR→RGB 转换
- 降低 `AI_FD_CONF_THRESHOLD` 到 `0.4f` 或 `0.35f` 作为临时解决方案
- 考虑用测试模式跳过相机、直接喂入 PC 端准备的测试图像，隔离问题源头

### 实时性

当前每帧约 100ms (~10 FPS)，大部分时间消耗在：
- uint8→float32 软件归一化 (128×128×3 = 49K 次循环)
- NPU 推理
- 后处理（候选框解码 + NMS）

如果需要提高帧率，可以考虑：
- 将 uint8→float32 归一化改为 DMA2D 加速
- 重新编译模型为 int8 I/O 以节省归一化开销（需 CubeMX 重新配置）
