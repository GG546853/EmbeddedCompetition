#include "RGBLED_task.h"
#include "sk9822.h"
#include <stdio.h>

void SK9822_SetByBitmap(uint32_t bitmap,
                                uint8_t r, uint8_t g, uint8_t b,
                                uint8_t brightness)
{
    for (int i = 0; i < 28; i++) {
        if (bitmap & (1u << (27 - i)))
            SK9822_SetGroup(i, r, g, b, brightness);
        else
            SK9822_SetGroup(i, 0, 0, 0, 0);
    }

    SK9822_Transmit();
}

osThreadId_t RGBLED_TaskHandle;
const osThreadAttr_t RGBLEDTask_attributes = {
  .name = "RGBLEDTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

void RGBLED_Flash(uint32_t nub)
{
	osThreadNew(RGBLED_Task, (void *)(uintptr_t)nub, &RGBLEDTask_attributes);
}

//osThreadNew(RGBLED_Task, (void *)(uintptr_t)bitmap, &RGBLEDTask_attributes)
void RGBLED_Task(void *argument)
{
    uint32_t bitmap = (uint32_t)(uintptr_t)argument;

    SK9822_Init();
    SK9822_SetByBitmap(bitmap, 255, 0, 0, 10);
    vTaskDelay(pdMS_TO_TICKS(6000));
    SK9822_SetAll(0, 0, 0, 0);
    SK9822_Transmit();
    osThreadExit();
}
