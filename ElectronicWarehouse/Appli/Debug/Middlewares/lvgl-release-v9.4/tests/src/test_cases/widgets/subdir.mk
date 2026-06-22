################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_animimg.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arc.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arclabel.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_bar.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btn.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btnmatrix.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_calendar.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_canvas.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_chart.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_checkbox.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_dropdown.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_image.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imagebutton.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imgfont.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_keyboard.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_label.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_led.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_line.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_list.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_lottie.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_msgbox.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_flags.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_pos.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_property.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_transform.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_tree.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_objid.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_roller.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_scale.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_slider.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_span.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinbox.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinner.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_switch.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_table.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_tabview.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_textarea.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_win.c 

OBJS += \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_animimg.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arc.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arclabel.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_bar.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btn.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btnmatrix.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_calendar.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_canvas.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_chart.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_checkbox.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_dropdown.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_image.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imagebutton.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imgfont.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_keyboard.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_label.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_led.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_line.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_list.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_lottie.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_msgbox.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_flags.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_pos.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_property.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_transform.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_tree.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_objid.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_roller.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_scale.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_slider.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_span.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinbox.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinner.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_switch.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_table.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_tabview.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_textarea.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_win.o 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_animimg.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arc.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arclabel.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_bar.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btn.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btnmatrix.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_calendar.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_canvas.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_chart.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_checkbox.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_dropdown.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_image.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imagebutton.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imgfont.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_keyboard.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_label.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_led.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_line.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_list.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_lottie.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_msgbox.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_flags.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_pos.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_property.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_transform.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_tree.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_objid.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_roller.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_scale.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_slider.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_span.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinbox.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinner.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_switch.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_table.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_tabview.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_textarea.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_win.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/%.o Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/%.su Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/%.cyclo: ../Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/%.c Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases-2f-widgets

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases-2f-widgets:
	-$(RM) ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_animimg.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_animimg.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_animimg.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_animimg.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arc.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arc.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arc.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arc.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arclabel.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arclabel.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arclabel.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_arclabel.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_bar.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_bar.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_bar.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_bar.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btn.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btn.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btn.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btn.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btnmatrix.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btnmatrix.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btnmatrix.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_btnmatrix.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_calendar.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_calendar.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_calendar.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_calendar.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_canvas.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_canvas.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_canvas.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_canvas.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_chart.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_chart.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_chart.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_chart.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_checkbox.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_checkbox.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_checkbox.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_checkbox.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_dropdown.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_dropdown.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_dropdown.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_dropdown.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_image.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_image.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_image.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_image.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imagebutton.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imagebutton.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imagebutton.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imagebutton.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imgfont.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imgfont.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imgfont.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_imgfont.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_keyboard.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_keyboard.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_keyboard.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_keyboard.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_label.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_label.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_label.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_label.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_led.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_led.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_led.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_led.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_line.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_line.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_line.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_line.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_list.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_list.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_list.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_list.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_lottie.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_lottie.d
	-$(RM) ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_lottie.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_lottie.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_msgbox.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_msgbox.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_msgbox.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_msgbox.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_flags.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_flags.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_flags.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_flags.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_pos.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_pos.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_pos.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_pos.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_property.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_property.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_property.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_property.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_transform.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_transform.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_transform.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_transform.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_tree.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_tree.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_tree.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_obj_tree.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_objid.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_objid.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_objid.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_objid.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_roller.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_roller.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_roller.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_roller.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_scale.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_scale.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_scale.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_scale.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_slider.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_slider.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_slider.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_slider.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_span.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_span.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_span.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_span.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinbox.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinbox.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinbox.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinbox.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinner.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinner.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinner.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_spinner.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_switch.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_switch.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_switch.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_switch.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_table.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_table.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_table.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_table.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_tabview.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_tabview.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_tabview.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_tabview.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_textarea.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_textarea.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_textarea.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_textarea.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_win.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_win.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_win.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/widgets/test_win.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases-2f-widgets

