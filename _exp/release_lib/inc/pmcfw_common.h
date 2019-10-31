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
* @defgroup UTILITY UTILITY: Common Utilities
* @brief  
*   This module provides a set of platform related utilities: macros, inline 
* functions and error codes. These utilities make it easier for customers to 
* develop their own applications. 
* @{ 
* @file 
* @brief
*   Often used macros and inlines that are common to all platforms.
*
*/
#ifndef _PMCFW_COMMON_H
#define _PMCFW_COMMON_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "cpuhal_api.h"

/*
* Enumerated Types
*/

/*
* Constants
*/

/*
* Macro Definitions
*/

/**
* @brief
*   _Pragma preprocessing operator from C99.  Using this operator is
*   equivalent to using #pragma.  Use this operator when the effect of 
*   a directive is desired within a macro.
* 
* @param[in] x - pragma text string passed into _Pragma operator
*
*
*
*
* @note
*   Copied from "Conditional Pragma Directives" on p644 of build_mips.pdf
* 
* @hideinitializer
*/
#define PRAGMA(x)   _Pragma(#x)

/**
* @brief
*   Instructs the compiler to allocate the sectname code or data that would
*   normally be allocated to secttype.  sectname must not contain spaces,
*   quotes, parentheses, or wildcards.
*
* @param[in] sectype - section type (e.g. text)
* @param[in] sectname - name of the section, as defined by linker map
*              (e.g. ".text_slowmem")
* 
* @note
*   Copied from "Conditional Pragma Directives" on p644 of build_mips.pdf
* 
* @hideinitializer
*/
#define CHANGE_SEC(sectype, sectname)   PRAGMA( ghs section sectype = sectname )

/**
* @brief
*   Instructs the compiler to make the next variable x-byte aligned,
*   where x is a power of 2 and no greater than 128.  If the next
*   variable is a non-static local variable, the compiler ignores this
*   directive. The compiler also inserts padding to make the memory
*   footprint of the variable to be a multiple of x.
*
* @param[in] x - number of bytes to align variable (power of 2)
* 
* @note
*   Copied from "Conditional Pragma Directives" on p644 of build_mips.pdf
* 
* @hideinitializer
*/
#define ALIGNVAR(x)     PRAGMA( alignvar ## ( ## x ## ) )

/**
* @brief
*   This macro aligns the next variable in memory with a minimum alignment
*   of the cache line.
* 
* @brief
*   1) The base address of the variable aligned to a cache line.
*     2) The variable is padded out to the cache line (ie. The sizeof()
*        operator always returns the multiple of cache-line size).
*   For example,
* 
*     PMC_CACHEALIGN_NEXT_VAR
*     PRIVATE struct_name my_struct_var;
* 
* @hideinitializer
*/
#define PMC_CACHEALIGN_NEXT_VAR     ALIGNVAR(HAL_MEM_NUMBYTES_CACHE_LINE)

/**
* @brief
*   This macro identifies the structure in memory with a minimum
*   alignment of the cache line.  That is,
* 
*     1) The base address is cache-aligned
*     2) The structure is padded out to the cache line (ie. The sizeof()
*        operator always returns the multiple of cache-line size)
* 
* NOTES:
* 
*   For maintenance reasons, wrap each structure individually rather 
*   than wrapping groups of structures.
* 
*   For example,
* 
*     PMC_BEGIN_CACHEALIGN_STRUCT
*     struct xxx_yyyy_struct
*     {
*         UINT32 rsvd_1;
*         UINT32 rsvd_2;          
*     }; 
*     PMC_END_CACHEALIGN_STRUCT 
* 
* @hideinitializer
*/
#define PMC_BEGIN_CACHEALIGN_STRUCT  PRAGMA(ghs struct_min_alignment(HAL_MEM_NUMBYTES_CACHE_LINE))
#define PMC_END_CACHEALIGN_STRUCT    PRAGMA(ghs struct_min_alignment())

/**
* @brief
*   This macro identifies the union in memory with a minimum
*   alignment of the cache line.  That is,
* 
*     1) The base address is cache-aligned
*     2) The union is padded out to the cache line (ie. The sizeof()
*        operator always returns the multiple of cache-line size)
* 
* @note
*     For maintenance reasons, wrap each union individually rather 
*   than wrapping groups of unions.
*
* @note
*     For example,
* 
* @note
*     PMC_BEGIN_CACHEALIGN_UNION
*     union xxx_yyyy_union
*     {
*         UINT32 rsvd_1;
*         UINT32 rsvd_2;          
*     }; 
*     PMC_END_CACHEALIGN_UNION 
* 
* @hideinitializer
*/
#define PMC_BEGIN_CACHEALIGN_UNION  PRAGMA(ghs struct_min_alignment(HAL_MEM_NUMBYTES_CACHE_LINE))
#define PMC_END_CACHEALIGN_UNION    PRAGMA(ghs struct_min_alignment())

/**
* @brief
*   Calculates the number of elements in a statically defined array.
*
* @param[in] x - statically defined array.
*
* @return 
*   Number of elements in the array.
*
* @hideinitializer
*/
#define PMC_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/**
* @brief
*   Align x to align, which must be a power of two.
* 
* @brief
*   Typically used to create an aligned region in an unaligned memory block.
*   datap = malloc(1000 + cache_line_size);
*   aligned_datap = (cast) PMC_ALIGN((UINT32) datap , cache_line_size);
*
* @param [in] x - number to align.
* @param [in] align - size to align to. Must be a power of two.
*
*   x aligned to align. x is unmodified if already aligned.
*
* @hideinitializer
*/
#define PMC_ALIGN(x,align) (((x)+(align)-1)&~((align)-1))

/**
* @brief
*   Workaround for Klocwork static checker to remove infinite
*   loop warning.
* 
* @hideinitializer
*/
#ifdef __KLOCWORK__
#define PMC_LOOP_FOREVER
#else
#define PMC_LOOP_FOREVER while(1)
#endif


/**
* @brief
*   Packs upper and lower 32-bit values into a UINT64.
*
* @param[in] low - lower 32-bits of 64-bit value.
* @param[in] high - upper 32-bits of 64-bit value.
*
* @return
*    upper and lower 32-bit values packed into a 64-bit value.
*
* @hideinitializer
*/
static inline UINT64 pmc_pack_64b(UINT32 low, UINT32 high)
{
    return ((UINT64)high << 32) | low;
}

/**
* @brief
*   Unpacks lower 32-bits from a 64-bit value.
*
* @param[in] packed - 64-bit value.
*
* @return
*    lower 32-bits of the 64-bit value.
*
* @hideinitializer
*/
static inline UINT32 pmc_low_32b(UINT64 packed)
{
    return (UINT32) packed;
}

/**
* @brief
*   Unpacks higher 32-bits from a 64-bit value.
*
* @param[in] packed - 64-bit value.
*
* @return
*    higher 32-bits of the 64-bit value.
*
* @hideinitializer
*/
static inline UINT32 pmc_high_32b(UINT64 packed)
{
    return (UINT32)(packed >> 32);
}

/**
* @brief
*   Increments 64-bit value.
*
* @param[in] low_ptr - ptr to low 32-bits of 64-bit value
* @param[in] high_ptr - ptr to high 32-bits of 64-bit value
* @param[in] inc - amount to increment
*
* @param[out] low_ptr - ptr to low 32-bits of incremented 64-bit value
* @param[out] high_ptr - ptr to high 32-bits of incremented 64-bit value
*
* @return 
*   none
*
* @note
*/
PRIVATE inline void pmc_inc_64b(UINT32 * const low_ptr, UINT32 * const high_ptr, 
                                const UINT32 inc)
{
    UINT64 val;

    val = pmc_pack_64b(*low_ptr, *high_ptr);
    val += inc;
    *low_ptr = pmc_low_32b(val);
    *high_ptr = pmc_high_32b(val);
}

/*
* Structures and Unions
*/

/*
* Global variables
*/

/*
* Function Prototypes
*/
PUBLIC void memset_align32(UINT32 *dst, UINT32 d, size_t n);
PUBLIC UINT32 memcpy_dst_align32(UINT32 *dst, UINT8 *src, UINT32 size);
PUBLIC void memcpy_src_align32(UINT8 *dst, UINT32 *src, UINT32 size);

#endif /* _PMCFW_COMMON_H */
/** @} end group */



