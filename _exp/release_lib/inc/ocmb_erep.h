/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2020 Microchip Technology Inc. All rights reserved. 
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
* @addtogroup OCMB_EREP
* @{ 
* @file 
* @brief
*   OCMB Error Reporting module declarations and APIs
* 
* @note 
*
*/ 
#ifndef _OCMB_EREP_H
#define _OCMB_EREP_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
* Enumerated Types
*/

/*
* Constants
*/

#define OCMB_EREP_SUB_CATEGORY_DONT_CARE    0x00FFFFFF
#define OCMB_EREP_REG_OFFSET_DONT_CARE      0xFFFFFFFF

/*
* Macro Definitions
*/

/*
* Structures and Unions
*/


/**
* @brief
*   Doorbells used to notify the Host on errors
* 
* @note
*   The enumerated type values **cannot** be greater than 4-bits in size.
*/
typedef enum
{
    ocmb_erep_db_1  = 0x2,  /**< Doorbell 1 - fatals.  0x2 represents bit 1. */
    ocmb_erep_db_2  = 0x4,  /**< Doorbell 2 - asserts and exceptions.  0x4 represents bit 2 */ 
    ocmb_erep_db_3  = 0x8,  /**< Doorbell 3 - continuous calibration error.  0x8 represents bit 3 */ 
} ocmb_erep_db_enum;


/**
* @brief
*   Error type
* 
* @note
*   The enumerated type values **cannot** be greater than 4-bits in size.
*/
typedef enum
{
    ocmb_erep_type_foxhound      = 0x1, /**< FoxHound Serdes Fatal */
    ocmb_erep_type_ddr4_phy      = 0x2, /**< DDR4 Phy Fatal */
    ocmb_erep_type_ddr4_phy_nf   = 0x3, /**< DDR4 Phy Fatal */
    ocmb_erep_type_gpbc_sys      = 0x4, /**< GPBC Sys Fatal */
    ocmb_erep_type_gpbc_peri     = 0x5, /**< GPBC Peri Fatal */
    ocmb_erep_type_gpbc_spi      = 0x6, /**< GPBC SPI Fatal */
    ocmb_erep_type_top           = 0x7, /**< TOP Fatal */
    ocmb_erep_type_top_nf        = 0x8, /**< TOP NON-Fatal*/
    ocmb_erep_type_fw_assert     = 0x9, /**< FW Assert */
    ocmb_erep_type_cpu_exc       = 0xA, /**< CPU Exception */
    ocmb_erep_type_hardware_wdt  = 0xB, /**< VPE0 hardware WDT expiry */
    ocmb_erep_type_interval_wdt  = 0xC  /**< VPE1 interval WDT expiry */
} ocmb_erep_type_enum;


/**
* @brief
*   This structure encapsulates all the information to be sent to the host on a
*   as extended error information.
* 
* @note
*   The firmware will pack the information in this structure into a set of hardware
*   scratchpads when it is ready to send the information to the host.
* 
*/
typedef struct
{
	BOOL                info_filled_flag;       /**< This flag is TRUE if there is info in struct. */
	ocmb_erep_type_enum err_type;               /**< The error_type. */
	BOOL                multiple_found_flag;    /**< Multiple fatals found in the hw_block. */
	UINT32              sub_category;           /**< The sub-category of the register.  Set to 0xFFFFFF to ignore. */
	UINT32              reg_offset;             /**< The offset into the register set, if required. */
	UINT32              err_data;               /**< The error data. */
} ocmb_erep_ext_err_struct;

/*
* Global variables
*/

/*
* Function Prototypes
*/

EXTERN void ocmb_erep_ext_err_fill(ocmb_erep_ext_err_struct ext_err);
EXTERN void ocmb_erep_db_ring(ocmb_erep_db_enum doorbell_num);
EXTERN void ocmb_erep_wdt_trigger(void);
EXTERN void ocmb_erep_init(void);

#endif /* _OCMB_EREP_H */


/** @} end addtogroup */


