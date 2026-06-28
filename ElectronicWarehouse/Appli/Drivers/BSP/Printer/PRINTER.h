#ifndef __PRINTER_H
#define __PRINTER_H

#include "main.h"   // 包含 HAL 库头文件
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "usart.h"
/* ================== 宏定义区 ================== */
// 对齐方式
#define ALIGN_LEFT      0
#define ALIGN_CENTER    1
#define ALIGN_RIGHT     2

// 切纸模式
#define CUT_FULL        0  // 全切
#define CUT_HALF        1  // 半切

// 字体大小模式
#define FONT_NORMAL     0  // 正常大小
#define FONT_DOUBLE     1  // 放大一倍(倍高倍宽)

/* ================== 函数声明区 ================== */

// 0. 底层发送
void Printer_SendCmd(uint8_t* cmd, uint16_t len);

// 1. 基础控制
void Printer_Init(void);
void Printer_Reset(void);
void Printer_Feed(uint8_t lines);
void Printer_Cut(uint8_t mode);
void Printer_SetIntensity(uint8_t level);

// 2. 状态查询
void Printer_CheckStatus(void);
void Printer_CheckPaperStatus(void);

// 3. 格式设置
void Set_ChineseFontSize(uint8_t enable);
void Set_FontSize(uint8_t width_mult, uint8_t height_mult);
void Set_FontStyle(uint8_t bold, uint8_t underline, uint8_t reverse);
void Set_Alignment(uint8_t align);
void Set_LineSpacing(uint8_t space);

// 4. 内容绘制
void Print_Text(char* str);
void Print_Line(void);
void Print_Barcode(char* data);
void Print_QRCode(uint8_t size, char* data);

// 5. 业务逻辑 (模板打印)
void Printer_PrintByTemplate(char* params_str);

#endif /* __PRINTER_H */