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
* @defgroup TOP TOP: Chip Info and TOP TSB Access
* @brief
*   This module allows firmware drivers to dynamically determine hardware
*   resource information at run-time.
*
*   This allows the same binary of the
*   firmware driver to run on multiple chip types which aids in testing
*   and maintenance. This method should be used when there are
*   only a few code differences between the drivers on the different
*   chip types AND the differences are not in the performance path.
*   Larger changes are better handled with #defines (to keep
*   the size of the driver down).
*   This API should only be used to query for PHYSICAL hardware resource
*   information that depends on the particular chip type.
*
* @{
* @file
* @brief
*   This file contains the public interface of the TOP module that are
*   required by external applications.
**
* @note
*
*/
#ifndef _TOP_API_H
#define _TOP_API_H

/*
* Include Files
*/
#include "pmc_hw.h"
#include "pmcfw_types.h"
#include "pmcfw_common.h"
#include "busio.h"

/*
* Macros
*/

/**
* @brief
*   This macro returns absolute address of a TOP_CORE or TOP_GB register.
*
* @param [in] reg_offset  - register offset
*
* @return
*   UINT32 absolute address of a TOP_CORE or TOP_GB register
*
* @hideinitializer
*/
#define TOP_REG_ADDR(reg_offset) \
    BUSIO_ADDR8(TOP_XCBI_BASE_ADDR, (reg_offset))

/**
* @brief
*   This macro reads a 32-bit value from a TOP_CORE or TOP_GB
*   register.
*
* @param [in] reg_offset  - register offset
*
* @return
*   UINT32 32-bit register value
*
* @hideinitializer
*/
#define TOP_REG_READ(reg_offset) \
    BUSIO_READ32_32_8(TOP_XCBI_BASE_ADDR, (reg_offset))

/**
* @brief
*   This macro writes a 32-bit value into TOP_CORE or TOP_GB
*   register.
*
* @param [in] reg_offset - register offset
* @param [in] reg_val    - register value
*
*
* @hideinitializer
*/
#define TOP_REG_WRITE(reg_offset, reg_val) \
    BUSIO_WRITE32_32_8(TOP_XCBI_BASE_ADDR, (reg_offset), (reg_val))

/**
* @brief
*   This macro writes completes a RMW for a TOP_CORE or TOP_GB
*   register.
*
* @param [in] reg_offset - register offset
* @param [in] mask       - register value mask
* @param [in] reg_val    - register value
*
*
* @hideinitializer
*/
#define TOP_REG_READ_WRITE(reg_offset, mask, reg_val) \
    busio_field_write32_32_8((void*)TOP_XCBI_BASE_ADDR, (reg_offset), (mask), (reg_val))

/**
* @brief
*   This macro reads a 32-bit value from an EFUSE register.
*
* @param [in] reg_offset  - register offset
*
* @return
*   UINT32 32-bit register value
*
* @hideinitializer
*/
#define TOP_EFUSE_REG_READ(reg_offset) \
    BUSIO_READ32_32_8(EFUSE_BASE, (reg_offset))

/*
* Enumerated Types
*/

/*
* Constants
*/

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
* Function Prototypes
*/

EXTERN unsigned top_device_id(VOID);

#endif /* _TOP_API_H */

/** @} end addtogroup */


