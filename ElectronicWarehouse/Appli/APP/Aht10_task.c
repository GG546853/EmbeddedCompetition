#include "Aht10_task.h"
#include "aht10.h"
#include "app_types.h"
#include "UART_protocol.h"
osThreadId_t Aht10_TaskHandle;
const osThreadAttr_t Aht10Task_attributes = {
  .name = "Aht10Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};



void Aht10_Task(void *argument)
{
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
	    osDelay(2000);
	}
error:
	Cabinet.Aht10_state = false;
	vTaskSuspend(NULL);
}
