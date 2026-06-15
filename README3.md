HyperRAM 时钟修复：ClockPrescaler 配置错误导致总线频率减半

 Context

 对比官方 IMX335 例程（352_IMX335）后发现，XSPI1 的 ClockPrescaler 配置存在差异，导致 HyperRAM 实际总线时钟仅为预期的 50%。

 HyperRAM 同时承载 DCMIPP 写入摄像头帧和 LTDC 读取显示帧。46 MHz 下带宽不够 → 花屏、画面撕裂。

 时钟链路

 PLL1 输出 = 1200 MHz（HSI 64MHz / PLLM 4 × PLLN 75 = 1200MHz）
 XSPI1 内核时钟 = PLL1 / IC4_Divider
 HyperRAM 总线时钟 = XSPI1 内核 / (ClockPrescaler + 1)

 修改内容

 文件：Appli/Core/Src/xspi.c

 1. 第 53 行 — 取消额外的 2 分频
    hxspi1.Init.ClockPrescaler = 1;  →  hxspi1.Init.ClockPrescaler = 0;

 2. 第 148 行 — IC4 保持 13 不变（已验证稳定）
    PeriphClkInitStruct.ICSelection[RCC_IC4].ClockDivider = 13;  // 保持不变

 修改结果

 修改前：1200 MHz / 13 / (1+1) = 46.15 MHz → 花屏、画面撕裂
 修改后：1200 MHz / 13 / (0+1) = 92.31 MHz → 画面正常、镜头移动时无雪花

 根因分析

 ClockPrescaler 寄存器语义：HW 实际分频 = Prescaler + 1。官方例程写 1-1=0（÷1），
 本项目写 1（÷2），导致 HyperRAM 总线时钟减半。

 200 MHz（官方 IC4=6, Prescaler=0）在正点原子 N647 板上读 HyperRAM ID 失败（信号完整性
 不如官方 DK 板），92 MHz 是本板稳定工作的频率。带宽约 185 MB/s，满足 DCMIPP 写 +
 LTDC 读的 135 MB/s 需求。


 单层 RGB888 方案：消除 LTDC 双图层 HyperRAM 带宽争用

 Context

 当前双图层架构（Layer 0 RGB565 + Layer 1 RGB888，均在 HyperRAM）导致花屏和崩溃。根因是 LTDC 在每个像素时钟交替从两个 HyperRAM 地址区域读取数据，导致 XSPI1
  HyperBus 不断重新寻址（每次 14 周期 ≈ 82ns 开销），FIFO 下溢。

 改为单层 RGB888 后，LTDC 从一段连续内存顺序读取，消除地址切换开销。即使数据总量相近，顺序访问的 HyperBus 效率远高于交替访问。

 设计决策：DCMIPP 直写单帧缓冲区 + 每帧重绘检测框

 单帧缓冲区方案（而非双缓冲 + DMA2D 拷贝）：
 - DCMIPP PIPE1 输出 RGB888 直接写入 g_ltdc_framebuf[480*800*3]
 - AI 检测框直接在 DCMIPP 帧回调中绘制到同一缓冲区
 - 每帧摄像头数据自然覆盖旧检测框，AI 每帧重绘

 权衡：DCMIPP 连续写入模式下，检测框会被摄像头新帧局部覆盖（产生类似 tearing 的效果）。但检测框仅几条线/几个点，AI 运行频率（约
 5-10fps）远低于摄像头帧率（30fps），视觉效果可接受。如需完美消除 tearing，可用 DCMIPP 快照模式（牺牲帧率），或 DMA2D 双缓冲（增加带宽），暂不作为首选。

 HyperRAM 使用：约 1.125MB（单帧缓冲区），比当前 1.875MB 减少 40%。

 带宽估算（单层 vs 当前双图层）：
 - 新方案：DCMIPP 写 66MB/s + LTDC 读 69MB/s = 135MB/s 顺序访问
 - 旧方案：DCMIPP 写 44MB/s + LTDC 读 115MB/s = 159MB/s 交替访问（实际效率仅 50-60%）

 修改清单

 1. rgblcd.c — 单 RGB888 帧缓冲区 + 单 LTDC 图层

 - 将 g_ltdc_lcd_framebuf[480*800] (uint16_t, RGB565) 改为 g_ltdc_framebuf[480*800*3] (uint8_t, RGB888)，放 .EXTRAM，32 字节对齐
 - 删除 g_ltdc_layer2_framebuf
 - rgblcd_init() 中：
   - Layer 0 配置：PixelFormat → LTDC_PIXEL_FORMAT_RGB888，BlendingFactor1/2 → CA（无 alpha 混合，因为只有一层）
   - 删除所有 Layer 1 配置代码（layer2_cfg, HAL_LTDC_ConfigLayer(&hltdc, &layer2_cfg, 1), ColorKeying 等）
   - 删除 memset(g_ltdc_layer2_framebuf, 0, ...)
 - rgblcd_fill()：改用 DMA2D R2M RGB888 输出格式，去除 CONVERTRGB5652ARGB8888 宏（直接写 RGB888 颜色）
 - rgblcd_color_fill()：改用 DMA2D M2M RGB888 格式
 - rgblcd_draw_point()：写入 3 字节 RGB888 而非 2 字节 RGB565
 - rgblcd_read_point()：读取 3 字节 RGB888
 - rgblcd_clear()：传入 RGB888 颜色值
 - rgblcd_show_char() / rgblcd_show_string() 等：适配 RGB888 像素写入

 2. rgblcd.h — 更新声明和接口

 - extern uint8_t g_ltdc_framebuf[480*800*3] 替代 extern uint16_t g_ltdc_lcd_framebuf[480*800]
 - 删除 g_ltdc_layer2_framebuf 声明
 - 函数签名如 rgblcd_draw_point 参数类型调整（uint16_t color → 3 字节或 uint32_t）
 - rgblcd_fill() / rgblcd_clear() / rgblcd_color_fill() 参数调整

 3. imx335.c — DCMIPP PIPE1 输出 RGB888

 - PIPE1 PixelPackerFormat: DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1 → DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1
 - PIPE1 PixelPipePitch: 1600 (800×2) → 2400 (800×3)
 - 删除 extern uint8_t g_ltdc_layer2_framebuf[480*800*3] 声明

 4. Sensor_task.c — 更新帧缓冲区引用

 - imx335_start_capture((uint32_t)g_ltdc_lcd_framebuf) → imx335_start_capture((uint32_t)g_ltdc_framebuf)

 5. AI_task.c — 直接绘制到单帧缓冲区

 - 删除 extern uint8_t g_ltdc_layer2_framebuf，新增 extern uint8_t g_ltdc_framebuf
 - 删除 extern uint16_t g_ltdc_lcd_framebuf
 - set_pixel()、fill_rect()：写入目标改为 g_ltdc_framebuf
 - draw_detections_on_display()：简化逻辑——不再需要"擦除旧框"（DCMIPP 新帧自然覆盖），直接绘制当前检测。删除 prev_det/prev_valid 静态变量
 - 删除 SCB_CleanInvalidateDCache_by_Addr 调用（MPU 已将 HyperRAM 设为 Non-Cacheable）
 - run_reid_pipeline() 中：调用 ai_crop_resize_face_112 的第一个参数从 g_ltdc_lcd_framebuf 改为 g_ltdc_framebuf，并适配类型（uint8_t* RGB888 而非 uint16_t*
  RGB565）

 6. app_x-cube-ai.c — 面部裁剪函数适配 RGB888

 - ai_crop_resize_face_112()：
   - 函数签名：uint16_t *src_fb → uint8_t *src_fb（源变为 RGB888）
   - 删除 extern uint8_t g_ltdc_layer2_framebuf，改用专用 crop_temp_buf（放在 .EXTRAM 或静态分配在函数内）
   - CROP 区域计算：RGB888 每像素 3 字节，temp_bytes = crop_w * crop_h * 3
   - DMA2D 源格式：DMA2D_INPUT_RGB888（不再是 RGB565）
   - DMA2D 输出格式：DMA2D_OUTPUT_RGB888
   - 软件缩放：从 RGB888 源（3 字节/像素）读取，而非 RGB565
   - 删除 r565/g565/b565 辅助函数，改为直接读取 R/G/B 字节
 - 在 app_x-cube-ai.h 更新函数声明

 7. app_x-cube-ai.h — 更新函数声明

 - ai_crop_resize_face_112(uint8_t *src_fb, ...) 参数类型更新

 8. rgblcdfont.c / rgblcd.c — 字体显示适配

 - rgblcd_show_char()：rgblcd_draw_point() 调用已适配 RGB888 后即自动兼容

 涉及文件汇总

 ┌─────────────────────────────────────┬────────────────────────────────────┐
 │                文件                 │              修改性质              │
 ├─────────────────────────────────────┼────────────────────────────────────┤
 │ Appli/Drivers/BSP/RGBLCD/rgblcd.c   │ 大量修改：单帧缓冲、单图层、RGB888 │
 ├─────────────────────────────────────┼────────────────────────────────────┤
 │ Appli/Drivers/BSP/RGBLCD/rgblcd.h   │ 更新声明和接口                     │
 ├─────────────────────────────────────┼────────────────────────────────────┤
 │ Appli/Drivers/BSP/IMX335/imx335.c   │ PIPE1 格式/pitch 修改              │
 ├─────────────────────────────────────┼────────────────────────────────────┤
 │ Appli/APP/Sensor_task.c             │ 帧缓冲区引用更新                   │
 ├─────────────────────────────────────┼────────────────────────────────────┤
 │ Appli/APP/AI_task.c                 │ 绘制目标改为单帧缓冲区，简化逻辑   │
 ├─────────────────────────────────────┼────────────────────────────────────┤
 │ Appli/X-CUBE-AI/App/app_x-cube-ai.c │ 面部裁剪适配 RGB888                │
 ├─────────────────────────────────────┼────────────────────────────────────┤
 │ Appli/X-CUBE-AI/App/app_x-cube-ai.h │ 更新函数声明                       │
 └─────────────────────────────────────┴────────────────────────────────────┘

   IMX335 Sensor (2592×1944, RAW10 RGGB)
  │  MIPI CSI-2: 2 lanes, 1200 Mbps PHY
  │
  └─ DCMIPP ─┬─ PIPE1 (显示) ─────────────────────────────────────┐
             │  ISP: Bayer→RGB, AE/AWB                            │
             │  PixelPacker: RGB888                                │
             │  Downsize: 2592×1944 → 800×480                     │
             │  DMA → g_ltdc_framebuf (HyperRAM 0x90000000)       │
             │                                                     ▼
             │                                      LTDC 读出 → LCD (33.33MHz)
             │                                      单层 RGB888, 800×480
             │
             └─ PIPE2 (AI) ───────────────────────────────────────┐
                ISP: Bayer→RGB                                    │
                Decimate: 1/4 H+V                                  │
                Downsize: 648×486 → 128×128                       │
                DMA → nn_input_u8 (内部 SRAM, noncacheable)       │
                                                       ▼
                                       AI_Task: uint8→float32 → NPU
                                                  │
                                                  ▼
                                       draw_detections → CPU 写回
                                       g_ltdc_framebuf (HyperRAM)

  HyperRAM 同时承载三件事（@ 92.31 MHz ≈ 185 MB/s 峰值）：

  ┌──────┬──────────────────┬────────────────────────┬───────────────────┐
  │ 行为 │       来源       │         数据量         │       带宽        │
  ├──────┼──────────────────┼────────────────────────┼───────────────────┤
  │ 写入 │ DCMIPP PIPE1 DMA │ 800×480×3 = 1.15 MB/帧 │ ~35 MB/s (@30fps) │
  ├──────┼──────────────────┼────────────────────────┼───────────────────┤
  │ 读出 │ LTDC 刷新屏幕    │ 800×480×3 × 60Hz       │ ~76 MB/s          │
  ├──────┼──────────────────┼────────────────────────┼───────────────────┤
  │ 写入 │ CPU 画检测框     │ 几条线/几个点          │ 可忽略            │
  └──────┴──────────────────┴────────────────────────┴───────────────────┘