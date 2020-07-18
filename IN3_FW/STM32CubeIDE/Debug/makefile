################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Core/ILI9341/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
IN3ATOR.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
IN3ATOR.list \

OBJCOPY_BIN += \
IN3ATOR.bin \


# All Target
all: IN3ATOR.elf secondary-outputs

# Tool invocations
IN3ATOR.elf: $(OBJS) $(USER_OBJS) C:\Users\pablo\Desktop\IN3\in3\IN3_FW\STM32CubeIDE\STM32F103RETX_FLASH.ld
	arm-none-eabi-gcc -o "IN3ATOR.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"C:\Users\pablo\Desktop\IN3\in3\IN3_FW\STM32CubeIDE\STM32F103RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="IN3ATOR.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

IN3ATOR.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "IN3ATOR.list"
	@echo 'Finished building: $@'
	@echo ' '

IN3ATOR.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "IN3ATOR.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
