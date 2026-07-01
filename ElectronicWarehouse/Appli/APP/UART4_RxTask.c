#include "UART4_RxTask.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usart.h"
#include "semphr.h"
#include "RGBLED_task.h"
#include "Electromagnet.h"
#include "Outbound_task.h"
osThreadId_t UART4_RxTaskHandle;
const osThreadAttr_t UART4_RxTask_attributes = {
  .name = "UART4_RxTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};

InventoryItem  inventory_item;
char           uart4_resp_str[512];
volatile uint32_t last_heartbeat_tick;
volatile uint8_t  esp32_online;

static InventoryItem       pending_item;
static volatile uint8_t    pending_store;

/* ---- Handler stubs (user implements) ---- */

void handle_led(const uint8_t *payload, uint8_t len)
{
    if (len < 4) return;

    uint32_t bitmap = ((uint32_t)payload[0] << 24)
                    | ((uint32_t)payload[1] << 16)
                    | ((uint32_t)payload[2] << 8)
                    |  (uint32_t)payload[3];

    RGBLED_Flash(bitmap);
}

void handle_drawer(const uint8_t *payload, uint8_t len)
{
    if (len < 1) return;

    Electromagnet_Open(payload[0]);
}


void handle_face_clear(const uint8_t *payload, uint8_t len)
{
}

void handle_alert(const uint8_t *payload, uint8_t len)
{
    if (len < 3) return;

    uint8_t  sub_type = payload[0];
    uint16_t raw_val  = ((uint16_t)payload[1] << 8) | payload[2];
    float    val      = raw_val * 0.1f;

    if (sub_type == 0x00) {
        Cabinet.temperature_th = val;
    } else if (sub_type == 0x01) {
        Cabinet.humidity_th    = val;
    }
}

void handle_heartbeat(const uint8_t *payload, uint8_t len)
{
    if (len < 1) return;

    uint8_t status = payload[0];

    Cabinet.WiFi_State  = (status & 0x01) ? true : false;  // bit0
    Cabinet.MQTT_State  = (status & 0x02) ? true : false;  // bit1
    Cabinet.ESP32_State = true;   // 能收到心跳 = ESP32 活着

    last_heartbeat_tick = xTaskGetTickCount();
}

void handle_inventory(const uint8_t *payload, uint8_t len)
{
    /* parse pa\0 name\0 type\0 qty(2B BE) → inventory_item */
    uint16_t pos = 0;
    const uint8_t *pa, *name, *type;

    /* pa */
    pa = &payload[pos];
    while (pos < len && payload[pos] != '\0') pos++;
    if (pos >= len) return;
    pos++;

    /* name */
    name = &payload[pos];
    while (pos < len && payload[pos] != '\0') pos++;
    if (pos >= len) return;
    pos++;

    /* type */
    type = &payload[pos];
    while (pos < len && payload[pos] != '\0') pos++;
    if (pos >= len) return;
    pos++;

    /* quantity (2 bytes big-endian) */
    if (pos + 3 > len) return;
    uint16_t qty = ((uint16_t)payload[pos] << 8) | payload[pos + 1];
    pos += 2;

    /* source (1 byte): 0x00=STM32, 0x01=App */
    uint8_t src = payload[pos];

    /* Fill struct */
    strncpy(inventory_item.pa,   (const char *)pa,   sizeof(inventory_item.pa) - 1);
    strncpy(inventory_item.name, (const char *)name, sizeof(inventory_item.name) - 1);
    strncpy(inventory_item.type, (const char *)type, sizeof(inventory_item.type) - 1);
    inventory_item.pa[sizeof(inventory_item.pa) - 1]     = '\0';
    inventory_item.name[sizeof(inventory_item.name) - 1] = '\0';
    inventory_item.type[sizeof(inventory_item.type) - 1] = '\0';
    inventory_item.quantity = qty;

    /* Format readable string */
    uint16_t off = 0;
    off += sprintf(uart4_resp_str + off, "pa:%s ", inventory_item.pa);
    off += sprintf(uart4_resp_str + off, "name:%s ", inventory_item.name);
    off += sprintf(uart4_resp_str + off, "type:%s ", inventory_item.type);
    off += sprintf(uart4_resp_str + off, "qty:%u", inventory_item.quantity);

    if (src == 0x00) {
        on_inventory_stm32(&inventory_item);
    } else {
        on_inventory_app(&inventory_item);
    }
}

/* ---- Inventory callbacks ---- */

void on_inventory_stm32(const InventoryItem *item)
{
    xSemaphoreGive(inventory_sem);
}

void on_inventory_app(const InventoryItem *item)
{
    memcpy(&pending_item, item, sizeof(InventoryItem));
    pending_store = 1;
}

void handle_store(const uint8_t *payload, uint8_t len)
{
    if (len < 2) return;
    if (!pending_store) return;

    char location[8];
    location[0] = (char)payload[0];
    location[1] = (char)('0' + (payload[1] / 10));
    location[2] = (char)('0' + (payload[1] % 10));
    location[3] = '\0';

    inventory_store_to_slot(&pending_item, location);
    pending_store = 0;
}

void inventory_store_to_slot(const InventoryItem *item, const char *location)
{
    if (location[0] != 'D' && location[0] != 'T') return;

    int index = atoi(&location[1]);

    if (location[0] == 'D') {
        if (index < 0 || index >= 6) return;
        memcpy(&inventory_item_D[index], item, sizeof(InventoryItem));
        printf("[Store] %s %s %s qty:%u -> D%02d\r\n",
               item->pa, item->name, item->type, item->quantity, index);
    } else {
        if (index < 0 || index >= 28) return;
        memcpy(&inventory_item_T[index], item, sizeof(InventoryItem));
        printf("[Store] %s %s %s qty:%u -> T%02d\r\n",
               item->pa, item->name, item->type, item->quantity, index);
    }
}

/* ---- UART4 Receive Task ---- */

void UART4_RxTask(void *argument)
{
    FrameParser parser;
    frame_parser_init(&parser);

    printf("[UART4] RxTask started\r\n");

    for (;;) {
        uint8_t byte;
        if (HAL_UART_Receive(&huart4, &byte, 1, 100) == HAL_OK) {
            int result = frame_parser_feed(&parser, byte);
            if (result == 1) {
                uart4_dispatch_frame(parser.type, parser.payload, parser.len);
            }
        }
    }
}
