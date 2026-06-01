
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_AI_H
#define __APP_AI_H
#ifdef __cplusplus
extern "C" {
#endif
/**
  ******************************************************************************
  * @file    app_x-cube-ai.h
  * @author  X-CUBE-AI C code generator
  * @brief   AI entry function definitions
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
/* Includes ------------------------------------------------------------------*/
#include "npu_cache.h"
#include "ll_aton_runtime.h"

void MX_X_CUBE_AI_Init(void);
void MX_X_CUBE_AI_Process(void);
/* USER CODE BEGIN includes */
/* Post-processing parameters -----------------------------------------------*/
#define AI_FD_MAX_DETECTIONS   3
#define AI_FD_NUM_KEYPOINTS    6
#define AI_FD_IMG_SIZE         128
#define AI_FD_CONF_THRESHOLD   0.5f
#define AI_FD_IOU_THRESHOLD    0.5f
#define AI_FD_GRID_0_NB_BOXES  512
#define AI_FD_GRID_1_NB_BOXES  384

/* Detection result structures ----------------------------------------------*/
typedef struct {
    float x_center, y_center;
    float width, height;
    float confidence;
    float keypoints[6][2];
} ai_detection_t;

typedef struct {
    ai_detection_t detections[AI_FD_MAX_DETECTIONS];
    uint32_t nb_detect;
} ai_result_t;



extern uint8_t *buffer_in;


void MX_X_CUBE_AI_Process_User(ai_result_t *result);
/* USER CODE END includes */
#ifdef __cplusplus
}
#endif
#endif /*__STMicroelectronics_X-CUBE-AI_10_2_0_H */
