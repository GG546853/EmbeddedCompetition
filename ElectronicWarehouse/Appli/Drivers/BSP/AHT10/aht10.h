/**
 ****************************************************************************************************
 * @file        aht10.h
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

#ifndef __AHT10_H
#define __AHT10_H

#include "main.h"

/* AHT10 I2C 7位地址定义 */
#define AHT10_ADDR          0x38
#define AHT10_WRITE_ADDR    ((AHT10_ADDR << 1))       /* 0x70 写地址 */
#define AHT10_READ_ADDR     ((AHT10_ADDR << 1) | 1)   /* 0x71 读地址 */

/* 函数声明 */
uint8_t aht10_init(void);                               /* 初始化AHT10 */
uint8_t aht10_read(float *humidity, float *temperature); /* 读取温湿度 */

#endif
