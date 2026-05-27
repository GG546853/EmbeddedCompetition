################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ELF_SRCS += \
../Model/st_ai_ws/build_rt_network/network.elf 

C_SRCS += \
../Model/st_ai_ws/build_rt_network/stedgeai_app_main.c 

O_SRCS += \
../Model/st_ai_ws/build_rt_network/ecloader.o \
../Model/st_ai_ws/build_rt_network/ll_aton.o \
../Model/st_ai_ws/build_rt_network/ll_aton_cipher.o \
../Model/st_ai_ws/build_rt_network/ll_aton_dbgtrc.o \
../Model/st_ai_ws/build_rt_network/ll_aton_debug.o \
../Model/st_ai_ws/build_rt_network/ll_aton_lib.o \
../Model/st_ai_ws/build_rt_network/ll_aton_lib_sw_operators.o \
../Model/st_ai_ws/build_rt_network/ll_aton_profiler.o \
../Model/st_ai_ws/build_rt_network/ll_aton_reloc_callbacks.o \
../Model/st_ai_ws/build_rt_network/ll_aton_reloc_network.o \
../Model/st_ai_ws/build_rt_network/ll_aton_rt_main.o \
../Model/st_ai_ws/build_rt_network/ll_aton_runtime.o \
../Model/st_ai_ws/build_rt_network/ll_aton_util.o \
../Model/st_ai_ws/build_rt_network/ll_sw_float.o \
../Model/st_ai_ws/build_rt_network/ll_sw_integer.o \
../Model/st_ai_ws/build_rt_network/network.o \
../Model/st_ai_ws/build_rt_network/stai_network.o \
../Model/st_ai_ws/build_rt_network/startup_stm32xx.o \
../Model/st_ai_ws/build_rt_network/stedgeai_app_main.o \
../Model/st_ai_ws/build_rt_network/syscalls.o \
../Model/st_ai_ws/build_rt_network/sysmem.o 

OBJS += \
./Model/st_ai_ws/build_rt_network/stedgeai_app_main.o 

C_DEPS += \
./Model/st_ai_ws/build_rt_network/stedgeai_app_main.d 


# Each subdirectory must supply rules for building sources it contributes
Model/st_ai_ws/build_rt_network/%.o Model/st_ai_ws/build_rt_network/%.su Model/st_ai_ws/build_rt_network/%.cyclo: ../Model/st_ai_ws/build_rt_network/%.c Model/st_ai_ws/build_rt_network/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Model" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Model-2f-st_ai_ws-2f-build_rt_network

clean-Model-2f-st_ai_ws-2f-build_rt_network:
	-$(RM) ./Model/st_ai_ws/build_rt_network/stedgeai_app_main.cyclo ./Model/st_ai_ws/build_rt_network/stedgeai_app_main.d ./Model/st_ai_ws/build_rt_network/stedgeai_app_main.o ./Model/st_ai_ws/build_rt_network/stedgeai_app_main.su

.PHONY: clean-Model-2f-st_ai_ws-2f-build_rt_network

