#include "Barcode_task.h"
#include "UART_protocol.h"
#include "UART4_RxTask.h"
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
  .stack_size = 512 * 4
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

            /* Wait for UART4_RxTask to receive INVENTORY frame */
            if (xSemaphoreTake(inventory_sem, pdMS_TO_TICKS(15000)) == pdTRUE) {
                printf("[Barcode] resp: %s\r\n", uart4_resp_str);

                char location[8];
                printf("[Barcode] Enter location (D00-D05 / T00-T27): ");
                g_uart_rx_sta = 0;
                while (!(g_uart_rx_sta & 0x8000)) {
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
                uint16_t loc_len = g_uart_rx_sta & 0x3FFF;
                if (loc_len > sizeof(location) - 1) loc_len = sizeof(location) - 1;
                memcpy(location, g_uart_rx_buf, loc_len);
                location[loc_len] = '\0';
                uart4_send_store(location);
                inventory_store_to_slot(&inventory_item, location);
                printf("[Barcode] stored at %s\r\n", location);
            } else {
                printf("[Barcode] UART4 timeout\r\n");
            }

            uart5_rx_len = 0;
            uart5_processing = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
