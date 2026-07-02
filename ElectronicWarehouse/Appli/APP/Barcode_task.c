#include "Barcode_task.h"
#include "UART_protocol.h"
#include "UART4_RxTask.h"
#include "Outbound_task.h"
#include "ui_bridge.h"
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
  .stack_size = 512 * 8
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
	osDelay(500);
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

                static int next_t = 0;
                char loc[4];
                sprintf(loc, "T%02d", next_t);
                inventory_store_to_slot(&inventory_item, loc);
                ui_sync_cabinets(inventory_item_T, 28);
                uart4_send_store(loc);
                next_t = (next_t + 1) % 28;
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
