################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_esp_littlefs.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_sd.cpp 

C_SRCS += \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_cbfs.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_fatfs.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_frogfs.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_littlefs.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_memfs.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_posix.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_stdio.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_uefi.c \
../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_win32.c 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_cbfs.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_fatfs.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_frogfs.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_littlefs.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_memfs.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_posix.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_stdio.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_uefi.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_win32.d 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_esp_littlefs.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_sd.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_cbfs.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_fatfs.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_frogfs.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_littlefs.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_memfs.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_posix.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_stdio.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_uefi.o \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_win32.o 

CPP_DEPS += \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_esp_littlefs.d \
./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_sd.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.o Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.su Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.cpp Middlewares/lvgl-release-v9.4/src/libs/fsdrv/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m55 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.o Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.su Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/libs/fsdrv/%.c Middlewares/lvgl-release-v9.4/src/libs/fsdrv/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-libs-2f-fsdrv

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-libs-2f-fsdrv:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_esp_littlefs.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_esp_littlefs.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_esp_littlefs.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_esp_littlefs.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_sd.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_sd.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_sd.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_arduino_sd.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_cbfs.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_cbfs.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_cbfs.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_cbfs.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_fatfs.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_fatfs.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_fatfs.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_fatfs.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_frogfs.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_frogfs.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_frogfs.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_frogfs.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_littlefs.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_littlefs.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_littlefs.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_littlefs.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_memfs.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_memfs.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_memfs.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_memfs.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_posix.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_posix.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_posix.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_posix.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_stdio.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_stdio.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_stdio.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_stdio.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_uefi.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_uefi.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_uefi.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_uefi.su ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_win32.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_win32.d ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_win32.o ./Middlewares/lvgl-release-v9.4/src/libs/fsdrv/lv_fs_win32.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-libs-2f-fsdrv

