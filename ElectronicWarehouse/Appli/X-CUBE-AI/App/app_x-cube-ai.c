
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
#include "fd_blazeface_pp_f32.h"
#include "fd_blazeface_anchors_0.h"
#include "fd_blazeface_anchors_1.h"

 extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];
 extern DCMIPP_HandleTypeDef hdcmipp;
 extern osSemaphoreId_t cam_frame_sem; // 确保在 main.c 中创建了这个信号量并在此声明
 extern DMA2D_HandleTypeDef hdma2d;

 uint8_t g_ai_cam_buf[128 * 128 * 3];

#define FD_TOTAL_ANCHORS (512 + 384)
static fd_pp_outBuffer_t pp_out_detections[FD_TOTAL_ANCHORS];
static fd_pp_keyPoints_t pp_out_keypoints[FD_TOTAL_ANCHORS * 6];
static fd_pp_out_t pp_output;
static fd_blazeface_pp_f32_static_param_t pp_params;
static int pp_initialized = 0;

static void _postprocess_init(void)
{
  if (pp_initialized) return;

  pp_params.nb_keypoints     = 6;
  pp_params.nb_detections_0  = 512;
  pp_params.nb_detections_1  = 384;
  pp_params.in_size          = 128;
  pp_params.max_boxes_limit  = 5;
  pp_params.conf_threshold   = 0.5f;
  pp_params.iou_threshold    = 0.5f;
  pp_params.pAnchors_0       = g_Anchors_0;
  pp_params.pAnchors_1       = g_Anchors_1;

  for (int i = 0; i < FD_TOTAL_ANCHORS; i++) {
    pp_out_detections[i].pKeyPoints = &pp_out_keypoints[i * 6];
  }
  pp_output.pOutBuff = pp_out_detections;

  pp_initialized = 1;
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

  /* Get all 4 output buffers (order as declared in network.h) */
  /* OUT sizes: [0]=32768, [1]=2048, [2]=1536, [3]=24576 */
  /* Sort by ascending size to pair: scores_1(1536), scores_0(2048), boxes_1(24576), boxes_0(32768) */
  uint8_t *out_bufs[4];
  size_t out_sizes[4];
  for (int i = 0; i < 4; i++) {
    out_bufs[i]  = (uint8_t *)LL_Buffer_addr_start(&obuffersInfos[i]);
    out_sizes[i] = obuffersInfos[i].offset_end - obuffersInfos[i].offset_start;
  }

  /* Simple sort by size: smallest = index 0 → scores stride 16 (384x1), then stride 8 (512x1), then boxes */
  int size_idx[4] = {0, 1, 2, 3};
  for (int i = 0; i < 4; i++) {
    for (int j = i + 1; j < 4; j++) {
      if (out_sizes[size_idx[i]] > out_sizes[size_idx[j]]) {
        int tmp = size_idx[i];
        size_idx[i] = size_idx[j];
        size_idx[j] = tmp;
      }
    }
  }

  LL_ATON_RT_RuntimeInit();

  buff_in_len = ibuffersInfos->offset_end - ibuffersInfos->offset_start;

  SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
  SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

  if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)buffer_in, DCMIPP_MODE_SNAPSHOT) != HAL_OK) {
      printf("ERROR: DCMIPP PIPE2 Start Failed!\r\n");
  }

  if(osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(500)) != osOK)
  	while(1);

  SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

  /* Run 1 inference */
  LL_ATON_RT_Init_Network(&NN_Instance_Default);
  do {
    ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
    if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
      LL_ATON_OSAL_WFE();
    }
  } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

  /* Invalidate output buffers from NPU cache */
  for (int i = 0; i < 4; i++) {
    uint32_t obuf_len = obuffersInfos[i].offset_end - obuffersInfos[i].offset_start;
    SCB_InvalidateDCache_by_Addr((uint32_t *)out_bufs[i], obuf_len);
  }

  /* ====== DIAGNOSTIC: raw output tensor statistics ====== */
  {
    float32_t *pScores_1 = (float32_t *)out_bufs[size_idx[0]];  /* 384 floats */
    float32_t *pScores_0 = (float32_t *)out_bufs[size_idx[1]];  /* 512 floats */
    float32_t *pBoxes_1  = (float32_t *)out_bufs[size_idx[2]];  /* 384*16 floats */
    float32_t *pBoxes_0  = (float32_t *)out_bufs[size_idx[3]];  /* 512*16 floats */

    /* Helper macro: stats for 1D tensor */
    #define TENSOR_STATS(name, data, n) do { \
      float32_t mn = (data)[0], mx = (data)[0]; \
      double s = 0.0; int z = 0; \
      for (int _i = 0; _i < (n); _i++) { \
        float32_t v = (data)[_i]; \
        if (v < mn) mn = v; if (v > mx) mx = v; \
        s += v; if (v == 0.0f) z++; \
      } \
      printf("  %-16s  min=% .4f  max=% .4f  mean=% .4f  zeros=%d/%d\r\n", \
             name, mn, mx, (float)(s/(n)), z, (n)); \
    } while(0)

    /* Helper macro: per-channel stats for box tensor (16 channels) */
    #define BOX_CH_STATS(label, data, n_det) do { \
      printf("  %s per-ch min/max/mean:\r\n", label); \
      const char *ch_names[16] = {"cx","cy","w","h","kp0x","kp0y","kp1x","kp1y", \
                                   "kp2x","kp2y","kp3x","kp3y","kp4x","kp4y","kp5x","kp5y"}; \
      for (int _ch = 0; _ch < 16; _ch++) { \
        float32_t mn = (data)[_ch], mx = (data)[_ch]; \
        double s = 0.0; int same = 0; \
        for (int _i = 0; _i < (n_det); _i++) { \
          float32_t v = (data)[_i * 16 + _ch]; \
          if (v < mn) mn = v; if (v > mx) mx = v; s += v; \
          if (v == (data)[_ch]) same++; \
        } \
        printf("    %s: min=% .4f max=% .4f mean=% .4f  same_as_1st=%d/%d\r\n", \
               ch_names[_ch], mn, mx, (float)(s/(n_det)), same, (n_det)); \
      } \
    } while(0)

    TENSOR_STATS("Scores_1 (384)", pScores_1, 384);
    TENSOR_STATS("Scores_0 (512)", pScores_0, 512);
    BOX_CH_STATS("Boxes_1 (384x16) ", pBoxes_1, 384);
    BOX_CH_STATS("Boxes_0 (512x16) ", pBoxes_0, 512);

    #undef TENSOR_STATS
    #undef BOX_CH_STATS
  }
  /* ====== END DIAGNOSTIC ====== */

  /* Post-processing: map sorted outputs → fd_blazeface_pp_f32_in_t */
  _postprocess_init();

  fd_blazeface_pp_f32_in_t pp_input;
  /* size_idx sorted ascending: [0] smallest → scores_1, [1] → scores_0, [2] → boxes_1, [3] → boxes_0 */
  pp_input.pScores_1    = (float32_t *)out_bufs[size_idx[0]];  /* 1536 B = 384 floats */
  pp_input.pScores_0    = (float32_t *)out_bufs[size_idx[1]];  /* 2048 B = 512 floats */
  pp_input.pRawBoxes_1  = (float32_t *)out_bufs[size_idx[2]];  /* 24576 B = 384*16 floats */
  pp_input.pRawBoxes_0  = (float32_t *)out_bufs[size_idx[3]];  /* 32768 B = 512*16 floats */

  fd_blazeface_pp_process_f32(&pp_input, &pp_output, &pp_params);

  LL_ATON_RT_DeInit_Network(&NN_Instance_Default);
  LL_ATON_RT_RuntimeDeInit();

  /* Print detection results */
  if (pp_output.nb_detect > 0) {
    printf("FD: %ld face(s) detected\r\n", pp_output.nb_detect);
    for (int32_t i = 0; i < pp_output.nb_detect; i++) {
      printf("  [%ld] x=%.3f y=%.3f w=%.3f h=%.3f conf=%.2f\r\n",
             i,
             pp_output.pOutBuff[i].x_center,
             pp_output.pOutBuff[i].y_center,
             pp_output.pOutBuff[i].width,
             pp_output.pOutBuff[i].height,
             pp_output.pOutBuff[i].conf);
    }
  }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
