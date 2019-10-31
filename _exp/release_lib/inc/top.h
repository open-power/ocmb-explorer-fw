/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2018, 2019 Microchip Technology Inc. All rights reserved. 
*                                                                               
* Licensed under the Apache License, Version 2.0 (the "License"); you may not 
* use this file except in compliance with the License. You may obtain a copy of 
* the License at http://www.apache.org/licenses/LICENSE-2.0
*                                                                               
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT 
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the 
* License for the specific language governing permissions and limitations under 
* the License.
********************************************************************************/


/**
* @addtogroup TOP
* @{
* @file
* @brief
*   This module allows firmware drivers to dynamically determine hardware
*   resource information at run-time.
*
*   Refer to top_api.h for details of this module.
*
* @note
*
*/
#ifndef _TOP_H
#define _TOP_H

/*
* Include Files
*/
#include "pmc_hw.h"
#include "pmcfw_types.h"
#include "pmcfw_common.h"
#include "busio.h"
#include "top_api.h"

/*
* Macros
*/

/*
* Enumerated Types
*/

/*
* Constants
*/
#define TOP_DEV_ID_PALLADIUM            0x8596

/* Other Devices */
#define TOP_DEV_ID_INVALID              UINT16_MAX

/*
* Explorere CPU_BOOT[1:0] for controlling boot mode
* 00 - Boot from SPI interface directly
* 01 - Normal boot from ROM
* 10 - Reserved
* 11 - Product Engineer Mode 0 (spinlock at start pboot)
*/
#define TOP_CPU_BOOT_SPI                    0x0
#define TOP_CPU_BOOT_FLASH_ROM              0x1
#define TOP_CPU_BOOT_RESERVED               0x2
#define TOP_CPU_BOOT_PE_MODE                0x3

/*
* Macro Definitions
*/

/*
* Structures and Unions
*/

/*
* Global Variables
*/

/*
** Function Prototypes and Pointers to Functions in RAM
**
** To accommodate PIC code executing in SPI flash and non-PIC code executing in
** RAM, the functions in RAM are accessed through pointers. The changes that were made:
**
**      - original function name:     $type func_name($type, $type)
**        changed with prepended '_': $type _func_name($type, $type)
**      - public function pointers initialized in source code files:
**        PUBLIC $type (*func_name_ptr)($type, $type) =  _func_name;
**      - private function pointers intitialized in source code files:
**        PRIVATE $type (*func_name_ptr)($type, $type) = _func_name;
**      - for public functions new define for original function name in header file:
**        #define func_name (*func_name_ptr)
**      - for private functions new define for original function name in source code file:
**        #define func_name (*func_name_ptr)
*/
typedef BOOL (*top_efuse_spi_is_quad_fn_ptr)(VOID);
EXTERN top_efuse_spi_is_quad_fn_ptr top_efuse_spi_is_quad_ptr;
#define top_efuse_spi_is_quad (*top_efuse_spi_is_quad_ptr)

typedef UINT32 (*top_efuse_spi_pin_driver_strength_get_fn_ptr)(VOID);
EXTERN top_efuse_spi_pin_driver_strength_get_fn_ptr top_efuse_spi_pin_driver_strength_get_ptr;
#define top_efuse_spi_pin_driver_strength_get (*top_efuse_spi_pin_driver_strength_get_ptr)

typedef UINT32 (*top_efuse_spi_pin_pullup_get_fn_ptr)(VOID);
EXTERN top_efuse_spi_pin_pullup_get_fn_ptr top_efuse_spi_pin_pullup_get_ptr;
#define top_efuse_spi_pin_pullup_get (*top_efuse_spi_pin_pullup_get_ptr)

typedef VOID (*top_spi_driver_strength_set_fn_ptr)(UINT32 value);
EXTERN top_spi_driver_strength_set_fn_ptr top_spi_driver_strength_set_ptr;
#define top_spi_driver_strength_set (*top_spi_driver_strength_set_ptr)

typedef VOID (*top_spi_pullup_value_set_fn_ptr)(UINT32 value);
EXTERN top_spi_pullup_value_set_fn_ptr top_spi_pullup_value_set_ptr;
#define top_spi_pullup_value_set (*top_spi_pullup_value_set_ptr)


typedef BOOL (*top_spi_wci_mode_get_fn_ptr)(VOID);
EXTERN top_spi_wci_mode_get_fn_ptr top_spi_wci_mode_get_ptr;
#define top_spi_wci_mode_get (*top_spi_wci_mode_get_ptr)

/*
* Function Prototypes
*/

EXTERN unsigned top_device_rev_absolute(VOID);
EXTERN BOOL top_efuse_bootstrap_spi_reset_en(VOID);
EXTERN BOOL top_efuse_spi_ecc_enable_get(VOID);
EXTERN UINT32 top_efuse_spi_rate_get(VOID);
EXTERN UINT8 top_bootstrap_twi_il_addr_get(VOID);
EXTERN VOID top_device_reset(VOID);
EXTERN BOOL top_public_key_valid(UINT32 key_id);
EXTERN BOOL top_twi_img_load_enable_get(VOID);
EXTERN UINT32 top_cpu_boot_mode_get(VOID);
EXTERN BOOL top_secure_boot_mode_get(VOID);
EXTERN BOOL top_secure_twil_mode_get(VOID);
EXTERN BOOL top_fw_download_enable(VOID);
EXTERN void top_onetime_sha_value_reg_write(UINT32 image_id, UINT32 *sha_buffer, UINT32 size);
EXTERN UINT32 top_efuse_twi_pin_driver_strength_get(VOID);
EXTERN void top_ddr_phy_reset(BOOL do_reset);
EXTERN VOID top_ram_code_ptr_adjust(UINT32 offset);
EXTERN VOID top_fatal_error_handler(void);
EXTERN UINT32 top_twi_pin_default_driver_strength_get(void);
EXTERN void top_twi_driver_strength_set(UINT32 value);
EXTERN void top_di_enable(void);

#endif /* _TOP_H */
/** @} end addtogroup */


