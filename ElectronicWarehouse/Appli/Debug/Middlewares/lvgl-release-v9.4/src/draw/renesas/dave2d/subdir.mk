################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_arc.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_border.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_fill.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_image.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_label.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_line.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_mask_rectangle.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_triangle.c \
../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_utils.c 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_arc.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_border.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_fill.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_image.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_label.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_line.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_mask_rectangle.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_triangle.d \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_utils.d 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_arc.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_border.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_fill.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_image.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_label.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_line.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_mask_rectangle.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_triangle.o \
./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_utils.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/%.o Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/%.su Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/%.c Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/Printer" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SK9822" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-renesas-2f-dave2d

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-renesas-2f-dave2d:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_arc.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_arc.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_arc.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_arc.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_border.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_border.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_border.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_border.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_fill.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_fill.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_fill.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_fill.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_image.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_image.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_image.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_image.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_label.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_label.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_label.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_label.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_line.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_line.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_line.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_line.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_mask_rectangle.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_mask_rectangle.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_mask_rectangle.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_mask_rectangle.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_triangle.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_triangle.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_triangle.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_triangle.su ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_utils.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_utils.d ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_utils.o ./Middlewares/lvgl-release-v9.4/src/draw/renesas/dave2d/lv_draw_dave2d_utils.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-renesas-2f-dave2d

