#include "Outbound_task.h"
#include "Electromagnet.h"
#include "RGBLED_task.h"
#include "UART4_RxTask.h"

osThreadId_t Outbound_TaskHandle;
const osThreadAttr_t OutboundTask_attributes = {
  .name = "OutboundTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};

InventoryItem inventory_item_T[28];
InventoryItem inventory_item_D[6];

HistoryRecord history_list[32];
int history_count;

AlarmRecord alarm_list[1];




void Outbound_Task(void *argument)
{
    uint32_t led_bitmap = 0;
    uint8_t  cab_mask   = 0;

    for (int i = 0; i < 28; i++) {
        if (inventory_item_T[i].judgment_out)
            led_bitmap |= (1u << (27 - i));
    }
    for (int i = 0; i < 6; i++) {
        if (inventory_item_D[i].judgment_out)
            cab_mask |= (1u << i);
    }

    if (led_bitmap != 0)
        osThreadNew(RGBLED_Task, (void *)(uintptr_t)led_bitmap, &RGBLEDTask_attributes);

    if (cab_mask != 0)
        osThreadNew(Electromagnet_Task, (void *)(uintptr_t)cab_mask, &ElectromagnetTask_attributes);

    for (int i = 0; i < 28; i++)
        inventory_item_T[i].judgment_out = false;
    for (int i = 0; i < 6; i++)
        inventory_item_D[i].judgment_out = false;

    osThreadExit();
}

