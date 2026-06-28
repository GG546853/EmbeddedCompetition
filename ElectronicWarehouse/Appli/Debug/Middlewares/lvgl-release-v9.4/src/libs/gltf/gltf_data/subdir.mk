################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_bind.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_animations.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_cache.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_injest.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_mesh.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_node.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_primitive.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_shader.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_skin.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_texture.cpp \
../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_uniform_locations.cpp 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_bind.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_animations.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_cache.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_injest.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_mesh.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_node.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_primitive.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_shader.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_skin.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_texture.o \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_uniform_locations.o 

CPP_DEPS += \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_bind.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_animations.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_cache.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_injest.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_mesh.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_node.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_primitive.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_shader.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_skin.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_texture.d \
./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_uniform_locations.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/%.o Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/%.su Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/%.cyclo: ../Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/%.cpp Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m55 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N647xx -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -c -I../Core/Inc -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/Printer" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP/ui" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/lvgl-release-v9.4/src/drivers/display/st_ltdc" -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/evision/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp/Inc" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP/isp_param_conf" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Middlewares/STM32_MW_ISP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/APP" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/AHT10" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/EEPROM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/HyperRAM" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/IMX335/STM32_IMX335" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/KEY" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/LED" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/RGBLCD" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SYS" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/TOUCH" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/UART" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/NORFlash" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/VL53L1X" -I"C:/Users/96022/Desktop/EmbeddedCompetition/ElectronicWarehouse/Appli/Drivers/BSP/SK9822" -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-libs-2f-gltf-2f-gltf_data

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-libs-2f-gltf-2f-gltf_data:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_bind.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_bind.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_bind.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_bind.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_animations.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_animations.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_animations.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_animations.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_cache.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_cache.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_cache.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_cache.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_injest.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_injest.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_injest.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_injest.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_mesh.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_mesh.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_mesh.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_mesh.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_node.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_node.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_node.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_node.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_primitive.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_primitive.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_primitive.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_primitive.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_shader.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_shader.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_shader.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_shader.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_skin.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_skin.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_skin.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_skin.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_texture.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_texture.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_texture.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_data_texture.su ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_uniform_locations.cyclo ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_uniform_locations.d ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_uniform_locations.o ./Middlewares/lvgl-release-v9.4/src/libs/gltf/gltf_data/lv_gltf_uniform_locations.su

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-libs-2f-gltf-2f-gltf_data

