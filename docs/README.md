# 文档索引

本目录存放项目设计、硬件、调试、日志与交付文档。文件名统一使用英文 kebab-case，正文保留中文（部分为英文原文）。

## 目录约定

| 目录 | 用途 |
|---|---|
| `design/` | 系统设计、数据流、模型参数 |
| `hardware/` | 硬件配置表 |
| `setup/` | 环境搭建、启动与烧录说明 |
| `troubleshooting/` | 踩坑与问题排查记录 |
| `ai/` | X-CUBE-AI 模型分析原始输出 |
| `logs/` | 调试日志与优化记录（日期前缀 `YYYY-MM-DD-`） |
| `reports/` | 竞赛报告等交付文档 |

## 文档

### 设计
- [AI 数据流与模型参数](design/ai-data-flow.md) — IMX335 → DCMIPP → NPU 的两级模型流程、坐标转换、阈值与串口命令

### 硬件
- [4.3 寸 RGB LCD 触摸配置](hardware/lcd-touch-config.md)

### 环境与烧录
- [Flash Boot 调试](setup/flash-boot-debugging.md)

### 问题排查
- [STM32 / CubeMX 踩坑记录](troubleshooting/gotchas.md)
- [UI 与摄像头调试记录](troubleshooting/ui-camera-debug.md) — 多屏切换、Layer 叠加、UI 移植、注册人脸雪花问题

### AI 分析
- [network_f 分析输出](ai/network-f-analyze.log)
- [network_fc 分析输出](ai/network-fc-analyze.log)

### 日志
- [2026-07-19 UART ReID 调试日志](logs/2026-07-19-uart-reid-debug.log)
- [2026-07-20 人脸识别距离采样](logs/2026-07-20-face-reid-distance-samples.log)
- [2026-10-22 DWT → FreeRTOS 运行时统计](logs/2026-10-22-dwt-runtime-stats.md)

### 报告
- [竞赛作品报告](reports/competition-report-2.pdf)

## 文件迁移对照

| 原路径 | 现路径 |
|---|---|
| `README1.md` | `docs/design/ai-data-flow.md` |
| `1.txt` | `docs/hardware/lcd-touch-config.md` |
| `Flash boot调试.txt` | `docs/setup/flash-boot-debugging.md` |
| `坑.txt` | `docs/troubleshooting/gotchas.md` |
| `新建 文本文档.txt` | `docs/logs/2026-07-20-face-reid-distance-samples.log` |
| `优化日志/2026-10-22.txt` | `docs/logs/2026-10-22-dwt-runtime-stats.md` |
| `作品报告2.pdf` | `docs/reports/competition-report-2.pdf` |
| `ElectronicWarehouse/README1.md` | `docs/troubleshooting/ui-camera-debug.md` |
| `ElectronicWarehouse/AI_ANALYZE.md` | `docs/ai/network-f-analyze.log` |
| `ElectronicWarehouse/fc_analyze.md` | `docs/ai/network-fc-analyze.log` |
| `ElectronicWarehouse/uart记录.txt` | `docs/logs/2026-07-19-uart-reid-debug.log` |
