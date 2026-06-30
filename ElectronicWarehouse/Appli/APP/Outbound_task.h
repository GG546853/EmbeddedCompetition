#ifndef __OUTBOUND_TASK_H
#define __OUTBOUND_TASK_H

#include "app_freertos.h"
#include "main.h"

typedef struct {
    char     pa[32];
    char     name[48];
    char     type[64];
    uint16_t quantity;
    bool     judgment_out;
} InventoryItem;

extern osThreadId_t Outbound_TaskHandle;
extern const osThreadAttr_t OutboundTask_attributes;
extern InventoryItem inventory_item_T[28];
extern InventoryItem inventory_item_D[6];

void Outbound_Task(void *argument);

#endif
