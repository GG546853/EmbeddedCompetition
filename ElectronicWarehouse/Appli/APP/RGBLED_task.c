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
