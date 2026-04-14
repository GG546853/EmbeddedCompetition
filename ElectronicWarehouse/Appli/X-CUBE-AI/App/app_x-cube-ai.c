
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
#include "npu_cache.h"

#define MAX_BOXES 200
 typedef struct {
     float x1, y1, x2, y2;
     float conf;
     int keep;
 } Box_t;

 extern uint8_t g_ai_cam_buf[320 * 320 * 3];
 extern uint16_t g_ltdc_layer2_framebuf[480 * 800];
 extern DCMIPP_HandleTypeDef hdcmipp;



 /* 声明 AI 模型需要的输入和输出缓存 (32字节对齐，放入 SRAM 中加快处理) */
// int8_t ai_input_tensor[3 * 320 * 320] __attribute__((aligned(32)));
// int8_t ai_output_tensor[7 * 2100] __attribute__((aligned(32))); // 你的模型输出是 7x2100

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
    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
	uint32_t buff_in_len;
	uint32_t buff_out_len;
    static Box_t boxes[MAX_BOXES];

	LL_ATON_RT_RetValues_t ll_aton_rt_ret = LL_ATON_RT_DONE;
	const LL_Buffer_InfoTypeDef * ibuffersInfos = NN_Interface_Default.input_buffers_info();
	const LL_Buffer_InfoTypeDef * obuffersInfos = NN_Interface_Default.output_buffers_info();
	buffer_in = (uint8_t *)LL_Buffer_addr_start(&ibuffersInfos[0]);
	buffer_out = (uint8_t *)LL_Buffer_addr_start(&obuffersInfos[0]);
	LL_ATON_RT_RuntimeInit();
	buff_in_len = ibuffersInfos->offset_end - ibuffersInfos->offset_start;
	buff_out_len = obuffersInfos->offset_end - obuffersInfos->offset_start;

	SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
	SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
	HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0, buffer_in, DCMIPP_MODE_SNAPSHOT);

	vTaskDelay(pdMS_TO_TICKS(5));
	SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

    uint32_t img_size = 320 * 320 * 3; // RGB888 总字节数
    uint8_t *pImg = (uint8_t *)buffer_in;
    for (uint32_t i = 0; i < img_size; i++) {
        pImg[i] ^= 0x80; // 这行等同于减去 128，且处理速度极快
    }

	SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
    LL_ATON_RT_Init_Network(&NN_Instance_Default);  // Initialize passed network instance object
    do {
      /* Execute first/next step */
      ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
      /* Wait for next event */
      if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
        LL_ATON_OSAL_WFE();
      }
    } while (ll_aton_rt_ret != LL_ATON_RT_DONE);
    memset(g_ltdc_layer2_framebuf, 0, sizeof(g_ltdc_layer2_framebuf));

    float scale = 1.781954170f;
    int zero_point = -122;
    int valid_count = 0;
    int8_t *int8_out = (int8_t *)buffer_out;
    /* 第一步：遍历所有2100个锚框，找出置信度大于阈值的框 */
    for(int i = 0; i < 2100; i++)
    {
        int8_t class0_q = int8_out[4 * 2100 + i];
        int8_t class1_q = int8_out[5 * 2100 + i];
        int8_t class2_q = int8_out[6 * 2100 + i];

        int8_t max_class_q = class0_q;
        if(class1_q > max_class_q) max_class_q = class1_q;
        if(class2_q > max_class_q) max_class_q = class2_q;

        float conf = (max_class_q - zero_point) * scale;

        // YOLOv8 int8 量化通常需要较高的阈值过滤噪点，设为 0.45
        if(conf > 0.45f && valid_count < MAX_BOXES)
        {
            float cx = (int8_out[0 * 2100 + i] - zero_point) * scale;
            float cy = (int8_out[1 * 2100 + i] - zero_point) * scale;
            float w  = (int8_out[2 * 2100 + i] - zero_point) * scale;
            float h  = (int8_out[3 * 2100 + i] - zero_point) * scale;

            // 存入结构体，转换为左上角和右下角坐标 (以 320x320 为基准)
            boxes[valid_count].x1 = cx - w / 2.0f;
            boxes[valid_count].y1 = cy - h / 2.0f;
            boxes[valid_count].x2 = cx + w / 2.0f;
            boxes[valid_count].y2 = cy + h / 2.0f;
            boxes[valid_count].conf = conf;
            boxes[valid_count].keep = 1;
            valid_count++;
        }
    }

    /* 第二步：NMS (非极大值抑制) - 删掉重叠的多余框 */
    for (int i = 0; i < valid_count; i++) {
        if (boxes[i].keep) {
            for (int j = i + 1; j < valid_count; j++) {
                if (boxes[j].keep) {
                    // 计算重叠区域的坐标
                    float xx1 = (boxes[i].x1 > boxes[j].x1) ? boxes[i].x1 : boxes[j].x1;
                    float yy1 = (boxes[i].y1 > boxes[j].y1) ? boxes[i].y1 : boxes[j].y1;
                    float xx2 = (boxes[i].x2 < boxes[j].x2) ? boxes[i].x2 : boxes[j].x2;
                    float yy2 = (boxes[i].y2 < boxes[j].y2) ? boxes[i].y2 : boxes[j].y2;

                    float intersection = 0.0f;
                    if (xx2 > xx1 && yy2 > yy1) {
                        intersection = (xx2 - xx1) * (yy2 - yy1);
                    }

                    float area_i = (boxes[i].x2 - boxes[i].x1) * (boxes[i].y2 - boxes[i].y1);
                    float area_j = (boxes[j].x2 - boxes[j].x1) * (boxes[j].y2 - boxes[j].y1);
                    float union_area = area_i + area_j - intersection;

                    float iou = (union_area > 0) ? (intersection / union_area) : 0;

                    // 如果两个框的重合度超过 45%，就认为它们框的是同一个物体，删掉后面的框
                    if (iou > 0.45f) {
                        boxes[j].keep = 0;
                    }
                }
            }
        }
    }

    /* 第三步：映射到 800x480 并画框 */
    for (int i = 0; i < valid_count; i++) {
        if (boxes[i].keep) {
            // 坐标映射：320x320 -> 800x480
            int display_x1 = (int)(boxes[i].x1 * 2.5f);
            int display_y1 = (int)(boxes[i].y1 * 1.5f);
            int display_x2 = (int)(boxes[i].x2 * 2.5f);
            int display_y2 = (int)(boxes[i].y2 * 1.5f);

            // 严格的边界检查 (防止画图越界死机)
            if (display_x1 < 0) display_x1 = 0;
            if (display_y1 < 0) display_y1 = 0;
            if (display_x2 > 799) display_x2 = 799;
            if (display_y2 > 479) display_y2 = 479;

            int w_lcd = display_x2 - display_x1;
            int h_lcd = display_y2 - display_y1;

            // 只有当宽高都大于0时才画框
            if (w_lcd > 0 && h_lcd > 0) {
                // RED 的值通常是 0xF800 (RGB565格式)
                rgblcd_layer2_draw_rect((uint16_t)display_x1, (uint16_t)display_y1,
                                        (uint16_t)w_lcd, (uint16_t)h_lcd, 0xF800);
            }
        }
    }
//	SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
//	SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

    LL_ATON_RT_Reset_Network(&NN_Instance_Default);

    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
