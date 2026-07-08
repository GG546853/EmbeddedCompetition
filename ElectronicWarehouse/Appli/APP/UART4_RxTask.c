#include "UART4_RxTask.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usart.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "RGBLED_task.h"
#include "Electromagnet.h"
#include "Outbound_task.h"
#include "vars.h"
#include "ui_bridge.h"
#include "Outbound_task.h"

osThreadId_t UART4_RxTaskHandle;
const osThreadAttr_t UART4_RxTask_attributes = {
  .name = "UART4_RxTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};

InventoryItem  inventory_item;
HistoryRecord history_list_t;
extern AlarmRecord alarm_list[1];

char           uart4_resp_str[512];
volatile uint32_t last_heartbeat_tick;
volatile uint8_t  esp32_online;

static InventoryItem       pending_item;
static volatile uint8_t    pending_store;

// ISR -> Task ring buffer
#define UART4_RING_SIZE 1024
static uint8_t  uart4_ring[UART4_RING_SIZE];
static volatile uint16_t uart4_ring_wr;  // ISR writes
static uint16_t uart4_ring_rd;           // Task reads

/* ---- ISR callback: push byte into ring buffer ---- */
void UART4_RxCallback(uint8_t byte)
{
    uint16_t next = (uart4_ring_wr + 1) % UART4_RING_SIZE;
    if (next != uart4_ring_rd) {  // not full
        uart4_ring[uart4_ring_wr] = byte;
        uart4_ring_wr = next;
    }
}

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

    ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_TS,
        ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_TS) + 1);
}

void handle_heartbeat(const uint8_t *payload, uint8_t len)
{
    if (len < 1) return;

    uint8_t status = payload[0];

    Cabinet.WiFi_State  = (status & 0x01) ? true : false;  // bit0
    Cabinet.MQTT_State  = (status & 0x02) ? true : false;  // bit1
    Cabinet.ESP32_State = true;

    last_heartbeat_tick = xTaskGetTickCount();
}

/* ---- Time sync ---- */

static uint16_t  base_year;
static uint8_t   base_month, base_day;
static uint8_t   base_hour, base_min, base_sec;
static TickType_t base_tick;
static uint8_t   time_synced;

static const uint8_t dim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int is_leap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

void handle_time(const uint8_t *payload, uint8_t len)
{
    if (len < 7) return;
    base_year  = ((uint16_t)payload[0] << 8) | payload[1];
    base_month = payload[2];
    base_day   = payload[3];
    base_hour  = payload[4];
    base_min   = payload[5];
    base_sec   = payload[6];
    base_tick  = xTaskGetTickCount();
    time_synced = 1;
    printf("[Time] synced: %04d-%02d-%02d %02d:%02d:%02d\r\n",
           base_year, base_month, base_day, base_hour, base_min, base_sec);
}

void get_current_time_str(char *buf, int bufsize)
{
    if (!time_synced) {
        snprintf(buf, bufsize, "----:--:-- --:--:--");
        return;
    }

    uint32_t elapsed = ((xTaskGetTickCount() - base_tick) * portTICK_PERIOD_MS) / 1000;
    uint32_t sec  = base_sec  + elapsed;
    uint32_t min  = base_min  + sec / 60;  sec %= 60;
    uint32_t hr   = base_hour + min / 60;  min %= 60;
    uint32_t days = hr / 24;               hr  %= 24;

    int y = base_year;
    int m = base_month;
    int d = base_day + (int)days;

    for (;;) {
        int max_d = dim[m - 1];
        if (m == 2 && is_leap(y)) max_d = 29;
        if (d <= max_d) break;
        d -= max_d;
        if (++m > 12) { m = 1; y++; }
    }

    snprintf(buf, bufsize, "%04d-%02d-%02d %02lu:%02lu:%02lu",
             y, m, d, hr, min, sec);
}

void handle_inventory(const uint8_t *payload, uint8_t len)
{
    /* parse pa\0 pc\0 name\0 type\0 qty(2B BE) src(1B) -> inventory_item */
    uint16_t pos = 0;
    const uint8_t *pa, *pc, *name, *type;

    /* pa */
    pa = &payload[pos];
    while (pos < len && payload[pos] != '\0') pos++;
    if (pos >= len) return;
    pos++;

    /* pc */
    pc = &payload[pos];
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
    strncpy(inventory_item.pc,   (const char *)pc,   sizeof(inventory_item.pc) - 1);
    strncpy(inventory_item.name, (const char *)name, sizeof(inventory_item.name) - 1);
    strncpy(inventory_item.type, (const char *)type, sizeof(inventory_item.type) - 1);
    inventory_item.pa[sizeof(inventory_item.pa) - 1]     = '\0';
    inventory_item.pc[sizeof(inventory_item.pc) - 1]     = '\0';
    inventory_item.name[sizeof(inventory_item.name) - 1] = '\0';
    inventory_item.type[sizeof(inventory_item.type) - 1] = '\0';
    inventory_item.quantity = qty;

    /* Format readable string */
    uint16_t off = 0;
    off += sprintf(uart4_resp_str + off, "pa:%s ", inventory_item.pa);
    off += sprintf(uart4_resp_str + off, "pc:%s ", inventory_item.pc);
    off += sprintf(uart4_resp_str + off, "name:%s ", inventory_item.name);
    off += sprintf(uart4_resp_str + off, "type:%s ", inventory_item.type);
    off += sprintf(uart4_resp_str + off, "qty:%u", inventory_item.quantity);

    Cabinet.Count = ui_get_integer(FLOW_GLOBAL_VARIABLE_CABINET_COUNT);
    Cabinet.Count = Cabinet.Count + 1;
    ui_set_integer(FLOW_GLOBAL_VARIABLE_CABINET_COUNT, Cabinet.Count);

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
    if (len < 3) return;
    if (!pending_store) return;

    char location[8];
    location[0] = (char)payload[0];
    location[1] = (char)('0' + (payload[1] / 10));
    location[2] = (char)('0' + (payload[1] % 10));
    location[3] = '\0';

    uint8_t flag = payload[2];

    uint8_t cab_id = (location[0] == 'T') ? (uint8_t)atoi(&location[1])
                                          : (uint8_t)(28 + atoi(&location[1]));

    if (flag == 0x00) {
        inventory_store_to_slot(&pending_item, location);
        ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);
        ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_CTG,
            ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_CTG) + 1);
        ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_LS,
            ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_LS) + pending_item.quantity);
    } else if (flag == 0x01) {
        inventory_add_quantity(&pending_item, location);
        ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);
        ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_LS,
            ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_LS) + pending_item.quantity);
    }

    if (cab_id <= 27) {
        RGBLED_Flash(1u << (27 - cab_id));
    } else {
        Electromagnet_Open((uint8_t)(1u << (cab_id - 28)));
    }

    HistoryRecord *r = &history_list[history_count];
    get_current_time_str(r->time, sizeof(r->time));
    strncpy(r->action, "put", sizeof(r->action));
    strncpy(r->pc, pending_item.pc, sizeof(r->pc));
    strncpy(r->user, Cabinet.User, sizeof(r->user));
    r->cabinet_id = cab_id;
    r->quantity   = pending_item.quantity;
    history_count++;
    uart4_send_history(r);
    ui_push_history(history_list, history_count);

    pending_store = 0;
}

void handle_miniapp_outbound(const uint8_t *payload, uint8_t len)
{
    if (len < 4) return;

    char    type = (char)payload[0];
    uint8_t slot = payload[1];
    uint16_t qty = ((uint16_t)payload[2] << 8) | payload[3];

    if (type == 'T') {
        if (slot >= 28) return;
        if (inventory_item_T[slot].pc[0] == '\0') return;
        if (inventory_item_T[slot].quantity < qty) return;
        inventory_item_T[slot].quantity -= qty;
    } else if (type == 'D') {
        if (slot >= 6) return;
        if (inventory_item_D[slot].pc[0] == '\0') return;
        if (inventory_item_D[slot].quantity < qty) return;
        inventory_item_D[slot].quantity -= qty;
    } else {
        return;
    }

    uint8_t cab_id = (type == 'T') ? slot : (uint8_t)(slot + 28);
    if (cab_id <= 27) {
        RGBLED_Flash(1u << (27 - cab_id));
    } else {
        Electromagnet_Open((uint8_t)(1u << (cab_id - 28)));
    }

    ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);

    ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_LS,
        ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_LS) - (int)qty);
}

void handle_qty_alert(const uint8_t *payload, uint8_t len)
{
    /* parse pc\0 cab(1B) qty(2B BE) */
    uint16_t pos = 0;
    while (pos < len && payload[pos] != '\0') pos++;
    if (pos + 3 >= len) return;
    pos++;

    uint8_t  cab = payload[pos];
    uint16_t qty = ((uint16_t)payload[pos + 1] << 8) | payload[pos + 2];

    strncpy(alarm_list[0].pc, (const char *)payload, sizeof(alarm_list[0].pc) - 1);
    alarm_list[0].pc[sizeof(alarm_list[0].pc) - 1] = '\0';
    alarm_list[0].cabinet_id = cab;
    alarm_list[0].quantity    = qty;

    ui_push_alarm(alarm_list, 1);
}

void inventory_store_to_slot(const InventoryItem *item, const char *location)
{
    if (location[0] != 'D' && location[0] != 'T') return;

    int index = atoi(&location[1]);

    if (location[0] == 'D') {
        if (index < 0 || index >= 6) return;
        memcpy(&inventory_item_D[index], item, sizeof(InventoryItem));
        printf("[Store] %s %s %s %s qty:%u -> D%02d\r\n",
               item->pa, item->pc, item->name, item->type, item->quantity, index);
    } else {
        if (index < 0 || index >= 28) return;
        memcpy(&inventory_item_T[index], item, sizeof(InventoryItem));
        printf("[Store] %s %s %s %s qty:%u -> T%02d\r\n",
               item->pa, item->pc, item->name, item->type, item->quantity, index);
    }
}

void inventory_add_quantity(const InventoryItem *item, const char *location)
{
    if (location[0] != 'D' && location[0] != 'T') return;

    int index = atoi(&location[1]);

    if (location[0] == 'D') {
        if (index < 0 || index >= 6) return;
        if (inventory_item_D[index].pc[0] == '\0') {
            memcpy(&inventory_item_D[index], item, sizeof(InventoryItem));
        } else {
            inventory_item_D[index].quantity += item->quantity;
        }
        printf("[DupStore] +%u -> D%02d (total:%u)\r\n",
               item->quantity, index, inventory_item_D[index].quantity);
    } else {
        if (index < 0 || index >= 28) return;
        if (inventory_item_T[index].pc[0] == '\0') {
            memcpy(&inventory_item_T[index], item, sizeof(InventoryItem));
        } else {
            inventory_item_T[index].quantity += item->quantity;
        }
        printf("[DupStore] +%u -> T%02d (total:%u)\r\n",
               item->quantity, index, inventory_item_T[index].quantity);
    }
}

/* ---- UART4 Receive Task ---- */

void UART4_RxTask(void *argument)
{
    osDelay(3500);  // wait for LVGL UI init

    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);

    FrameParser parser;
    frame_parser_init(&parser);
    printf("[UART4] RxTask started (ISR+ring)\r\n");

    for (;;) {
        while (uart4_ring_rd != uart4_ring_wr) {
            uint8_t byte = uart4_ring[uart4_ring_rd];
            uart4_ring_rd = (uart4_ring_rd + 1) % UART4_RING_SIZE;

            int result = frame_parser_feed(&parser, byte);
            if (result == 1) {
                uart4_dispatch_frame(parser.type, parser.payload, parser.len);
            }
        }
        osDelay(1);  // ring empty, yield CPU
    }
}
