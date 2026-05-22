# BlazeFace 人脸检测调试总结

## 硬件平台
- MCU: STM32N647 (Cortex-M55 + NPU)
- 摄像头: IMX335 (128×128 RGB888 输入)
- 模型存储: NOR Flash (XSPI2, 0x71000000)
- NPU 激活内存: npuRAM4 (0x34270000) + npuRAM5 (0x342E0000)
- 显示屏: RGB LCD 800×480 (LTDC + DMA2D)

## 模型信息
- 模型: `blazeface_front_128_quant_pc_ff_od_wider_face.tflite`
- 输入: 1×128×128×3 (float32, 192KB)
- 输出: 4 个 buffer，需按大小升序排序
  - boxes_0: 512×16 (32768B, f32)
  - proba_0: 512×1 (2048B, f32)
  - boxes_1: 384×16 (24576B, f32)
  - proba_1: 384×1 (1536B, f32)
- 编译: X-CUBE-AI v10.2.0 + Neural ART Compiler (LL_ATON)
- 共 95 个 epoch，其中 26 个为 SW 实现

## 调试过程

### 问题 1: `--native-float` 导致 output 仍为 FLOAT32

**现象**: 即使去掉 `--native-float`，输出仍然是 FLOAT32 格式而非 INT8。

**原因**: `--native-float` 控制的是 DequantizeLinear 是否被融合到 NPU 硬件操作中：
- **开启时**: DequantizeLinear 被 NPU 内部完成，无 SW epoch，但 STAI API 层面可能看不到 INT8 中间结果
- **关闭时**: DequantizeLinear 变成显式的 SW 操作，从 NOR Flash 读取 scale/zero_point，逐元素执行 `f32 = (int8 - zp) * scale`

**解决**: 从 `user_neuralart.json` 的 `n6-allmems-O3` profile 中移除 `--native-float`，CubeMX 重新 Generate Code。输出格式不变（仍为 FLOAT32），但多了 4 个 SW DequantizeLinear epoch。

### 问题 2: proba（分类置信度）全为零

**现象**: 
- boxes_0/boxes_1 的 RAW hex 输出有非零 f32 值（如 `00 00 98 41` = 19.0f）
- proba_0/proba_1 的 RAW hex 输出全为零，偶有 `00 00 00 80`（-0.0f）
- `blazeface_decode_stride()` 返回 stride0=0, stride1=0
- 屏幕上无检测框

**根本原因**: NOR Flash 中存储的 proba 的 scale 和 zero_point 值为 0.0 和 0：
```
[L2] proba_0 scale=0.000000 zp=0    ← 应为 scale=0.03693692, zp=49
[L2] proba_1 scale=0.000000 zp=0    ← 应为 scale=1.2246984,  zp=126
```

DequantizeLinear SW 操作计算 `(int8 - 0) * 0.0 = 0.0`，导致所有 proba 输出归零。

### 问题 3: NOR Flash 中 scale/zp 为何是 0

**排查过程**:
1. 确认量化 JSON 中有正确的 scale/zp 值（`*_Q.json`）
2. 确认 `network.c` 中 SW DequantizeLinear 引用的 NOR Flash 地址与 debug 代码一致
3. 检查编译产物 ELF 文件，发现 **ELF 中没有任何 NOR Flash (0x71000000) 的 section**
4. 工程使用 `LRUN_RAMxspi1.ld` 链接脚本，所有代码/数据都在内部 RAM (0x34000000) 和 HyperRAM (0x90000000)
5. NOR Flash 的权重数据需要通过 STM32CubeProgrammer 的 External Loader 单独烧录

**推测原因**: 去掉 `--native-float` 重新生成模型后，权重数据中新增了 4 个 scale（各 4B）+ 4 个 zero_point（各 1B）= 约 44 字节的新常量。这些数据位于 NOR Flash 偏移 165792~166225 区间。而旧的 NOR Flash 内容在这些偏移处为 0x00，因此 scale 和 zp 读为 0。

boxes 的 scale/zp 位于更低的 NOR Flash 偏移（165760~165791），有正确的旧数据，所以 boxes 反量化正常工作。

### 问题 4: proba_1 的 int8 原始数据全为零

**现象**: 
```
[L2] proba_1 int8@0x342E7E00: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
```

proba_1 的 NPU int8 输出在所有帧中都是零（偶尔有一个 -128）。即使修复了 scale/zp，`(0-126)*1.2247 = -154.3`，经过 sigmoid ≈ 0，仍然无法检出。

proba_0 的 int8 数据正常（如 `0 0 -20 66 0 0 -32 66...`），反量化后 `(66-49)*0.0369 = 0.628`，经过 sigmoid ≈ 0.65，可正常检出人脸。

**可能原因**: AI_ANALYZE 报告中 epoch 76 标记为 `??`，可能与 proba_1 路径有关。需要进一步排查 NPU 硬件执行或模型结构问题。

## 当前修复方案

在 `app_x-cube-ai.c` 推理后处理中，手动执行 proba 的反量化：

```c
// proba_0: 512 anchors, scale=0.03693692, zp=49
int8_t  *p0_in  = (int8_t *)(0x342E0000UL + 40960);  // 0x342EA000
float   *p0_out = proba_0;                            // 0x342F6000
for (int i = 0; i < 512; i++)
    p0_out[i] = ((float)(p0_in[i] - 49)) * 0.0369369201362133f;

// proba_1: 384 anchors, scale=1.2246984, zp=126
int8_t  *p1_in  = (int8_t *)(0x342E0000UL + 32256);  // 0x342E7E00
float   *p1_out = proba_1;                            // 0x342E7800
for (int i = 0; i < 384; i++)
    p1_out[i] = ((float)(p1_in[i] - 126)) * 1.2246984243392944f;
```

## 待解决问题

1. **proba_1 int8 数据全为零**: 导致 stride-1（384 个 anchor）无法检出人脸，需排查 NPU 输出或模型
2. **NOR Flash 权重烧录**: 需通过 STM32CubeProgrammer + External Loader 重新烧录完整权重数据，之后可移除手动反量化代码
3. **NOR Flash 数据缺失根因**: 确认 CubeMX Generate Code 流程是否缺少 NOR Flash 烧录步骤的提示

## 文件修改记录

| 文件 | 修改内容 |
|------|----------|
| `user_neuralart.json` | 移除 `--native-float` 选项 |
| `network.c` / `network.h` | CubeMX 重新生成（增加 4 个 SW DequantizeLinear） |
| `app_x-cube-ai.c` | 手动反量化 proba_0/proba_1 |
