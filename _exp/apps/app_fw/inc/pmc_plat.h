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

/********************************************************************************
*   DESCRIPTION :
*     Higher level definitions for the SRCx platform. Each platform has
*     their own copy of pmc_plat.h  This header file allows platform
*     specific defines WITHOUT the name of platform being part of each
*     define. This makes module re-use much easier. These definitions
*     are higher level than pmc_hw.h and pmc_plat.h is only include
*     by modules that need it.
*
*     pmc_hw.h contains lower level platform definitions and is generic
*     enough that almost all modules include pmc_hw.h. pmc_hw.h is not
*     allowed to contain typedefs and C constructs.
*
*******************************************************************************/


#ifndef _PMC_PLAT_H
#define _PMC_PLAT_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "pmc_hw.h"
#include "pmcfw_common.h"
#include "cpuhal.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

#ifndef PMC_PLAT_DEBUG
#define PMC_PLAT_DEBUG 0
#endif

/* SRAM memory sections
** Application FW only has 2 memory sections - free_mem and memory required for FW authentication. 
** All these memories are located in SRAM memory.
*/
#define MAX_MEMORY_SECTION              2     /*0 - FREE Mem, 1 - memory used by FW authentication module*/
#define PMC_MEM_FREE                    0     /* By convention type 0 is internal .free_mem section for all platforms  */
#define AUTH_MEMORY_SECTION_ID          1     /* ID of memory reserved for FW authentication */



#define pmc_virt_to_phy_loc pmc_virt_to_phy_ddr
#define pmc_phy_to_virt_loc pmc_phy_to_virt_ddr


/*
** Macro Definitions
*/

/* all asserts in the performance path are debug conditional */
#if (PMC_PLAT_DEBUG == 1)
#define PMC_PLAT_ASSERT(condition, error_code) PMCFW_ASSERT(condition, error_code)
#else
#define PMC_PLAT_ASSERT(condition, error_code)
#endif

/* 
** Set USE_BOOTROM in CRYPTO_ROUTINE_SOURCE define when code is referring to 
** BOOTROM implementation of SHA code using function pointer
*/
#define USE_BOOTROM         1

/* 
** Set USE_CRYPTO_LIB in CRYPTO_ROUTINE_SOURCE define when code is referring to 
** mbedTLS SHA library code
*/
#define USE_CRYPTO_LIB      2

/* 
** Define to point the location of SHA lib implementation
*/
#define CRYPTO_ROUTINE_SOURCE    USE_BOOTROM



/****************************************************************************
*
* MACRO: PMC_INITFUNC & PMC_END_INITFUNC
* __________________________________________________________________________
*
* DESCRIPTION:
*   Identifies function as needed for bootup or init only. Typically used to place
*   these functions in slower memory.
*
*   USAGE RULES
*
*   1) wrap all functions that can only ever be used during bootup or init
*   with PMC_INITFUNC and PCM_END_INITFUNC.
*   2) NO functions that can be called during runtime (by any team) should ever
*   be wrapped. If you aren't certain, don't wrap it!
*   3) For maintenance reasons, wrap each function individually rather
*   than wrapping groups of functions.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*
* NOTES:
*
* For example:
*
*   PMC_INITFUNC
*   PRIVATE inline UINT16 egsm_fifo_bank_id(UINT16 id)
*   {
*   if (id < EGSM_FIFO_BANK_SIZE)
*   {
*       return 0;
*   }
*   return 1;
*   PMC_END_INITFUNC
*
*   Copied from "Conditional Pragma Directives" on p644 of build_mips.pdf
*
*****************************************************************************/
#define PMC_INITFUNC
#define PMC_END_INITFUNC


/****************************************************************************
*
* MACRO: PMC_RAM_PROGRAM & PMC_END_RAM_PROGRAM
* __________________________________________________________________________
*
* DESCRIPTION:
*   Identifies function that is loaded in ROM but executes from RAM.
*
*   USAGE RULES
*
*   1) wrap all functions that are to be executed from RAM with
*   PMC_RAM_PROGRAM and PCM_END_RAM_PROGRAM.
*   2) For maintenance reasons, wrap each function individually rather
*   than wrapping groups of functions.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*
* NOTES:
*
* For example:
*
*   PMC_RAM_PROGRAM
*   PRIVATE inline UINT16 egsm_fifo_bank_id(UINT16 id)
*   {
*   if (id < EGSM_FIFO_BANK_SIZE)
*   {
*       return 0;
*   }
*   return 1;
*   }
*   PMC_END_RAM_PROGRAM
*
*   Copied from "Conditional Pragma Directives" on p644 of build_mips.pdf
*
*****************************************************************************/
#define PMC_RAM_PROGRAM         CHANGE_SEC(text, ".text_rammem")
#define PMC_END_RAM_PROGRAM     CHANGE_SEC(text, default)


/****************************************************************************
*
* MACRO: pmc_assert_cached
* __________________________________________________________________________
*
* DESCRIPTION:
*   Asserts that pointer is a valid cached address.
*
* INPUTS:
*   ptr - Pointer to be checked.
*
* OUTPUTS:
*   None.
*
* RETURNS:
*
* NOTES:
*
*****************************************************************************/
PRIVATE inline void pmc_assert_cached(const void *ptr)
{
#ifdef DISABLE_FOR_NOW
    const UINT32 high_nibble = ((UINT32) ptr) >> 28;

    PMC_PLAT_ASSERT( (high_nibble == 0x8) ||
                     (high_nibble == 0x9) ||
                     (high_nibble == 0xc) ||
                     (high_nibble == 0xd),
        PMCFW_ERR_INVALID_PTR);
#endif
}

/****************************************************************************
*
* MACRO: pmc_phy_to_virt_ddr_in_kuseg_region
* __________________________________________________________________________
*
* DESCRIPTION:
*   converts a physical DDR address into a virtual CPU address in DDR memory
*   space in the MIPS kuseg region (0x0000_0000:0x7FFF_FFFF).
*
* INPUTS:
*   addr - physical DDR address
*
* OUTPUTS:
*
* RETURNS:
*   pointer to DDR memory for use by the processor.
*
* NOTES:
*   Since KUSEG is a mapped region the TLB determines if the memory is
*   cached or not. The TLB is typically set so this region is uncached.
*
*****************************************************************************/
PRIVATE inline void *pmc_phy_to_virt_ddr_in_kuseg_region(const UINT32 addr)
{
    PMC_PLAT_ASSERT(addr < 0x80000000, PMCFW_ERR_INVALID_PTR);

    return (void *) addr;
} /* pmc_phy_to_virt_ddr_in_kuseg_region */



/*
** Structures and Unions
*/

/*
** Global variables
*/

/* Memory Sections defined in linker file. */
EXTERN UINT8 __ghsbegin_free_mem[];
EXTERN UINT8 __ghsend_free_mem[];
EXTERN UINT32 __ghsbegin_image_vec_tlb_ref[];
EXTERN UINT8 __ghsbegin_fw_auth_mem[];
EXTERN UINT8 __ghsend_fw_auth_mem[];
EXTERN UINT8 __ghsbegin_text[];
EXTERN UINT8 __ghsend_text[];
EXTERN UINT8 __ghsbegin_text_rammem[];
EXTERN UINT8 __ghsend_text_rammem[];
EXTERN UINT8 __ghsbegin_image_bev_reset[];
EXTERN UINT8 __ghsend_image_vec_extra[];
EXTERN UINT8 __ghsbegin_handoff_data[];
EXTERN UINT8 __ghsbegin_pboot_sda_patch[];

/*
** Function Prototypes
*/


#endif /* _PMC_PLAT_H */



