#ifndef __PRINTER_TASK_H
#define __PRINTER_TASK_H

#include "app_freertos.h"
#include "main.h"

extern osThreadId_t Printer_TaskHandle;
extern const osThreadAttr_t PrinterTask_attributes;

void Printer_Task(void *argument);

#endif
