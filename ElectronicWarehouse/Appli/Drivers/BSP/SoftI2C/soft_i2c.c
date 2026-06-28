#include "soft_i2c.h"

/**
 * @brief  微秒级延时（NOP 忙等循环，不依赖任何硬件定时器）
 *         I2C 协议对时钟精度不敏感，只要不超过器件上限即可
 */
void soft_i2c_delay_us(uint32_t us)
{
    /* 粗略校准: 每循环约 4~5 个 CPU 周期
     *          factor = SystemCoreClock / 5000000 ≈ 120@600MHz
     *          确保 timing 不会太快，保守取略大值 */
    uint32_t factor = SystemCoreClock / 4000000U;
    if (factor == 0) factor = 1;
    uint32_t count = us * factor;
    for (volatile uint32_t i = 0; i < count; i++)
    {
        __NOP();
    }
}

/**
 * @brief  初始化软件 I2C 引脚（开漏输出模式, 初始释放总线为高电平）
 */
void soft_i2c_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* SCL */
    GPIO_InitStruct.Pin       = SOFT_I2C_SCL_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SCL_PORT, &GPIO_InitStruct);

    /* SDA */
    GPIO_InitStruct.Pin       = SOFT_I2C_SDA_PIN;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);

    /* 释放总线 → HIGH */
    SCL_H();
    SDA_H();
    soft_i2c_delay_us(10);
}

/**
 * @brief  I2C 起始信号: SCL 高电平时 SDA 下降沿
 */
void soft_i2c_start(void)
{
    SDA_H();
    soft_i2c_delay_us(SOFT_I2C_DELAY_SETUP);
    SCL_H();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
    SDA_L();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HOLD);
    SCL_L();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
}

/**
 * @brief  I2C 停止信号: SCL 高电平时 SDA 上升沿
 */
void soft_i2c_stop(void)
{
    SDA_L();
    soft_i2c_delay_us(SOFT_I2C_DELAY_SETUP);
    SCL_H();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
    SDA_H();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
}

/**
 * @brief  写一个字节到 I2C 总线, 返回 ACK 状态
 * @param  data: 要发送的字节
 * @retval 0: ACK, 1: NACK
 */
uint8_t soft_i2c_write_byte(uint8_t data)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
            SDA_H();
        else
            SDA_L();
        soft_i2c_delay_us(SOFT_I2C_DELAY_SETUP);
        SCL_H();
        soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
        SCL_L();
        soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
        data <<= 1;
    }

    /* 第 9 个时钟: 读 ACK */
    SDA_H();  /* 释放 SDA, 等待从机拉低 */
    soft_i2c_delay_us(SOFT_I2C_DELAY_SETUP);
    SCL_H();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
    uint8_t ack = SDA_READ;
    SCL_L();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);

    return ack;  /* 0 = ACK, 1 = NACK */
}

/**
 * @brief  从 I2C 总线读一个字节
 * @param  ack: 主机应答, 0 = ACK, 1 = NACK
 * @retval 读到的字节
 */
uint8_t soft_i2c_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t data = 0;

    SDA_H();  /* 释放 SDA, 让从机驱动 */
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
        SCL_H();
        soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
        if (SDA_READ)
            data |= 1;
        SCL_L();
        soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
    }

    /* 第 9 个时钟: 发送 ACK / NACK */
    if (ack)
        SDA_H();  /* NACK */
    else
        SDA_L();  /* ACK */
    soft_i2c_delay_us(SOFT_I2C_DELAY_SETUP);
    SCL_H();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
    SCL_L();
    soft_i2c_delay_us(SOFT_I2C_DELAY_HALF);
    SDA_H();  /* 释放总线 */

    return data;
}
