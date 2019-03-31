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
task_2.lss \

FLASH_IMAGE += \
task_2.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: task_2.elf secondary-outputs

dependents:
	-cd /home/sachin/embedded/eclipse/program/ses/Debug && $(MAKE) all

# Tool invocations
task_2.elf: $(OBJS) $(USER_OBJS) /home/sachin/embedded/eclipse/program/ses/Debug/libses.a
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,task_2.map -L"/home/sachin/embedded/eclipse/program/ses/Debug" -L"/home/sachin/embedded/eclipse/program/ses" -L"/home/sachin/embedded/eclipse/program/ses/Debug" -L"/home/sachin/embedded/eclipse/program/ses/Debug" -mmcu=atmega128rfa1 -o "task_2.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

task_2.lss: task_2.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S task_2.elf  >"task_2.lss"
	@echo 'Finished building: $@'
	@echo ' '

task_2.hex: task_2.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex task_2.elf  "task_2.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: task_2.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega128rfa1 task_2.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) task_2.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:
/home/sachin/embedded/eclipse/program/ses/Debug/libses.a:

-include ../makefile.targets
