#include "Battery_task.h"
#include "adc.h"
#include "app_types.h"
#include "UART_protocol.h"
#include "vars.h"
#include "ui_bridge.h"

osThreadId_t Battery_TaskHandle;
const osThreadAttr_t BatteryTask_attributes = {
  .name = "BatteryTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};

void Battery_Task(void *argument)
{
	osDelay(3500);

	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
	Cabinet.Battery_state = true;

	while (1)
	{
		HAL_ADC_Start(&hadc2);
		if (HAL_ADC_PollForConversion(&hadc2, 10) == HAL_OK)
		{
			uint32_t raw = HAL_ADC_GetValue(&hadc2);
			float v_adc = (raw / 4095.0f) * 3.3f;
			float v_bat = v_adc * (15.1f / 5.1f);
			Cabinet.battery_voltage = v_bat;
			//uart4_send_voltage(v_bat);
			ui_set_integer(FLOW_GLOBAL_VARIABLE_ELE_CQTY, (float)100);
		}
		HAL_ADC_Stop(&hadc2);

		osDelay(2000);
	}
}
