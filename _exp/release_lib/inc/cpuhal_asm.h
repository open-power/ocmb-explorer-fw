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
*     This file contains the public hardware abstraction layer (hal) definitions
*     that are to be used by assembly files
* @note 
*/ 

/*---- Compile Options -----------------------------------------------------*/
#ifndef _CPUHAL_ASM_H
#define _CPUHAL_ASM_H

/*---- Constant and Type Declarations --------------------------------------*/

#define HAL_MEM_NUMBYTES_CACHE_LINE             32
#ifdef PMC_USE_MALTA
#define HAL_MEM_NUMBYTES_L2_CACHE_LINE          64
#else
#define HAL_MEM_NUMBYTES_L2_CACHE_LINE          32
#endif
#define HAL_MEM_ICACHE_WAY_NB                   4
#define HAL_MEM_DCACHE_WAY_NB                   4
#define HAL_MEM_SCACHE_WAY_NB                   8
#define HAL_MEM_CACHE_LINE_MASK                 0xFFFFFFE0
#ifdef PMC_USE_MALTA
#define HAL_MEM_L2_CACHE_LINE_MASK              0xFFFFFFC0
#else
#define HAL_MEM_L2_CACHE_LINE_MASK              0xFFFFFFE0
#endif

/* 
The sizes of the unmapped KSEG0 and KSEG1 address spaces are hard limited to 512 MB 
*/ 
#define HAL_MEM_UNMAPPED_PHY_ADDR_MASK  0x1FFFFFFF

/* cached coherent */
/* note that the SRCv memory map is out-of date; the meanings of */
/* KSEG0/2 were reversed as part of the single-image changes */
#define MIPS_BASE_KSEG0 0x80000000
#define MIPS_KSEG0(addr) (MIPS_BASE_KSEG0 | ((UINT32)(addr) & HAL_MEM_UNMAPPED_PHY_ADDR_MASK))
#define IS_MIPS_KSEG0(addr) (MIPS_BASE_KSEG0 == ((UINT32)(addr) & (~HAL_MEM_UNMAPPED_PHY_ADDR_MASK)))

/* uncached */
#define MIPS_BASE_KSEG1 0xA0000000
#define MIPS_KSEG1(addr) (MIPS_BASE_KSEG1 | ((UINT32)(addr) & HAL_MEM_UNMAPPED_PHY_ADDR_MASK))
#define IS_MIPS_KSEG1(addr) (MIPS_BASE_KSEG1 == ((UINT32)(addr) & (~HAL_MEM_UNMAPPED_PHY_ADDR_MASK)))

/* cached non-coherent */
#define MIPS_BASE_KSEG2 0xC0000000
#define MIPS_KSEG2(addr) (MIPS_BASE_KSEG2 | ((UINT32)(addr) & HAL_MEM_UNMAPPED_PHY_ADDR_MASK))
#define IS_MIPS_KSEG2(addr) (MIPS_BASE_KSEG2 == ((UINT32)(addr) & (~HAL_MEM_UNMAPPED_PHY_ADDR_MASK)))

#define CPUHAL_ICACHE                          0x00   /**< Primary instruction cache */
#define CPUHAL_DCACHE                          0x01   /**< Primary data cache */
#define CPUHAL_TCACHE                          0x02   /**< Tertiary cache */
#define CPUHAL_SCACHE                          0x03   /**< Secondary cache */

/*
* I (index invalidate);
* D (index writeback invalidate/index invalidate);
* S,T (index writeback invalidate/index invalidate)
*/
#define CPUHAL_CACHE_OP_IDX_INV                0x00
#define CPUHAL_CACHE_OP_IDX_LOAD_TAG           0x01   /**< All cache types */
#define CPUHAL_CACHE_OP_IDX_STORE_TAG          0x02   /**< All cache types */
#define CPUHAL_CACHE_OP_HIT_INV                0x04   /**< I,D; S,T */
/*
* I (fill)
* D (hit writeback invalidate/index invalidate)
* S,T (hit writeback invalidate/hit invalidate)
*/
#define CPUHAL_CACHE_OP_HIT_WB_INV             0x05
#define CPUHAL_CACHE_OP_HIT_WB                 0x06   /**< D,S,T */
#define CPUHAL_CACHE_OP_FETCH_LOCK             0x07   /**< I,D */

#define CPUHAL_DCACHE_FETCH_LOCK               ((CPUHAL_CACHE_OP_FETCH_LOCK << 2) | CPUHAL_DCACHE)
#define CPUHAL_DCACHE_HIT_WB_INV               ((CPUHAL_CACHE_OP_HIT_WB_INV << 2) | CPUHAL_DCACHE)

#define CPUHAL_SCACHE_FETCH_LOCK               ((CPUHAL_CACHE_OP_FETCH_LOCK << 2) | CPUHAL_SCACHE)

/* Coherency page attribute (for TLB/CM init) */
#define CPUHAL_CCA_WT_BIT             0
#define CPUHAL_CCA_WTNA_BIT           1
#define CPUHAL_CCA_UC_BIT             2
#define CPUHAL_CCA_C_NC_WB_WA_BIT     3
#define CPUHAL_CCA_C_WB_WA_C_RMRE_BIT 4
#define CPUHAL_CCA_C_WB_WA_C_RMRS_BIT 5
#define CPUHAL_CCA_UCA_BIT            7

#define CPUHAL_COHERENCY_TLB_C_FIELD_BITOFF      3
#define CPUHAL_COHERENCY_TLB_C_FIELD_BITMSK      0x38
#define CPUHAL_COHERENCY_TLB_C_FIELD(setting)    (((setting) << CPUHAL_COHERENCY_TLB_C_FIELD_BITOFF) & CPUHAL_COHERENCY_TLB_C_FIELD_BITMSK)

#define CPUHAL_TLB_PAGE_SIZE_8K     0x0000
#define CPUHAL_TLB_PAGE_SIZE_32K    0x0003
#define CPUHAL_TLB_PAGE_SIZE_128K   0x000F
#define CPUHAL_TLB_PAGE_SIZE_512K   0x003F
#define CPUHAL_TLB_PAGE_SIZE_2M     0x00FF
#define CPUHAL_TLB_PAGE_SIZE_8M     0x03FF
#define CPUHAL_TLB_PAGE_SIZE_32M    0x0FFF
#define CPUHAL_TLB_PAGE_SIZE_128M   0x3FFF
#define CPUHAL_TLB_PAGE_SIZE_512M   0xFFFF

#define CPUHAL_TLB_PAGE_SIZE_TO_KB(page_size)    (((page_size) + 1) * 8)
#define CPUHAL_TLB_KB_TO_PAGE_SIZE(k_bytes)      (((k_bytes) / 8) - 1)
#define CPUHAL_TLB_PAGE_SIZE_MULTIPLIER          4
#define CPUHAL_TLB_MIN_PAGE_SIZE                 CPUHAL_TLB_PAGE_SIZE_8K
#define CPUHAL_TLB_MAX_PAGE_SIZE                 CPUHAL_TLB_PAGE_SIZE_512M

#define CPUHAL_COHERENCY_TLB_PAGE_NUMBER_BITOFF              16
#define CPUHAL_COHERENCY_TLB_ENTRY_START_BITOFF              24
#define CPUHAL_COHERENCY_TLB_PAGE_NUMBER(number_of_entry)    (number_of_entry << CPUHAL_COHERENCY_TLB_PAGE_NUMBER_BITOFF)
#define CPUHAL_COHERENCY_TLB_ENTRY_START(first_entry_number) (first_entry_number << CPUHAL_COHERENCY_TLB_ENTRY_START_BITOFF)
/* entry valid bit */
#define CPUHAL_TLB_PAGE_V_BIT               1
#define CPUHAL_TLB_PAGE_V_BIT_BITOFF        1

/* entry dirty bit */
#define CPUHAL_TLB_PAGE_D_BIT               1
#define CPUHAL_TLB_PAGE_D_BIT_BITOFF        2

/* entry global bit */
#define CPUHAL_TLB_PAGE_G_BIT               1
#define CPUHAL_TLB_PAGE_G_BIT_BITOFF        0

/* entry global, dirty and valid set to 1 */
#define CPUHAL_TLB_PAGE_GVD_ENABLED_BIT  ((CPUHAL_TLB_PAGE_G_BIT << CPUHAL_TLB_PAGE_G_BIT_BITOFF) | \
                                             (CPUHAL_TLB_PAGE_V_BIT << CPUHAL_TLB_PAGE_V_BIT_BITOFF) | \
                                             (CPUHAL_TLB_PAGE_D_BIT << CPUHAL_TLB_PAGE_D_BIT_BITOFF))



#endif /* #ifdef _CPUHAL_ASM_H */

/** @} end addtogroup */


