#ifndef __VL53L1X_BSP_H
#define __VL53L1X_BSP_H

#include <stdint.h>

#define VL53L1X_I2C_ADDR  0x52

uint8_t vl53l1x_boot_state(uint8_t *state);
uint8_t vl53l1x_get_sensor_id(uint16_t *id);
uint8_t vl53l1x_init(void);
uint8_t vl53l1x_start_ranging(void);
uint8_t vl53l1x_stop_ranging(void);
uint8_t vl53l1x_is_data_ready(uint8_t *ready);
uint8_t vl53l1x_clear_interrupt(void);
uint8_t vl53l1x_read_distance(uint16_t *distance_mm);
uint8_t vl53l1x_set_distance_mode(uint16_t mode);
uint8_t vl53l1x_set_timing_budget_ms(uint16_t budget_ms);

#endif
