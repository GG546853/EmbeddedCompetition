#include "Sensor_task.h"
#include "imx335.h"
#include "rgblcd.h"
osThreadId_t Sensor_TaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .priority = (osPriority_t) osPriorityNormal + 1,
  .stack_size = 512 * 4
};

void Sensor_Task(void *argument)
{

	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, 1);
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, 1);

	  while (imx335_init())
	  {

		  __NOP();
	  }

	  imx335_set_wb_mode(6500);

	  /* 摄像头 I2C 只用一次，释放 PD14 给触摸屏硬件 I2C2，PC2 置高释放 */
	  imx335_io_deinit();
	  printf("[Sensor] Camera init OK, I2C released to touch\r\n");

	  if (imx335_start_capture((uint32_t)g_ltdc_framebuf) != 0) {
	      printf("[Sensor] start_capture FAILED\r\n");
	  }

	  while(1)
	  {
		  imx335_isp_background_process();
		  vTaskDelay(pdMS_TO_TICKS(10));
	  }
}
