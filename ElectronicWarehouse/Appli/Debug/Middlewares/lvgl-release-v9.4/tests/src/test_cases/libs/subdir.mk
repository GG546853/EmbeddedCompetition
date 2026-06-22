################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_barcode.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bin_decoder.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bmp.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_ffmpeg.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_font_stress.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_freetype.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libjpeg_turbo.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libpng.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_lodepng.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_memmove.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_qrcode.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_svg_decoder.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tiny_ttf.c \
../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tjpgd.c 

OBJS += \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_barcode.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bin_decoder.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bmp.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_ffmpeg.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_font_stress.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_freetype.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libjpeg_turbo.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libpng.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_lodepng.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_memmove.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_qrcode.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_svg_decoder.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tiny_ttf.o \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tjpgd.o 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_barcode.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bin_decoder.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bmp.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_ffmpeg.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_font_stress.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_freetype.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libjpeg_turbo.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libpng.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_lodepng.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_memmove.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_qrcode.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_svg_decoder.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tiny_ttf.d \
./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tjpgd.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/%.o Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/%.su Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/%.cyclo: ../Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/%.c Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases-2f-libs

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases-2f-libs:
	-$(RM) ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_barcode.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_barcode.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_barcode.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_barcode.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bin_decoder.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bin_decoder.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bin_decoder.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bin_decoder.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bmp.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bmp.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bmp.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_bmp.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_ffmpeg.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_ffmpeg.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_ffmpeg.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_ffmpeg.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_font_stress.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_font_stress.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_font_stress.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_font_stress.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_freetype.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_freetype.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_freetype.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_freetype.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libjpeg_turbo.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libjpeg_turbo.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libjpeg_turbo.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libjpeg_turbo.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libpng.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libpng.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libpng.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_libpng.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_lodepng.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_lodepng.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_lodepng.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_lodepng.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_memmove.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_memmove.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_memmove.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_memmove.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_qrcode.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_qrcode.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_qrcode.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_qrcode.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_svg_decoder.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_svg_decoder.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_svg_decoder.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_svg_decoder.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tiny_ttf.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tiny_ttf.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tiny_ttf.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tiny_ttf.su ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tjpgd.cyclo ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tjpgd.d ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tjpgd.o ./Middlewares/lvgl-release-v9.4/tests/src/test_cases/libs/test_tjpgd.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-tests-2f-src-2f-test_cases-2f-libs

