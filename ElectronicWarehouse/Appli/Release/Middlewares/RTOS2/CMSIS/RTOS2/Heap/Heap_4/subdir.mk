################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c 

OBJS += \
./Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/heap_4.o 

C_DEPS += \
./Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/heap_4.o: C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-RTOS2-2f-CMSIS-2f-RTOS2-2f-Heap-2f-Heap_4

clean-Middlewares-2f-RTOS2-2f-CMSIS-2f-RTOS2-2f-Heap-2f-Heap_4:
	-$(RM) ./Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/heap_4.cyclo ./Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/heap_4.d ./Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/heap_4.o ./Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/heap_4.su

.PHONY: clean-Middlewares-2f-RTOS2-2f-CMSIS-2f-RTOS2-2f-Heap-2f-Heap_4

