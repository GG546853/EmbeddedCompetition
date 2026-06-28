#include "Electromagnet.h"
osThreadId_t Electromagnet_TaskHandle;
const osThreadAttr_t ElectromagnetTask_attributes = {
  .name = "ElectromagnetTask",
  .priority = (osPriority_t) osPriorityNormal + 1,
  .stack_size = 512 * 4
};

void Electromagnet_Open(uint8_t cabinet_mask)
{
    if (cabinet_mask & 0x20) HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
    if (cabinet_mask & 0x10) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
    if (cabinet_mask & 0x08) HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_7,  GPIO_PIN_SET);
    if (cabinet_mask & 0x04) HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_6,  GPIO_PIN_SET);
    if (cabinet_mask & 0x02) HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_5,  GPIO_PIN_SET);
    if (cabinet_mask & 0x01) HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_4,  GPIO_PIN_SET);

    vTaskDelay(pdMS_TO_TICKS(500));

    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_RESET);
}

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
