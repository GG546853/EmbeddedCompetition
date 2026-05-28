################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include ll_aton/subdir.mk
-include STM32N6xx/subdir.mk
-include Model/subdir.mk
-include Middlewares/pp/Src/subdir.mk
-include Middlewares/STM32_MW_ISP/isp/Src/subdir.mk
-include Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/subdir.mk
-include Middlewares/RTOS2/CMSIS/RTOS2/Core/TZ_Secure_Only/subdir.mk
-include Drivers/STM32N6xx_HAL_Driver/Src/subdir.mk
-include Drivers/STM32N6xx_HAL_Driver/subdir.mk
-include Drivers/BSP/UART/subdir.mk
-include Drivers/BSP/TOUCH/subdir.mk
-include Drivers/BSP/SYS/subdir.mk
-include Drivers/BSP/RGBLCD/subdir.mk
-include Drivers/BSP/NORFlash/subdir.mk
-include Drivers/BSP/LED/subdir.mk
-include Drivers/BSP/KEY/subdir.mk
-include Drivers/BSP/IMX335/STM32_IMX335/subdir.mk
-include Drivers/BSP/IMX335/subdir.mk
-include Drivers/BSP/HyperRAM/subdir.mk
-include Drivers/BSP/EEPROM/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include APP/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ElectronicWarehouse_Appli
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
ElectronicWarehouse_Appli.elf \

MAP_FILES += \
ElectronicWarehouse_Appli.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
ElectronicWarehouse_Appli.list \

OBJCOPY_BIN += \
ElectronicWarehouse_Appli.bin \


# All Target
all: main-build

# Main-build Target
main-build: ElectronicWarehouse_Appli.elf secondary-outputs

# Tool invocations
ElectronicWarehouse_Appli.elf ElectronicWarehouse_Appli.map: $(OBJS) $(USER_OBJS) C:\Users\96022\Desktop\EmbeddedCompetition\ElectronicWarehouse\Appli\STM32N647X0HXQ_LRUN_RAMxspi1.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "ElectronicWarehouse_Appli.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m55 -T"C:\Users\96022\Desktop\EmbeddedCompetition\ElectronicWarehouse\Appli\STM32N647X0HXQ_LRUN_RAMxspi1.ld" --specs=nosys.specs -Wl,-Map="ElectronicWarehouse_Appli.map" -Wl,--gc-sections -static -Wl,--cmse-implib -Wl,--out-implib=./secure_nsclib.o -L"C:\Users\96022\Desktop\EmbeddedCompetition\ElectronicWarehouse\Appli\Middlewares\STM32_MW_ISP\evision\Lib" -L"C:\ST\STEdgeAI\4.0\Middlewares\ST\AI\Lib\GCC\ARMCortexM55" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

ElectronicWarehouse_Appli.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "ElectronicWarehouse_Appli.list"
	@echo 'Finished building: $@'
	@echo ' '

ElectronicWarehouse_Appli.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "ElectronicWarehouse_Appli.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ElectronicWarehouse_Appli.bin ElectronicWarehouse_Appli.elf ElectronicWarehouse_Appli.list ElectronicWarehouse_Appli.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
