#ifndef __BARCODE_TASK_H
#define __BARCODE_TASK_H

#include "app_freertos.h"
#include "main.h"
#include "usart.h"

extern osThreadId_t Barcode_TaskHandle;
extern const osThreadAttr_t BarcodeTask_attributes;

void Barcode_Task(void *argument);
void UART5_RxCallback(uint8_t byte);
void UART5_IDLE_Callback(void);

#endif
