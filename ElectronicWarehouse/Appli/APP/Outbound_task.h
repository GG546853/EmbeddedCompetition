#ifndef __OUTBOUND_TASK_H
#define __OUTBOUND_TASK_H

#include "app_freertos.h"
#include "main.h"
#include "app_types.h"

extern osThreadId_t Outbound_TaskHandle;
extern const osThreadAttr_t OutboundTask_attributes;

extern InventoryItem inventory_item_T[28];
extern InventoryItem inventory_item_D[6];
extern HistoryRecord history_list[32];
extern int history_count;

void Outbound_Task(void *argument);

#endif
