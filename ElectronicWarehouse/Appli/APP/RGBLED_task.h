#ifndef __RGBLED_TASK_H
#define __RGBLED_TASK_H

#include "app_freertos.h"
#include "main.h"

extern osThreadId_t RGBLED_TaskHandle;
extern const osThreadAttr_t RGBLEDTask_attributes;

void RGBLED_Task(void *argument);
void SK9822_SetByBitmap(uint32_t bitmap,
                                uint8_t r, uint8_t g, uint8_t b,
                                uint8_t brightness);
#endif
