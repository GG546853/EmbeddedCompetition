#include "LV_task.h"
#include "lvgl.h"
#include "lv_st_ltdc.h"   // LVGL 内置 STM32 LTDC 驱动
#include "lv_port_indev.h"
#include "ui/ui.h"        // EEZ Flow UI
#include "ltdc.h"
#include <string.h>
#include "task.h"          // uxTaskGetStackHighWaterMark
#include "vars.h"
#include "ui_bridge.h"
#include "app_types.h"


UBaseType_t g_lv_stack_high_water;  // 启动后剩余栈空间（单位：word，4 字节）
osThreadId_t LV_TaskHandle;
const osThreadAttr_t LVTask_attributes = {
  .name = "LV_Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4096 * 24
};

// UI 专用帧缓冲 — Layer 1, RGB565, 800x480, 分配在 HyperRAM
uint8_t g_ltdc_ui_framebuf[800 * 480 * 2] __attribute__((section(".EXTRAM"), aligned(32)));

static void lv_port_init(void) {
    lv_init();

    // 配置 LTDC Layer 1 作为 UI 层
    // Layer 0 由 rgblcd_init() 配置，用于摄像头 + AI (RGB888)
    // Layer 1 给 LVGL 使用 (RGB565)，叠加在 Layer 0 之上
    LTDC_LayerCfgTypeDef ui_layer_cfg = {0};
    ui_layer_cfg.WindowX0 = 0;
    ui_layer_cfg.WindowX1 = 800;
    ui_layer_cfg.WindowY0 = 0;
    ui_layer_cfg.WindowY1 = 480;
    ui_layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    ui_layer_cfg.Alpha = 255;
    ui_layer_cfg.Alpha0 = 255;
    ui_layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    ui_layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    ui_layer_cfg.FBStartAdress = (uint32_t)g_ltdc_ui_framebuf;
    ui_layer_cfg.ImageWidth = 800;
    ui_layer_cfg.ImageHeight = 480;
    ui_layer_cfg.Backcolor.Blue = 0;
    ui_layer_cfg.Backcolor.Green = 0;
    ui_layer_cfg.Backcolor.Red = 0;
    memset(g_ltdc_ui_framebuf, 0, sizeof(g_ltdc_ui_framebuf));
    HAL_LTDC_ConfigLayer(&hltdc, &ui_layer_cfg, 1);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

    // LVGL 部分缓冲渲染 → DMA2D flush 到 Layer 1 帧缓冲
    static lv_color_t buf1[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];
    static lv_color_t buf2[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];
    lv_st_ltdc_create_partial(buf1, buf2, sizeof(buf1), 1);  // layer_idx = 1

    lv_port_indev_init();  // 触摸

    lv_tick_set_cb(xTaskGetTickCount);
}

void LVGL_Task(void *argument) {

	vTaskDelay(pdMS_TO_TICKS(1000));
    lv_port_init();

    ui_init();  // EEZ Flow UI 初始化

    g_lv_stack_high_water = uxTaskGetStackHighWaterMark(NULL);  // 记录剩余栈


    for (;;) {
        lv_timer_handler();
        ui_tick();
        vTaskDelay(pdMS_TO_TICKS(5));

    }
}





void _exit(int status) {
    (void)status;
    while(1);
}

int _gettimeofday(struct timeval *tv, void *tzvp) {
    (void)tv;
    (void)tzvp;
    return 0;
}

