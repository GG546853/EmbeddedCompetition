#ifndef __RGBLED_TASK_H
#define __RGBLED_TASK_H

#include "app_freertos.h"
#include "main.h"

#define CAM_WIDTH       800
#define CAM_HEIGHT      480

#define AI_INPUT_WIDTH  416
#define AI_INPUT_HEIGHT 416

extern osThreadId_t RGBLED_TaskHandle;
extern const osThreadAttr_t RGBLEDTask_attributes;

void RGBLED_Task(void *argument);

#endif
