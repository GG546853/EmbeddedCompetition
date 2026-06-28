#include "VL53L1X_task.h"
#include "vl53l1x_bsp.h"
#include <stdio.h>
#include "i2c.h"

osThreadId_t VL53L1X_TaskHandle;
const osThreadAttr_t VL53L1XTask_attributes = {
  .name = "VL53L1XTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

void VL53L1X_Task(void *argument)
{
    uint8_t state;
    uint16_t sensor_id;
    uint8_t ready;
    uint16_t distance_mm;


    printf("[VL53L1X] Scanning I2C bus...\r\n");
    for (uint16_t addr = 0x02; addr < 0xFE; addr += 2) {
        if (HAL_I2C_IsDeviceReady(&hi2c4, addr, 1, 10) == HAL_OK) {
        	printf("[VL53L1X]   Found device at 0x%02X\r\n", addr);
        }
    }
    printf("[VL53L1X] Scan done\r\n");

    /* Wait for sensor to boot */
    do {
        vl53l1x_boot_state(&state);
        vTaskDelay(pdMS_TO_TICKS(2));
    } while (state == 0);

    /* Verify sensor ID */
    if (vl53l1x_get_sensor_id(&sensor_id) != 0 || sensor_id != 0xEACC) {
        printf("[VL53L1X] Bad sensor ID: 0x%04X\r\n", sensor_id);
        goto error;
    }
    printf("[VL53L1X] Sensor ID OK: 0x%04X\r\n", sensor_id);

    /* Initialise sensor */
    if (vl53l1x_init() != 0) {
        printf("[VL53L1X] Init FAILED\r\n");
        goto error;
    }

    /* Configure: long distance mode, 100ms timing budget */
    vl53l1x_set_distance_mode(2);
    vl53l1x_set_timing_budget_ms(100);

    /* Start continuous ranging */
    if (vl53l1x_start_ranging() != 0) {
        printf("[VL53L1X] Start ranging FAILED\r\n");
        goto error;
    }

    printf("[VL53L1X] Ranging started\r\n");

    for (;;) {
        if (vl53l1x_is_data_ready(&ready) == 0 && ready) {
            vl53l1x_read_distance(&distance_mm);
            vl53l1x_clear_interrupt();
            printf("[VL53L1X] Distance: %u mm\r\n", distance_mm);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

error:
    printf("[VL53L1X] Task stopped due to error\r\n");
    vTaskSuspend(NULL);
}
