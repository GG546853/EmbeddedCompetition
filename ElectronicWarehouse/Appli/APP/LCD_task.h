#ifndef __LCD_TASK_H
#define __LCD_TASK_H

#include "app_freertos.h"
#include "main.h"

extern osThreadId_t LED_TaskHandle;
extern const osThreadAttr_t LEDTask_attributes;

void led_Task(void *argument);

#endif
