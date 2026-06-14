
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
#include "ai_platform.h"


/* Post-processing parameters -----------------------------------------------*/
#define AI_FD_MAX_DETECTIONS   1
#define AI_FD_NUM_KEYPOINTS    6
#define AI_FD_IMG_SIZE         128
#define AI_FD_CONF_THRESHOLD   0.5f
#define AI_FD_IOU_THRESHOLD    0.3f
#define AI_FD_GRID_0_NB_BOXES  512
#define AI_FD_GRID_1_NB_BOXES  384
#define AI_FD_BOX_SCALE_W      1.00f
#define AI_FD_BOX_SCALE_H      1.20f
#define AI_FD_EMA_ALPHA        0.30f
#define AI_FD_TRACK_IOU_THRESH 0.50f

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
extern uint8_t *buffer_in_fc;
extern uint8_t *buffer_out_fc;

void MX_X_CUBE_AI_Process_User(ai_result_t *result);

/* Face crop + resize: 800x480 RGB888 display buffer -> 112x112 RGB888 */
void ai_crop_resize_face_112(uint8_t *src_fb, ai_detection_t *det, uint8_t *output);

/* --- Face Re-ID definitions ------------------------------------------------ */
#define FACE_EMBEDDING_DIM     128
#define FACE_GALLERY_MAX        10
#define FACE_MATCH_THRESHOLD    1.0f   /* L2-normalised distance in [0,2]; <1.0 ≈ cos_sim >0.5 */
#define FACE_RATIO_THRESHOLD    0.75f  /* best_dist / second_dist; <0.75 means confident match */
#define FACE_CONFIDENT_DIST     0.3f   /* below this distance, skip ratio check entirely */
#define FACE_NAME_MAX           16

typedef struct {
    float embedding[FACE_EMBEDDING_DIM];
    char name[FACE_NAME_MAX];
} face_entry_t;

/* Quantize uint8 RGB [0,255] → int8 matching network_fc input scale */
void quantize_u8_to_s8(const uint8_t *u8, int8_t *s8, uint32_t count);

/* Run network_fc inference: must have face crop already in buffer_in_fc */
void ai_face_reid_run(float *embedding_out);

/* Gallery management */
int  ai_face_enroll(const float *embedding, const char *name);
int  ai_face_identify(const float *embedding, char *name_out, float *dist_out);
void ai_face_gallery_print(void);
/* USER CODE END includes */
#ifdef __cplusplus
}
#endif
#endif /*__STMicroelectronics_X-CUBE-AI_10_2_0_H */
