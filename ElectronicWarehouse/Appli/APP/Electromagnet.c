#include "Electromagnet.h"
osThreadId_t Electromagnet_TaskHandle;
const osThreadAttr_t ElectromagnetTask_attributes = {
  .name = "ElectromagnetTask",
  .priority = (osPriority_t) osPriorityNormal + 1,
  .stack_size = 512 * 4
};
//3 锁  6 电磁铁
void Electromagnet_Task(void *argument)
{

	HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_3, 1);
	vTaskDelay(pdMS_TO_TICKS(1000));
	HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_3, 0);
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));

	}
}
