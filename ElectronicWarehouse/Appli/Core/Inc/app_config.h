  #ifndef __APP_CONFIG_H
  #define __APP_CONFIG_H

  #include "stm32n6xx_hal.h"
  #include "postprocess_conf.h"

  /* 显示屏 */
  #define LCD_BG_WIDTH                            800
  #define LCD_BG_HEIGHT                           480
  #define LCD_FG_WIDTH                            LCD_BG_WIDTH
  #define LCD_FG_HEIGHT                           LCD_BG_HEIGHT

  /* 摄像头 */
  #define CAMERA_MIRROR_FLIP                      CMW_MIRRORFLIP_NONE

  /* 模型输入 */
  #define NN_WIDTH                                128
  #define NN_HEIGHT                               128
  #define NN_FORMAT                               DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1
  #define NN_BPP                                  3
  #define NN_BUFFER_OUT_SIZE                      (4 * 1024)  // 按 stedgeai 输出的实际大小填

  /* 后处理 */
  #define POSTPROCESS_TYPE                        POSTPROCESS_FD_BLAZEFACE_UI
  #define AI_FD_BLAZEFACE_PP_MAX_BOXES_LIMIT      (3)
  #define AI_FD_BLAZEFACE_PP_CONF_THRESHOLD       (0.8f)
  #define AI_FD_BLAZEFACE_PP_IOU_THRESHOLD        (0.5f)

  #endif /* __APP_CONFIG_H__ */
