################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/lv_blend_helium.S 

OBJS += \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/lv_blend_helium.o 

S_UPPER_DEPS += \
./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/lv_blend_helium.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/%.o: ../Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/%.S Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m55 -c -I../Core/Inc -I../../Secure_nsclib -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM55_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../X-CUBE-AI/App -I../X-CUBE-AI -I../../Middlewares/ST/AI/Npu/Devices/STM32N6XX -I../../Middlewares/ST/AI/Inc -I../../Middlewares/ST/AI/Npu/ll_aton -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw-2f-blend-2f-helium

clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw-2f-blend-2f-helium:
	-$(RM) ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/lv_blend_helium.d ./Middlewares/lvgl-release-v9.4/src/draw/sw/blend/helium/lv_blend_helium.o

.PHONY: clean-Middlewares-2f-lvgl-2d-release-2d-v9-2e-4-2f-src-2f-draw-2f-sw-2f-blend-2f-helium

