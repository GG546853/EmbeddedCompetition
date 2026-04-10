
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Neural ART Embedded Client API
  *          Re-target of the printf function is out-of-scope.
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\[user_name]\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"

/* USER CODE BEGIN includes */
#include "core_cm55.h" // 需要包含内核头文件用于 Cache 操作
#include "Sensor_task.h"
#include "RGBLED_task.h"
#include "rgblcd.h"
#include "network.h"
/* USER CODE END includes */

/* Entry points --------------------------------------------------------------*/

LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(Default)
uint8_t *buffer_in;
uint8_t *buffer_out;

void set_clk_sleep_mode(void)
{
  /* Leave clocks enabled in Low Power modes */
  // Low-power clock enable misc
#if defined (CPU_IN_SECURE_STATE)
  __HAL_RCC_DBG_CLK_SLEEP_ENABLE();
#endif
  __HAL_RCC_XSPIPHYCOMP_CLK_SLEEP_ENABLE();

  // Low-power clock enable for memories
  __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_FLEXRAM_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_CACHEAXIRAM_MEM_CLK_SLEEP_ENABLE();
  // LP clock AHB1: None
  // LP clock AHB2: None
  // LP clock AHB3
#if defined (CPU_IN_SECURE_STATE)
  __HAL_RCC_RIFSC_CLK_SLEEP_ENABLE();
  __HAL_RCC_RISAF_CLK_SLEEP_ENABLE();
  __HAL_RCC_IAC_CLK_SLEEP_ENABLE();
#endif
  // LP clock AHB4: None
  // LP clocks AHB5
  __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();
  __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();
  __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
  __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
  // LP clocks APB1: None
  // LP clocks APB2
  __HAL_RCC_USART1_CLK_SLEEP_ENABLE();
  // LP clocks APB4: None
  // LP clocks APB5: None
}

void MX_X_CUBE_AI_Init(void)
{
    __HAL_RCC_AXISRAM2_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE();
    RAMCFG_SRAM2_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM3_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM4_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM5_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM6_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    set_clk_sleep_mode();
    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();
    npu_cache_init();
    /* USER CODE BEGIN 5 */
    HAL_NVIC_SetPriority(NPU_IRQn, 5, 0);
    // 2. 使能 NPU 中断
    HAL_NVIC_EnableIRQ(NPU_IRQn);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
	LL_ATON_RT_RetValues_t ll_aton_rt_ret = LL_ATON_RT_DONE;
	 const LL_Buffer_InfoTypeDef * ibuffersInfos = NN_Interface_Default.input_buffers_info();
	 const LL_Buffer_InfoTypeDef * obuffersInfos = NN_Interface_Default.output_buffers_info();

    buffer_in  = (uint8_t *)LL_Buffer_addr_start(&ibuffersInfos[0]);
    buffer_out = (uint8_t *)LL_Buffer_addr_start(&obuffersInfos[0]);

    //LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    // 处理 Cache 一致性
    // 因为摄像头数据是通过外设(DCMIPP/LTDC)通过总线直接写入 EXTRAM 的
    // CPU 的 D-Cache 可能缓存了旧的内存数据。必须先无效化 Cache，确保 CPU 读到最新的摄像头画面！
//    SCB_InvalidateDCache_by_Addr((uint32_t *)g_ltdc_lcd_framebuf, (CAM_WIDTH * CAM_HEIGHT * 2));
//    Preprocess_Camera_Image(g_ltdc_lcd_framebuf, (int8_t *)buffer_in);
    //因为 buffer_in 是 CPU 写进去的，NPU（硬件外设）要去读取，需要清理(Clean) Cache
//    SCB_CleanDCache_by_Addr((uint32_t *)buffer_in, (416 * 416 * 3));
    do {
            // 运行 Epoch 块
            ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);

            // 如果 NPU 正在硬件运算，CPU 可以进入休眠等待中断(WFE)
            if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
                LL_ATON_OSAL_WFE();
            }
        } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

    SCB_InvalidateDCache_by_Addr((uint32_t *)buffer_out, 24843);
    int8_t *yolo_output = (int8_t *)buffer_out;
    float scale = 1.812996390f;
    int32_t zero_point = -107;
    int offset_x = (800 - 416) / 2;
    int offset_y = (480 - 416) / 2;
    int num_anchors = 3549; // 分析报告里的 3549
    int num_elements = 7;   // [x, y, w, h, obj_conf, cls1, cls2]
    LL_ATON_RT_DeInit_Network(&NN_Instance_Default);
    for (int i = 0; i < num_anchors; i++)
    {
        // 提取并反量化置信度 (obj_conf 是第 4 个元素，索引为 i*7 + 4)
        int8_t raw_conf = yolo_output[i * num_elements + 4];
        float obj_conf = (raw_conf - zero_point) * scale;

        // 设置一个阈值，只有置信度极高才处理 (实际中建议用 sigmoid 函数，此处做简化判断)
        // 这里的阈值(比如 > 0.5) 需要根据你模型的实际输出情况微调
        if (obj_conf > 0.5f)
        {
            // 反量化坐标 x, y, w, h
            float cx = (yolo_output[i * num_elements + 0] - zero_point) * scale;
            float cy = (yolo_output[i * num_elements + 1] - zero_point) * scale;
            float w  = (yolo_output[i * num_elements + 2] - zero_point) * scale;
            float h  = (yolo_output[i * num_elements + 3] - zero_point) * scale;

            // 转换成左上角坐标 (针对 416x416 的图像系)
            int box_x = (int)(cx - w / 2.0f);
            int box_y = (int)(cy - h / 2.0f);
            int box_w = (int)w;
            int box_h = (int)h;

            // 映射回 800x480 屏幕原图坐标
            int final_x = box_x + offset_x;
            int final_y = box_y + offset_y;

            // 在 LCD 缓冲区画一个红色的框 (RGB565 红色是 0xF800)
            LCD_Draw_Rect(g_ltdc_lcd_framebuf, final_x, final_y, box_w, box_h, 0xF800);

            // 每次画完框也要刷一下 Cache，确保改动送到 LCD 外设
            // SCB_CleanDCache_by_Addr(...) (如果是整屏刷新，可以直接在任务大循环里Clean整个LCD buffer)
        }
    }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
