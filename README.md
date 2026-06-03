# BlazeFace 人脸检测 + Face Re-ID — STM32N6 嵌入式部署

## 项目概述

将 BlazeFace 人脸检测模型 + face-reidentification-retail-0095 人脸识别模型串联部署到 STM32N6 NPU 上，配合 IMX335 摄像头实时采集、LTDC 显示屏实时叠加检测结果，支持串口指令注册和识别人脸。

- **推理帧率**: ~10 FPS（每帧检测，含捕获+归一化+NPU推理+后处理+绘图）
- **检测输入分辨率**: 128×128×3 float32
- **ReID 输入分辨率**: 128×128×3 int8（串口指令触发，非每帧执行）
- **模型编译**: STM32CubeMX X-CUBE-AI，`--native-float = true`
- **最大检测数**: 3 张人脸
- **6 个关键点**: 左眼、右眼、鼻尖、嘴角、左耳、右耳
- **人脸库**: 最多 10 张人脸，256 维特征向量

## 硬件环境

| 组件 | 型号 |
|------|------|
| MCU | STM32N6 (NUCLEO-N657X0-Q) |
| NPU | Neural-ART 加速器 |
| 摄像头 | IMX335 (MIPI CSI) |
| 显示屏 | 800×480 RGB LCD (LTDC) |
| 存储器 | XSPI1 HyperRAM (32MB) + XSPI2 NOR Flash + AXISRAM |
| 外部 RAM | 0x90000000 (XSPI1 内存映射 HyperRAM) |

## 软件环境

- **RTOS**: FreeRTOS (CMSIS-RTOS v2)
- **编译器**: STM32CubeIDE / GCC ARM
- **模型工具链**: STM32CubeMX X-CUBE-AI + ST Edge AI
- **HAL 驱动**: STM32N6 HAL (DCMIPP, LTDC, DMA2D, NPU, XSPI)
- **串口**: USART1, 115200 8N1, BSP 中断驱动接收

---

## 双模型参数对比

| 属性 | network_f (检测) | network_fc (识别) |
|------|-----------------|-------------------|
| 源模型 | blazeface_front_128 (TFLite) | face-reidentification-retail-0095 (ONNX) |
| 输入 | [1,128,128,3] float32 (192KB) | [1,128,128,3] int8 (48KB) |
| 输出 | 4 个 float32 tensor (scores+boxes) | 1 个 int8 tensor [1,256] → float32 embedding (1KB) |
| NPU 激活内存 | npuRAM4+5 = 528KB | npuRAM3+4 = 768KB |
| 权重地址 | 0x71000000 (162KB, NOR Flash) | 0x71200000 (1.33MB, NOR Flash) |
| HW/SW Epochs | 69 HW + 26 SW = 95 | 113 HW + 5 SW (PReLU) = 118 |

**两个模型共享 npuRAM4，地址重叠 → 必须串行执行 Init→Run→DeInit。**

---

## 架构与数据流

```
IMX335 摄像头
    │
    ├── PIPE1 (DCMIPP): 800×480 RGB565 → LTDC Layer 1 (显示预览 + 人脸裁剪源)
    │
    └── PIPE2 (DCMIPP): 128×128 RGB888 → nn_input_u8[48KB]
                                              │
                              【每帧循环】      │
                                    uint8→float32 归一化 /255.0
                                              │
                                    network_f Init→Run→DeInit (NPU)
                                              │
                                    BlazeFace 后处理 (NMS)
                                              │
                              LTDC Layer 2 绘制检测框 + 关键点
                                              │
                              【串口指令触发 ReID】│
                                    检测到人脸时:
                                      坐标映射: 128×128 → 800×480
                                              │
                              ┌── DMA2D M2M_PFC ──────────────┐
                              │ 源: 0x90000000 (HyperRAM)      │
                              │ 目标: g_ltdc_layer2_framebuf   │
                              │     尾部 (内部 SRAM)            │
                              │ 格式: RGB565 (PFC 去行距)       │
                              └────────────────────────────────┘
                                              │
                              软件双线性缩放 内部SRAM → 128×128 RGB888
                                              │
                              uint8→int8 LUT 量化
                                              │
                                    network_fc Init→Run→DeInit (NPU)
                                              │
                              256 维 float32 embedding
                                              │
                                    L2 归一化
                                              │
                              与特征库比对 (欧氏距离)
                                              │
                              LCD 显示结果 + 串口打印
```

### 串口指令

| 指令 | 功能 |
|------|------|
| `register <name>` | 下一帧采集人脸特征并注册到库中 |
| `identify` | 下一帧识别人脸，与库中所有人脸比对 |
| `gallery` | 列出已注册人脸 |
| `help` | 显示帮助信息 |

---

## 关键文件

| 文件 | 说明 |
|------|------|
| `ElectronicWarehouse/Appli/APP/AI_task.c` | AI 推理 FreeRTOS 任务 + 串口指令解析 + ReID 调度 |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` | NPU 推理入口、BlazeFace 后处理、DMA2D 人脸裁剪、L2 归一化、人脸库管理、network_fc 推理封装 |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h` | 检测/ReID 结构体 & 阈值定义 |
| `ElectronicWarehouse/Appli/APP/fd_blazeface_anchors.h` | 896 个锚点坐标 (Grid 0: 512, Grid 1: 384) |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_f.h` | 检测模型 I/O 规格 |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_fc.h` | ReID 模型 I/O 规格 |
| `ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD/rgblcd.c` | LCD 驱动 (DMA2D 绘图, LTDC 刷新) |
| `ElectronicWarehouse/Appli/Drivers/BSP/UART/uart.c` | 串口中断驱动接收 |
| `ElectronicWarehouse/Appli/Core/Src/main.c` | HyperRAM 初始化、NPU RIF 授权 |

---

## 部署步骤

### 1. CubeMX 配置

- 用 X-CUBE-AI 导入 `blazeface_front_128_quant_pc_ff_od_wider_face.tflite`
- 用 X-CUBE-AI 导入 `face-reidentification-retail-0095_int8.onnx`
- 编译选项: `--native-float = true`, `--optimization = 3`
- network_f 输入: float32 (196,608 bytes)
- network_fc 输入: int8 (49,152 bytes, scale=0.007874, zp=0)
- network_fc 输出: int8 (256 bytes, scale=0.010616, zp=0)

### 2. DCMIPP PIPE2 配置

在 `imx335.c` 的 `imx335_dcmipp_init()` 中：
- PIPE2: 128×128 RGB888 输出
- Decimation: 1/4 + downsize
- PixelPacker: RGB888_YUV444_1
- Pitch: 128 × 3 = 384 bytes

### 3. 双模型实例注册

在 `app_x-cube-ai.c` 中：
```c
LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(network_f)
LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(network_fc)
```
在 `MX_X_CUBE_AI_Init()` 中获取两个模型的 input/output buffer 指针。

### 4. 添加锚点文件

从 STM32N6 官方例程复制锚点常量：
- `g_Anchors_0[1024]` — Grid 0: 512 个锚点 × 2 坐标
- `g_Anchors_1[768]` — Grid 1: 384 个锚点 × 2 坐标

### 5. 实现后处理

在 `app_x-cube-ai.c` 中：

1. 按字节大小排序 4 个输出张量，区分 Score 和 Box
2. 按元素数量匹配 Score ↔ Box（`scnt == bcnt`）
3. 反量化（当前使用 float32 输出，无需反量化）
4. 解码：`x_center = boxes[0] / 128 + anchor_x`, `width = boxes[2] / 128 * AI_FD_BOX_SCALE_W`
5. Sigmoid 置信度
6. NMS with configurable IoU threshold

### 6. 人脸裁剪 (DMA2D)

**关键**: CPU 不能直接读取 XSPI1 HyperRAM (0x90000000) 中的显示帧缓冲，必须通过 DMA2D 总线主控访问。

```c
// DMA2D M2M_PFC: HyperRAM → 内部 SRAM (RGB565, 处理源行距)
hdma2d.Init.Mode = DMA2D_M2M_PFC;
hdma2d.LayerCfg[0].InputOffset = 800 - crop_w;  // 跳过源行距
HAL_DMA2D_Start(&hdma2d, src_addr, dst_addr, crop_w, crop_h);
// 然后从内部 SRAM 做软件双线性缩放到 128×128 RGB888
```

### 7. uint8→int8 量化

使用 256 字节预计算 LUT：
```
s8[i] = round(i / 255.0 / 0.00787401572)
```
将 128×128×3 uint8 RGB 图像转为 network_fc 的 int8 输入。

### 8. L2 归一化

face-reidentification-retail-0095 输出的 256 维 embedding 需要 L2 归一化后再计算欧氏距离：
```c
void l2_normalize(float *v, int dim) {
    float sum_sq = Σ(v[i]²);
    float inv_norm = 1.0 / sqrt(sum_sq);
    for (int i = 0; i < dim; i++) v[i] *= inv_norm;
}
```
归一化后距离范围 [0, 2]，匹配阈值 1.0（余弦相似度 > 0.5）。

### 9. 创建 AI 推理任务

FreeRTOS 任务 `AITask`:
- 等待摄像头就绪 (1s)
- 循环: 捕获帧 → 归一化 → network_f 推理 → 后处理 → 绘制显示 → 串口命令检查 → ReID 触发

### 10. NPU RIF 授权

**关键步骤**: 在 `main.c` 的 `SystemIsolation_Config()` 中添加 NPU RIF 授权，将 NPU 配置为安全特权主设备。缺少此步骤会导致 NPU 输出完全错误。

---

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
RIMC_master.MasterCID = RIF_CID_1;
RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_NPU, &RIMC_master);
HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_NPU,
    RIF_ATTRIBUTE_PRIV | RIF_ATTRIBUTE_SEC);
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

### Bug 8: 串口 RX 无响应 (ReID 集成后)

**现象**: 发送 `register zs` 等命令，串口无任何反馈。

**原因**: AI_task.c 中使用自定义的轮询方式 `serial_readline()` 读取 `UART_FLAG_RXFNE`，但 BSP 的 `HAL_UART_Receive_IT` 已在中断中消耗了 RXNE 标志，轮询永远读不到数据。

**修复**: 重写 `process_serial_commands()` 直接使用 BSP 提供的全局变量 `g_uart_rx_sta` (bit15=行就绪) 和 `g_uart_rx_buf[]`，移除轮询。修改 `#include "usart.h"` → `#include "uart.h"`。

### Bug 9: CPU 读取 HyperRAM 导致总线故障 (人脸裁剪崩溃)

**现象**: `register zs` 后，在 `ai_crop_resize_face_128()` 访问 `g_ltdc_lcd_framebuf` 时系统崩溃，显示屏冻结，调试器断开。串口最后输出 `[CROP] first read: idx=195 addr=0x90000186`。

**原因**: `g_ltdc_lcd_framebuf` 位于 `.EXTRAM` 段 (0x90000000, XSPI1 HyperRAM)。DCMIPP 和 LTDC 通过各自的 DMA 总线主控可以访问 HyperRAM（已在 RIF 中授权），但 **CPU 的加载/存储指令无法直接读取 XSPI1 内存映射 HyperRAM**。推测原因：
- CACHEAXI/MPU 未正确配置 0x90000000 区域的缓存属性
- CPU 对 XSPI1 内存映射区域的加载操作触发总线故障

**调试过程**:
1. 首先怀疑是 NMS 或后处理问题，添加了 `AI_FD_CONF_THRESHOLD` 和 `AI_FD_IOU_THRESHOLD` 调整
2. 添加了最小框尺寸过滤器 `if (det.width < 0.10f || det.height < 0.10f) continue;`
3. 在 `run_reid_pipeline()` 中添加 DBG1~DBG5 分步打印，确认崩溃在 DBG1 和 DBG2 之间
4. 在 `ai_crop_resize_face_128()` 内部添加逐行打印，定位到第一次 `src_fb[...]` 访问即崩溃
5. 确认崩溃地址 0x90000186 在 XSPI1 HyperRAM 范围内
6. 检查 `main.c` 发现 HyperRAM 初始化在 `#ifdef DEBUG` 内，但 `.cproject` 确认 DEBUG 已定义

**修复**: 用 DMA2D M2M_PFC 硬件拷贝替代 CPU 直接读取 HyperRAM：
- DMA2D 作为独立总线主控已在 RIF 中授权（`HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D, ...)`）
- Step 1: DMA2D M2M_PFC 将裁剪区域从 HyperRAM 拷贝到 `g_ltdc_layer2_framebuf` 尾部（内部 SRAM），利用 PFC 的 InputOffset 处理源行距
- Step 2: 从内部 SRAM 做软件双线性缩放到 128×128 RGB888
- 临时借用 layer2 缓冲区尾部（约 400KB），无额外内存分配

### Bug 10: ReID identify 始终返回 "no match"

**现象**: 注册人脸后立即 identify，始终显示 "no match"，即使是同一人的同一角度。

**调试过程**:
1. 首先怀疑是识别阈值 `FACE_MATCH_THRESHOLD=0.6f` 过高
2. 添加 embedding 前 8 维的打印：register 和 identify 的值方向相似但数值略有差异
3. 计算发现原始 embedding 的 L2 模长约 8-9，导致欧氏距离偏大（~0.65），刚好超过 0.6 阈值
4. 根本原因：`face-reidentification-retail-0095` 的输出 embedding 未经 L2 归一化，不同图像的同一个人 embedding 模长可能不同，原始欧氏距离受模长影响

**修复**:
- 在 `app_x-cube-ai.c` 中添加 `l2_normalize()` 函数
- `ai_face_enroll()`: 存储前对 embedding 做 L2 归一化
- `ai_face_identify()`: 比对前对查询 embedding 做 L2 归一化
- `FACE_MATCH_THRESHOLD` 调整到 `1.0f`（归一化后距离范围 [0,2]，<1.0 ≈ 余弦相似度 >0.5）

### Bug 11: 检测框偏小且帧间大小跳变

**现象**: 检测框比实际人脸小（眼睛经常在框外），且框的大小在帧间来回切换（大框→小框→大框）。

**原因**:
1. **框偏小**: `blazeface_front_128_quant` 仅在 128×128 分辨率下训练，模型对 width/height 的回归天然偏差。中心位置有 anchor 作为锚点回归较准，但宽高直接从 raw 值除以 128 得到归一化尺寸，缺乏尺度校准，导致框系统性偏小。
2. **帧间跳变**: BlazeFace 的双网格（Grid 0 16×16 + Grid 1 8×8）对同一人脸产生不同大小的候选框。相机噪声导致各候选框的置信度每帧微变，NMS 按置信度排序选出最高分框，结果在大小框之间交替。

**修复**:
- 添加 `AI_FD_BOX_SCALE_W=1.20f` / `AI_FD_BOX_SCALE_H=1.40f` 缩放系数，decoding 时扩大框的宽高
- 添加 `ai_detection_temporal_smooth()` 函数，用 IoU 匹配前后帧检测框，对 x/y/w/h 做 EMA（指数移动平均，α=0.30），消除帧间跳变
- 降低 `AI_FD_CONF_THRESHOLD` 从 0.60 到 0.50，减少大框（通常置信度偏低）被过早过滤
- 新增 `AI_FD_TRACK_IOU_THRESH=0.50f` 控制帧间匹配的门槛，IoU > 0.5 视为同一人脸应用平滑，低于此值视为新人脸直接初始化

**注意**: `AI_FD_BOX_SCALE_H` 设为 1.40 大于 `AI_FD_BOX_SCALE_W` 的 1.20，因为人脸天然竖长，BlazeFace 低分辨率下对高度低估更严重。这两个值需要根据实际屏显效果微调（步进 0.05）。

---

## 当前参数

| 参数 | 值 |
|------|-----|
| AI_FD_MAX_DETECTIONS | 3 |
| AI_FD_NUM_KEYPOINTS | 6 |
| AI_FD_IMG_SIZE | 128 |
| AI_FD_CONF_THRESHOLD | 0.50f |
| AI_FD_IOU_THRESHOLD | 0.3f |
| AI_FD_BOX_SCALE_W | 1.20f |
| AI_FD_BOX_SCALE_H | 1.40f |
| AI_FD_EMA_ALPHA | 0.30f |
| AI_FD_TRACK_IOU_THRESH | 0.50f |
| FACE_EMBEDDING_DIM | 256 |
| FACE_GALLERY_MAX | 10 |
| FACE_MATCH_THRESHOLD | 1.0f (L2 归一化后距离) |
| FACE_NAME_MAX | 16 |
| CROP_MARGIN | 0.3f (30% 扩展) |
| CROP_OUT_SZ | 128 |
| FC_INPUT_SCALE | 0.00787401572f |
| FC_OUTPUT_SCALE | 0.0106163137f |

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

### ReID 人脸裁剪质量

当前裁剪方案：DMA2D M2M_PFC 拷贝 RGB565 裁剪区域 → 内部 SRAM → 软件双线性缩放至 128×128 RGB888。

改进方向：
- 直接用 DMA2D 完成缩放（需研究 STM32N6 DMA2D 的 PFC 缩放寄存器配置）
- 裁剪源改用 DCMIPP PIPE2 的 128×128 输出（已缩放到 AI 输入尺寸），但 128×128 中人脸占比小会损失细节
- 添加人脸对齐（基于关键点做仿射变换），提高 ReID 精度
