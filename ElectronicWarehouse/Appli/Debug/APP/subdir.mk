################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/AI_task.c \
../APP/Barcode_task.c \
../APP/Electromagnet.c \
../APP/LCD_task.c \
../APP/LV_task.c \
../APP/Printer_task.c \
../APP/RGBLED_task.c \
../APP/Sensor_task.c \
../APP/TempHumidity_task.c \
../APP/UART_protocol.c \
../APP/VL53L1X_task.c 

C_DEPS += \
./APP/AI_task.d \
./APP/Barcode_task.d \
./APP/Electromagnet.d \
./APP/LCD_task.d \
./APP/LV_task.d \
./APP/Printer_task.d \
./APP/RGBLED_task.d \
./APP/Sensor_task.d \
./APP/TempHumidity_task.d \
./APP/UART_protocol.d \
./APP/VL53L1X_task.d 

OBJS += \
./APP/AI_task.o \
./APP/Barcode_task.o \
./APP/Electromagnet.o \
./APP/LCD_task.o \
./APP/LV_task.o \
./APP/Printer_task.o \
./APP/RGBLED_task.o \
./APP/Sensor_task.o \
./APP/TempHumidity_task.o \
./APP/UART_protocol.o \
./APP/VL53L1X_task.o 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o APP/%.su APP/%.cyclo: ../APP/%.c APP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/Printer" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SK9822" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-APP

clean-APP:
	-$(RM) ./APP/AI_task.cyclo ./APP/AI_task.d ./APP/AI_task.o ./APP/AI_task.su ./APP/Barcode_task.cyclo ./APP/Barcode_task.d ./APP/Barcode_task.o ./APP/Barcode_task.su ./APP/Electromagnet.cyclo ./APP/Electromagnet.d ./APP/Electromagnet.o ./APP/Electromagnet.su ./APP/LCD_task.cyclo ./APP/LCD_task.d ./APP/LCD_task.o ./APP/LCD_task.su ./APP/LV_task.cyclo ./APP/LV_task.d ./APP/LV_task.o ./APP/LV_task.su ./APP/Printer_task.cyclo ./APP/Printer_task.d ./APP/Printer_task.o ./APP/Printer_task.su ./APP/RGBLED_task.cyclo ./APP/RGBLED_task.d ./APP/RGBLED_task.o ./APP/RGBLED_task.su ./APP/Sensor_task.cyclo ./APP/Sensor_task.d ./APP/Sensor_task.o ./APP/Sensor_task.su ./APP/TempHumidity_task.cyclo ./APP/TempHumidity_task.d ./APP/TempHumidity_task.o ./APP/TempHumidity_task.su ./APP/UART_protocol.cyclo ./APP/UART_protocol.d ./APP/UART_protocol.o ./APP/UART_protocol.su ./APP/VL53L1X_task.cyclo ./APP/VL53L1X_task.d ./APP/VL53L1X_task.o ./APP/VL53L1X_task.su

.PHONY: clean-APP

