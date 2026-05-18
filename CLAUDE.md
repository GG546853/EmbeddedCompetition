# 智能元器件仓储系统 — ElectronicWarehouse

## 硬件平台
- **MCU**: STM32N647 (STM32N6 系列，Cortex-M55 + NPU)
- **开发板**: 正点原子 (ALIENTEK) N6
- **摄像头**: IMX335 (5MP, CSI 接口, DCMIPP)
- **显示屏**: RGB LCD 800×480 (LTDC + DMA2D 硬件加速)
- **外部存储**: HyperRAM 32MB (XSPI1, 显存), NOR Flash (XSPI2, 模型权重 + 代码)
- **触摸**: FT5206 / GT9xxx (I2C)
- **调试串口**: USART1 (printf 重定向)

## 软件框架
- **IDE**: STM32CubeIDE (生成 HAL/Middleware 代码)
- **RTOS**: FreeRTOS (CMSIS-OS2 封装)
- **AI 推理**: X-CUBE-AI v10.2.0 + Neural ART Compiler (STM32N6 NPU)
- **UI**: TouchGFX (当前仅 simulator 构建通过，设备端 UI 尚未实质开发)
- **安全**: TrustZone 使能 (Secure state), RIF 隔离配置
- **编译链**: ARM GCC (cortex-m55, secure)

## 目录结构
```
ElectronicWarehouse/
├── Appli/
│   ├── Core/           # CubeMX 生成的 HAL 初始化 + FreeRTOS
│   ├── APP/            # 应用层任务 (Sensor_task, RGBLED_task, LCD_task)
│   ├── Drivers/BSP/    # 外设 BSP 驱动 (imx335, rgblcd, hyperram, norflash, touch, led, key, eeprom, uart, sys)
│   ├── X-CUBE-AI/App/  # AI 初始化 + 推理 + YuNet 后处理 (核心业务代码)
│   ├── TouchGFX/       # TouchGFX UI 项目 (1 screen: Screen1)
│   └── Middlewares/     # ST/AI/Npu (NPU 驱动) + FreeRTOS
├── Drivers/            # CMSIS + STM32N6xx_HAL
└── .ai/                # AI 模型分析报告 (c_info.json)
```

## AI 模型

### 1. YuNet — 人脸检测 (`network` / Default)
- 源模型: `yunetn_320_qdq_int8.onnx`
- 输入: 1×3×320×320 (int8, NCHW, NHWC→NCHW 转换 + offset -128)
- 输出: 12 个 buffer (3 个 stride: 8/16/32, 每个有 cls/obj/bbox/kps)
- 规模: ~160M MACs, ~295K 参数
- 权重: NOR Flash (0x71000000), 激活: NPU RAM4/5 (~825KB)
- 后处理: C 手写 `yunet_decode()` + `do_nms()` (在 `app_x-cube-ai.c`)

### 2. Face Re-identification — 人脸识别 (`network_1`)
- 源模型: `facereidentificationretail0095_int8.onnx` / `mobilefacenet`
- 输入: 1×3×128×128 / 1×3×112×112 (int8)
- 输出: 256 字节 embedding 向量
- **状态**: 模型已编译，尚未集成到推理管线

## 核心数据流 (Sensor_Task → MX_X_CUBE_AI_Process)
1. IMX335 连续捕获 (DCMIPP PIPE2 snapshot 模式) → `g_ai_cam_buf[320*320*3]`
2. 信号量 `cam_frame_sem` 等待帧就绪
3. `transpose_rgb_to_nchw()`: NHWC uint8 → NCHW int8 (减 128)
4. NPU 推理 (YuNet, 72 epochs, 其中 2 个为 SW)
5. `yunet_decode()`: int8→float 反量化 + anchor 解码 → `FaceDetection boxes[]`
6. `do_nms()`: 按 score 排序 + IoU 阈值 0.45 剔除
7. DMA2D 清屏 Layer2 + 画红色矩形框 (320×320 坐标 ×2.5/×1.5 映射到 800×480)
8. USART printf 输出 12 个 output buffer size (调试)

## FreeRTOS 任务
| 任务 | 优先级 | 职责 |
|------|--------|------|
| defaultTask | Normal | 空闲 |
| RGBLED_Task | Normal | LED/Touch UI (当前空转) |
| Sensor_Task | Normal+1 | 摄像头 ISP → AI 推理 → 结果叠加 (主循环) |

## 当前状态与已知问题
- **分支**: `scheme_ai_0.1`
- **最新提交**: "依旧得不到图像" — 人脸检测框可以叠加到 LCD，但最终显示效果异常
- YuNet 解码和 buffer 打印输出看似正确，怀疑输入图像预处理有问题
- DMA2D 画框逻辑做了坐标映射 (320→800, 320→480)，大量边界裁剪保护
- printf 输出会阻塞推理管线，仅在 debug 使用

## 编码约定
- 用户代码在 `/* USER CODE BEGIN ... */` 和 `/* USER CODE END ... */` 之间
- CubeMX 重新生成会覆盖 USER CODE 之外的代码
- 主要编辑文件: `app_x-cube-ai.c` (AI 管线), `Sensor_task.c` (任务), BSP 驱动
- 调试: Debug 模式使用内部 RAM (0x34000400 VTOR), Release 模式从 NOR Flash 启动 (0x70100400)
- 外设初始化借鉴正点原子 N6 开发板 HAL 例程风格

## 构建
- STM32CubeIDE 工程: `ElectronicWarehouse/Appli/`
- Debug 配置通过 CubeIDE `.launch` 文件管理
- `.claudeignore` 已排除 Debug/Release/中间文件来加速 Claude Code 扫描
