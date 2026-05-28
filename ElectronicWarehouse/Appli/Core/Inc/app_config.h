#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#include "arm_math.h"

/* Model Post-Processing Type */
#define POSTPROCESS_TYPE    POSTPROCESS_FD_BLAZEFACE_UI

/* BlazeFace PP Parameters */
#define AI_FD_BLAZEFACE_PP_NB_KEYPOINTS      (6)
#define AI_FD_BLAZEFACE_PP_NB_CLASSES        (1)
#define AI_FD_BLAZEFACE_PP_IMG_SIZE          (128)
#define AI_FD_BLAZEFACE_PP_OUT_0_NB_BOXES    (512)
#define AI_FD_BLAZEFACE_PP_OUT_1_NB_BOXES    (384)
#define AI_FD_BLAZEFACE_PP_MAX_BOXES_LIMIT   (3)
#define AI_FD_BLAZEFACE_PP_CONF_THRESHOLD    (0.8)
#define AI_FD_BLAZEFACE_PP_IOU_THRESHOLD     (0.5)

/* Display */
#define COLOR_BGR  (0)
#define COLOR_RGB  (1)
#define COLOR_MODE COLOR_RGB

#endif /* __APP_CONFIG_H */
