################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/_test_template.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_align_flex.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim_timeline.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_area.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_array.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_bindings.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_circle_buf.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_click.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_config.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_stress.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_widgets.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_display.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_draw_buf.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event_trickle.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_file_explorer.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_loader.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_manager.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_fs.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gesture_pinch.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid_fr.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gridnav.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_group.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_hover.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_reset.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_wait_release.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_lcd.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_align.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_flex.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_grid.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_math.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_mem.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_observer.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_profiler.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_recolor.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_screen_load.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_snapshot.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_style.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg_anim.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_translation.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_tree.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_txt.c 

OBJS += \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/_test_template.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_align_flex.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim_timeline.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_area.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_array.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_bindings.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_circle_buf.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_click.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_config.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_stress.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_widgets.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_display.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_draw_buf.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event_trickle.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_file_explorer.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_loader.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_manager.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_fs.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gesture_pinch.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid_fr.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gridnav.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_group.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_hover.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_reset.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_wait_release.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_lcd.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_align.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_flex.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_grid.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_math.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_mem.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_observer.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_profiler.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_recolor.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_screen_load.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_snapshot.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_style.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg_anim.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_translation.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_tree.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_txt.o 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/_test_template.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_align_flex.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim_timeline.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_area.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_array.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_bindings.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_circle_buf.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_click.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_config.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_stress.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_widgets.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_display.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_draw_buf.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event_trickle.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_file_explorer.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_loader.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_manager.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_fs.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gesture_pinch.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid_fr.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gridnav.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_group.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_hover.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_reset.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_wait_release.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_lcd.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_align.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_flex.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_grid.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_math.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_mem.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_observer.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_profiler.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_recolor.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_screen_load.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_snapshot.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_style.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg_anim.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_translation.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_tree.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_txt.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/tests/src/test_cases/%.o Middlewares/lvgl-release-v9.4/tests/src/test_cases/%.su Middlewares/lvgl-release-v9.4/tests/src/test_cases/%.cyclo: ../Middlewares/lvgl-release-v9.4/tests/src/test_cases/%.c Middlewares/lvgl-release-v9.4/tests/src/test_cases/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases:
	-$(RM) ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/_test_template.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/_test_template.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/_test_template.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/_test_template.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_align_flex.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_align_flex.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_align_flex.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_align_flex.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim_timeline.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim_timeline.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim_timeline.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_anim_timeline.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_area.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_area.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_area.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_area.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_array.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_array.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_array.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_array.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_bindings.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_bindings.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_bindings.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_bindings.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_circle_buf.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_circle_buf.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_circle_buf.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_circle_buf.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_click.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_click.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_click.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_click.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_config.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_config.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_config.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_config.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_stress.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_stress.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_stress.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_stress.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_widgets.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_widgets.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_widgets.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_demo_widgets.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_display.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_display.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_display.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_display.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_draw_buf.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_draw_buf.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_draw_buf.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_draw_buf.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event_trickle.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event_trickle.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event_trickle.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_event_trickle.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_file_explorer.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_file_explorer.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_file_explorer.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_file_explorer.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_loader.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_loader.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_loader.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_loader.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_manager.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_manager.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_manager.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_font_manager.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_fs.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_fs.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_fs.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_fs.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gesture_pinch.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gesture_pinch.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gesture_pinch.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gesture_pinch.su
	-$(RM) ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid_fr.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid_fr.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid_fr.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_grid_fr.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gridnav.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gridnav.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gridnav.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_gridnav.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_group.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_group.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_group.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_group.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_hover.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_hover.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_hover.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_hover.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_reset.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_reset.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_reset.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_reset.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_wait_release.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_wait_release.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_wait_release.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_indev_wait_release.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_lcd.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_lcd.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_lcd.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_lcd.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_align.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_align.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_align.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_align.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_flex.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_flex.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_flex.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_flex.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_grid.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_grid.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_grid.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_margin_grid.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_math.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_math.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_math.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_math.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_mem.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_mem.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_mem.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_mem.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_observer.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_observer.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_observer.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_observer.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_profiler.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_profiler.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_profiler.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_profiler.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_recolor.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_recolor.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_recolor.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_recolor.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_screen_load.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_screen_load.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_screen_load.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_screen_load.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_snapshot.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_snapshot.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_snapshot.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_snapshot.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_style.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_style.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_style.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_style.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg_anim.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg_anim.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg_anim.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_svg_anim.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_translation.cyclo
	-$(RM) ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_translation.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_translation.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_translation.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_tree.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_tree.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_tree.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_tree.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_txt.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_txt.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_txt.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/test_txt.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases

