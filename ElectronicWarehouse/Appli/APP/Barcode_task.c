#include "Barcode_task.h"
#include "UART_protocol.h"
#include <stdio.h>
#include <string.h>

#define BARCODE_RX_BUF_SIZE  256
#define UART4_RESP_BUF_SIZE  512

static uint8_t  uart5_rx_buf[BARCODE_RX_BUF_SIZE];
static volatile uint16_t uart5_rx_len;
static volatile uint8_t  uart5_data_ready;
static volatile uint8_t  uart5_processing;

char     uart4_resp_str[UART4_RESP_BUF_SIZE];
volatile uint8_t  uart4_resp_ready;
InventoryItem inventory_item;

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

static void parse_inventory(const uint8_t *payload, uint8_t len)
{
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
    if (pos + 2 > len) return;
    uint16_t qty = ((uint16_t)payload[pos] << 8) | payload[pos + 1];

    /* Fill struct with safe truncation */
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
    uart4_resp_ready = 1;
}

void Barcode_Task(void *argument)
{
    uint8_t frame_out[259];

    __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);

    printf("[Barcode] UART5 IT+IDLE started (9600)\r\n");

    for (;;) {
        if (uart5_data_ready) {
            uart5_data_ready = 0;
            uart5_processing = 1;

            uint8_t len = (uint8_t)uart5_rx_len;

            printf("[Barcode] rx %u bytes\r\n", len);

            /* Build 0x18 barcode frame and send to ESP32 via UART4 */
            uint8_t frame_len = protocol_build_frame(FRAME_TYPE_BARCODE, uart5_rx_buf, len, frame_out);
            HAL_UART_Transmit(&huart4, frame_out, frame_len, 100);
            HAL_UART_AbortReceive(&huart4);

            /* Wait for ESP32 response: parse framed response via UART4 */
            FrameParser parser;
            frame_parser_init(&parser);
            int got_response = 0;
            uint32_t start_tick = xTaskGetTickCount();

            while (!got_response) {
                uint8_t byte;
                HAL_StatusTypeDef s = HAL_UART_Receive(&huart4, &byte, 1, 100);
                if (s == HAL_OK) {
                    int result = frame_parser_feed(&parser, byte);
                    if (result == 1) {
                        if (parser.type == FRAME_TYPE_INVENTORY) {
                            printf("[Barcode] got INVENTORY frame, len=%u\r\n", parser.len);
                            parse_inventory(parser.payload, parser.len);
                            printf("[Barcode] resp: %s\r\n", uart4_resp_str);
                            got_response = 1;
                        }
                    }
                } else {
                    uint32_t elapsed = (xTaskGetTickCount() - start_tick) * portTICK_PERIOD_MS;
                    if (elapsed >= 15000) {
                        printf("[Barcode] UART4 timeout (%lu ms)\r\n", elapsed);
                        break;
                    }
                }
            }

            uart5_rx_len = 0;
            uart5_processing = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
