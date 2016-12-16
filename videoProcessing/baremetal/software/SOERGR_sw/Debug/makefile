################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/src/subdir.mk
-include src/multicore_sup/subdir.mk
-include src/IP_SW/SobelIP/SW/function/subdir.mk
-include src/IP_SW/SobelIP/SW/driver/Rule1/subdir.mk
-include src/IP_SW/SobelIP/SW/config/subdir.mk
-include src/IP_SW/GrayscaleIP/SW/function/subdir.mk
-include src/IP_SW/GrayscaleIP/SW/driver/Rule2/subdir.mk
-include src/IP_SW/GrayscaleIP/SW/driver/Rule1/subdir.mk
-include src/IP_SW/GrayscaleIP/SW/config/subdir.mk
-include src/IP_SW/ErodeIP/SW/function/subdir.mk
-include src/IP_SW/ErodeIP/SW/driver/Rule1/subdir.mk
-include src/IP_SW/ErodeIP/SW/config/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
SOERGR_sw.elf.size \


# All Target
all: SOERGR_sw.elf secondary-outputs

# Tool invocations
SOERGR_sw.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM gcc linker'
	arm-xilinx-eabi-gcc -L"/nfs/TUEIEDAprojects/SoCOptimization/uzair/architectures/SEG/SDK/SDK_Workspace/SOERGR_sw/src/lib" -Wl,-T -Wl,../src/lscript.ld -L../../standalone_bsp_0/ps7_cortexa9_0/lib -o "SOERGR_sw.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SOERGR_sw.elf.size: SOERGR_sw.elf
	@echo 'Invoking: ARM Print Size'
	arm-xilinx-eabi-size SOERGR_sw.elf  |tee "SOERGR_sw.elf.size"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(S_UPPER_DEPS) SOERGR_sw.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
