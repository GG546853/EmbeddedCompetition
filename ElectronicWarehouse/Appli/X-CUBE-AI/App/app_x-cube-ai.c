
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
#include <math.h>

#define MAX_BOXES 100
typedef struct {
    float x1, y1, x2, y2;
    float conf;
    int keep;
} Box_t;

extern uint8_t g_ai_cam_buf[224 * 224 * 3];
extern uint16_t g_ltdc_layer2_framebuf[480 * 800];
extern DCMIPP_HandleTypeDef hdcmipp;
extern osSemaphoreId_t cam_frame_sem; // 确保在 main.c 中创建了这个信号量并在此声明

static inline float sigmoid(float x) {
    return 1.0f / (1.0f + expf(-x));
}

 static const float anchors[10] = {0.9883f, 3.3606f, 2.1194f, 5.3759f, 3.0520f, 9.1336f, 5.5517f, 9.3066f, 9.7260f, 11.1422f};
 static Box_t boxes[MAX_BOXES];

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
    uint32_t buff_out_len;


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

    HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)g_ai_cam_buf, DCMIPP_MODE_SNAPSHOT);

    if(osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(100)) != osOK) {
        return;
    }
    SCB_InvalidateDCache_by_Addr((uint32_t*)g_ai_cam_buf, 224 * 224 * 3);

    memcpy(buffer_in, g_ai_cam_buf, 224 * 224 * 3);
    SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    do {
       ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
       if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
         LL_ATON_OSAL_WFE();
       }
     } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

    uint32_t aligned_out_len = ((buff_out_len + 31) / 32) * 32;
    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_out, aligned_out_len);

    // 清空 LCD 图层2，准备画新框
    memset(g_ltdc_layer2_framebuf, 0, sizeof(g_ltdc_layer2_framebuf));

    // 6. YOLOv2 后处理解码
    // 输出形状: 1 x 7 x 7 x 30 (grid=7x7, 5个锚框，每个锚框6个值: tx, ty, tw, th, obj, cls)
    float *f32_out = (float *)buffer_out;
    int valid_count = 0;

    for (int cy = 0; cy < 7; cy++) {
        for (int cx = 0; cx < 7; cx++) {
            for (int a = 0; a < 5; a++) {
                // 计算当前锚框在数组中的索引
                int index = (cy * 7 * 30) + (cx * 30) + (a * 6);

                // YOLOv2 解码逻辑
                float tx = f32_out[index + 0];
                float ty = f32_out[index + 1];
                float tw = f32_out[index + 2];
                float th = f32_out[index + 3];
                float to = f32_out[index + 4]; // 物体置信度
                float tc = f32_out[index + 5]; // 类别置信度 (人)

                // 目标存在概率
                float obj_score = sigmoid(to);
                float cls_score = sigmoid(tc);
                float final_score = obj_score * cls_score;

                // 阈值过滤
                if (final_score > 0.45f && valid_count < MAX_BOXES) {

                    // 计算在 7x7 网格上的相对中心坐标
                    float bx = (sigmoid(tx) + cx) / 7.0f;
                    float by = (sigmoid(ty) + cy) / 7.0f;

                    // 计算相对宽和高
                    float bw = (anchors[a * 2 + 0] * expf(tw)) / 7.0f;
                    float bh = (anchors[a * 2 + 1] * expf(th)) / 7.0f;

                    // 存入结构体，转换为归一化的左上角和右下角坐标 (0.0 ~ 1.0)
                    boxes[valid_count].x1 = bx - bw / 2.0f;
                    boxes[valid_count].y1 = by - bh / 2.0f;
                    boxes[valid_count].x2 = bx + bw / 2.0f;
                    boxes[valid_count].y2 = by + bh / 2.0f;
                    boxes[valid_count].conf = final_score;
                    boxes[valid_count].keep = 1;
                    valid_count++;
                }
            }
        }
    }

    // 7. NMS (非极大值抑制)
    for (int i = 0; i < valid_count; i++) {
        if (boxes[i].keep) {
            for (int j = i + 1; j < valid_count; j++) {
                if (boxes[j].keep) {
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

                    // IOU 阈值 45%
                    if (iou > 0.45f) {
                        // 谁置信度小，就淘汰谁
                        if (boxes[i].conf > boxes[j].conf) {
                            boxes[j].keep = 0;
                        } else {
                            boxes[i].keep = 0;
                            break;
                        }
                    }
                }
            }
        }
    }

    // 8. 映射到 LCD (800x480) 并绘制边界框
    for (int i = 0; i < valid_count; i++) {
        if (boxes[i].keep) {
            int display_x1 = (int)(boxes[i].x1 * 800.0f);
            int display_y1 = (int)(boxes[i].y1 * 480.0f);
            int display_x2 = (int)(boxes[i].x2 * 800.0f);
            int display_y2 = (int)(boxes[i].y2 * 480.0f);

            if (display_x1 < 0) display_x1 = 0;
            if (display_y1 < 0) display_y1 = 0;
            if (display_x2 > 799) display_x2 = 799;
            if (display_y2 > 479) display_y2 = 479;

            int w_lcd = display_x2 - display_x1;
            int h_lcd = display_y2 - display_y1;

            if (w_lcd > 0 && h_lcd > 0) {
                // 画 3 个像素厚度的红框，保证瞎子都能看见
                for (int thick = 0; thick < 3; thick++) {
                    if (display_x1 + thick < display_x2 && display_y1 + thick < display_y2) {
                        rgblcd_layer2_draw_rect(
                            (uint16_t)(display_x1 + thick),
                            (uint16_t)(display_y1 + thick),
                            (uint16_t)(w_lcd - thick * 2),
                            (uint16_t)(h_lcd - thick * 2),
                            0xF800 // 红色
                        );
                    }
                }
            }
        }
    }

    LL_ATON_RT_Reset_Network(&NN_Instance_Default);
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
