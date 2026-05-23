#ifndef __FD_BLAZEFACE_PP_F32_H__
#define __FD_BLAZEFACE_PP_F32_H__

#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"
#include "arm_math.h"

/* Error codes */
#define AI_FD_PP_ERROR_NO        (0)
#define AI_FD_PP_ERROR           (-2)

/* Offsets in the 16-value box output per anchor */
#define FD_BLAZEFACE_XCENTER      (0)
#define FD_BLAZEFACE_YCENTER      (1)
#define FD_BLAZEFACE_WIDTH        (2)
#define FD_BLAZEFACE_HEIGHT       (3)
#define FD_BLAZEFACE_KEYPOINTS    (4)

typedef struct {
  float32_t x;
  float32_t y;
} fd_pp_keyPoints_t;

typedef struct {
  float32_t x_center;
  float32_t y_center;
  float32_t width;
  float32_t height;
  float32_t conf;
  int32_t   class_index;
  fd_pp_keyPoints_t *pKeyPoints;
} fd_pp_outBuffer_t;

typedef struct {
  fd_pp_outBuffer_t *pOutBuff;
  int32_t nb_detect;
} fd_pp_out_t;

/* Input: raw float32 model outputs (4 tensors) */
typedef struct {
  float32_t *pRawBoxes_0;    /* (1,512,16) bbox + keypoints stride 8  */
  float32_t *pScores_0;      /* (1,512,1)  confidence logits stride 8 */
  float32_t *pRawBoxes_1;    /* (1,384,16) bbox + keypoints stride 16 */
  float32_t *pScores_1;      /* (1,384,1)  confidence logits stride 16 */
} fd_blazeface_pp_f32_in_t;

/* Static configuration (no quantization params needed for float32 outputs) */
typedef struct {
  int32_t   nb_keypoints;
  int32_t   nb_detections_0;       /* 512 */
  int32_t   nb_detections_1;       /* 384 */
  int32_t   in_size;               /* 128 */
  int32_t   max_boxes_limit;
  float32_t conf_threshold;        /* sigmoid score threshold, e.g. 0.5 */
  float32_t iou_threshold;         /* NMS IoU threshold, e.g. 0.5 */
  int32_t   nb_detect;             /* internal: current detection count */
  const float32_t *pAnchors_0;     /* 1024 = 512 * 2 (x_center, y_center) */
  const float32_t *pAnchors_1;     /* 768  = 384 * 2 (x_center, y_center) */
} fd_blazeface_pp_f32_static_param_t;

/* Main post-processing entry point */
int32_t fd_blazeface_pp_process_f32(fd_blazeface_pp_f32_in_t *pInput,
                                    fd_pp_out_t *pOutput,
                                    fd_blazeface_pp_f32_static_param_t *pParams);

#ifdef __cplusplus
 }
#endif

#endif /* __FD_BLAZEFACE_PP_F32_H__ */
