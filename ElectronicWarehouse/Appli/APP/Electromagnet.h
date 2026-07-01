#ifndef __ELECTROMAGNET_H
#define __ELECTROMAGNET_H

#include "app_freertos.h"
#include "main.h"

extern osThreadId_t Electromagnet_TaskHandle;
extern const osThreadAttr_t ElectromagnetTask_attributes;

void Electromagnet_Task(void *argument);
void Electromagnet_Open(uint8_t cabinet_mask);


#endif
