#include "RGBLED_task.h"
#include "touch.h"
#include "rgblcd.h"
osThreadId_t RGBLED_TaskHandle;
const osThreadAttr_t RGBLEDTask_attributes = {
  .name = "RGBLEDTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};



void RGBLED_Task(void *argument)
{

	while(1)
	{
//		  rgblcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
//		  rgblcd_show_string(30, 70, 200, 16, 16, "TOUCH TEST", RED);
//		  rgblcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void Preprocess_Camera_Image(uint16_t *src_buf, int8_t *dst_buf)
{
    // 1. 计算中心裁剪的起始坐标
    int start_x = (CAM_WIDTH - AI_INPUT_WIDTH) / 2;
    int start_y = (CAM_HEIGHT - AI_INPUT_HEIGHT) / 2;

    // 如果屏幕尺寸小于 AI 尺寸，防止越界
    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;

    // 2. CHW 格式的通道指针偏移
    // R 通道从 0 开始，G 通道从 416*416 开始，B 通道从 2*416*416 开始
    int plane_size = AI_INPUT_WIDTH * AI_INPUT_HEIGHT;
    int8_t *r_plane = dst_buf;
    int8_t *g_plane = dst_buf + plane_size;
    int8_t *b_plane = dst_buf + 2 * plane_size;

    int dst_idx = 0;

    // 3. 遍历目标尺寸的像素
    for (int y = 0; y < AI_INPUT_HEIGHT; y++)
    {
        for (int x = 0; x < AI_INPUT_WIDTH; x++)
        {
            // 获取原图坐标
            int src_x = start_x + x;
            int src_y = start_y + y;

            // 读取 RGB565 像素
            uint16_t pixel = src_buf[src_y * CAM_WIDTH + src_x];

            // 提取 R, G, B 分量 (5-bit, 6-bit, 5-bit)
            uint8_t r5 = (pixel >> 11) & 0x1F;
            uint8_t g6 = (pixel >> 5)  & 0x3F;
            uint8_t b5 = pixel         & 0x1F;

            // 扩展到 8-bit (RGB888) -> 采用位移补偿法提高精度
            uint8_t r8 = (r5 << 3) | (r5 >> 2);
            uint8_t g8 = (g6 << 2) | (g6 >> 4);
            uint8_t b8 = (b5 << 3) | (b5 >> 2);

            // 4. 量化转换：uint8 [0, 255] 映射到 int8 [-128, 127]
            // 公式：int8_val = (int)uint8_val - 128
            r_plane[dst_idx] = (int8_t)((int)r8 - 128);
            g_plane[dst_idx] = (int8_t)((int)g8 - 128);
            b_plane[dst_idx] = (int8_t)((int)b8 - 128);

            dst_idx++;
        }
    }
}

void LCD_Draw_Rect(uint16_t *framebuf, int x, int y, int w, int h, uint16_t color)
{
    // 假设屏幕分辨率
    int screen_w = 800;
    int screen_h = 480;

    // 边界保护
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + w >= screen_w) w = screen_w - x - 1;
    if (y + h >= screen_h) h = screen_h - y - 1;

    // 画上下两条横线
    for (int i = x; i < x + w; i++) {
        framebuf[y * screen_w + i] = color;
        framebuf[(y + h) * screen_w + i] = color;
    }
    // 画左右两条竖线
    for (int i = y; i < y + h; i++) {
        framebuf[i * screen_w + x] = color;
        framebuf[i * screen_w + (x + w)] = color;
    }
}
