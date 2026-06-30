#ifndef __AHT10_TASK_H
#define __AHT10_TASK_H

#include "app_freertos.h"
#include "main.h"


extern osThreadId_t Aht10_TaskHandle;
extern const osThreadAttr_t Aht10Task_attributes;

void Aht10_Task(void *argument);

#endif
