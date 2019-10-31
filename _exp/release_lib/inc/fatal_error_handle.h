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


#ifndef _FATAL_ERROR_HANDLE_H
#define _FATAL_ERROR_HANDLE_H

/*
** Include Files
*/
#include "pmcfw_types.h"

/*
** Enumerated Types
*/

/**
* @brief
*   Enumerated type for the hardware block from which the fatal interrupt originated.
* 
* @note
*   The enumerated type values cannot be greater than 4-bits in size.
*/
typedef enum
{
	fatal_hw_block_foxhound  = 0x1, /**< FoxHound Serdes */
	fatal_hw_block_ddr4_phy  = 0x2, /**< DDR4 Phy */
	fatal_hw_block_gpbc_sys  = 0x3, /**< GPBC Sys */
	fatal_hw_block_gpbc_peri = 0x4, /**< GPBC Peri */
	fatal_hw_block_gpbc_spi  = 0x5, /**< GPBC SPI */
	fatal_hw_block_top       = 0x6, /**< TOP */
    fatal_hw_block_opsw      = 0x7, /**< OPSW */
	fatal_hw_block_pcse_irq  = 0x8, /**< PCSE_IRQ */
} fatal_hw_block_enum;


/*
** Constants
*/
#define FATAL_TO_HOST_INFO_SUB_CATEGORY_DEFAULT 0xFF

/*
** Strutures and Unions
*/

/**
* @brief
*   This structure encapsulates all the information to be sent to the host on a
*   fatal interrupt.
* 
* @note
*   The firmware will pack the information in this structure into a set of hardware
*   scratchpads when it is ready to send the information to the host.
* 
*/
typedef struct
{
	BOOL                info_filled_flag;       /**< This flag is TRUE if there is info in struct. */
	fatal_hw_block_enum hw_block;               /**< The HW block. */
	BOOL                multiple_found_flag;    /**< Multiple fatals found in the hw_block. */
	UINT8               sub_category;           /**< The sub-category of the register.  Set to 0xFF to ignore. */
	UINT32              reg_offset;             /**< The offset into the register set. */
	UINT32              reg_val;                /**< The value of the register. */
} fatal_to_host_info_struct;


/*
** Public Function Prototypes
*/

EXTERN VOID fatal_error_handler(void *int_num);
EXTERN VOID fatal_to_host_info_set (fatal_to_host_info_struct * fatal_info_ptr,
                                    fatal_hw_block_enum         hw_block,
                                    UINT8                       sub_category,
                                    UINT32                      reg_offset,
                                    UINT32                      reg_val);
EXTERN VOID fatal_to_host_report_send (fatal_to_host_info_struct to_host_fatal_info);
EXTERN VOID fatal_error_check_and_report (UINT32 reg_val, 
                                          UINT32 mask, 
                                          UINT32 reg_offset, 
                                          UINT32 sub_reg_num,
                                          const char * prefix_string,
                                          fatal_hw_block_enum hw_block_id,
                                          fatal_to_host_info_struct * to_host_fatal_info_ptr);


#endif /* _FATAL_ERROR_HANDLE_H */



