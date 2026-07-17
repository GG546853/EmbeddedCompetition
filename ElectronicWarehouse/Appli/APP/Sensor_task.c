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

	  /* 创建摄像头 I2C 与触摸屏 I2C2 共享 PD14 的互斥锁 */
	  cam_i2c_mutex = osMutexNew(NULL);

	  while (imx335_init())
	  {

		  __NOP();
	  }

	  imx335_set_wb_mode(0);

	  printf("[Sensor] Camera init OK, I2C mutex active\r\n");

	  if (imx335_start_capture((uint32_t)g_ltdc_framebuf) != 0) {
	      printf("[Sensor] start_capture FAILED\r\n");
	  }

	  while(1)
	  {
		  imx335_isp_background_process();
		  vTaskDelay(pdMS_TO_TICKS(10));
	  }
}
