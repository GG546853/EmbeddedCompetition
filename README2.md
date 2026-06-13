# MobileFaceNet 人脸身份识别模型替换修改记录

## 改动概述

将人脸身份识别模型从 `face-reidentification-retail-0095_int8.onnx`（Intel OpenVINO, int8, 256-D 输出）替换为 `mobilefacenet_int8_faces.onnx`（MobileFaceNet, int8, 128-D 输出）。

---

## 新模型信息

| 参数 | 值 |
|------|-----|
| 模型文件 | `mobilefacenet_int8_faces.onnx` |
| 输入 | int8, 1x3x112x112, scale=0.00784313772 |
| 输出 | f32, 1x128（512 Bytes） |
| 权重 | 1,156,289 B（1.13 MB）@ octoFlash 0x71200000 |
| 激活内存 | 4,116,480 B（3.93 MiB），含 cpuRAM2（1 MB）+ npuRAM3-6 + hyperRAM |
| SW Epochs | 110 / 183（大量 PRelu 层在 CPU 软件回退执行） |

---

## 修改文件列表

### 一、X-CUBE-AI 自动生成（整体替换）

| 文件 | 说明 |
|------|------|
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_fc.c` | 新 NPU 调度图，183 个 epoch |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_fc.h` | IO 缓冲区大小：IN_1_SIZE_BYTES=37632, OUT_1_SIZE_BYTES=512 |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_fc_generate_report.txt` | 新模型编译报告 |
| `ElectronicWarehouse/network_fc_atonbuf.xSPI2.raw` | 新权重二进制文件，烧录到 octoFlash 0x71200000 |
| `ElectronicWarehouse/.ai/` 下的 c_info.json | 新模型 buffer 元数据 |

### 二、应用代码手动修改

#### 1. `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h`

| 改动 | 原因 |
|------|------|
| `FACE_EMBEDDING_DIM 256 -> 128` | 新模型输出 128 维向量 |
| 函数声明 `ai_crop_resize_face_128` -> `ai_crop_resize_face_112` | 新模型输入尺寸 112x112 |

#### 2. `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c`

| 改动 | 原因 |
|------|------|
| `CROP_OUT_SZ 128 -> 112` | 新模型输入尺寸 112x112 |
| 函数名 `ai_crop_resize_face_128` -> `ai_crop_resize_face_112` | 同上 |
| `FC_INPUT_SCALE 0.00787401572f -> 0.00784313772f` | 新模型输入量化 scale |
| `quant_u8_to_s8_lut[256]` 重新计算 | 配合新 scale，等效 round(u8 * 0.5) |

> 注：`ai_face_reid_run()` 无需修改，原有 `DataType_FLOAT` 分支已能处理 f32 输出。

#### 3. `ElectronicWarehouse/Appli/APP/AI_task.c`

| 改动 | 原因 |
|------|------|
| `ai_crop_resize_face_128` -> `ai_crop_resize_face_112` | 适配新函数名 |

#### 4. `ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD/rgblcd.c`

| 改动 | 原因 |
|------|------|
| `g_ltdc_layer2_framebuf` 从 BSS（默认段）移到 EXTRAM（HyperRAM 0x90000000） | 释放 AXISRAM2 供 NPU 使用。原先 section 属性被注释掉导致落入 BSS |

#### 5. `ElectronicWarehouse/Appli/STM32N647X0HXQ_LRUN_RAMxspi1.ld`（Debug 链接器脚本）

| 改动 | 原因 |
|------|------|
| RAM LENGTH `2047K -> 1023K` | 应用只使用 AXISRAM1（0x34000400-0x340FFFFF），AXISRAM2（0x34100000-0x34200000）留给 NPU |

#### 6. `ElectronicWarehouse/Appli/STM32N647X0HXQ_ROMxspi2_RAMxspi1.ld`（Release 链接器脚本）

| 改动 | 原因 |
|------|------|
| RAM LENGTH `2048K -> 1024K` | 同上，Release 版本 ORIGIN 为 0x34000000 |

---

## 问题排查记录

### 问题 1：首次运行 HardFault（octoFlash 0x718079a8）

- **现象**：`[FC] Init done, running epochs...` 后立即 HardFault
- **原因**：新 `network_fc.c` 编译进去了，但新权重 `.raw` 文件未烧录到 octoFlash，NPU 读取旧权重数据中的错误描述符
- **解决**：用 `arm-none-eabi-objcopy` 将 `.raw` 转为 `.hex` 并烧录到 0x71200000

### 问题 2：烧录权重后仍 HardFault

- **现象**：`[FC] Init done, running epochs...` 后 HardFault，地址不在 octoFlash 范围
- **原因**：新模型使用 cpuRAM2（AXISRAM2, 0x34100000-0x34200000）存放 1 MB 激活数据，但链接器脚本将 AXISRAM2 分配给了应用程序（BSS 和栈）。`g_ltdc_layer2_framebuf`（1.12 MB）正好落在 0x34088b0c 处，与 NPU 激活缓冲区重叠。NPU 写入激活数据破坏了应用全局变量/栈
- **解决**：
  1. 将 `g_ltdc_layer2_framebuf` 移到 HyperRAM EXTRAM 段
  2. 链接器脚本 RAM 缩小为仅 AXISRAM1，AXISRAM2 留给 NPU

---

## 修改后内存布局

```
0x34000000 ┌─────────────────┐
0x34000400 │  应用代码/数据    │  <- 链接器 RAM: Debug 1023K / Release 1024K
           │  BSS, 堆, 栈    │     (仅 AXISRAM1)
0x34100000 ├─────────────────┤
           │  NPU 激活内存    │  <- network_fc cpuRAM2 pool (1 MB)
0x34200000 ├─────────────────┤
           │  NPU 激活内存    │  <- npuRAM3 (448 KB)
0x34270000 ├─────────────────┤
           │  NPU 激活内存    │  <- npuRAM4 (448 KB, 与 network_f 共享)
0x342E0000 ├─────────────────┤
           │  NPU 激活内存    │  <- npuRAM5 (448 KB)
0x34350000 ├─────────────────┤
           │  NPU 激活内存    │  <- npuRAM6 (84 KB)
0x34365000 ├─────────────────┤

  ...（其他外设）...

0x71200000 ┌─────────────────┐
           │  network_fc 权重 │  <- octoFlash (1.10 MB)
0x7131A4D0 ├─────────────────┤

  ...

0x90000000 ┌─────────────────┐
           │  帧缓冲 + NPU   │  <- HyperRAM (32 MB)
           │  overflow       │
0x92000000 └─────────────────┘
```
