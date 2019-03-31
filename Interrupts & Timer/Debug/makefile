################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
task_4.lss \

FLASH_IMAGE += \
task_4.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: task_4.elf secondary-outputs

# Tool invocations
task_4.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,task_4.map -L"/home/sachin/embedded/eclipse/program/ses" -L"/home/sachin/embedded/eclipse/program/ses/Debug" -mmcu=atmega128rfa1 -o "task_4.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

task_4.lss: task_4.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S task_4.elf  >"task_4.lss"
	@echo 'Finished building: $@'
	@echo ' '

task_4.hex: task_4.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex task_4.elf  "task_4.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: task_4.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega128rfa1 task_4.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) task_4.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
