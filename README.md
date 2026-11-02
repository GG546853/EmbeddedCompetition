# 智能仓储终端（Electronic Warehouse）

基于 STM32N647 的嵌入式智能仓储终端，集成人脸识别出入库、条码扫描、环境监测、打印与电磁锁控制，运行 FreeRTOS + LVGL。

## 功能

- 人脸检测与识别（BlazeFace + MobileFaceNet，STM32N6 NPU 加速）
- 出入库记录与本地存储
- 摄像头预览与 LVGL 多屏 UI
- 温湿度（AHT10）、距离（VL53L1X）、电池、编码器等传感器
- 条码扫描、打印机、电磁锁、RGB 指示灯

## 技术栈

| 项 | 说明 |
|---|---|
| MCU | STM32N647X0HXQ（Cortex-M55 + NPU） |
| RTOS | FreeRTOS（CMSIS-RTOS2），Percepio TraceRecorder 调试 |
| UI | LVGL 9.4 + EEZ Studio 生成的界面 |
| AI | X-CUBE-AI 10.2.0，`network_f` 人脸检测 / `network_fc` 人脸重识别 |
| 外设 | IMX335 摄像头、DCMIPP、LTDC、DMA2D、XSPI、SDMMC、ADC、I2C、UART 等 |

## 目录结构

```text
.
├── README.md
├── docs/                          # 设计、硬件、调试、日志、报告
└── ElectronicWarehouse/
    ├── ElectronicWarehouse.ioc    # CubeMX 工程
    ├── Drivers/                   # CMSIS + HAL
    ├── Middlewares/               # ST AI、FreeRTOS 等共享组件
    ├── Secure_nsclib/
    └── Appli/                     # 应用工程 ElectronicWarehouse_Appli
        ├── APP/                   # 业务任务、协议、UI
        ├── Core/                  # CubeMX 生成的时钟/外设/中断
        ├── Drivers/BSP/           # IMX335、LCD 等板级驱动
        ├── Middlewares/           # LVGL 9.4、CMSIS-RTOS2、TraceRecorder
        ├── X-CUBE-AI/             # NPU 模型与后处理
        └── Model/                 # X-CUBE-AI 内存池
```

## 构建

1. 安装 STM32CubeIDE（含 STM32N6 支持）和 X-CUBE-AI 10.2.0 软件包。
2. 导入两个工程：`ElectronicWarehouse/`（CubeMX 根工程）和 `ElectronicWarehouse/Appli/`（`ElectronicWarehouse_Appli`）。
3. 选择 `Release` 或 `Debug` 构建配置后编译。
4. 通过 STM32CubeIDE 的 `.launch` 配置或 STM32CubeProgrammer 烧录；Flash boot 相关问题见 [Flash Boot 调试](docs/setup/flash-boot-debugging.md)。

> `Appli/Debug/`、`Appli/Release/` 是 CubeIDE 生成的构建产物，不入库，首次构建会自动生成。

## 文档

- [文档索引](docs/README.md)
- [AI 数据流与模型参数](docs/design/ai-data-flow.md)
- [踩坑记录](docs/troubleshooting/gotchas.md)
- [UI 与摄像头调试记录](docs/troubleshooting/ui-camera-debug.md)
- [竞赛作品报告](docs/reports/competition-report-2.pdf)

## 说明

- LVGL 的显示驱动有本地补丁（DMA2D 加锁与缓存清理），位于
  `ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc/lv_st_ltdc.c`，升级 LVGL 时需保留。
- UI 代码由 EEZ Studio 生成，生成的字体/图片源文件较大，修改时注意不要与手写逻辑混放。
