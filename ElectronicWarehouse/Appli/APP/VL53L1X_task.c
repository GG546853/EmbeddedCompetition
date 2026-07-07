#include "VL53L1X_task.h"
#include "vl53l1x_bsp.h"
#include <stdio.h>
#include "i2c.h"
#include "app_types.h"
#include "rgblcd.h"

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
    Cabinet.System_state = true;
    Cabinet.distance_mm_th = 400;
    /* Wait for sensor to boot */
    do {
        vl53l1x_boot_state(&state);
        vTaskDelay(pdMS_TO_TICKS(2));
    } while (state == 0);

    /* Verify sensor ID */
    if (vl53l1x_get_sensor_id(&sensor_id) != 0 || sensor_id != 0xEACC) {

        goto error;
    }
    /* Initialise sensor */
    if (vl53l1x_init() != 0) {

        goto error;
    }
    /* Configure: long distance mode, 100ms timing budget */
    vl53l1x_set_distance_mode(2);
    vl53l1x_set_timing_budget_ms(100);

    /* Start continuous ranging */
    if (vl53l1x_start_ranging() != 0) {
        goto error;
    }
    Cabinet.VL53L1X_state = true;

    uint8_t  proximity_state = 0;  /* 0=near, 1=far */
    uint32_t state_enter_tick = xTaskGetTickCount();

    for (;;) {
        if (vl53l1x_is_data_ready(&ready) == 0 && ready) {
            vl53l1x_read_distance(&Cabinet.distance_mm);
            vl53l1x_clear_interrupt();

            uint32_t now = xTaskGetTickCount();
            uint32_t elapsed = (now - state_enter_tick) * portTICK_PERIOD_MS;

            if (Cabinet.distance_mm > Cabinet.distance_mm_th) {
                if (proximity_state == 0) {
                    proximity_state = 1;
                    state_enter_tick = now;
                } else if (elapsed >= 5000) {
                    Cabinet.System_state = false;
                    RGBLCD_BL(0);
                }
            } else {
                if (proximity_state == 1) {
                    proximity_state = 0;
                    state_enter_tick = now;
                } else if (elapsed >= 1000) {
                    Cabinet.System_state = true;
                    RGBLCD_BL(1);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }

error:
	Cabinet.VL53L1X_state = false;
    vTaskSuspend(NULL);
}
