# EmbeddedCompetition — STM32N6 AI 人脸检测

## 硬件平台

| 组件 | 型号/配置 |
|------|----------|
| MCU | STM32N647 (Cortex-M55 + NPU) |
| 开发板 | 正点原子 (ALIENTEK) N6 |
| 摄像头 | IMX335 (5MP, CSI → DCMIPP) |
| 显示屏 | RGB LCD 800×480 (LTDC + DMA2D) |
| 外部存储 | HyperRAM 32MB (XSPI1, 显存), NOR Flash (XSPI2, 模型权重) |
| 触摸 | FT5206 / GT9xxx (I2C) |
| 调试串口 | USART1 (printf 重定向) |

## 数据流

```
IMX335 (2592×1944 RAW10)
  → DCMIPP PIPE1: 缩小到 800×480 RGB565 → LTDC Layer 0 (LCD 预览)
  → DCMIPP PIPE2: 缩小到 128×128 RGB888 → NPU 输入 buffer
  → NPU 推理 (BlazeFace)
  → 后处理: 解码 + sigmoid + NMS
  → 串口输出 / LCD 叠加框
```

## 分支说明

- `main` — 初始版本
- `scheme_ai_0.3` — **当前分支**：BlazeFace int8 模型调试

## 模型调试记录

### 模型 1: YuNet (yunetn_320_qdq_int8.onnx) — 已放弃

**来源**: `stm32ai-modelzoo-4.1.0/face_detection/yunet/`

**失败原因**: ONNX QDQ (per-tensor) int8 量化在 STM32N6 NPU 上产生饱和/死神经元。

**诊断过程**:
1. 修复 BGR/RGB 通道问题 (camera RGB → model BGR)
2. 添加三 stride 诊断统计 (min/max/avg/sat127)
3. 添加 DCMIPP 时序检测 → 排除同步问题
4. 添加饱和锚点过滤 (cls==127 或 obj==127 直接跳过)
5. S16/S32 固定有 9 个和 2 个死神经元 (跨 4 轮测试不变)
6. S8 cls/obj 随场景剧烈变化 (sat127: 38~419/1600)

**结论**: ONNX QDQ int8 量化模型在 STM32N6 NPU 上不可用。

---

### 模型 2: BlazeFace (blazeface_front_128_int8.tflite) — 同样失败

**来源**: `stm32ai-modelzoo-4.1.0/face_detection/facedetect_front/`

**编译**: ST Edge AI Core v2.2.0 → NPU Neural Art Compiler

**模型结构**:
- 输入: f32(1×128×128×3), 输出: 4× f32 tensor
- 95 epochs, 26 SW (Quantize/Dequantize/Transpose), 69 HW (NPU)
- 权重 162 KB (octoFlash), 激活 528 KB (npuRAM4/5)

**输出张量**:

| 名称 | 形状 | 大小 | 含义 |
|------|------|------|------|
| Transpose_246_out_0 | (1,512,16) | 32 KB | stride 8: bbox(4) + 6关键点(12) |
| Transpose_255_out_0 | (1,512,1) | 2 KB | stride 8: 置信度 logits |
| Transpose_237_out_0 | (1,384,1) | 1.5 KB | stride 16: 置信度 logits |
| Transpose_228_out_0 | (1,384,16) | 24 KB | stride 16: bbox(4) + 6关键点(12) |

**诊断方法**:

1. **阶段 1 — 后处理实现**: 创建 float32 版后处理模块 (`fd_blazeface_pp_f32.c/h`)，含 anchor 解码 + sigmoid + NMS + 置信度过滤。发现检测结果的边界框坐标全部异常（天文数字或负数），置信度看似正常（0.91-0.94）。

2. **阶段 2 — 原始张量统计诊断**: 在 `app_x-cube-ai.c` 中添加 `_dump_tensor_stats()` 直接打印模型 4 个输出张量的 min/max/mean 统计。

**诊断结果** (参见 `ElectronicWarehouse/Serial2.txt`):

**Scores_0 (stride 8, 512 anchors) — 部分正常**:
```
min=-6.5  max=2.9  mean=1.4
→ sigmoid(2.9) ≈ 0.95, 与检测置信度吻合
```

**Scores_1 (stride 16, 384 anchors) — 全部失活**:
```
min=-308.6  max=-17.1  ← 全部为负
→ sigmoid(-17) ≈ 0, 该检测头完全无效
```

**Boxes_0 / Boxes_1 — 量化损坏（关键证据）**:
```
Boxes_1 cx: min=-1.83e38  max=9.21e37  ← 接近 float32 上限 (3.4e38)
Boxes_0 cx: min=-3.72e37  max=3.37e38
```
- 三帧数据的 min/max **完全相同**，与输入图像无关
- 数值达到 10³⁸ 量级（float32 最大值约 3.4×10³⁸）
- mean 为 NaN（浮点求和溢出）
- 16 个通道全部异常

**根本原因**: TFLite per-channel int8 量化在 STM32N6 NPU 上仍然产生饱和。Box Regression Head 的 dequantize 输出为极端值（接近 float32 max），Score Head stride 8 部分存活但 stride 16 全部失活。这与 YuNet 的 ONNX QDQ 量化失败模式一致，表明问题出在 **STM32N6 NPU 的 int8 计算精度或量化参数传递**，而非特定量化格式。

**对比**:

| 症状 | YuNet (QDQ) | BlazeFace (per-channel) |
|------|-------------|------------------------|
| 分类头部分存活 | ✓ | ✓ (stride 8 能用) |
| 回归头全部损坏 | ✓ | ✓ (16通道全异常) |
| 输出值跨帧不变 | ✓ | ✓ |
| 极端量级输出 | sat127 | ~10³⁸ (接近 FLT_MAX) |

---

## 关键文件

| 文件 | 说明 |
|------|------|
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` | AI 推理主循环 + 后处理 + 诊断代码 |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network.h` | 模型输入输出张量定义 |
| `ElectronicWarehouse/Appli/APP/fd_blazeface_pp_f32.c` | BlazeFace float32 后处理实现 |
| `ElectronicWarehouse/Appli/APP/fd_blazeface_pp_f32.h` | 后处理接口 |
| `ElectronicWarehouse/Appli/APP/fd_blazeface_anchors_0.h` | 512 anchors (stride 8) |
| `ElectronicWarehouse/Appli/APP/fd_blazeface_anchors_1.h` | 384 anchors (stride 16) |
| `ElectronicWarehouse/Appli/APP/Sensor_task.c` | 传感器/AI 任务 |
| `ElectronicWarehouse/Serial.txt` | 初步检测结果串口输出 |
| `ElectronicWarehouse/Serial2.txt` | 含张量统计的诊断串口输出 |
| `ElectronicWarehouse/network_analysis_report.txt` | ST Edge AI 模型分析报告 |
