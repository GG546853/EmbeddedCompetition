#include "vl53l1_platform.h"
#include "vl53l1_error_codes.h"
#include "vl53l1_platform_user_config.h"
#include "i2c.h"
#include "cmsis_os2.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

extern osMutexId_t i2c2_mutex;

#ifndef VL53L1_GetI2cBus
#   define VL53L1_GetI2cBus(...)  osMutexAcquire(i2c2_mutex, osWaitForever)
#endif

#ifndef VL53L1_PutI2cBus
#   define VL53L1_PutI2cBus(...)  osMutexRelease(i2c2_mutex)
#endif

#define I2C_TIME_OUT_BASE  10
#define I2C_TIME_OUT_BYTE   1

static uint8_t _I2CBuffer[256];

static int _I2CWrite(uint16_t Dev, uint8_t *pdata, uint32_t count)
{
    return HAL_I2C_Master_Transmit(&hi2c4, Dev, pdata, count,
                                   I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE);
}

static int _I2CRead(uint16_t Dev, uint8_t *pdata, uint32_t count)
{
    return HAL_I2C_Master_Receive(&hi2c4, Dev | 1, pdata, count,
                                  I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE);
}

VL53L1_Error VL53L1_WriteMulti(uint16_t Dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    if (count > sizeof(_I2CBuffer) - 1) {
        return VL53L1_ERROR_INVALID_PARAMS;
    }
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    memcpy(&_I2CBuffer[2], pdata, count);
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, count + 2);
    VL53L1_PutI2cBus();
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_ReadMulti(uint16_t Dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        VL53L1_PutI2cBus();
        return VL53L1_ERROR_CONTROL_INTERFACE;
    }
    status_int = _I2CRead(Dev, pdata, count);
    VL53L1_PutI2cBus();
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WrByte(uint16_t Dev, uint16_t index, uint8_t data)
{
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    _I2CBuffer[2] = data;
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, 3);
    VL53L1_PutI2cBus();
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WrWord(uint16_t Dev, uint16_t index, uint16_t data)
{
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    _I2CBuffer[2] = data >> 8;
    _I2CBuffer[3] = data & 0x00FF;
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, 4);
    VL53L1_PutI2cBus();
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WrDWord(uint16_t Dev, uint16_t index, uint32_t data)
{
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    _I2CBuffer[2] = (data >> 24) & 0xFF;
    _I2CBuffer[3] = (data >> 16) & 0xFF;
    _I2CBuffer[4] = (data >> 8)  & 0xFF;
    _I2CBuffer[5] = (data >> 0)  & 0xFF;
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, 6);
    VL53L1_PutI2cBus();
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_RdByte(uint16_t Dev, uint16_t index, uint8_t *data)
{
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        VL53L1_PutI2cBus();
        return VL53L1_ERROR_CONTROL_INTERFACE;
    }
    status_int = _I2CRead(Dev, data, 1);
    VL53L1_PutI2cBus();
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_RdWord(uint16_t Dev, uint16_t index, uint16_t *data)
{
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        VL53L1_PutI2cBus();
        return VL53L1_ERROR_CONTROL_INTERFACE;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 2);
    VL53L1_PutI2cBus();
    *data = ((uint16_t)_I2CBuffer[0] << 8) + (uint16_t)_I2CBuffer[1];
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_RdDWord(uint16_t Dev, uint16_t index, uint32_t *data)
{
    _I2CBuffer[0] = index >> 8;
    _I2CBuffer[1] = index & 0xFF;
    VL53L1_GetI2cBus();
    int status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        VL53L1_PutI2cBus();
        return VL53L1_ERROR_CONTROL_INTERFACE;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 4);
    VL53L1_PutI2cBus();
    *data = ((uint32_t)_I2CBuffer[0] << 24) + ((uint32_t)_I2CBuffer[1] << 16)
          + ((uint32_t)_I2CBuffer[2] << 8)  + (uint32_t)_I2CBuffer[3];
    return (status_int != 0) ? VL53L1_ERROR_CONTROL_INTERFACE : VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WaitMs(uint16_t dev, int32_t wait_ms)
{
    (void)dev;
    if (wait_ms > 0) {
        vTaskDelay(pdMS_TO_TICKS(wait_ms));
    }
    return VL53L1_ERROR_NONE;
}
