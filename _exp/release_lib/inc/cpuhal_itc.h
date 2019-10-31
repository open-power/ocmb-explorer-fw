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
* @addtogroup CPUHAL
* @{ 
* @file 
* @brief
*   This file provides the interface to MIPS ITC cells.
* @note 
*/ 

#ifndef __CPUHAL_ITC_H__
#define __CPUHAL_ITC_H__

#include "pmcfw_types.h"
#include "pmcfw_mid.h"

/* 
** See ITC View Addresses table in "ITC Views" section
** of the MIPS Software User Manual
*/ 
/* Empty/Full Synchronized View address bit */
#define CPUHAL_ITC_EF_VIEW_OFFSET              0x00000010
/* Control View address bit */
#define CPUHAL_ITC_CTL_VIEW_OFFSET             0x00000008

/* See ITC Storage section of the MIPS Software User Manual */
/* This bit indicates whether an ITC cell is configured as a FIFO */
#define CPUHAL_ITC_CELL_IS_FIFO_BITOFF         17
#define CPUHAL_ITC_CELL_IS_FIFO_BITMSK         (0x1 << CPUHAL_ITC_CELL_IS_FIFO_BITOFF)

/* 
** See ITCAddressMap0 and ITCAddressMap1 Register formats
*/
/* EntryGrain 0 = 128B - This defines the cell spacing */
#define CPUHAL_ITC_BLOCK_ENTRYGRAIN            0

/* ITC enable bit for setting the base address */
#define CPUHAL_ITC_BLOCK_ITC_EN                1

/* Spacing between cells (address bitshift) is EntryGrain is 2^7+n where n is
* the EntryGrain field in ITC config register */
#define CPUHAL_ITC_CELL_BITSHIFT               (7 + CPUHAL_ITC_BLOCK_ENTRYGRAIN)

/* AddrMask defines the size of the ITC memory block */
#define CPUHAL_ITC_BLOCK_ADDRMASK_BITOFF       10
#define CPUHAL_ITC_BLOCK_ADDRMASK              (CPUHAL_ITC_ADDR_MASK << CPUHAL_ITC_BLOCK_ADDRMASK_BITOFF)

EXTERN UINT16 cpuhal_itc_get_num_entries(void);
EXTERN void   cpuhal_itc_init(UINT32 base_address);
EXTERN UINT32 *cpuhal_itc_mutex_create(UINT32 init_full);
EXTERN UINT32 *cpuhal_itc_fifo_create(void);
EXTERN UINT32 cpuhal_itc_fifo_depth_get(UINT32 *fifo_ptr);

#endif /* __CPUHAL_ITC_H__ */
/** @} end addtogroup */


