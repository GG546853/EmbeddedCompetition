#include "Sensor_task.h"
#include "imx335.h"
#include "rgblcd.h"
#include "app_x-cube-ai.h"
osThreadId_t Sensor_TaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .priority = (osPriority_t) osPriorityNormal + 1,
  .stack_size = 1024 * 4
};

extern DMA2D_HandleTypeDef hdma2d;


void Sensor_Task(void *argument)
{
	  //rgblcd_show_string(30, 110, 200, 16, 16, "IMX335 OK!   ", RED);
	  imx335_start_capture((uint32_t)g_ltdc_lcd_framebuf);
	  //imx335_stop_capture();
	while(1)
	{
		imx335_isp_background_process();
		MX_X_CUBE_AI_Process();
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}
