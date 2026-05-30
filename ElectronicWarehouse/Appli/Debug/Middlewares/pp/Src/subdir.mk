################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/pp/Src/app_postprocess_fd_blazeface_ui.c \
../Middlewares/pp/Src/fd_pp_blazeface.c \
../Middlewares/pp/Src/vision_models_pp.c 

OBJS += \
./Middlewares/pp/Src/app_postprocess_fd_blazeface_ui.o \
./Middlewares/pp/Src/fd_pp_blazeface.o \
./Middlewares/pp/Src/vision_models_pp.o 

C_DEPS += \
./Middlewares/pp/Src/app_postprocess_fd_blazeface_ui.d \
./Middlewares/pp/Src/fd_pp_blazeface.d \
./Middlewares/pp/Src/vision_models_pp.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/pp/Src/%.o Middlewares/pp/Src/%.su Middlewares/pp/Src/%.cyclo: ../Middlewares/pp/Src/%.c Middlewares/pp/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_FREERTOS -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -DARM_MATH_CM55 -DUSE_HAL_DRIVER -DSTM32N647xx -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/STM32N6xx_HAL_Driver/Inc" -I"C:/ST/STEdgeAI/4.0/Middlewares/ST/AI/Npu/Devices/STM32N6xx" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Drivers/CMSIS/DSP/Include" -I"C:/ST/STEdgeAI/4.0/Middlewares/ST/AI/Npu/ll_aton" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/pp/Inc" -I"C:/ST/STEdgeAI/4.0/Middlewares/ST/AI/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Model" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-pp-2f-Src

clean-Middlewares-2f-pp-2f-Src:
	-$(RM) ./Middlewares/pp/Src/app_postprocess_fd_blazeface_ui.cyclo ./Middlewares/pp/Src/app_postprocess_fd_blazeface_ui.d ./Middlewares/pp/Src/app_postprocess_fd_blazeface_ui.o ./Middlewares/pp/Src/app_postprocess_fd_blazeface_ui.su ./Middlewares/pp/Src/fd_pp_blazeface.cyclo ./Middlewares/pp/Src/fd_pp_blazeface.d ./Middlewares/pp/Src/fd_pp_blazeface.o ./Middlewares/pp/Src/fd_pp_blazeface.su ./Middlewares/pp/Src/vision_models_pp.cyclo ./Middlewares/pp/Src/vision_models_pp.d ./Middlewares/pp/Src/vision_models_pp.o ./Middlewares/pp/Src/vision_models_pp.su

.PHONY: clean-Middlewares-2f-pp-2f-Src

