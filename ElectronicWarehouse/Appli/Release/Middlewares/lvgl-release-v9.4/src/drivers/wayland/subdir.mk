################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland_smm.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_cache.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_dmabuf.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_keyboard.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer_axis.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_seat.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_shm.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_touch.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window_decorations.c \
../Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_xdg_shell.c 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland_smm.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_cache.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_dmabuf.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_keyboard.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer_axis.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_seat.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_shm.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_touch.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window_decorations.d \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_xdg_shell.d 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland_smm.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_cache.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_dmabuf.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_keyboard.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer_axis.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_seat.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_shm.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_touch.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window_decorations.o \
./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_xdg_shell.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/drivers/wayland/%.o Middlewares/lvgl-release-v9.4/src/drivers/wayland/%.su Middlewares/lvgl-release-v9.4/src/drivers/wayland/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/drivers/wayland/%.c Middlewares/lvgl-release-v9.4/src/drivers/wayland/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-drivers-2f-wayland

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-drivers-2f-wayland:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland_smm.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland_smm.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland_smm.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wayland_smm.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_cache.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_cache.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_cache.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_cache.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_dmabuf.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_dmabuf.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_dmabuf.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_dmabuf.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_keyboard.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_keyboard.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_keyboard.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_keyboard.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer_axis.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer_axis.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer_axis.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_pointer_axis.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_seat.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_seat.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_seat.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_seat.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_shm.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_shm.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_shm.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_shm.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_touch.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_touch.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_touch.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_touch.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window_decorations.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window_decorations.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window_decorations.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_window_decorations.su ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_xdg_shell.cyclo ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_xdg_shell.d ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_xdg_shell.o ./Middlewares/lvgl-release-v9.4/src/drivers/wayland/lv_wl_xdg_shell.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-drivers-2f-wayland

