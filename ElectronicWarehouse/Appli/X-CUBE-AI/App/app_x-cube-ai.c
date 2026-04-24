
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

#define GRID_SIZE    7
#define NUM_ANCHORS  5
#define ATTRS  6
#define NUM_CLASSES  1  // 人体检测模型通常只有1类
#define CONF_THRESH  0.5f
#define NMS_THRESH   0.45f

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

static inline float sigmoid(float x) {
    return 1.0f / (1.0f + expf(-x));
}

 static const float anchors[10] = {0.9883f, 3.3606f, 2.1194f, 5.3759f, 3.0520f, 9.1336f, 5.5517f, 9.3066f, 9.7260f, 11.1422f};

 __attribute__((section(".camera_buf"))) __attribute__((aligned(32))) Box boxes[2100];

 int result_count = 0;




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

    memset(buffer_in, 0xAA, buff_in_len);
    SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)buffer_in, DCMIPP_MODE_SNAPSHOT) != HAL_OK) {
        printf("ERROR: DCMIPP PIPE2 Start Failed!\r\n"); // 看看会不会打印这个错误
    }

    if(osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(100)) != osOK) {
    	printf("ERROR: Camera Timeout!\r\n");
        return;
    }

    //SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    do {
       ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
       if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
         LL_ATON_OSAL_WFE();
       }
     } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_out, buff_out_len);



    // 1. 获取输出缓冲区 (1x7x7x30 f32)
        float *out_data = (float *)buffer_out;
        result_count = 0;

        printf("--- NPU RAW Output Check ---\r\n");
        for(int i = 0; i < 10; i++) {
            printf("out_data[%d] = %f\r\n", i, out_data[i]);
        }
        printf("----------------------------\r\n");

        // 2. 解码 YOLO 输出
        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                for (int a = 0; a < NUM_ANCHORS; a++) {
                    // 计算索引：HWC 格式 (7, 7, 5 * 6)
                	int base = (y * GRID_SIZE + x) * (ATTRS * NUM_ANCHORS) + (a * ATTRS);

                    float tx = out_data[base + 0];
                    float ty = out_data[base + 1];
                    float tw = out_data[base + 2];
                    float th = out_data[base + 3];
                    float tc = out_data[base + 4];
                    float tclass = out_data[base + 5];

                    if (tw > 10.0f) tw = 10.0f;
                    else if (tw < -10.0f) tw = -10.0f;

                    if (th > 10.0f) th = 10.0f;
                    else if (th < -10.0f) th = -10.0f;

                    float obj_conf = 1.0f / (1.0f + expf(-tc));
                    float class_prob = 1.0f / (1.0f + expf(-tclass));
                    float conf = obj_conf * class_prob;
                    if (conf > 0.75f) {
                        // 解码中心点坐标 (Sigmoid后加上网格偏移，再除以网格总数归一化)
                        float bx = (1.0f / (1.0f + expf(-tx)) + x) / GRID_SIZE;
                        float by = (1.0f / (1.0f + expf(-ty)) + y) / GRID_SIZE;

                        // 解码宽高 (乘以对应的Anchor宽高的指数，再除以网格总数归一化)
                        float bw = (anchors[2 * a] * expf(tw)) / GRID_SIZE;
                        float bh = (anchors[2 * a + 1] * expf(th)) / GRID_SIZE;

                        // 将归一化坐标(0~1)映射到模型输入分辨率 (224x224)
                        float cx_input = bx * 224.0f;
                        float cy_input = by * 224.0f;
                        float w_input  = bw * 224.0f;
                        float h_input  = bh * 224.0f;

                        // 存入 boxes 结构体，格式转换为 (左上角x1,y1, 右下角x2,y2)
                        if (result_count < 2100) { // 确保不超过结构体数组上限
                            boxes[result_count].x1 = cx_input - w_input / 2.0f;
                            boxes[result_count].y1 = cy_input - h_input / 2.0f;
                            boxes[result_count].x2 = cx_input + w_input / 2.0f;
                            boxes[result_count].y2 = cy_input + h_input / 2.0f;
                            boxes[result_count].conf = conf;
                            boxes[result_count].keep = 1;
                            result_count++;
                        }
                    }

                }
            }
        }

        for (int i = 0; i < result_count - 1; i++) {
            for (int j = i + 1; j < result_count; j++) {
                if (boxes[i].conf < boxes[j].conf) {
                    Box temp = boxes[i];
                    boxes[i] = boxes[j];
                    boxes[j] = temp;
                }
            }
        }

        for (int i = 0; i < result_count; i++) {
             if (boxes[i].keep) {
                 for (int j = i + 1; j < result_count; j++) {
                     if (boxes[j].keep) {
                         float x1 = (boxes[i].x1 > boxes[j].x1) ? boxes[i].x1 : boxes[j].x1;
                         float y1 = (boxes[i].y1 > boxes[j].y1) ? boxes[i].y1 : boxes[j].y1;
                         float x2 = (boxes[i].x2 < boxes[j].x2) ? boxes[i].x2 : boxes[j].x2;
                         float y2 = (boxes[i].y2 < boxes[j].y2) ? boxes[i].y2 : boxes[j].y2;

                         float inter_w = x2 - x1;
                         float inter_h = y2 - y1;
                         float intersection = 0.0f;

                         if (inter_w > 0 && inter_h > 0) {
                             intersection = inter_w * inter_h;
                         }

                         float area_i = (boxes[i].x2 - boxes[i].x1) * (boxes[i].y2 - boxes[i].y1);
                         float area_j = (boxes[j].x2 - boxes[j].x1) * (boxes[j].y2 - boxes[j].y1);
                         float union_area = area_i + area_j - intersection;
                         float iou = (union_area > 0) ? (intersection / union_area) : 0;

                         if (iou > 0.45f) {
                             boxes[j].keep = 0;
                         }
                     }
                 }
             }
         }

        // ================== 映射并在屏幕绘制 ==================
        int final_count = 0;

        hdma2d.Init.Mode = DMA2D_R2M;                 // 寄存器到内存(纯色填充)
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;  // 设定输出颜色格式
        hdma2d.Init.OutputOffset = 0;                 // 【关键】重置偏移量为0，否则清屏会错乱
        if (HAL_DMA2D_Init(&hdma2d) != HAL_OK) {
            // 初始化错误处理
        }
        // 清理/准备图层
        HAL_DMA2D_ConfigLayer(&hdma2d, 1);
        // 这里的lcd_fg_buffer为800*480*3字节的全屏缓冲
        HAL_DMA2D_Start(&hdma2d, 0x00000000, (uint32_t)g_ltdc_layer2_framebuf, 800, 480);
        HAL_DMA2D_PollForTransfer(&hdma2d, 1000);

        for (int i = 0; i < result_count; i++) {
            if (boxes[i].keep) {
                final_count++;

                // 注意：因为你的模型输入是224x224，所以映射比例基于224
                float scale_x = 800.0f / 224.0f;
                float scale_y = 480.0f / 224.0f;

                int display_x1 = (int)(boxes[i].x1 * scale_x);
                int display_y1 = (int)(boxes[i].y1 * scale_y);
                int display_x2 = (int)(boxes[i].x2 * scale_x);
                int display_y2 = (int)(boxes[i].y2 * scale_y);

                int display_width = display_x2 - display_x1;
                int display_height = display_y2 - display_y1;

                /* 边界检查，防止越界 */
                if (display_x1 < 0) display_x1 = 0;
                if (display_y1 < 0) display_y1 = 0;
                if (display_x2 > 800) display_x2 = 800;
                if (display_y2 > 480) display_y2 = 480;

                display_width = display_x2 - display_x1;
                display_height = display_y2 - display_y1;

                if (display_width <= 0 || display_height <= 0) continue;

                /* 只有当宽度和高度大于0才绘制 */
                if (display_y1 >= 480) display_y1 = 479;
                if (display_y2 > 480) display_y2 = 480;
                if (display_x1 >= 800) display_x1 = 799;
                if (display_x2 > 800) display_x2 = 800;

                display_width = display_x2 - display_x1;
                display_height = display_y2 - display_y1;

                if (display_width <= 0 || display_height <= 0) continue;
                /* 绘制上边框 */
                if (display_y1 < 480) {
                    hdma2d.Init.Mode = DMA2D_R2M;
                    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
                    hdma2d.Init.OutputOffset = 800 - display_width;
                    hdma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR;
                    HAL_DMA2D_Init(&hdma2d);
                    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
                    // 3代表RGB888格式(每像素3字节)
                    HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + display_x1) * 3], display_width, 1);
                    HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                }
                /* 绘制下边框 */
                if (display_y2 <= 480 && display_y2 > display_y1) {
                    hdma2d.Init.OutputOffset = 800 - display_width;
                    HAL_DMA2D_Init(&hdma2d);
                    HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[((display_y2 - 1) * 800 + display_x1) * 3], display_width, 1);
                    HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                }

                /* 绘制左边框 */
                if (display_x1 < 800) {
                    hdma2d.Init.OutputOffset = 800 - 1;
                    HAL_DMA2D_Init(&hdma2d);
                    HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + display_x1) * 3], 1, display_height);
                    HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                }

                /* 绘制右边框 */
                if (display_x2 <= 800 && display_x2 > display_x1) {
                    hdma2d.Init.OutputOffset = 800 - 1;
                    HAL_DMA2D_Init(&hdma2d);
                    HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + (display_x2 - 1)) * 3], 1, display_height);
                    HAL_DMA2D_PollForTransfer(&hdma2d, 100);
                }
            }

        }

        LL_ATON_RT_Reset_Network(&NN_Instance_Default);

        vTaskDelay(pdMS_TO_TICKS(10));
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
