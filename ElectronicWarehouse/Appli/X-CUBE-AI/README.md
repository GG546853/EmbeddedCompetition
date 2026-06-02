# X-CUBE-AI 多模型部署配置

## 项目概述

在 STM32N647 开发板上部署两个 AI 模型：

| 模型名 | 用途 | 输入 | 权重大小 | 权重 NOR Flash 地址 |
|--------|------|------|---------|-------------------|
| `network_f` | BlazeFace 人脸检测 | 128x128x3 | 163 KB | `0x71000000` |
| `network_fc` | Face Re-ID 人脸识别 | 128x128x3 | 1.33 MB | `0x71200000` |

两个模型**顺序执行**，不并行。激活内存 (activations) 可共享。

## NOR Flash 内存布局 (32MB, 0x70000000 ~ 0x72000000)

```
0x70000000 ┌─────────────────────┐
           │  FSBL (First Stage  │
           │  Boot Loader)       │
0x70100000 ├─────────────────────┤
           │  Application 固件    │
0x71000000 ├─────────────────────┤
           │  network_f 权重      │  ← stm32n6_net_f.mpool (octoFlash: 0x71000000, 14MB)
           │  (BlazeFace, 163KB) │
0x71200000 ├─────────────────────┤
           │  network_fc 权重     │  ← stm32n6_net_fc.mpool (octoFlash: 0x71200000, 12MB)
           │  (Face Re-ID,       │
           │   1.33MB)           │
0x72000000 └─────────────────────┘
```

## 自定义 mpool 文件

位置：`Appli\Model\my_mpools\`

| 文件 | octoFlash 起始 | 大小 | 对应 profile |
|------|---------------|------|-------------|
| `stm32n6_net_f.mpool` | `0x71000000` | 14 MB | `net_f_flash` |
| `stm32n6_net_fc.mpool` | `0x71200000` | 12 MB | `net_fc_flash` |

mpool 文件也同步到了 X-CUBE-AI 安装目录：
`C:\Users\96022\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AI\10.2.0\scripts\N6_scripts\my_mpools\`

## user_neuralart.json 新增 profile

文件位置：
`C:\Users\96022\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AI\10.2.0\scripts\N6_scripts\user_neuralart.json`

新增了两个 profile：

- **`net_f_flash`** → 使用 `stm32n6_net_f.mpool`
- **`net_fc_flash`** → 使用 `stm32n6_net_fc.mpool`

## 生成模型的方式

CubeMX 中两个模型共享同一个 profile，所以需要**分次生成**：

1. CubeMX → X-CUBE-AI → Advanced Settings → 选 **`net_f_flash`** → Analyze → Generate
2. CubeMX → X-CUBE-AI → Advanced Settings → 选 **`net_fc_flash`** → Analyze → Generate

生成后检查 `network_f_generate_report.txt` 和 `network_fc_generate_report.txt` 中 `octoFlash` 地址不重叠。

## 烧录权重文件

```bash
# 模型 A 权重 (0x71000000)
arm-none-eabi-objcopy -I binary network_f_atonbuf.xSPI2.raw \
  --change-addresses 0x71000000 -O ihex network_f_data.hex

# 模型 B 权重 (0x71200000)  
arm-none-eabi-objcopy -I binary network_fc_atonbuf.xSPI2.raw \
  --change-addresses 0x71200000 -O ihex network_fc_data.hex
```

## 参考资料

- ST 官方多模型示例: [x-cube-n6-ai-hand-landmarks](https://github.com/STMicroelectronics/x-cube-n6-ai-hand-landmarks)
- ST 社区: [Deployment of multiple Neural Networks on STM32N6](https://community.st.com/t5/edge-ai/deployment-of-multiple-neural-networks-on-stm32n6/td-p/894678)
- ST 社区: [STM32N6 X-CUBE-AI multiple models](https://community.st.com/t5/edge-ai/stm32n6-x-cube-ai-multiple-models/td-p/879135)
