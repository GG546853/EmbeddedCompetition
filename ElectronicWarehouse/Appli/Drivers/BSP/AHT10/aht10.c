/**
 ****************************************************************************************************
 * @file        aht10.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-13
 * @brief       AHT10 温湿度传感器驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 N647开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "aht10.h"

extern I3C_HandleTypeDef hi3c2;

/**
 * @brief   通过 I3C 的 I2C 兼容模式发送数据
 * @param   data: 发送数据缓冲区
 * @param   len : 发送数据长度
 * @retval  HAL 状态
 */
static HAL_StatusTypeDef aht10_i2c_write(uint8_t *data, uint8_t len)
{
    I3C_PrivateTypeDef private_desc = {0};
    I3C_XferTypeDef     xfer = {0};

    private_desc.TargetAddr = AHT10_WRITE_ADDR;
    private_desc.TxBuf.pBuffer = data;
    private_desc.TxBuf.Size = len;
    private_desc.Direction = HAL_I3C_DIRECTION_WRITE;

    if (HAL_I3C_AddDescToFrame(&hi3c2, NULL, &private_desc, &xfer, 1, I2C_PRIVATE_WITHOUT_ARB_STOP) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_I3C_Ctrl_Transmit(&hi3c2, &xfer, 100);
}

/**
 * @brief   通过 I3C 的 I2C 兼容模式接收数据
 * @param   buf: 接收数据缓冲区
 * @param   len: 接收数据长度
 * @retval  HAL 状态
 */
static HAL_StatusTypeDef aht10_i2c_read(uint8_t *buf, uint8_t len)
{
    I3C_PrivateTypeDef private_desc = {0};
    I3C_XferTypeDef     xfer = {0};

    private_desc.TargetAddr = AHT10_WRITE_ADDR;
    private_desc.RxBuf.pBuffer = buf;
    private_desc.RxBuf.Size = len;
    private_desc.Direction = HAL_I3C_DIRECTION_READ;

    if (HAL_I3C_AddDescToFrame(&hi3c2, NULL, &private_desc, &xfer, 1, I2C_PRIVATE_WITHOUT_ARB_STOP) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_I3C_Ctrl_Receive(&hi3c2, &xfer, 100);
}

/**
 * @brief   初始化AHT10
 * @param   无
 * @retval  初始化结果
 * @arg     0: 初始化成功
 * @arg     1: 初始化失败
 */
uint8_t aht10_init(void)
{
    uint8_t init_cmd[3] = {0xE1, 0x08, 0x00};

    if (aht10_i2c_write(init_cmd, 3) != HAL_OK)
    {
        return 1;
    }

    HAL_Delay(20);  /* 等待传感器完成校准 */
    return 0;
}

/**
 * @brief   从 AHT10 读取温湿度
 * @param   humidity    : 传出参数，相对湿度 (%)
 * @param   temperature : 传出参数，温度 (℃)
 * @retval  读取结果
 * @arg     0: 读取成功
 * @arg     1: I3C 通信失败
 * @arg     2: 传感器忙（正在转换中）
 */
uint8_t aht10_read(float *humidity, float *temperature)
{
    uint8_t  trig_cmd[3] = {0xAC, 0x33, 0x00};
    uint8_t  raw_data[6] = {0};
    uint32_t S_rh, S_t;

    /* 1. 发送触发测量命令 */
    if (aht10_i2c_write(trig_cmd, 3) != HAL_OK)
    {
        return 1;
    }

    /* 2. 延时 80ms 等待 ADC 转换完成 */
    HAL_Delay(80);

    /* 3. 读取 6 字节原始数据 */
    if (aht10_i2c_read(raw_data, 6) != HAL_OK)
    {
        return 1;
    }

    /* 4. 检查忙碌位 (Bit[7])，为 1 表示设备忙 */
    if (raw_data[0] & 0x80)
    {
        return 2;
    }

    /* 5. 拼接 20 位湿度原始值 */
    S_rh = ((uint32_t)raw_data[1] << 12)
         | ((uint32_t)raw_data[2] << 4)
         | (raw_data[3] >> 4);

    /* 6. 拼接 20 位温度原始值 */
    S_t = (((uint32_t)raw_data[3] & 0x0F) << 16)
        | ((uint32_t)raw_data[4] << 8)
        |  raw_data[5];

    /* 7. 根据手册公式转换为物理值 */
    *humidity    = ((float)S_rh / 1048576.0f) * 100.0f;
    *temperature = ((float)S_t  / 1048576.0f) * 200.0f - 50.0f;

    return 0;
}
