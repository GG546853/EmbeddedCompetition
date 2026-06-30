#include "Aht10_task.h"
#include "aht10.h"
osThreadId_t Aht10_TaskHandle;
const osThreadAttr_t Aht10Task_attributes = {
  .name = "Aht10Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};

void Aht10_Task(void *argument)
{
	float  T = 0;
	float  H = 0;
	aht10_init();
	while(1)
	{
		aht10_read(&H, &T);
		printf("T = %.2f, H = %.2f", T, H);
	    osDelay(2000);
	}
}

