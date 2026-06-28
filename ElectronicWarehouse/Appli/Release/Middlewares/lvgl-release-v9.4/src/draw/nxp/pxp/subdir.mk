################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_buf_pxp.c \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp.c \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_fill.c \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_img.c \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_layer.c \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_cfg.c \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_osa.c \
../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_utils.c 

C_DEPS += \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_buf_pxp.d \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp.d \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_fill.d \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_img.d \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_layer.d \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_cfg.d \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_osa.d \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_utils.d 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_buf_pxp.o \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp.o \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_fill.o \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_img.o \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_layer.o \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_cfg.o \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_osa.o \
./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_utils.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/%.o Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/%.su Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/%.c Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/Printer" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SK9822" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-nxp-2f-pxp

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-nxp-2f-pxp:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_buf_pxp.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_buf_pxp.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_buf_pxp.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_buf_pxp.su ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp.su ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_fill.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_fill.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_fill.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_fill.su ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_img.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_img.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_img.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_img.su ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_layer.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_layer.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_layer.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_draw_pxp_layer.su ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_cfg.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_cfg.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_cfg.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_cfg.su ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_osa.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_osa.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_osa.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_osa.su ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_utils.cyclo ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_utils.d ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_utils.o ./Middlewares/lvgl-release-v9.4/src/draw/nxp/pxp/lv_pxp_utils.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-nxp-2f-pxp

