#include "printer.h"
/* 
 * 声明外部变量：假设你在 STM32CubeMX 中配置打印机连接在 USART1，
 * 那么在 main.c 中会有一个 UART_HandleTypeDef huart1;
 * 请根据你的实际情况，把这里的 huart_printer 改成对应的串口句柄（比如 huart1 或 huart2）  extern UART_HandleTypeDef huart2;
 */
extern UART_HandleTypeDef huart7;

#define huart_printer huart7

/* ================== 0. 底层发送 ================== */
void Printer_SendCmd(uint8_t* cmd, uint16_t len) {
    // 阻塞式发送，超时时间设为 1000ms
    HAL_UART_Transmit(&huart_printer, cmd, len, 1000);
}

/* ================== 1. 基础控制 ================== */
void Printer_Init(void) {
    uint8_t cmd_gbk[] = {0x1B, 0x74, 0x00};  // ESC t 0 选择 GBK
    HAL_UART_Transmit(&huart_printer, cmd_gbk, sizeof(cmd_gbk), 100);
}

void Printer_Reset(void) {
    uint8_t cmd[] = {0x1E, 0x23, 0x52, 0x54, 0x46, 0x41, 0x2A, 0x3B}; // RS#RTFA*;
    Printer_SendCmd(cmd, sizeof(cmd));
}

void Printer_Feed(uint8_t lines) {
    uint8_t cmd[] = {0x1B, 0x64, lines}; // ESC d n
    Printer_SendCmd(cmd, sizeof(cmd));
}

void Printer_Cut(uint8_t mode) {
    uint8_t cmd[] = {0x1D, 0x56, mode}; // GS V m
    Printer_SendCmd(cmd, sizeof(cmd));
}

void Printer_SetIntensity(uint8_t level) {
    char cmd[32];
    if (level > 39) level = 39;
    sprintf(cmd, "\x1E\x23PTDP=%d;", level);
    Printer_SendCmd((uint8_t*)cmd, strlen(cmd));
}

/* ================== 2. 状态查询 ================== */
void Printer_CheckStatus(void) {
    uint8_t cmd[] = {0x10, 0x04, 0x01};
    Printer_SendCmd(cmd, sizeof(cmd));
}

void Printer_CheckPaperStatus(void) {
    uint8_t cmd[] = {0x10, 0x04, 0x04};
    Printer_SendCmd(cmd, sizeof(cmd));
}

/* ================== 3. 格式设置 ================== */
void Set_ChineseFontSize(uint8_t enable) {
    uint8_t cmd[] = {0x1C, 0x57, enable ? 0x01 : 0x00}; // FS W n
    Printer_SendCmd(cmd, sizeof(cmd));
}

void Set_FontSize(uint8_t width_mult, uint8_t height_mult) {
    uint8_t w = (width_mult > 0 && width_mult <= 8) ? width_mult - 1 : 0;
    uint8_t h = (height_mult > 0 && height_mult <= 8) ? height_mult - 1 : 0;
    uint8_t n = (w << 4) | h;
    uint8_t cmd[] = {0x1D, 0x21, n}; // GS ! n
    Printer_SendCmd(cmd, sizeof(cmd));
}

void Set_FontStyle(uint8_t bold, uint8_t underline, uint8_t reverse) {
    uint8_t cmd_bold[] = {0x1B, 0x45, bold ? 1 : 0};      
    uint8_t cmd_ul[]   = {0x1B, 0x2D, underline ? 2 : 0}; 
    uint8_t cmd_rev[]  = {0x1D, 0x42, reverse ? 1 : 0};   
    
    Printer_SendCmd(cmd_bold, sizeof(cmd_bold));
    Printer_SendCmd(cmd_ul, sizeof(cmd_ul));
    Printer_SendCmd(cmd_rev, sizeof(cmd_rev));
}

void Set_Alignment(uint8_t align) {
    uint8_t cmd[] = {0x1B, 0x61, align}; // ESC a n
    Printer_SendCmd(cmd, sizeof(cmd));
}

void Set_LineSpacing(uint8_t space) {
    uint8_t cmd[] = {0x1B, 0x33, space}; // ESC 3 n
    Printer_SendCmd(cmd, sizeof(cmd));
}

/* ================== 4. 内容绘制 ================== */
void Print_Text(char* str) {
    Printer_SendCmd((uint8_t*)str, strlen(str));
}

void Print_Line(void) {
    char* line = "--------------------------------\n";
    Printer_SendCmd((uint8_t*)line, strlen(line));
}

void Print_Barcode(char* data) {
    uint8_t len = strlen(data);
    uint8_t cmd_txt_pos[] = {0x1D, 0x48, 0x02};
    Printer_SendCmd(cmd_txt_pos, sizeof(cmd_txt_pos));
    
    uint8_t cmd_height[] = {0x1D, 0x68, 60};
    Printer_SendCmd(cmd_height, sizeof(cmd_height));
    
    uint8_t cmd_print[] = {0x1D, 0x6B, 73, len};
    Printer_SendCmd(cmd_print, sizeof(cmd_print));
    Printer_SendCmd((uint8_t*)data, len);
}

void Print_QRCode(uint8_t size, char* data) {
    uint16_t len = strlen(data);
    uint16_t k = len + 3;
    uint8_t pL = k & 0xFF;
    uint8_t pH = (k >> 8) & 0xFF;

    uint8_t cmd_size[] = {0x1D, 0x28, 0x6B, 0x03, 0x00, 0x31, 0x43, size};
    Printer_SendCmd(cmd_size, sizeof(cmd_size));

    uint8_t cmd_store[] = {0x1D, 0x28, 0x6B, pL, pH, 0x31, 0x50, 0x30};
    Printer_SendCmd(cmd_store, sizeof(cmd_store));
    Printer_SendCmd((uint8_t*)data, len);

    uint8_t cmd_print[] = {0x1D, 0x28, 0x6B, 0x03, 0x00, 0x31, 0x51, 0x30};
    Printer_SendCmd(cmd_print, sizeof(cmd_print));
}

/* ================== 5. 业务逻辑 ================== */
void Printer_PrintByTemplate(char* params_str) {
    char mat_name[32] = {0};
    char mat_spec[32] = {0};
    char mat_qty[16] = {0};
    
    // 解析数据
    sscanf(params_str, "%[^,],%[^,],%s", mat_name, mat_spec, mat_qty);

    Printer_Init();
    
    // 打印表头
    Set_Alignment(ALIGN_CENTER); 
    Set_ChineseFontSize(FONT_DOUBLE); 
    Set_FontStyle(1, 0, 0); 
    Print_Text("最近使用物料\n");
    
    // 打印基本信息
    Set_ChineseFontSize(FONT_NORMAL); 
    Set_FontStyle(0, 0, 0); 
    Set_Alignment(ALIGN_LEFT); 
    Print_Line();
    
    char buffer[128];
    sprintf(buffer, "物料名称: %s\n规格参数: %s\n打印数量: %s\n", mat_name, mat_spec, mat_qty);
    Print_Text(buffer);
    
    Print_Line();
    
    // 打印扫码枪用的条码
    Set_Alignment(ALIGN_CENTER); 
    Print_Barcode(mat_spec); 
    Print_Text("\n"); 
    
    // 走纸并切断
    Printer_Feed(4); 
    Printer_Cut(CUT_HALF);
}
