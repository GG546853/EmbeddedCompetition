#include "Sensor_task.h"
#include "imx335.h"
#include "rgblcd.h"
osThreadId_t Sensor_TaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .priority = (osPriority_t) osPriorityNormal + 1,
  .stack_size = 512 * 4
};

extern DMA2D_HandleTypeDef hdma2d;

void Sensor_Task(void *argument)
{
	  rgblcd_show_string(30, 110, 200, 16, 16, "IMX335 OK!   ", RED);
	  imx335_start_capture((uint32_t)g_ltdc_lcd_framebuf);
	  //imx335_stop_capture();
	while(1)
	{
/*		hdma2d.Init.Mode = DMA2D_M2M; // 内存到内存模式
		//hdma2d.Init.OutputOffset = 1280 - 800; // 屏幕宽度 - 图像宽度 (极其关键，解决画面倾斜！)
		HAL_DMA2D_Init(&hdma2d);

		// 启动 DMA2D 搬运 (源地址：内部小数组，目的地址：外部大数组)
		HAL_DMA2D_Start(&hdma2d,
		                (uint32_t)internal_test_buf,
		                (uint32_t)g_ltdc_lcd_framebuf,
		                800,
		                480);
		HAL_DMA2D_PollForTransfer(&hdma2d, 100); // 等待搬运完成
*/
		imx335_isp_background_process();

		vTaskDelay(pdMS_TO_TICKS(1));
	}
}
