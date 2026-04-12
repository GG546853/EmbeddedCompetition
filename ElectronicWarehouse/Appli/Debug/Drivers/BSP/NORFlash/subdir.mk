################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/NORFlash/norflash.c \
../Drivers/BSP/NORFlash/norflash_by25fq128el.c \
../Drivers/BSP/NORFlash/norflash_mx25um25645g.c \
../Drivers/BSP/NORFlash/norflash_xspi.c 

OBJS += \
./Drivers/BSP/NORFlash/norflash.o \
./Drivers/BSP/NORFlash/norflash_by25fq128el.o \
./Drivers/BSP/NORFlash/norflash_mx25um25645g.o \
./Drivers/BSP/NORFlash/norflash_xspi.o 

C_DEPS += \
./Drivers/BSP/NORFlash/norflash.d \
./Drivers/BSP/NORFlash/norflash_by25fq128el.d \
./Drivers/BSP/NORFlash/norflash_mx25um25645g.d \
./Drivers/BSP/NORFlash/norflash_xspi.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/NORFlash/%.o Drivers/BSP/NORFlash/%.su Drivers/BSP/NORFlash/%.cyclo: ../Drivers/BSP/NORFlash/%.c Drivers/BSP/NORFlash/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-NORFlash

clean-Drivers-2f-BSP-2f-NORFlash:
	-$(RM) ./Drivers/BSP/NORFlash/norflash.cyclo ./Drivers/BSP/NORFlash/norflash.d ./Drivers/BSP/NORFlash/norflash.o ./Drivers/BSP/NORFlash/norflash.su ./Drivers/BSP/NORFlash/norflash_by25fq128el.cyclo ./Drivers/BSP/NORFlash/norflash_by25fq128el.d ./Drivers/BSP/NORFlash/norflash_by25fq128el.o ./Drivers/BSP/NORFlash/norflash_by25fq128el.su ./Drivers/BSP/NORFlash/norflash_mx25um25645g.cyclo ./Drivers/BSP/NORFlash/norflash_mx25um25645g.d ./Drivers/BSP/NORFlash/norflash_mx25um25645g.o ./Drivers/BSP/NORFlash/norflash_mx25um25645g.su ./Drivers/BSP/NORFlash/norflash_xspi.cyclo ./Drivers/BSP/NORFlash/norflash_xspi.d ./Drivers/BSP/NORFlash/norflash_xspi.o ./Drivers/BSP/NORFlash/norflash_xspi.su

.PHONY: clean-Drivers-2f-BSP-2f-NORFlash

