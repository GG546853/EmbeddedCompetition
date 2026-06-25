################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_arc.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_border.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_box_shadow.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_fill.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_grad.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_img.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_letter.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_line.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask_rect.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_transform.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_triangle.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_utils.c \
../Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_vector.c 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_arc.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_border.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_box_shadow.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_fill.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_grad.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_img.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_letter.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_line.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask_rect.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_transform.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_triangle.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_utils.d \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_vector.d 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_arc.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_border.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_box_shadow.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_fill.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_grad.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_img.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_letter.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_line.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask_rect.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_transform.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_triangle.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_utils.o \
./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_vector.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/draw/sw/%.o Middlewares/lvgl-release-v9.4/src/draw/sw/%.su Middlewares/lvgl-release-v9.4/src/draw/sw/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/draw/sw/%.c Middlewares/lvgl-release-v9.4/src/draw/sw/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_arc.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_arc.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_arc.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_arc.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_border.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_border.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_border.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_border.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_box_shadow.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_box_shadow.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_box_shadow.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_box_shadow.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_fill.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_fill.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_fill.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_fill.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_grad.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_grad.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_grad.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_grad.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_img.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_img.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_img.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_img.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_letter.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_letter.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_letter.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_letter.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_line.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_line.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_line.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_line.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask_rect.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask_rect.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask_rect.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_mask_rect.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_transform.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_transform.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_transform.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_transform.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_triangle.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_triangle.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_triangle.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_triangle.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_utils.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_utils.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_utils.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_utils.su ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_vector.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_vector.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_vector.o ./Middlewares/lvgl-release-v9.4/src/draw/sw/lv_draw_sw_vector.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw

