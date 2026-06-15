# AI 部分总结：双模型人脸检测 + 识别流水线

## 使用模型

项目使用两个 ONNX 模型，均通过 STM32 Neural-ART (X-CUBE-AI) 编译为 NPU 可执行代码。

### 模型一：BlazeFace 人脸检测（network_f）

| 项目 | 值 |
|------|-----|
| 原始模型 | `blazeface_front_128_quant_pc_ff_od_wider_face` |
| 输入 | float32, 1×3×128×128, [0,1] 归一化 |
| 输出 | 4 个 tensor：2 组 score (384+512) + 2 组 box (每锚点 16 值) |
| 用途 | 检测画面中人脸位置、置信度、6 个关键点（眼/鼻/嘴/耳） |
| NPU 内存 | 196608 bytes (128×128×3 float32) |

### 模型二：MobileFaceNet 人脸识别（network_fc）

| 项目 | 值 |
|------|-----|
| 原始模型 | `mobilefacenet_int8_faces` |
| PC 端测试工程 | `C:\Users\96022\Desktop\FACE_INT8\face_recog.py` |
| 量化脚本 | `C:\Users\96022\Desktop\FACE_INT8\quantize.py` |
| 输入 | int8, 1×3×112×112, scale=0.007843138, zp=0 |
| 输出 | float32, 1×128（L2 归一化 embedding 向量） |
| 用途 | 提取人脸 128 维特征向量用于注册/识别 |
| NPU 内存 | 37632 bytes (112×112×3 int8) |

### PC 端测试工程

路径：`C:\Users\96022\Desktop\FACE_INT8`

关键文件：
- `face_recog.py` — 摄像头实时人脸录入与识别，YuNet 检测 + MobileFaceNet 识别
- `quantize.py` — ONNX Runtime 静态量化脚本（ss/sa QDQ INT8）
- `evaluate.py` — 模型精度评估
- `mobilefacenet_int8_faces.onnx` — 部署到 STM32N6 的最终 INT8 模型
- `blazeface_front_128_quant_pc_ff_od_wider_face` — 检测模型（ST Edge AI Model Zoo）

---

## 整体思路：双模型两阶段流水线

```
摄像头 → DCMIPP 双管道 → NPU 检测 → CPU 裁剪 → NPU 识别 → 结果输出
```

### 第一阶段：人脸检测（每帧运行）

1. IMX335 传感器输出 2592×1944 RAW10，经 MIPI CSI-2 (2-lane, 1200Mbps) 进入 DCMIPP
2. DCMIPP PIPE2 提取 AI 用的小图：
   - 1/4 水平+垂直抽取 → 648×486
   - ISP Bayer→RGB
   - Downsize → 128×128 RGB888
   - DMA 写入 `nn_input_u8`（内部 SRAM, noncacheable）
3. CPU 做 uint8→float32 归一化（`/255.0`）写入 `buffer_in`
4. NPU 运行 network_f 推理
5. 后处理：解码两组 grid 的 score+box → sigmoid → NMS → EMA 时序平滑

### 第二阶段：人脸识别（按需触发）

仅在收到串口命令时运行，不浪费 NPU 算力：

1. DMA2D 从 800×480 显示帧缓冲中裁剪人脸区域 → `crop_temp_buf` (HyperRAM)
2. CPU 双线性缩放到 112×112 → `FACE_CROP_BUF`（复用 `nn_input_u8`）
3. uint8→int8 量化：`s8 = u8 - 128`（匹配 MobileFaceNet [-1,1] 归一化）
4. NPU 运行 network_fc 推理 → 128 维 float32 embedding
5. L2 归一化 → 与 gallery 中已注册 embedding 比对（L2 距离 + ratio 校验）

---

## 数据流全景

```
IMX335 Sensor (2592×1944, RAW10 RGGB)
│  MIPI CSI-2: 2 lanes, 1200 Mbps PHY
│
└─ DCMIPP ─┬─ PIPE1 (显示) ─────────────────────────────────────┐
           │  ISP: Bayer→RGB, AE/AWB                            │
           │  Downsize: 2592×1944 → 800×480                     │
           │  PixelPacker: RGB888                                │
           │  DMA → g_ltdc_framebuf (HyperRAM 0x90000000)       │
           │                                                     ▼
           │                                      LTDC 读出 → LCD (33.33MHz)
           │                                      单层 RGB888, 800×480
           │
           └─ PIPE2 (AI) ───────────────────────────────────────┐
              ISP: Bayer→RGB                                    │
              Decimate: 1/4 H+V → 648×486                       │
              Downsize: 648×486 → 128×128                       │
              PixelPacker: RGB888                                │
              DMA → nn_input_u8 (内部 SRAM, noncacheable)       │
                                                     ▼
                                     AI_Task (FreeRTOS, 每帧):
                                       1. uint8→float32 /255.0 → buffer_in
                                       2. NPU: network_f 推理
                                       3. 后处理: decode+NMS+EMA → ai_result_t
                                       4. DMA2D 画框到 g_ltdc_framebuf
                                       5. 串口命令解析
                                       6. ReID pipeline（按需触发）
```

### ReID Pipeline 详细流程（仅串口触发时运行）

```
g_ltdc_framebuf (800×480 RGB888)
  │
  ├─ DMA2D M2M_PFC 裁剪人脸区域 → crop_temp_buf (HyperRAM)
  │   源: 800×480 RGB888 stride, 裁剪区域由检测框+margin决定
  │
  ├─ CPU 双线性插值 缩放 → 112×112 RGB888 (FACE_CROP_BUF)
  │
  ├─ quantize_u8_to_s8: u8 - 128 → int8 [-128,127]
  │   → buffer_in_fc (NPU RAM)
  │
  ├─ NPU: network_fc 推理
  │   输出: 128 维 float32 embedding
  │   反量化: float = (int8 - zp) * scale
  │
  └─ 注册或识别:
       ├─ register: 多采 5 次 L2-归一化后求均值 → 写入 gallery
       └─ identify: L2 距离 → ratio check → 返回最佳匹配
```

### HyperRAM 带宽分配（@ 92.31 MHz ≈ 185 MB/s 峰值）

| 行为 | 来源 | 数据量 | 带宽 |
|------|------|--------|------|
| 写入 | DCMIPP PIPE1 DMA | 800×480×3 = 1.15 MB/帧 | ~35 MB/s (@30fps) |
| 读出 | LTDC 刷新屏幕 | 800×480×3 × 60Hz | ~76 MB/s |
| 写入 | CPU 画检测框 | 几条线/几个点 | 可忽略 |
| 读写 | ReID crop (按需) | 约 250×250×3 | 瞬时 |

---

## 串口命令使用说明

嵌入式端通过串口（USART1）交互，命令以 `\r\n` 结尾：

| 命令 | 功能 |
|------|------|
| `register <name>` | 开始录入人脸，自动采集第 1 次，之后每次发 `s` 采集 1 次 |
| `s` | 在录入模式下采集一次当前人脸（换个角度） |
| `identify` | 识别当前人脸，显示匹配结果 |
| `gallery` | 列出已注册人脸 |
| `help` | 显示帮助信息 |

### 录入流程示例

```
[CMD] register zkw
[REID] Enrolling 'zkw' — send 's' for next sample
[REID] Enroll sample 1/5 for 'zkw'       ← 自动第 1 次

[CMD] s                                   ← 换个角度
[REID] Enroll sample 2/5 for 'zkw'

... 重复 3 次 ...

[CMD] s
[REID] Enroll sample 5/5 for 'zkw'
[REID] Enrolled 'zkw'                     ← 自动完成，均值存入 gallery
```

### 识别流程示例

```
[CMD] identify
[REID] Matched: 'zkw' (dist=0.394)
```

### 识别失败示例

```
[CMD] identify
[REID] No match (dist=1.234)              ← 无匹配或距离过大
```

---

## 关键参数

定义在 `app_x-cube-ai.h`：

```c
#define FACE_EMBEDDING_DIM     128   // 人脸特征向量维度
#define FACE_GALLERY_MAX        10   // 最多注册人数
#define FACE_MATCH_THRESHOLD    1.0f  // L2 距离阈值（<1.0 ≈ cos_sim >0.5）
#define FACE_RATIO_THRESHOLD    0.75f // best/second 距离比上限
#define FACE_CONFIDENT_DIST     0.3f  // 低于此距离直接通过，不查 ratio
#define FACE_ENROLL_SAMPLES      5    // 多采注册次数
#define AI_FD_CONF_THRESHOLD    0.5f  // 检测置信度阈值
#define AI_FD_IOU_THRESHOLD     0.3f  // NMS 阈值
#define AI_FD_EMA_ALPHA         0.30f // 检测框时序平滑系数
#define AI_FD_TRACK_IOU_THRESH  0.50f // 跨帧追踪 IoU 阈值
#define CROP_MARGIN             0.0f  // 裁剪时检测框外扩比例（当前关掉）
```

### 匹配策略

1. 查询 embedding 与 gallery 中所有条目计算 L2 距离
2. 选最佳 + 次佳匹配
3. 如果最佳距离 < `FACE_CONFIDENT_DIST` (0.3)：直接通过
4. 否则，如果次佳是**不同人**且 `best/second > FACE_RATIO_THRESHOLD`：拒绝（模糊匹配）
5. 如果最佳距离 < `FACE_MATCH_THRESHOLD` (1.0)：返回匹配

---

## 涉及文件

```
EmbeddedCompetition/ElectronicWarehouse/
├── Appli/
│   ├── APP/
│   │   ├── AI_task.c               # AI 主任务、检测框绘制、串口命令、ReID 调度
│   │   └── Sensor_task.c           # 摄像头帧信号量
│   ├── Drivers/BSP/IMX335/
│   │   └── imx335.c                # DCMIPP PIPE1+PIPE2 配置
│   ├── Drivers/BSP/RGBLCD/
│   │   ├── rgblcd.c                # LTDC 显示、单层 RGB888 帧缓冲
│   │   └── rgblcd.h                # 显示接口
│   └── X-CUBE-AI/App/
│       ├── app_x-cube-ai.c         # NPU 推理、后处理、裁剪缩放、量化、gallery
│       ├── app_x-cube-ai.h         # 结构体定义、参数宏、API 声明
│       ├── network_f.h             # BlazeFace 网络参数
│       ├── network_fc.h            # MobileFaceNet 网络参数
│       └── fd_blazeface_anchors.h  # BlazeFace anchor 数据
```

---

## 待解决问题

### 1. 屏幕中出现人脸时轻微抽搐

**现象**：画面中有人脸时，LCD 显示出现轻微抖动/闪烁。

**分析**：检测到人脸时，CPU 会在 DCMIPP PIPE1 连续写入的间隙中停止管道（`HAL_DCMIPP_CSI_PIPE_Stop`），用 DMA2D 画检测框，再重启管道（`HAL_DCMIPP_CSI_PIPE_Start`）。这个停止-重启过程中，LTDC 可能读到不完整的帧数据，产生撕裂/抽搐。由于 DCMIPP 新帧很快覆盖错误区域，这一现象不影响模型 network_fc 的输入质量（模型输入来自 PIPE2 独立路径），仅影响视觉效果。

**可能改进方向**：使用 DCMIPP 快照模式 + 双缓冲，让画框和摄像头写入互不干扰；或在 VSYNC 中断中画框。

### 2. 参数可能待优化（仅两人测试）

**现状**：目前仅对 2 个人的面部数据进行了功能验证，关键参数（`FACE_MATCH_THRESHOLD`、`FACE_RATIO_THRESHOLD`、`FACE_CONFIDENT_DIST`）基于 PC 端经验和嵌入式端少量测试设定。

**风险**：多人场景下可能出现：
- 阈值过严 → 同人被拒识（false reject）
- 阈值过松 → 不同人被误识（false accept）
- ratio check 参数不适应当前 embedding 分布

**建议**：收集 5-10 人的多角度数据，在嵌入式端统计 L2 距离分布（同人 vs 不同人），重新校准阈值。

### 3. 模型一（BlazeFace）极小概率检测框不完整

**现象**：极少数帧中人脸检测框缺少眼部等关键点，或框的位置/大小偏差较大。

**分析**：BlazeFace 在 INT8 量化后精度有轻微损失，且输入仅为 128×128（原始模型在 256×256 上训练），对极端角度/光照的鲁棒性有限。EMA 时序平滑（α=0.3）可以缓解抖动，但无法修正系统性错误。

**可能改进方向**：提高检测输入分辨率（如 192×192 或 256×256，需评估 NPU 算力余量）；或替换为更鲁棒的检测模型（如 YuNet，但计算量更大）。
