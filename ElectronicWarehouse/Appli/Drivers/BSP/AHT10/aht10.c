#include "aht10.h"
#include "../SoftI2C/soft_i2c.h"

/**
 * @brief  通过软 I2C 向 AHT10 写数据
 * @param  data: 数据缓冲区
 * @param  len : 数据长度
 * @retval 0:成功, 1:失败（NACK）
 */
static uint8_t aht10_write(uint8_t *data, uint8_t len)
{
    uint8_t i;

    soft_i2c_start();
    if (soft_i2c_write_byte((AHT10_ADDR << 1) | 0) != 0)
    {
        soft_i2c_stop();
        return 1;
    }

    for (i = 0; i < len; i++)
    {
        if (soft_i2c_write_byte(data[i]) != 0)
        {
            soft_i2c_stop();
            return 1;
        }
    }

    soft_i2c_stop();
    return 0;
}

/**
 * @brief  通过软 I2C 从 AHT10 读数据
 * @param  buf: 接收缓冲区
 * @param  len: 读取长度
 * @retval 0:成功, 1:失败
 */
static uint8_t aht10_read_raw(uint8_t *buf, uint8_t len)
{
    uint8_t i;

    soft_i2c_start();
    if (soft_i2c_write_byte((AHT10_ADDR << 1) | 1) != 0)
    {
        soft_i2c_stop();
        return 1;
    }

    for (i = 0; i < len; i++)
    {
        buf[i] = soft_i2c_read_byte((i == (len - 1)) ? 1 : 0);
    }

    soft_i2c_stop();
    return 0;
}

/**
 * @brief  初始化 AHT10（发送校准命令）
 * @retval 0:成功, 1:失败
 */
uint8_t aht10_init(void)
{
    uint8_t init_cmd[3] = {0xE1, 0x08, 0x00};

    soft_i2c_init();

    if (aht10_write(init_cmd, 3) != 0)
        return 1;

    HAL_Delay(20);
    return 0;
}

/**
 * @brief  读取 AHT10 温湿度
 * @param  humidity   : 传出参数，相对湿度 (%)
 * @param  temperature: 传出参数，温度 (℃)
 * @retval 0:成功, 1:通信失败, 2:传感器忙
 */
uint8_t aht10_read(float *humidity, float *temperature)
{
    uint8_t  trig_cmd[3] = {0xAC, 0x33, 0x00};
    uint8_t  raw_data[6] = {0};
    uint32_t S_rh, S_t;

    /* 触发测量 */
    if (aht10_write(trig_cmd, 3) != 0)
        return 1;

    /* 等待 ADC 转换 */
    HAL_Delay(80);

    /* 读 6 字节原始数据 */
    if (aht10_read_raw(raw_data, 6) != 0)
        return 1;

    /* 检查忙碌位 */
    if (raw_data[0] & 0x80)
        return 2;

    /* 拼接 20 位湿度 */
    S_rh = ((uint32_t)raw_data[1] << 12)
         | ((uint32_t)raw_data[2] << 4)
         | (raw_data[3] >> 4);

    /* 拼接 20 位温度 */
    S_t = (((uint32_t)raw_data[3] & 0x0F) << 16)
        | ((uint32_t)raw_data[4] << 8)
        |  raw_data[5];

    /* 转换为物理值 */
    *humidity    = ((float)S_rh / 1048576.0f) * 100.0f;
    *temperature = ((float)S_t  / 1048576.0f) * 200.0f - 50.0f;

    return 0;
}
