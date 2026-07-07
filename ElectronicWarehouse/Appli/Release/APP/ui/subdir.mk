################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../APP/ui/eez-flow.cpp \
../APP/ui/ui_bridge.cpp 

C_SRCS += \
../APP/ui/actions.c \
../APP/ui/images.c \
../APP/ui/screens.c \
../APP/ui/styles.c \
../APP/ui/ui.c \
../APP/ui/ui_font_chinese.c \
../APP/ui/ui_font_chinese10.c \
../APP/ui/ui_font_chinese18.c \
../APP/ui/ui_font_chinese24.c \
../APP/ui/ui_font_chinese8.c \
../APP/ui/ui_image_p_iamge.c \
../APP/ui/ui_image_page_activity.c \
../APP/ui/ui_image_page_dashboard.c \
../APP/ui/ui_image_page_inventory.c \
../APP/ui/ui_image_page_printfer.c \
../APP/ui/ui_image_page_search.c \
../APP/ui/ui_image_page_uimode.c \
../APP/ui/ui_image_page_wms.c 

C_DEPS += \
./APP/ui/actions.d \
./APP/ui/images.d \
./APP/ui/screens.d \
./APP/ui/styles.d \
./APP/ui/ui.d \
./APP/ui/ui_font_chinese.d \
./APP/ui/ui_font_chinese10.d \
./APP/ui/ui_font_chinese18.d \
./APP/ui/ui_font_chinese24.d \
./APP/ui/ui_font_chinese8.d \
./APP/ui/ui_image_p_iamge.d \
./APP/ui/ui_image_page_activity.d \
./APP/ui/ui_image_page_dashboard.d \
./APP/ui/ui_image_page_inventory.d \
./APP/ui/ui_image_page_printfer.d \
./APP/ui/ui_image_page_search.d \
./APP/ui/ui_image_page_uimode.d \
./APP/ui/ui_image_page_wms.d 

OBJS += \
./APP/ui/actions.o \
./APP/ui/eez-flow.o \
./APP/ui/images.o \
./APP/ui/screens.o \
./APP/ui/styles.o \
./APP/ui/ui.o \
./APP/ui/ui_bridge.o \
./APP/ui/ui_font_chinese.o \
./APP/ui/ui_font_chinese10.o \
./APP/ui/ui_font_chinese18.o \
./APP/ui/ui_font_chinese24.o \
./APP/ui/ui_font_chinese8.o \
./APP/ui/ui_image_p_iamge.o \
./APP/ui/ui_image_page_activity.o \
./APP/ui/ui_image_page_dashboard.o \
./APP/ui/ui_image_page_inventory.o \
./APP/ui/ui_image_page_printfer.o \
./APP/ui/ui_image_page_search.o \
./APP/ui/ui_image_page_uimode.o \
./APP/ui/ui_image_page_wms.o 

CPP_DEPS += \
./APP/ui/eez-flow.d \
./APP/ui/ui_bridge.d 


# Each subdirectory must supply rules for building sources it contributes
APP/ui/%.o APP/ui/%.su APP/ui/%.cyclo: ../APP/ui/%.c APP/ui/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/Printer" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SK9822" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
APP/ui/%.o APP/ui/%.su APP/ui/%.cyclo: ../APP/ui/%.cpp APP/ui/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m55 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/Printer" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SK9822" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-APP-2f-ui

clean-APP-2f-ui:
	-$(RM) ./APP/ui/actions.cyclo ./APP/ui/actions.d ./APP/ui/actions.o ./APP/ui/actions.su ./APP/ui/eez-flow.cyclo ./APP/ui/eez-flow.d ./APP/ui/eez-flow.o ./APP/ui/eez-flow.su ./APP/ui/images.cyclo ./APP/ui/images.d ./APP/ui/images.o ./APP/ui/images.su ./APP/ui/screens.cyclo ./APP/ui/screens.d ./APP/ui/screens.o ./APP/ui/screens.su ./APP/ui/styles.cyclo ./APP/ui/styles.d ./APP/ui/styles.o ./APP/ui/styles.su ./APP/ui/ui.cyclo ./APP/ui/ui.d ./APP/ui/ui.o ./APP/ui/ui.su ./APP/ui/ui_bridge.cyclo ./APP/ui/ui_bridge.d ./APP/ui/ui_bridge.o ./APP/ui/ui_bridge.su ./APP/ui/ui_font_chinese.cyclo ./APP/ui/ui_font_chinese.d ./APP/ui/ui_font_chinese.o ./APP/ui/ui_font_chinese.su ./APP/ui/ui_font_chinese10.cyclo ./APP/ui/ui_font_chinese10.d ./APP/ui/ui_font_chinese10.o ./APP/ui/ui_font_chinese10.su ./APP/ui/ui_font_chinese18.cyclo ./APP/ui/ui_font_chinese18.d ./APP/ui/ui_font_chinese18.o ./APP/ui/ui_font_chinese18.su ./APP/ui/ui_font_chinese24.cyclo ./APP/ui/ui_font_chinese24.d ./APP/ui/ui_font_chinese24.o ./APP/ui/ui_font_chinese24.su ./APP/ui/ui_font_chinese8.cyclo ./APP/ui/ui_font_chinese8.d ./APP/ui/ui_font_chinese8.o ./APP/ui/ui_font_chinese8.su ./APP/ui/ui_image_p_iamge.cyclo ./APP/ui/ui_image_p_iamge.d ./APP/ui/ui_image_p_iamge.o ./APP/ui/ui_image_p_iamge.su ./APP/ui/ui_image_page_activity.cyclo ./APP/ui/ui_image_page_activity.d ./APP/ui/ui_image_page_activity.o ./APP/ui/ui_image_page_activity.su ./APP/ui/ui_image_page_dashboard.cyclo ./APP/ui/ui_image_page_dashboard.d ./APP/ui/ui_image_page_dashboard.o ./APP/ui/ui_image_page_dashboard.su ./APP/ui/ui_image_page_inventory.cyclo ./APP/ui/ui_image_page_inventory.d ./APP/ui/ui_image_page_inventory.o ./APP/ui/ui_image_page_inventory.su ./APP/ui/ui_image_page_printfer.cyclo ./APP/ui/ui_image_page_printfer.d ./APP/ui/ui_image_page_printfer.o ./APP/ui/ui_image_page_printfer.su ./APP/ui/ui_image_page_search.cyclo ./APP/ui/ui_image_page_search.d ./APP/ui/ui_image_page_search.o ./APP/ui/ui_image_page_search.su ./APP/ui/ui_image_page_uimode.cyclo ./APP/ui/ui_image_page_uimode.d ./APP/ui/ui_image_page_uimode.o ./APP/ui/ui_image_page_uimode.su ./APP/ui/ui_image_page_wms.cyclo ./APP/ui/ui_image_page_wms.d ./APP/ui/ui_image_page_wms.o ./APP/ui/ui_image_page_wms.su

.PHONY: clean-APP-2f-ui

