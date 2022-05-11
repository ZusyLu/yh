################################################################################
# Automatically-generated file. Do not edit!
################################################################################

export WINEDEBUG=-all

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include zcl-framework-core/subdir.mk
-include xxapp/xx_serial/subdir.mk
-include xxapp/xx_project/subdir.mk
-include xxapp/xx_nwk/subdir.mk
-include xxapp/xx_memory/subdir.mk
-include xxapp/xx_hal/subdir.mk
-include xxapp/xx_common/subdir.mk
-include xxapp/xx_button/subdir.mk
-include xxapp/xx_blink_led/subdir.mk
-include xxapp/subdir.mk
-include update-tc-link-key/subdir.mk
-include strong-random/subdir.mk
-include simple-main/subdir.mk
-include serial/EFR32/subdir.mk
-include serial/subdir.mk
-include scan-dispatch/subdir.mk
-include reporting/subdir.mk
-include power-configuration-server/subdir.mk
-include nvm3/EFR32/subdir.mk
-include nvm3/subdir.mk
-include network-steering/subdir.mk
-include mbedtls/EFR32_2_2/subdir.mk
-include mbedtls/EFR32/subdir.mk
-include manufacturing-library-cli/subdir.mk
-include idle-sleep/subdir.mk
-include ias-zone-server/subdir.mk
-include hal-library/EFR32_xx/subdir.mk
-include hal-library/EFR32_2/subdir.mk
-include hal-library/EFR32/subdir.mk
-include generic-interrupt-control/EFR32/subdir.mk
-include end-device-support/subdir.mk
-include emlib/subdir.mk
-include emdrv/subdir.mk
-include ember-minimal-printf/subdir.mk
-include efr32/subdir.mk
-include diagnostic-server/subdir.mk
-include debug-jtag-stub/EFR32/subdir.mk
-include debug-basic-library/EFR32/subdir.mk
-include counters/subdir.mk
-include coexistence-stub/EFR32/subdir.mk
-include ccm-encryption/subdir.mk
-include battery-monitor/EFR32/subdir.mk
-include basic/subdir.mk
-include antenna-stub/subdir.mk
-include adc/EFR32/subdir.mk
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
main-build: BODY_SENSOR.out

# Tool invocations
BODY_SENSOR.out: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: IAR Linker for ARM'
	ilinkarm $(OBJS) $(USER_OBJS) -o "BODY_SENSOR.out" --config "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//platform/base/hal/micro/cortexm3/efm32/iar-cfg.icf" --config_def NULL_BTL=1 --config_def FLASH_SIZE=524288 --config_def RAM_SIZE=32768 --config_def SIMEEPROM_SIZE=32768 --config_def LOCKBITS_IN_MAINFLASH_SIZE=8192 --config_def EMBER_MALLOC_HEAP_SIZE=0 --config_def HEADER_SIZE=512 --config_def PSSTORE_SIZE=0 --config_def LONGTOKEN_SIZE=0 --config_def BTL_SIZE=24576 --no_wrap_diagnostics "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/cbke-163k1-stub-library-cortexm3-iar-efr32mg22-rail/cbke-163k1-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/hal-library-cortexm3-iar-efr32mg22-rail/hal-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/cbke-stub-library-cortexm3-iar-efr32mg22-rail/cbke-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/zigbee-r22-support-stub-library-cortexm3-iar-efr32mg22-rail/zigbee-r22-support-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//platform/radio/rail_lib/autogen/librail_release/librail_efr32xg22_iar_release.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/multi-network-stub-library-cortexm3-iar-efr32mg22-rail/multi-network-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/packet-validate-library-cortexm3-iar-efr32mg22-rail/packet-validate-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/zigbee-pro-leaf-stack-cortexm3-iar-efr32mg22-rail/zigbee-pro-leaf-stack.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//platform/emdrv/nvm3/lib/libnvm3_CM33_iar.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/gp-stub-library-cortexm3-iar-efr32mg22-rail/gp-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/cbke-library-dsa-sign-stub-cortexm3-iar-efr32mg22-rail/cbke-library-dsa-sign-stub.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/sim-eeprom2-to-nvm3-upgrade-stub-library-cortexm3-iar-efr32mg22-rail/sim-eeprom2-to-nvm3-upgrade-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/cbke-library-dsa-verify-stub-cortexm3-iar-efr32mg22-rail/cbke-library-dsa-verify-stub.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/multi-pan-stub-library-cortexm3-iar-efr32mg22-rail/multi-pan-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/mfglib-library-cortexm3-iar-efr32mg22-rail/mfglib-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/binding-table-library-cortexm3-iar-efr32mg22-rail/binding-table-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/zll-stub-library-cortexm3-iar-efr32mg22-rail/zll-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/end-device-bind-stub-library-cortexm3-iar-efr32mg22-rail/end-device-bind-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/debug-basic-library-cortexm3-iar-efr32mg22-rail/debug-basic-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/cbke-283k1-stub-library-cortexm3-iar-efr32mg22-rail/cbke-283k1-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/install-code-library-cortexm3-iar-efr32mg22-rail/install-code-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/debug-extended-stub-library-cortexm3-iar-efr32mg22-rail/debug-extended-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/cbke-library-dsa-verify-283k1-stub-cortexm3-iar-efr32mg22-rail/cbke-library-dsa-verify-283k1-stub.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/source-route-stub-library-cortexm3-iar-efr32mg22-rail/source-route-stub-library.a" "D:/v5/developer/sdks/gecko_sdk_suite/v3.2//protocol/zigbee/build/security-library-link-keys-stub-cortexm3-iar-efr32mg22-rail/security-library-link-keys-stub.a" --entry __iar_program_start --map "D:/v5/v5_workspace/BODY_SENSOR/IAR ARM - Default - EFR32MG22C224F512IM32/BODY_SENSOR.map" --log_file "D:/v5/v5_workspace/BODY_SENSOR/IAR ARM - Default - EFR32MG22C224F512IM32/BODY_SENSOR.log" --log initialization,modules,sections,veneers --diag_suppress=Lp012 --redirect _Printf=_PrintfSmallNoMb --redirect _Scanf=_ScanfFullNoMb  
	ielftool --srec --verbose "BODY_SENSOR.out" "BODY_SENSOR.s37"  
	ielftool --ihex --verbose "BODY_SENSOR.out" "BODY_SENSOR.hex"  
	ielftool --bin --verbose "BODY_SENSOR.out" "BODY_SENSOR.bin"
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(MSA_UPPER_DEPS)$(EXECUTABLES)$(OBJS)$(ASM_DEPS)$(S79_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(MSA_DEPS)$(C_DEPS)$(S79_UPPER_DEPS) BODY_SENSOR.out
	-@echo ' '

pre-build:
	-cmd //c 'D:\v5\V5_WOR~1\BODY_S~1/BODY_SENSOR-prebuild.bat' 'BODY_SENSOR' 'D:/v5/v5_workspace/BODY_SENSOR' 'D:/iar/arm' 'D:\v5'
	-@echo ' '

post-build:
	-cmd //c 'D:\v5\V5_WOR~1\BODY_S~1/BODY_SENSOR-postbuild.bat' 'BODY_SENSOR' 'D:/v5/v5_workspace/BODY_SENSOR' 'D:/iar/arm' 'D:\v5'
	-@echo ' '

.PHONY: all clean dependents main-build pre-build post-build

-include ../makefile.targets
