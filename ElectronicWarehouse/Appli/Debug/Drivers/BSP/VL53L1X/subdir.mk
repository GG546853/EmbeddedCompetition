################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/VL53L1X/VL53L1X_api.c \
../Drivers/BSP/VL53L1X/VL53L1X_calibration.c \
../Drivers/BSP/VL53L1X/vl53l1_platform.c \
../Drivers/BSP/VL53L1X/vl53l1x_bsp.c 

C_DEPS += \
./Drivers/BSP/VL53L1X/VL53L1X_api.d \
./Drivers/BSP/VL53L1X/VL53L1X_calibration.d \
./Drivers/BSP/VL53L1X/vl53l1_platform.d \
./Drivers/BSP/VL53L1X/vl53l1x_bsp.d 

OBJS += \
./Drivers/BSP/VL53L1X/VL53L1X_api.o \
./Drivers/BSP/VL53L1X/VL53L1X_calibration.o \
./Drivers/BSP/VL53L1X/vl53l1_platform.o \
./Drivers/BSP/VL53L1X/vl53l1x_bsp.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/VL53L1X/%.o Drivers/BSP/VL53L1X/%.su Drivers/BSP/VL53L1X/%.cyclo: ../Drivers/BSP/VL53L1X/%.c Drivers/BSP/VL53L1X/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/Printer" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SK9822" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-VL53L1X

clean-Drivers-2f-BSP-2f-VL53L1X:
	-$(RM) ./Drivers/BSP/VL53L1X/VL53L1X_api.cyclo ./Drivers/BSP/VL53L1X/VL53L1X_api.d ./Drivers/BSP/VL53L1X/VL53L1X_api.o ./Drivers/BSP/VL53L1X/VL53L1X_api.su ./Drivers/BSP/VL53L1X/VL53L1X_calibration.cyclo ./Drivers/BSP/VL53L1X/VL53L1X_calibration.d ./Drivers/BSP/VL53L1X/VL53L1X_calibration.o ./Drivers/BSP/VL53L1X/VL53L1X_calibration.su ./Drivers/BSP/VL53L1X/vl53l1_platform.cyclo ./Drivers/BSP/VL53L1X/vl53l1_platform.d ./Drivers/BSP/VL53L1X/vl53l1_platform.o ./Drivers/BSP/VL53L1X/vl53l1_platform.su ./Drivers/BSP/VL53L1X/vl53l1x_bsp.cyclo ./Drivers/BSP/VL53L1X/vl53l1x_bsp.d ./Drivers/BSP/VL53L1X/vl53l1x_bsp.o ./Drivers/BSP/VL53L1X/vl53l1x_bsp.su

.PHONY: clean-Drivers-2f-BSP-2f-VL53L1X

