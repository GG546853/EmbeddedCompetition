#include "lcd_task.h"
#include "led.h"

osThreadId_t LED_TaskHandle;
const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

void led_Task(void *argument)
{
	while(1)
	{
		LED0_TOGGLE();
		vTaskDelay(pdMS_TO_TICKS(500));
		LED1_TOGGLE();
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
