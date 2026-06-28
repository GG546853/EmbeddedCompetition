#include "RGBLED_task.h"
#include "sk9822.h"
#include <stdio.h>

static void hsv2rgb(uint8_t h, uint8_t s, uint8_t v,
                    uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (s == 0) {
        *r = *g = *b = v;
        return;
    }

    uint8_t region = h / 43;
    uint8_t remainder = (h - region * 43) * 6;

    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0:  *r = v; *g = t; *b = p; break;
    case 1:  *r = q; *g = v; *b = p; break;
    case 2:  *r = p; *g = v; *b = t; break;
    case 3:  *r = p; *g = q; *b = v; break;
    case 4:  *r = t; *g = p; *b = v; break;
    default: *r = v; *g = p; *b = q; break;
    }
}

osThreadId_t RGBLED_TaskHandle;
const osThreadAttr_t RGBLEDTask_attributes = {
  .name = "RGBLEDTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

void RGBLED_Task(void *argument)
{
    SK9822_Init();
//    printf("[RGBLED] SK9822 initialized, %d LEDs, %d groups\r\n",
//           SK9822_NUM_LEDS, SK9822_NUM_GROUPS);
//
//    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        /* Phase 1: light groups one by one */
//        for (uint8_t g = 0; g < SK9822_NUM_GROUPS; g++) {
//            uint8_t r, grn, b;
//            /* 28 colors spread across the hue wheel */
//            hsv2rgb(g * 255 / SK9822_NUM_GROUPS, 255, 128, &r, &grn, &b);
//            SK9822_SetGroup(g, r, grn, b, 20);
//            SK9822_Transmit();
//
//            printf("[RGBLED] Group %2d ON  R=%3u G=%3u B=%3u\r\n", g, r, grn, b);
//            vTaskDelay(pdMS_TO_TICKS(200));
//        }

    	for(int i = 0; i <= 23; i++)
    	{
    		SK9822_SetGroup(i, 200, 0, 0, 20);
    		SK9822_Transmit();
    		vTaskDelay(pdMS_TO_TICKS(500));
    	}


        /* Phase 2: clear all, then restart */
        //SK9822_Clear();

        //printf("[RGBLED] All off, restarting...\r\n");

       // vTaskDelay(pdMS_TO_TICKS(500));
    }
}
