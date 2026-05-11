
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

#include "core_cm55.h"
#include "Sensor_task.h"
#include "RGBLED_task.h"
#include "rgblcd.h"
#include "network.h"
#include "npu_cache.h"
#include"dma2d.h"
#include <math.h>

 typedef struct {
 float x1, y1, x2, y2;
 float conf;
 int keep;
 } Box;

 //extern uint8_t g_ai_cam_buf[];
 extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];
 extern DCMIPP_HandleTypeDef hdcmipp;
 extern osSemaphoreId_t cam_frame_sem; // 确保在 main.c 中创建了这个信号量并在此声明
 extern DMA2D_HandleTypeDef hdma2d;
 extern uint8_t g_ai_cam_buf[320 * 320 * 3];

  __attribute__((section(".camera_buf")))
 __attribute__((aligned(32)))
 Box boxes[2100];

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
    set_clk_sleep_mode();
    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();
    npu_cache_init();
    /* USER CODE BEGIN 5 */
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

    __HAL_RCC_RAMCFG_CLK_ENABLE();
    RAMCFG_HandleTypeDef hramcfg = {0};
    hramcfg.Instance =  RAMCFG_SRAM3_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance =  RAMCFG_SRAM4_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance =  RAMCFG_SRAM5_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance =  RAMCFG_SRAM6_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();    /* For display frame buffer */
    __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();    /* For NN weights */
    __HAL_RCC_NPU_CLK_SLEEP_ENABLE();      /* For NN inference */
    __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE(); /* For NN inference */
    __HAL_RCC_LTDC_CLK_SLEEP_ENABLE();     /* For display */
    __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE();    /* For display */
    __HAL_RCC_DCMIPP_CLK_SLEEP_ENABLE();   /* For camera configuration retention */
    __HAL_RCC_CSI_CLK_SLEEP_ENABLE();      /* For camera configuration retention */

    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
    uint32_t buff_in_len;
    LL_ATON_RT_RetValues_t ll_aton_rt_ret = LL_ATON_RT_DONE;
    const LL_Buffer_InfoTypeDef * ibuffersInfos = NN_Interface_Default.input_buffers_info();
    const LL_Buffer_InfoTypeDef * obuffersInfos = NN_Interface_Default.output_buffers_info();
    buffer_in = (uint8_t *)LL_Buffer_addr_start(&ibuffersInfos[0]);

    LL_ATON_RT_RuntimeInit();
    buff_in_len = ibuffersInfos->offset_end - ibuffersInfos->offset_start;

    // 清理输入 Cache
    SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)buffer_in, DCMIPP_MODE_SNAPSHOT) != HAL_OK) {
        printf("ERROR: DCMIPP PIPE2 Start Failed!\r\n");
    }

    if(osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(100)) != osOK)
        while(1);

    for (int inferenceNb = 0; inferenceNb < 1; ++inferenceNb) {
        SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
        SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

        LL_ATON_RT_Init_Network(&NN_Instance_Default);
        do {
            ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
            if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
                LL_ATON_OSAL_WFE();
            }
        } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

        // =====================================================================
        // 【关键修复 1】：必须无效化所有 12 个输出张量的 Cache
        // =====================================================================
        for (int i = 0; i < 12; i++) {
            uint32_t addr = (uint32_t)LL_Buffer_addr_start(&obuffersInfos[i]);
            uint32_t len = obuffersInfos[i].offset_end - obuffersInfos[i].offset_start;
            SCB_InvalidateDCache_by_Addr((uint32_t*)addr, len);
        }

        // =====================================================================
        // 【关键修复 2】：定义提取 12 个输出中我们需要的 6 个张量指针 (int8_t)
        // =====================================================================
        int8_t *cls_8  = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[0]); // 1600x1
        int8_t *cls_16 = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[1]); // 400x1
        int8_t *cls_32 = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[2]); // 100x1

        int8_t *reg_8  = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[6]); // 1600x4
        int8_t *reg_16 = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[7]); // 400x4
        int8_t *reg_32 = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[8]); // 100x4

        // 从你的 Analyze 报告中抄来的反量化参数 (Scale 和 ZeroPoint)
        float s_cls_8 = 0.003292752f, z_cls_8 = -128.0f;
        float s_cls_16= 0.003212299f, z_cls_16= -128.0f;
        float s_cls_32= 0.002707064f, z_cls_32= -128.0f;
        float s_reg_8 = 0.011400674f, z_reg_8 = -61.0f;
        float s_reg_16= 0.011022569f, z_reg_16= -46.0f;
        float s_reg_32= 0.012532321f, z_reg_32= -16.0f;

        int valid_count = 0;

        // =====================================================================
        // 【关键修复 3】：手撕 YuNet 的反量化与 Anchor 解码逻辑
        // =====================================================================
        // 我们写一个统一的宏/内联逻辑来处理 3 个不同的特征层 (Stride=8, 16, 32)
        int strides[] = {8, 16, 32};
        int map_sizes[] = {40, 20, 10}; // 320/8=40, 320/16=20, 320/32=10
        int8_t* cls_ptrs[] = {cls_8, cls_16, cls_32};
        int8_t* reg_ptrs[] = {reg_8, reg_16, reg_32};
        float s_cls[] = {s_cls_8, s_cls_16, s_cls_32};
        float z_cls[] = {z_cls_8, z_cls_16, z_cls_32};
        float s_reg[] = {s_reg_8, s_reg_16, s_reg_32};
        float z_reg[] = {z_reg_8, z_reg_16, z_reg_32};

        for (int s = 0; s < 3; s++) {
            int stride = strides[s];
            int map_size = map_sizes[s];

            for (int y = 0; y < map_size; y++) {
                for (int x = 0; x < map_size; x++) {
                    int idx = y * map_size + x;

                    // 1. 读取并反量化置信度
                    float conf = ((float)cls_ptrs[s][idx] - z_cls[s]) * s_cls[s];

                    // 阈值过滤 (YuNet 建议 0.4 到 0.6)
                    if (conf > 0.5f) {
                        // 2. 读取并反量化 BBox 偏移量
                        float dx = ((float)reg_ptrs[s][idx * 4 + 0] - z_reg[s]) * s_reg[s];
                        float dy = ((float)reg_ptrs[s][idx * 4 + 1] - z_reg[s]) * s_reg[s];
                        float dw = ((float)reg_ptrs[s][idx * 4 + 2] - z_reg[s]) * s_reg[s];
                        float dh = ((float)reg_ptrs[s][idx * 4 + 3] - z_reg[s]) * s_reg[s];

                        // 3. Anchor 解码公式，还原为 320x320 尺寸下的真实像素坐标
                        float cx = (x + dx) * stride;
                        float cy = (y + dy) * stride;
                        float w  = expf(dw) * stride;
                        float h  = expf(dh) * stride;

                        // 4. 存入你的 boxes 数组
                        if(valid_count < 2100) {
                            boxes[valid_count].x1 = cx - w / 2.0f;
                            boxes[valid_count].y1 = cy - h / 2.0f;
                            boxes[valid_count].x2 = cx + w / 2.0f;
                            boxes[valid_count].y2 = cy + h / 2.0f;
                            boxes[valid_count].conf = conf;
                            boxes[valid_count].keep = 1;
                            valid_count++;
                        }
                    }
                }
            }
        }

        // =====================================================================
        // 【关键修复 4】：做 NMS 前，必须按置信度从大到小排序
        // =====================================================================
        for (int i = 0; i < valid_count - 1; i++) {
            for (int j = 0; j < valid_count - i - 1; j++) {
                if (boxes[j].conf < boxes[j + 1].conf) {
                    Box temp = boxes[j];
                    boxes[j] = boxes[j + 1];
                    boxes[j + 1] = temp;
                }
            }
        }

        // 你的 NMS 代码 (基本保持不变，建议将 IoU 阈值从 0.7 降为 0.45 以免重叠框剔除失败)
        for (int i = 0; i < valid_count; i++) {
            if(boxes[i].keep){
                for(int j = i + 1; j < valid_count; j++){
                    if(boxes[j].keep){
                        float x1 = (boxes[i].x1 > boxes[j].x1) ? boxes[i].x1 : boxes[j].x1;
                        float y1 = (boxes[i].y1 > boxes[j].y1) ? boxes[i].y1 : boxes[j].y1;
                        float x2 = (boxes[i].x2 < boxes[j].x2) ? boxes[i].x2 : boxes[j].x2;
                        float y2 = (boxes[i].y2 < boxes[j].y2) ? boxes[i].y2 : boxes[j].y2;
                        float intersection = (x2 - x1) * (y2 - y1);
                        if (intersection < 0) intersection = 0;
                        float area_i = (boxes[i].x2 - boxes[i].x1) * (boxes[i].y2 - boxes[i].y1);
                        float area_j = (boxes[j].x2 - boxes[j].x1) * (boxes[j].y2 - boxes[j].y1);
                        float union_area = area_i + area_j - intersection;
                        float iou = (union_area > 0) ? (intersection / union_area) : 0;

                        if(iou > 0.45f){ // 【微调】：目标检测通用 NMS 阈值为 0.45 左右
                            boxes[j].keep = 0;
                        }
                    }
                }
            }
        }

        // =====================================================================
        // 后续你的 DMA2D 画图代码 (完全保留，未做任何修改！)
        // =====================================================================
        int final_count = 0;
        hdma2d.Init.Mode = DMA2D_R2M;
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
        hdma2d.Init.OutputOffset = 0;
        if (HAL_DMA2D_Init(&hdma2d) != HAL_OK) { }

        HAL_DMA2D_ConfigLayer(&hdma2d,1);
        HAL_DMA2D_Start(&hdma2d, 0x00000000, (uint32_t)g_ltdc_layer2_framebuf, 800, 480);
        HAL_DMA2D_PollForTransfer(&hdma2d, 1000);

        for (int i = 0; i < valid_count; i++) {
            if(boxes[i].keep){
                final_count++;
                int display_x1 = (int)(boxes[i].x1 * 2.5f);
                int display_y1 = (int)(boxes[i].y1 * 1.5f);
                int display_x2 = (int)(boxes[i].x2 * 2.5f);
                int display_y2 = (int)(boxes[i].y2 * 1.5f);

                // ... (此处省略你原有的边界限制和 4 条边的 DMA2D 绘制代码，直接沿用即可) ...
                int display_width = display_x2 - display_x1;
                int display_height = display_y2 - display_y1;

                if (display_x1 < 0) display_x1 = 0;
                if (display_y1 < 0) display_y1 = 0;
                if (display_x2 > 800) display_x2 = 800;
                if (display_y2 > 480) display_y2 = 480;
                display_width = display_x2 - display_x1;
                display_height = display_y2 - display_y1;
                if (display_width < 0) display_width = 0;
                if (display_height < 0) display_height = 0;
                if (display_width > 0 && display_height > 0) {
                    if (display_y1 >= 480) display_y1 = 479;
                    if (display_y2 > 480) display_y2 = 480;
                    if (display_x1 >= 800) display_x1 = 799;
                    if (display_x2 > 800) display_x2 = 800;
                    display_width = display_x2 - display_x1;
                    display_height = display_y2 - display_y1;
                    if (display_width <= 0 || display_height <= 0) continue;

                    if (display_y1 < 480) {
                        hdma2d.Init.Mode = DMA2D_R2M;
                        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
                        hdma2d.Init.OutputOffset = 800 - display_width;
                        hdma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR;
                        HAL_DMA2D_Init(&hdma2d);
                        HAL_DMA2D_ConfigLayer(&hdma2d, 1);
                        HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + display_x1) * 3], display_width, 1);
                        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                    }
                    if (display_y2  <= 480 && display_y2 > display_y1) {
                        hdma2d.Init.OutputOffset = 800 - display_width;
                        HAL_DMA2D_Init(&hdma2d);
                        HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[((display_y2 - 1) * 800 + display_x1) * 3], display_width, 1);
                        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                    }
                    if (display_x1  < 800) {
                        hdma2d.Init.OutputOffset = 800 - 1;
                        HAL_DMA2D_Init(&hdma2d);
                        HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + display_x1) * 3], 1, display_height);
                        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                    }
                    if (display_x2 <= 800 && display_x2 > display_x1) {
                        hdma2d.Init.OutputOffset = 800 - 1;
                        HAL_DMA2D_Init(&hdma2d);
                        HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + (display_x2 - 1)) * 3], 1, display_height);
                        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                    }
                }
            }
        }

        LL_ATON_RT_Reset_Network(&NN_Instance_Default);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
