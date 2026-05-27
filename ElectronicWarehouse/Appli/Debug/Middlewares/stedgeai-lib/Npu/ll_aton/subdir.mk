################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/stedgeai-lib/Npu/ll_aton/ecloader.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_dbgtrc.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_debug.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_freertos.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_rtos_template.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_threadx.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_zephyr.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_profiler.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_callbacks.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_network.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.c \
../Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.c 

OBJS += \
./Middlewares/stedgeai-lib/Npu/ll_aton/ecloader.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_dbgtrc.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_debug.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_freertos.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_rtos_template.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_threadx.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_zephyr.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_profiler.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_callbacks.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_network.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.o \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.o 

C_DEPS += \
./Middlewares/stedgeai-lib/Npu/ll_aton/ecloader.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_dbgtrc.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_debug.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_freertos.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_rtos_template.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_threadx.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_zephyr.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_profiler.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_callbacks.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_network.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.d \
./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/stedgeai-lib/Npu/ll_aton/%.o Middlewares/stedgeai-lib/Npu/ll_aton/%.su Middlewares/stedgeai-lib/Npu/ll_aton/%.cyclo: ../Middlewares/stedgeai-lib/Npu/ll_aton/%.c Middlewares/stedgeai-lib/Npu/ll_aton/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -c -I"C:/Users/96022/STM32Cube/Repository/STM32Cube_FW_N6_V1.3.0/Drivers/CMSIS/DSP/Include" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Model" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stedgeai-lib/Npu/ll_aton" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stedgeai-lib/Npu/Devices/STM32N6xx" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stedgeai-lib/Inc" -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/ai-postprocessing-wrapper" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-stedgeai-2d-lib-2f-Npu-2f-ll_aton

clean-Middlewares-2f-stedgeai-2d-lib-2f-Npu-2f-ll_aton:
	-$(RM) ./Middlewares/stedgeai-lib/Npu/ll_aton/ecloader.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ecloader.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ecloader.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ecloader.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_dbgtrc.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_dbgtrc.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_dbgtrc.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_dbgtrc.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_debug.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_debug.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_debug.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_debug.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_freertos.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_freertos.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_freertos.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_freertos.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_rtos_template.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_rtos_template.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_rtos_template.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_rtos_template.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_threadx.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_threadx.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_threadx.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_threadx.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_zephyr.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_zephyr.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_zephyr.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_osal_zephyr.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_profiler.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_profiler.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_profiler.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_profiler.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_callbacks.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_callbacks.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_callbacks.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_callbacks.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_network.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_network.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_network.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_reloc_network.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.su ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.cyclo ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.d ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.o ./Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.su

.PHONY: clean-Middlewares-2f-stedgeai-2d-lib-2f-Npu-2f-ll_aton

