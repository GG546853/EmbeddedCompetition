################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_al88.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888_premultiplied.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_i1.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_l8.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565_swapped.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.c 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_al88.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888_premultiplied.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_i1.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_l8.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565_swapped.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.d 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_al88.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888_premultiplied.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_i1.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_l8.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565_swapped.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/draw/sw/blend/%.o Middlewares/lvgl-release-v9.4/src/draw/sw/blend/%.su Middlewares/lvgl-release-v9.4/src/draw/sw/blend/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/%.c Middlewares/lvgl-release-v9.4/src/draw/sw/blend/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw-2f-blend

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw-2f-blend:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_al88.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_al88.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_al88.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_al88.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888_premultiplied.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888_premultiplied.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888_premultiplied.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888_premultiplied.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_i1.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_i1.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_i1.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_i1.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_l8.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_l8.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_l8.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_l8.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565_swapped.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565_swapped.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565_swapped.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565_swapped.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw-2f-blend

