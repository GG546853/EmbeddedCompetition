#include "Barcode_task.h"
#include "UART_protocol.h"
#include "UART4_RxTask.h"
#include "Outbound_task.h"
#include "ui_bridge.h"
#include "vars.h"
#include "RGBLED_task.h"
#include "Electromagnet.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "uart.h"
#define BARCODE_RX_BUF_SIZE  256

static uint8_t  uart5_rx_buf[BARCODE_RX_BUF_SIZE];
static volatile uint16_t uart5_rx_len;
static volatile uint8_t  uart5_data_ready;
static volatile uint8_t  uart5_processing;

osThreadId_t Barcode_TaskHandle;
const osThreadAttr_t BarcodeTask_attributes = {
  .name = "BarcodeTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 8
};

void UART5_RxCallback(uint8_t byte)
{
    if (!uart5_processing && uart5_rx_len < BARCODE_RX_BUF_SIZE - 1) {
        uart5_rx_buf[uart5_rx_len++] = byte;
    }
}

void UART5_IDLE_Callback(void)
{
    if (uart5_rx_len > 0 && !uart5_processing) {
        uart5_data_ready = 1;
    }
}

void Barcode_Task(void *argument)
{
	osDelay(3500);
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);

    printf("[Barcode] UART5 IT+IDLE started (9600)\r\n");

    for (;;) {
        if (uart5_data_ready) {
            uart5_data_ready = 0;
            uart5_processing = 1;

            uint8_t len = (uint8_t)uart5_rx_len;

            printf("[Barcode] rx %u bytes\r\n", len);

            uart4_send_barcode(uart5_rx_buf, len);

            /* 清空可能残留的信号量 */
            while (xSemaphoreTake(inventory_sem, 0) == pdTRUE);

            /* Wait for UART4_RxTask to receive INVENTORY frame */
            if (xSemaphoreTake(inventory_sem, pdMS_TO_TICKS(15000)) == pdTRUE) {
                printf("[Barcode] resp: %s\r\n", uart4_resp_str);

                int  dup_idx = -1;
                char dup_type = 0;

                for (int i = 0; i < 28; i++) {
                    if (inventory_item_T[i].pc[0] != '\0' &&
                        strcmp(inventory_item_T[i].pc, inventory_item.pc) == 0) {
                        dup_idx  = i;
                        dup_type = 'T';
                        break;
                    }
                }
                if (dup_idx < 0) {
                    for (int i = 0; i < 6; i++) {
                        if (inventory_item_D[i].pc[0] != '\0' &&
                            strcmp(inventory_item_D[i].pc, inventory_item.pc) == 0) {
                            dup_idx  = i;
                            dup_type = 'D';
                            break;
                        }
                    }
                }

                if (dup_idx >= 0) {
                    char loc[4];
                    sprintf(loc, "%c%02d", dup_type, dup_idx);
                    inventory_add_quantity(&inventory_item, loc);
                    ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);
                    ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_LS,
                        ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_LS) + inventory_item.quantity);
                    uint8_t cab_id = (dup_type == 'T') ? (uint8_t)dup_idx
                                                       : (uint8_t)(28 + dup_idx);

                    if (cab_id <= 27) {
                        RGBLED_Flash(1u << (27 - cab_id));
                    } else {
                        Electromagnet_Open((uint8_t)(1u << (cab_id - 28)));
                    }

                    HistoryRecord *r = &history_list[history_count];
                    get_current_time_str(r->time, sizeof(r->time));
                    strncpy(r->action, "put", sizeof(r->action));
                    strncpy(r->pc, inventory_item.pc, sizeof(r->pc));
                    strncpy(r->user, Cabinet.User, sizeof(r->user));
                    r->cabinet_id = cab_id;
                    r->quantity   = inventory_item.quantity;
                    history_count++;
                    uart4_send_history(r);
                    ui_push_history(history_list, history_count);
                } else {
                    ui_set_string(FLOW_GLOBAL_VARIABLE_TEMP_NAME, inventory_item.name);
                    ui_set_integer(FLOW_GLOBAL_VARIABLE_TEMP_QTY,  inventory_item.quantity);
                    ui_set_string(FLOW_GLOBAL_VARIABLE_TEMP_PC,   inventory_item.pc);
                    ui_set_string(FLOW_GLOBAL_VARIABLE_TEMP_PA,   inventory_item.pa);
                    ui_set_string(FLOW_GLOBAL_VARIABLE_TEMP_SPEC, inventory_item.type);

                    ui_set_integer(FLOW_GLOBAL_VARIABLE_CID, -1);
                    ui_set_integer(FLOW_GLOBAL_VARIABLE_SHOW_SELECT_DIALOG, 1);

                    while (1) {
                        int cid = ui_get_integer(FLOW_GLOBAL_VARIABLE_CID);
                        if (cid != -1) {
                            char loc[4];
                            if (cid < 28) {
                                sprintf(loc, "T%02d", cid % 100);
                            } else {
                                sprintf(loc, "D%02d", (cid - 28) % 100);
                            }
                            inventory_store_to_slot(&inventory_item, loc);
                            ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);
                            ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_CTG,
                                ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_CTG) + 1);
                            ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_LS,
                                ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_LS) + inventory_item.quantity);
                            uart4_send_store(loc);
                            ui_set_integer(FLOW_GLOBAL_VARIABLE_CID, -1);

                            if (cid <= 27) {
                                RGBLED_Flash(1u << (27 - cid));
                            } else {
                                Electromagnet_Open((uint8_t)(1u << (cid - 28)));
                            }

                            HistoryRecord *r = &history_list[history_count];
                            get_current_time_str(r->time, sizeof(r->time));
                            strncpy(r->action, "put", sizeof(r->action));
                            strncpy(r->pc, inventory_item.pc, sizeof(r->pc));
                            strncpy(r->user, Cabinet.User, sizeof(r->user));
                            r->cabinet_id = (uint8_t)cid;
                            r->quantity   = inventory_item.quantity;
                            history_count++;
                            uart4_send_history(r);
                            ui_push_history(history_list, history_count);
                            break;
                        }
                        vTaskDelay(pdMS_TO_TICKS(50));
                    }
                }
            } else {
                printf("[Barcode] UART4 timeout\r\n");
            }

            uart5_rx_len = 0;
            uart5_processing = 0;

            // 重新使能 UART5 中断，防止 HAL 错误处理将其关闭
            __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE | UART_IT_IDLE);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
