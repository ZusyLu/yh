################################################################################
# Automatically-generated file. Do not edit!
################################################################################

export WINEDEBUG=-all

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include zcl-framework-core/subdir.mk
-include update-tc-link-key/subdir.mk
-include strong-random/subdir.mk
-include simple-main/subdir.mk
-include serial/EFR32/subdir.mk
-include serial/subdir.mk
-include scan-dispatch/subdir.mk
-include on-off/subdir.mk
-include nvm3/EFR32/subdir.mk
-include nvm3/subdir.mk
-include network-steering/subdir.mk
-include network-creator-security/subdir.mk
-include network-creator/subdir.mk
-include mbedtls/EFR32/subdir.mk
-include interpan/subdir.mk
-include hal-library/EFR32_xx/subdir.mk
-include hal-library/EFR32_2/subdir.mk
-include hal-library/EFR32/subdir.mk
-include emlib/subdir.mk
-include emdrv/subdir.mk
-include ember-minimal-printf/subdir.mk
-include efr32/subdir.mk
-include debug-jtag/EFR32/subdir.mk
-include debug-basic-library/EFR32/subdir.mk
-include counters/subdir.mk
-include concentrator/subdir.mk
-include coexistence-stub/EFR32/subdir.mk
-include ccm-encryption/subdir.mk
-include basic/subdir.mk
-include app-custom-application/subdir.mk
-include antenna-stub/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(MSA_UPPER_DEPS)),)
-include $(MSA_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S79_DEPS)),)
-include $(S79_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(MSA_DEPS)),)
-include $(MSA_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S79_UPPER_DEPS)),)
-include $(S79_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	$(MAKE) --no-print-directory pre-build
	$(MAKE) --no-print-directory main-build

# Main-build Target
main-build: ZigbeeDebugger.out

# Tool invocations
ZigbeeDebugger.out: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: IAR Linker for ARM'
	@echo 'Adding linker inputs to ilinkCommandLine_ZigbeeDebugger.out.xcl'
	echo. $(foreach file,$(OBJS), $(file)) >> ilinkCommandLine_ZigbeeDebugger.out.xcl
	echo. $(foreach file,$(USER_OBJS), $(file)) >> ilinkCommandLine_ZigbeeDebugger.out.xcl
	@echo 'Invoking linker reading args from file: ilinkCommandLine_ZigbeeDebugger.out.xcl'
	ilinkarm -f "G:\SiliconLabs\SimplicityStudio\v5_workspace\ZigbeeDebugger\IAR ARM - Default - EFR32MG21A020F768IM32\ilinkCommandLine_ZigbeeDebugger.out.xcl" 
	ielftool --srec --verbose "ZigbeeDebugger.out" "ZigbeeDebugger.s37"  
	ielftool --ihex --verbose "ZigbeeDebugger.out" "ZigbeeDebugger.hex"  
	ielftool --bin --verbose "ZigbeeDebugger.out" "ZigbeeDebugger.bin"
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(MSA_UPPER_DEPS)$(EXECUTABLES)$(OBJS)$(ASM_DEPS)$(S79_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(MSA_DEPS)$(C_DEPS)$(S79_UPPER_DEPS) ZigbeeDebugger.out
	-@echo ' '

pre-build:
	-cmd //c 'G:\SILICO~1\SIMPLI~1\V5_WOR~1\ZI9446~1/ZigbeeDebugger-prebuild.bat' 'ZigbeeDebugger' 'G:/SiliconLabs/SimplicityStudio/v5_workspace/ZigbeeDebugger' 'E:/iar/arm' 'G:\SiliconLabs\SimplicityStudio\v5'
	-@echo ' '

post-build:
	-cmd //c 'G:\SILICO~1\SIMPLI~1\V5_WOR~1\ZI9446~1/ZigbeeDebugger-postbuild.bat' 'ZigbeeDebugger' 'G:/SiliconLabs/SimplicityStudio/v5_workspace/ZigbeeDebugger' 'E:/iar/arm' 'G:\SiliconLabs\SimplicityStudio\v5'
	-@echo ' '

.PHONY: all clean dependents main-build pre-build post-build

-include ../makefile.targets
