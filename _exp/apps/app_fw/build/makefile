#********************************************************************************
# MICROCHIP PM8596 EXPLORER FIRMWARE
#                                                                               
# Copyright (c) 2018, 2019 Microchip Technology Inc. All rights reserved. 
#                                                                               
# Licensed under the Apache License, Version 2.0 (the "License"); you may not 
# use this file except in compliance with the License. You may obtain a copy of 
# the License at http://www.apache.org/licenses/LICENSE-2.0
#                                                                               
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT 
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the 
# License for the specific language governing permissions and limitations under 
# the License.
# --------------------------------------------------------------------------
# DESCRIPTION  :  Makefile for app_fw
#
# NOTES        :  None.
#
#*******************************************************************************/
MODDIR := $(dir $(lastword $(MAKEFILE_LIST)))..

.DEFAULT_GOAL := all
export AT ?= @
export V  ?= @

# Ensure all required variables are set
ifndef PLAT_DIR
  $(error "PLAT_DIR is not defined (e.g. _exp)")
endif

# Top level directory product name
export PMC_PROD=$(PLAT_DIR)
export PMC_TOP_LEVEL=$(PLAT_DIR)

# Program name
PROGRAM=app_fw

# PATH to the top level related to the current directory
SRCTL= ../../../..
APP_PLAT_DIR=..

# Set default CSV file pointing to the key for signing if not specified
ifndef CSV
CSV=$(SRCTL)/tools/bin/explorer_pubkeys/app_fw_key1.csv
endif

ifndef BLD_MODE
BLD_MODE=dev
endif

# This is not a library
PMC_LIBRARY=no

# Tools
# set the compiler you want to use:
# - sde for sde-gcc

# - ghs for green hills compiler
export TOOLS=ghs

# Define the program source code
PROCESSOR=app_fw

# External modules that need to be compiled
VENDOR_MODULES=

# FW version 
export FW_VERSION=app_version
# Modules who need to be compiled
CONVERTED_MODULES=

# Initialize the Source File Lists
LOCAL_CSOURCES := $(APP_PLAT_DIR)/src/app_fw_main.c \
                  $(APP_PLAT_DIR)/src/app_fw_plat.c \
                  $(APP_PLAT_DIR)/src/app_fw_cmdsvr.c \
                  $(APP_PLAT_DIR)/src/app_fw_ddr.c \
                  $(APP_PLAT_DIR)/src/app_fw_bringup.c \
                  $(APP_PLAT_DIR)/src/app_fw_ech_twi_handler.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/printf/printf.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/log/log_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/top/top_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/twi/twi_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/spi/spi_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/spi_flash/spi_flash_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ech/ech.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ech/ech_oc.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ech/ech_twi_common.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ech/ech_twi_boot_config.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ech/ech_pqm.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/seeprom_mm/seeprom_mm_bootstrap.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ocmb/ocmb_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/cicint/exp_gic.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/adapter_info/adapter_info.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/flash_partition/flash_partition_info.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/mem/mem_alloc.c  \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/stack_trace/stack_trace_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/serdes/serdes_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/temp_sensor/temp_sensor_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ddr_phy/ddr_phy_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/exp_ddr_ctrlr/exp_ddr_ctrlr_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ddr/ddr_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/fam/fam_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/fatal/fatal_plat.c \
				  $(SRCTL)/$(PMC_TOP_LEVEL)/src/flashloader/flashloader_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/crash_dump/crash_dump_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/temp_sensor/temp_sensor_driver_plat.c \
                  $(SRCTL)/$(PMC_TOP_LEVEL)/src/ccb/ccb_plat.c
                                  


LOCAL_CSOURCES_VENDOR := 

LOCAL_ASMSOURCES :=


LINKERFILES= $(APP_PLAT_DIR)/build/app_fw.ld
# list of header directories. These are directories that are not include as modules
export INCLUDEDIRS := $(SRCTL)/$(PMC_TOP_LEVEL)/inc \
                      $(SRCTL)/shared/pmc_cpu/explorer/inc \
                      $(APP_PLAT_DIR)/inc \
                      $(SRCTL)/shared/spi/inc \
                      $(SRCTL)/shared/log_app/inc \
                      $(SRCTL)/shared/pboot/inc \
                      $(SRCTL)/shared/m34k/inc \
                      $(SRCTL)/shared/cmdsvr_func/inc \
                      $(SRCTL)/shared/dcsue/inc \
                      $(SRCTL)/shared/pmcfw_shared/inc \
                      $(SRCTL)/shared/busio/inc \
                      $(SRCTL)/shared/hw_reg/inc \
                      $(SRCTL)/shared/mem/inc \
                      $(SRCTL)/shared/twim/inc \
                      $(SRCTL)/shared/sys_timer/inc \
                      $(SRCTL)/shared/uart/inc \
                      $(SRCTL)/shared/tsh/inc \
                      $(SRCTL)/shared/cmdsvr/inc \
                      $(SRCTL)/shared/exp_cmdsvr/inc \
                      $(SRCTL)/shared/seepromx/inc \
                      $(SRCTL)/shared/ocmb/inc \
                      $(SRCTL)/shared/fam/inc \
                      $(SRCTL)/shared/crc/inc \
                      $(SRCTL)/shared/exp_ddr_ctrlr/inc \
                      $(SRCTL)/shared/ddr_exp/inc \
                      $(SRCTL)/shared/serdes/inc \
                      $(SRCTL)/shared/char_io/inc \
                      $(SRCTL)/shared/ccb/inc \
                      $(SRCTL)/shared/spi_flash/inc \
                      $(SRCTL)/shared/top_exp/inc \
                      $(SRCTL)/shared/reset/inc \
                      $(SRCTL)/shared/gpbce/inc \
                      $(SRCTL)/shared/cicint/inc \
                      $(SRCTL)/shared/cmdsvr_func/inc \
                      $(SRCTL)/shared/crash_dump/inc \
                      $(SRCTL)/shared/fatal/inc \
                      $(SRCTL)/shared/opsw/inc \
                      $(SRCTL)/shared/pcse/inc \
					  $(SRCTL)/shared/flashloader/inc \
                      $(SRCTL)/ddr_phy_toolbox/inc \
                      $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/software \
                      $(SRCTL)/$(PMC_TOP_LEVEL)/libs/ddr_phy/inc \
                      $(SRCTL)/shared/cpuhal/inc

ifeq ($(BLD_MODE),dev)
 LIB_MODULES := $(PMC_TOP_LEVEL)/libs/ddr_phy
 LIB_MODULES+=$(PMC_TOP_LEVEL)/libs/ghs_startup
 LIB_MODULES+=$(PMC_TOP_LEVEL)/libs/shared_module
else ifeq ($(BLD_MODE),release)
 INCLUDEDIRS+= $(SRCTL)/$(PMC_TOP_LEVEL)/release_lib/inc
 LDFLAGS+= -L$(SRCTL)/$(PMC_TOP_LEVEL)/release_lib/lib -llibddr_phy.a -llibghs_startup.a -llibshared_module.a
else
  $(error BLD_MODE has not be defined or unsupported BLD_MODE, please use "BLD_MODE=dev" or "BLD_MODE=release" )
endif

export FW_VERSION_FILE :=$(SRCTL)/$(PMC_TOP_LEVEL)/src/fw_version/fw_version_info.c

# Define UART_MODE flag (full_mode/simple_mode)
UART_MODE=simple_mode

ENDIAN=little

CPPFLAGS+=-pic
LDFLAGS+= -memory -e exc_reset -pic -nostartfile -nostdlib -llibansi.a -llibarch.a -llib8bit.a 

LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4/ddr4_pmu_train_imem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4/ddr4_pmu_train_dmem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_2d/ddr4_2d_pmu_train_imem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_2d/ddr4_2d_pmu_train_dmem.bin

LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_rdimm/ddr4_rdimm_pmu_train_imem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_rdimm/ddr4_rdimm_pmu_train_dmem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_rdimm2d/ddr4_rdimm2d_pmu_train_imem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_rdimm2d/ddr4_rdimm2d_pmu_train_dmem.bin

LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_lrdimm/ddr4_lrdimm_pmu_train_imem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_lrdimm/ddr4_lrdimm_pmu_train_dmem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_lrdimm2d/ddr4_lrdimm2d_pmu_train_imem.bin
LDFLAGS+= -rawimport  $(SRCTL)/ddr_phy_toolbox/vendor/ddr_phy_lib/firmware/ddr4_lrdimm2d/ddr4_lrdimm2d_pmu_train_dmem.bin
LDFLAGS+= -rawimport  $(FW_VERSION).bin

all: $(PROGRAM).elf
	rm -f $(FW_VERSION).bin
ifdef SIGN
	$(APP_PLAT_DIR)/build/sign_fw_image.sh -a $(SRCTL)/tools/bin/lib/gobinz.exe -i $(PROGRAM).mem -o signed_$(PROGRAM).mem -c $(CSV)
	$(APP_PLAT_DIR)/build/make_fw_partition.sh -i signed_$(PROGRAM).mem -a signed_$(PROGRAM)_jtag_A.out -b signed_$(PROGRAM)_jtag_B.out -f signed_$(PROGRAM)_full_mage.out 
endif

$(PROGRAM).elf: $(FW_VERSION).bin
# Pull in all the standard rules
include ${SRCTL}/${PMC_TOP_LEVEL}/build/rules.mak





