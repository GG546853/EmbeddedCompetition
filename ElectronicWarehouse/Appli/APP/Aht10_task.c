#include "Aht10_task.h"
#include "aht10.h"
#include "app_types.h"
#include "UART_protocol.h"
#include "vars.h"
#include "ui_bridge.h"

osThreadId_t Aht10_TaskHandle;
const osThreadAttr_t Aht10Task_attributes = {
  .name = "Aht10Task",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};

void Aht10_Task(void *argument)
{
	osDelay(3500);  // 等待 LVGL UI 初始化完成

	if(aht10_init())
	{
		goto error;
	}
	Cabinet.Aht10_state = true;
	while(1)
	{
		aht10_read(&Cabinet.humidity, &Cabinet.temperature);
		uart4_send_temperature(Cabinet.temperature);
		uart4_send_humidity(Cabinet.humidity);
		ui_set_integer(FLOW_GLOBAL_VARIABLE_TEM, (int)Cabinet.temperature);
		ui_set_integer(FLOW_GLOBAL_VARIABLE_HUM, (int)Cabinet.humidity);
	    osDelay(2000);
	}
error:
	Cabinet.Aht10_state = false;
	vTaskSuspend(NULL);
}
