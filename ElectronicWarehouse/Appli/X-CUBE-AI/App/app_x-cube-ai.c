
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
     float x1, y1, x2, y2;   // 矩形框
     float score;            // 最终得分 (cls * obj)
     struct {
         float x, y;
     } landmarks[5];         // 5个关键点
     int keep;               // NMS 标志位
 } FaceDetection;

 typedef struct {
     int stride;
     int map_size;
     // 6个Tensor的索引 (基于12个输出的典型排列)
     int cls_idx; // 分类
     int obj_idx; // 目标存在感
     int reg_idx; // 框回归
     int kps_idx; // 关键点回归
 } YuNetLayer;

 float scales[12] = {
     0.003292752, 0.003212299, 0.002707064, // 0,1,2: Cls
     0.003796757, 0.003871595, 0.000669989, // 3,4,5: Obj  <-- 已修复
     0.011400674, 0.011022569, 0.012532321, // 6,7,8: Reg
     0.019418273, 0.015351823, 0.016416648  // 9,10,11:Kps <-- 已修复
 };
 int32_t zps[12] = {-128, -128, -128, -128, -128, -128, -61, -46, -16, -26, -33, -45};

#define CONF_THRESH 0.5f
#define MAX_CANDIDATES 1024


 //extern uint8_t g_ai_cam_buf[];
 extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];
 extern DCMIPP_HandleTypeDef hdcmipp;
 extern osSemaphoreId_t cam_frame_sem; // 确保在 main.c 中创建了这个信号量并在此声明
 extern DMA2D_HandleTypeDef hdma2d;
 extern uint8_t g_ai_cam_buf[320 * 320 * 3];

  __attribute__((section(".camera_buf")))
 __attribute__((aligned(32)))

 FaceDetection boxes[2100];

  int yunet_decode(const LL_Buffer_InfoTypeDef* obuffersInfos, FaceDetection* results) {
      int valid_count = 0;

      // YuNet 的三个尺度定义
      YuNetLayer layers[3] = {
          {8,  40, 0, 3, 6, 9},   // Stride 8:  cls=0, obj=3, reg=6, kps=9
          {16, 20, 1, 4, 7, 10},  // Stride 16: cls=1, obj=4, reg=7, kps=10
          {32, 10, 2, 5, 8, 11}   // Stride 32: cls=2, obj=5, reg=8, kps=11
      };

      for (int l = 0; l < 3; l++) {
          YuNetLayer layer = layers[l];
          int8_t *p_cls = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[layer.cls_idx]);
          int8_t *p_obj = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[layer.obj_idx]);
          int8_t *p_reg = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[layer.reg_idx]);
          int8_t *p_kps = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[layer.kps_idx]);

          for (int y = 0; y < layer.map_size; y++) {
              for (int x = 0; x < layer.map_size; x++) {
                  int idx = y * layer.map_size + x;

                  // 1. 反量化得分并计算最终 Score = cls * obj
                  float score_cls = (float)(p_cls[idx] - zps[layer.cls_idx]) * scales[layer.cls_idx];
                  float score_obj = (float)(p_obj[idx] - zps[layer.obj_idx]) * scales[layer.obj_idx];
                  float final_score = score_cls * score_obj;

                  if (final_score > CONF_THRESH) {

                      if (valid_count >= MAX_CANDIDATES) break;

                      // 2. 解码边界框 (BBox)
                      // dx, dy, dw, dh
                      float dx = (float)(p_reg[idx * 4 + 0] - zps[layer.reg_idx]) * scales[layer.reg_idx];
                      float dy = (float)(p_reg[idx * 4 + 1] - zps[layer.reg_idx]) * scales[layer.reg_idx];
                      float dw = (float)(p_reg[idx * 4 + 2] - zps[layer.reg_idx]) * scales[layer.reg_idx];
                      float dh = (float)(p_reg[idx * 4 + 3] - zps[layer.reg_idx]) * scales[layer.reg_idx];

                      // 中心点解码公式: (grid_index + offset) * stride
                      float cx = (x + dx) * layer.stride;
                      float cy = (y + dy) * layer.stride;
                      float w  = expf(dw) * layer.stride;
                      float h  = expf(dh) * layer.stride;

                      results[valid_count].x1 = cx - w / 2.0f;
                      results[valid_count].y1 = cy - h / 2.0f;
                      results[valid_count].x2 = cx + w / 2.0f;
                      results[valid_count].y2 = cy + h / 2.0f;
                      results[valid_count].score = final_score;
                      results[valid_count].keep = 1;

                      // 3. 解码 5 个关键点 (Landmarks)
                      for (int n = 0; n < 5; n++) {
                          float kpx = (float)(p_kps[idx * 10 + n * 2 + 0] - zps[layer.kps_idx]) * scales[layer.kps_idx];
                          float kpy = (float)(p_kps[idx * 10 + n * 2 + 1] - zps[layer.kps_idx]) * scales[layer.kps_idx];
                          results[valid_count].landmarks[n].x = (x + kpx) * layer.stride;
                          results[valid_count].landmarks[n].y = (y + kpy) * layer.stride;
                      }

                      valid_count++;
                  }
              }
          }
      }
      return valid_count;
  }

  void do_nms(FaceDetection* boxes, int count, float iou_thresh) {
      // 1. 冒泡排序（按 score 从高到低）
      for (int i = 0; i < count - 1; i++) {
          for (int j = 0; j < count - i - 1; j++) {
              if (boxes[j].score < boxes[j+1].score) {
                  FaceDetection temp = boxes[j];
                  boxes[j] = boxes[j+1];
                  boxes[j+1] = temp;
              }
          }
      }

      // 2. IoU 计算与剔除
      for (int i = 0; i < count; i++) {
          if (!boxes[i].keep) continue;
          for (int j = i + 1; j < count; j++) {
              if (!boxes[j].keep) continue;

              float xx1 = fmaxf(boxes[i].x1, boxes[j].x1);
              float yy1 = fmaxf(boxes[i].y1, boxes[j].y1);
              float xx2 = fminf(boxes[i].x2, boxes[j].x2);
              float yy2 = fminf(boxes[i].y2, boxes[j].y2);

              float w = fmaxf(0, xx2 - xx1);
              float h = fmaxf(0, yy2 - yy1);
              float inter = w * h;
              if (inter > 0) {
                  float area_i = (boxes[i].x2 - boxes[i].x1) * (boxes[i].y2 - boxes[i].y1);
                  float area_j = (boxes[j].x2 - boxes[j].x1) * (boxes[j].y2 - boxes[j].y1);
                  float iou = inter / (area_i + area_j - inter);
                  if (iou > iou_thresh) boxes[j].keep = 0;
              }
          }
      }
  }

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

        for (int i = 0; i < 12; i++) {
            uint32_t addr = (uint32_t)LL_Buffer_addr_start(&obuffersInfos[i]);
            uint32_t len = obuffersInfos[i].offset_end - obuffersInfos[i].offset_start;
            SCB_InvalidateDCache_by_Addr((uint32_t*)addr, len);
        }
        for (int i = 0; i < 12; i++) {
            uint32_t addr = (uint32_t)LL_Buffer_addr_start(&obuffersInfos[i]);

            int8_t *ptr = (int8_t *)LL_Buffer_addr_start(&obuffersInfos[i]);
            printf("Tensor [%d] (len %ld): %d, %d, %d, %d\n\r",
                       i,
                       obuffersInfos[i].offset_end - obuffersInfos[i].offset_start,
                       ptr[0], ptr[1], ptr[2], ptr[3]);

            uint32_t len = obuffersInfos[i].offset_end - obuffersInfos[i].offset_start;
            SCB_InvalidateDCache_by_Addr((uint32_t*)addr, len);
        }

        int count = yunet_decode(obuffersInfos, boxes);
        printf("Valid detection candidates: %d\n\r", count);
        do_nms(boxes, count, 0.45f);

        int final_count = 0;

        hdma2d.Init.Mode = DMA2D_R2M;                 // 寄存器到内存(纯色填充)
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;  // 设定输出颜色格式
        hdma2d.Init.OutputOffset = 0;                 // 【关键】重置偏移量为0，否则清屏会错乱
        if (HAL_DMA2D_Init(&hdma2d) != HAL_OK) {
            // 初始化错误处理
        }

        HAL_DMA2D_ConfigLayer(&hdma2d,1);
        HAL_DMA2D_Start(&hdma2d, 0x00000000, (uint32_t)g_ltdc_layer2_framebuf, 800, 480);
        HAL_DMA2D_PollForTransfer(&hdma2d, 1000);

        for (int i = 0; i < count; i++) {
        	if(boxes[i].keep){
        		final_count++;
        		int display_x1 = (int)(boxes[i].x1 * 2.5);
        		int display_y1 = (int)(boxes[i].y1 * 1.5);
        		int display_x2 = (int)(boxes[i].x2 * 2.5);
        		int display_y2 = (int)(boxes[i].y2 * 1.5);
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
            //vTaskDelay(pdMS_TO_TICKS(10));
    }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
