################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/examples/assets/animimg001.c \
../Middlewares/lvgl-release-v9.4/examples/assets/animimg002.c \
../Middlewares/lvgl-release-v9.4/examples/assets/animimg003.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_caret_down.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_argb.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_indexed16.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_rgb.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_hand.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_skew_strip.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_star.c \
../Middlewares/lvgl-release-v9.4/examples/assets/img_svg_img.c \
../Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_left.c \
../Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_mid.c \
../Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_right.c 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/examples/assets/animimg001.d \
./Middlewares/lvgl-release-v9.4/examples/assets/animimg002.d \
./Middlewares/lvgl-release-v9.4/examples/assets/animimg003.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_caret_down.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_argb.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_indexed16.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_rgb.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_hand.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_skew_strip.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_star.d \
./Middlewares/lvgl-release-v9.4/examples/assets/img_svg_img.d \
./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_left.d \
./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_mid.d \
./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_right.d 

OBJS += \
./Middlewares/lvgl-release-v9.4/examples/assets/animimg001.o \
./Middlewares/lvgl-release-v9.4/examples/assets/animimg002.o \
./Middlewares/lvgl-release-v9.4/examples/assets/animimg003.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_caret_down.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_argb.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_indexed16.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_rgb.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_hand.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_skew_strip.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_star.o \
./Middlewares/lvgl-release-v9.4/examples/assets/img_svg_img.o \
./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_left.o \
./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_mid.o \
./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_right.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/examples/assets/%.o Middlewares/lvgl-release-v9.4/examples/assets/%.su Middlewares/lvgl-release-v9.4/examples/assets/%.cyclo: ../Middlewares/lvgl-release-v9.4/examples/assets/%.c Middlewares/lvgl-release-v9.4/examples/assets/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-examples-2f-assets

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-examples-2f-assets:
	-$(RM) ./Middlewares/lvgl-release-v9.4/examples/assets/animimg001.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/animimg001.d ./Middlewares/lvgl-release-v9.4/examples/assets/animimg001.o ./Middlewares/lvgl-release-v9.4/examples/assets/animimg001.su ./Middlewares/lvgl-release-v9.4/examples/assets/animimg002.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/animimg002.d ./Middlewares/lvgl-release-v9.4/examples/assets/animimg002.o ./Middlewares/lvgl-release-v9.4/examples/assets/animimg002.su ./Middlewares/lvgl-release-v9.4/examples/assets/animimg003.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/animimg003.d ./Middlewares/lvgl-release-v9.4/examples/assets/animimg003.o ./Middlewares/lvgl-release-v9.4/examples/assets/animimg003.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_caret_down.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_caret_down.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_caret_down.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_caret_down.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_argb.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_argb.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_argb.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_argb.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_indexed16.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_indexed16.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_indexed16.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_indexed16.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_rgb.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_rgb.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_rgb.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_cogwheel_rgb.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_hand.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_hand.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_hand.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_hand.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_skew_strip.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_skew_strip.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_skew_strip.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_skew_strip.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_star.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_star.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_star.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_star.su ./Middlewares/lvgl-release-v9.4/examples/assets/img_svg_img.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/img_svg_img.d ./Middlewares/lvgl-release-v9.4/examples/assets/img_svg_img.o ./Middlewares/lvgl-release-v9.4/examples/assets/img_svg_img.su ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_left.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_left.d ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_left.o ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_left.su ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_mid.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_mid.d ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_mid.o ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_mid.su ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_right.cyclo ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_right.d ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_right.o ./Middlewares/lvgl-release-v9.4/examples/assets/imgbtn_right.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-examples-2f-assets

