#include "vl53l1x_bsp.h"
#include "VL53L1X_api.h"

uint8_t vl53l1x_boot_state(uint8_t *state)
{
    return VL53L1X_BootState(VL53L1X_I2C_ADDR, state);
}

uint8_t vl53l1x_get_sensor_id(uint16_t *id)
{
    return VL53L1X_GetSensorId(VL53L1X_I2C_ADDR, id);
}

uint8_t vl53l1x_init(void)
{
    return VL53L1X_SensorInit(VL53L1X_I2C_ADDR);
}

uint8_t vl53l1x_start_ranging(void)
{
    return VL53L1X_StartRanging(VL53L1X_I2C_ADDR);
}

uint8_t vl53l1x_stop_ranging(void)
{
    return VL53L1X_StopRanging(VL53L1X_I2C_ADDR);
}

uint8_t vl53l1x_is_data_ready(uint8_t *ready)
{
    return VL53L1X_CheckForDataReady(VL53L1X_I2C_ADDR, ready);
}

uint8_t vl53l1x_clear_interrupt(void)
{
    return VL53L1X_ClearInterrupt(VL53L1X_I2C_ADDR);
}

uint8_t vl53l1x_read_distance(uint16_t *distance_mm)
{
    return VL53L1X_GetDistance(VL53L1X_I2C_ADDR, distance_mm);
}

uint8_t vl53l1x_set_distance_mode(uint16_t mode)
{
    return VL53L1X_SetDistanceMode(VL53L1X_I2C_ADDR, mode);
}

uint8_t vl53l1x_set_timing_budget_ms(uint16_t budget_ms)
{
    return VL53L1X_SetTimingBudgetInMs(VL53L1X_I2C_ADDR, budget_ms);
}
