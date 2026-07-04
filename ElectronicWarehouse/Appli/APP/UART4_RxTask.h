#ifndef __UART4_RXTASK_H
#define __UART4_RXTASK_H

#include "app_freertos.h"
#include "main.h"
#include "UART_protocol.h"
#include "app_types.h"
extern osThreadId_t UART4_RxTaskHandle;
extern const osThreadAttr_t UART4_RxTask_attributes;

void UART4_RxTask(void *argument);
void UART4_RxCallback(uint8_t byte);  // called from ISR

/* Inventory data (filled by handle_inventory) */


extern InventoryItem  inventory_item;
extern char           uart4_resp_str[512];
extern volatile uint32_t last_heartbeat_tick;
extern volatile uint8_t  esp32_online;
extern osSemaphoreId_t inventory_sem;

/* ---- Handler declarations (implementation up to user) ---- */
void handle_led(const uint8_t *payload, uint8_t len);
void handle_drawer(const uint8_t *payload, uint8_t len);
void handle_face_clear(const uint8_t *payload, uint8_t len);
void handle_alert(const uint8_t *payload, uint8_t len);
void handle_heartbeat(const uint8_t *payload, uint8_t len);
void handle_inventory(const uint8_t *payload, uint8_t len);
void handle_time(const uint8_t *payload, uint8_t len);
void handle_store(const uint8_t *payload, uint8_t len);
void get_current_time_str(char *buf, int bufsize);

/* Inventory source callbacks (user implements) */
void on_inventory_stm32(const InventoryItem *item);
void on_inventory_app(const InventoryItem *item);

void inventory_store_to_slot(const InventoryItem *item, const char *location);
void inventory_add_quantity(const InventoryItem *item, const char *location);
void history_add(const char *pc, uint16_t qty, uint8_t cab_id);

#endif
