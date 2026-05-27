################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/fd_pp_blazeface.c \
../Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp.c \
../Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_is8.c \
../Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_iu8.c 

OBJS += \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/fd_pp_blazeface.o \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp.o \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_is8.o \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_iu8.o 

C_DEPS += \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/fd_pp_blazeface.d \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp.d \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_is8.d \
./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_iu8.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/%.o Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/%.su Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/%.cyclo: ../Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/%.c Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -c -I"C:/Users/96022/STM32Cube/Repository/STM32Cube_FW_N6_V1.3.0/Drivers/CMSIS/DSP/Include" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Model" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stedgeai-lib/Npu/ll_aton" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stedgeai-lib/Npu/Devices/STM32N6xx" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stedgeai-lib/Inc" -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/ai-postprocessing-wrapper" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-stm32-2d-vision-2d-models-2d-postprocessing-2f-lib_vision_models_pp-2f-Src

clean-Middlewares-2f-stm32-2d-vision-2d-models-2d-postprocessing-2f-lib_vision_models_pp-2f-Src:
	-$(RM) ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/fd_pp_blazeface.cyclo ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/fd_pp_blazeface.d ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/fd_pp_blazeface.o ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/fd_pp_blazeface.su ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp.cyclo ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp.d ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp.o ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp.su ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_is8.cyclo ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_is8.d ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_is8.o ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_is8.su ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_iu8.cyclo ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_iu8.d ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_iu8.o ./Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/vision_models_pp_maxi_iu8.su

.PHONY: clean-Middlewares-2f-stm32-2d-vision-2d-models-2d-postprocessing-2f-lib_vision_models_pp-2f-Src

