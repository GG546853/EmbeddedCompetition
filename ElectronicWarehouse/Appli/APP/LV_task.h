#ifndef __LV_TASK_H__
#define __LV_TASK_H__

#include "app_freertos.h"
#include "main.h"

#define LV_HOR_RES_MAX  800
#define LV_VER_RES_MAX  480

extern osThreadId_t LV_TaskHandle;
extern const osThreadAttr_t LVTask_attributes;

void LVGL_Task(void *argument);

#endif
