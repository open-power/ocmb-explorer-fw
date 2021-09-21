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
* @addtogroup UTILITY UTILITY: Common Utilities
*
* @{ 
* @file 
* @brief
*    Macros for converting endianess.
*
* @note
*    Either CPU_LITTLE or CPU_BIG has to be defined.
*/
#ifndef _PMCFW_ENDIAN_H
#define _PMCFW_ENDIAN_H

/*
* Include Files
*/
#include "pmcfw_compiler.h"
#include "pmcfw_types.h"
#include "pmcfw_common.h"

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
*   Endian-swap for 16, 32, or 64 bytes.
*
* @param[in] x - value to be swapped.
*
* @return 
*   Bytes in x are swapped endian-wise.
*
* @hideinitializer
*/
#define PMCFW_ENDIAN_SWAP_16(x) \
            (((((UINT16) x) & 0x00FF) << 8) | \
             ((((UINT16) x) & 0xFF00) >> 8))

#define PMCFW_ENDIAN_SWAP_32(x) \
            (((((UINT32) x) & 0x000000FF) << 24) | \
             ((((UINT32) x) & 0x0000FF00) <<  8) | \
             ((((UINT32) x) & 0x00FF0000) >>  8) | \
             ((((UINT32) x) & 0xFF000000) >> 24))

#define PMCFW_ENDIAN_SWAP_64(x) \
            (((((UINT64) x) & 0x00000000000000FFULL) << 56) | \
             ((((UINT64) x) & 0x000000000000FF00ULL) << 40) | \
             ((((UINT64) x) & 0x0000000000FF0000ULL) << 24) | \
             ((((UINT64) x) & 0x00000000FF000000ULL) <<  8) | \
             ((((UINT64) x) & 0x000000FF00000000ULL) >>  8) | \
             ((((UINT64) x) & 0x0000FF0000000000ULL) >> 24) | \
             ((((UINT64) x) & 0x00FF000000000000ULL) >> 40) | \
             ((((UINT64) x) & 0xFF00000000000000ULL) >> 56))


/**
* @brief
*   PMCFW_LE_TO_CPU_*
*        PMCFW_CPU_TO_BE_*
*        PMCFW_BE_TO_CPU_*
* __________________________________________________________________________
*
* @brief
*   Coverting host to little/big endian in 16, 32, or 64 bytes.
*
* @param[in] x - value to be converted.
*
* @return
*   x in little/big endian.
*
* @hideinitializer
*/
#if defined(CPU_LITTLE)

#define PMCFW_CPU_TO_LE_16(x) (x)
#define PMCFW_CPU_TO_LE_32(x) (x)
#define PMCFW_CPU_TO_LE_64(x) (x)

#define PMCFW_LE_TO_CPU_16(x) (x)
#define PMCFW_LE_TO_CPU_32(x) (x)
#define PMCFW_LE_TO_CPU_64(x) (x)

#define PMCFW_CPU_TO_BE_16(x) PMCFW_ENDIAN_SWAP_16(x)
#define PMCFW_CPU_TO_BE_32(x) PMCFW_ENDIAN_SWAP_32(x)
#define PMCFW_CPU_TO_BE_64(x) PMCFW_ENDIAN_SWAP_64(x)

#define PMCFW_BE_TO_CPU_16(x) PMCFW_ENDIAN_SWAP_16(x)
#define PMCFW_BE_TO_CPU_32(x) PMCFW_ENDIAN_SWAP_32(x)
#define PMCFW_BE_TO_CPU_64(x) PMCFW_ENDIAN_SWAP_64(x)

#elif defined(CPU_BIG)

#define PMCFW_CPU_TO_LE_16(x) PMCFW_ENDIAN_SWAP_16(x)
#define PMCFW_CPU_TO_LE_32(x) PMCFW_ENDIAN_SWAP_32(x)
#define PMCFW_CPU_TO_LE_64(x) PMCFW_ENDIAN_SWAP_64(x)

#define PMCFW_LE_TO_CPU_16(x) PMCFW_ENDIAN_SWAP_16(x)
#define PMCFW_LE_TO_CPU_32(x) PMCFW_ENDIAN_SWAP_32(x)
#define PMCFW_LE_TO_CPU_64(x) PMCFW_ENDIAN_SWAP_64(x)

#define PMCFW_CPU_TO_BE_16(x) (x)
#define PMCFW_CPU_TO_BE_32(x) (x)
#define PMCFW_CPU_TO_BE_64(x) (x)

#define PMCFW_BE_TO_CPU_16(x) (x)
#define PMCFW_BE_TO_CPU_32(x) (x)
#define PMCFW_BE_TO_CPU_64(x) (x)

#else

#error Neither CPU_LITTLE nor CPU_BIG were defined

#endif


/**
* @brief
*   Efficient endian swap for 16-bit halfword.
* 
*  @param[in] x - 16-bit halfword to convert
* 
* @return 
*   Bytes in x swapped endian-wise for a 16-bit value.
* 
* 
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT16 pmcfw_endian_asm_swap_16(UINT16 x)
{
%reg x
    wsbh    $2, x               // Swap bytes of 16-bit halfword
%mem x
    lhu     $2, x               // Read 16-bit variable into register
    wsbh    $2, $2              // Swap bytes of 16-bit halfword
%con x
    li      $2, x               // Read constant into register
    wsbh    $2, $2              // Swap bytes of 16-bit halfword
%error
}

/**
* @brief
*   Efficient endian swap for 32-bit word.
* 
* @brief
*   Example swapping endian-type:
*     Word to swap      = 0x89ABCDEF
*     Byte swap each    = 0xAB89EFCD
*     16-bit halfword
*     Swapped halfwords = 0xEFCDAB89
* 
* @param[in] x - 32-bit word to convert
* 
* @return 
*   Bytes in x swapped endian-wise for a 32-bit value.
* 
* 
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 pmcfw_endian_asm_swap_32(UINT32 x)
{
%reg x
    wsbh    $2, x               // Swap bytes for each 16-bit halfword.
    rotr    $2, $2, 16          // Swap halfwords
%mem x
    lw      $2, x               // Read 32-bit variable into register
    wsbh    $2, $2              // Swap bytes for each 16-bit halfword.
    rotr    $2, $2, 16          // Swap halfwords
%con x
    li      $2, x               // Read constant into register
    wsbh    $2, $2              // Swap bytes for each 16-bit halfword.
    rotr    $2, $2, 16          // Swap halfwords
%error
}

/**
* @brief
*   Efficient endian swap for 64-bit doubleword.
* 
*   Example swapping endian-type:
*                              upper_w   lower_w
*     Doubleword to swap = 0x(01234567)(89ABCDEF)
*   
*     Return value:
*       Upper word = $3 ($v1) = 0xEFCDAB89
*                             = byte-swapped and rotated 0x89ABCDEF (lower_w)
* 
*       Lower word = $2 ($v0) = 0x67452301
*                             = byte-swapped and rotated 0x01234567 (upper_w)
* 
* @return
* @param[in]  lower_w - Lower 32-bits of doubleword to convert
* @param[in]  upper_w - Upper 32-bits of doubleword to convert
* 
* @return  
*   64-bit value of input doubleword swapped endian-wise.
* 
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT64 pmcfw_endian_asm_swap_64(UINT32 lower_w, UINT32 upper_w)
{
%reg lower_w %reg upper_w
    wsbh    $3, lower_w         // Swap bytes for each 16-bit halfword
    rotr    $3, $3, 16          // Swap halfwords

    wsbh    $2, upper_w         // Swap bytes for each 16-bit halfword
    rotr    $2, $2, 16          // Swap halfwords                     
%reg lower_w %mem upper_w
    wsbh    $3, lower_w         // Swap bytes for each 16-bit halfword
    rotr    $3, $3, 16          // Swap halfwords                     

    lw      $2, upper_w         // Read 32-bit upper word variable into register
    wsbh    $2, $2              // Swap bytes for each 16-bit halfword
    rotr    $2, $2, 16          // Swap halfwords                     
%reg lower_w %con upper_w
    wsbh    $3, lower_w         // Swap bytes for each 16-bit halfword
    rotr    $3, $3, 16          // Swap halfwords                     

    li      $2, upper_w         // Read 32-bit upper word constant into register
    wsbh    $2, $2              // Swap bytes for each 16-bit halfword
    rotr    $2, $2, 16          // Swap halfwords                     
%mem lower_w %reg upper_w
    lw      $3, lower_w         // See above permutations for remaining cases
    wsbh    $3, $3
    rotr    $3, $3, 16

    wsbh    $2, upper_w
    rotr    $2, $2, 16
%mem lower_w %mem upper_w
    lw      $3, lower_w
    wsbh    $3, $3
    rotr    $3, $3, 16

    lw      $2, upper_w
    wsbh    $2, $2
    rotr    $2, $2, 16
%mem lower_w %con upper_w
    lw      $3, lower_w
    wsbh    $3, $3
    rotr    $3, $3, 16

    li      $2, upper_w
    wsbh    $2, $2
    rotr    $2, $2, 16
%con lower_w %reg upper_w
    li      $3, lower_w
    wsbh    $3, $3
    rotr    $3, $3, 16

    wsbh    $2, upper_w
    rotr    $2, $2, 16
%con lower_w %mem upper_w
    li      $3, lower_w
    wsbh    $3, $3
    rotr    $3, $3, 16

    lw      $2, upper_w
    wsbh    $2, $2
    rotr    $2, $2, 16
%con lower_w %con upper_w
    li      $3, lower_w
    wsbh    $3, $3
    rotr    $3, $3, 16

    li      $2, upper_w
    wsbh    $2, $2
    rotr    $2, $2, 16
%error
}

/**
* @brief
*   Efficient endian swap for 16-bit halfword.
*
* @param[in] x - 16-bit halfword to convert
*
* @return 
*   Bytes in x swapped endian-wise for a 16-bit value.
*
*/
PRIVATE inline UINT16 pmcfw_endian_efficient_swap_16(UINT16 x)
{
    return pmcfw_endian_asm_swap_16(x);
}

/**
* @brief
*   Efficient endian swap for 32-bit word.
*
* @param[in] x - 32-bit word to convert
*
* @return 
*   Bytes in x swapped endian-wise for a 32-bit value.
*
*/
PRIVATE inline UINT32 pmcfw_endian_efficient_swap_32(UINT32 x)
{
    return pmcfw_endian_asm_swap_32(x);
}

/**
* @brief
*   Efficient endian swap for 64-bit doubleword.
*
* @param[in] x - 64-bit doubleword to convert
*
* @return 
*   Bytes in x swapped endian-wise for a 64-bit value.
*
*/
PRIVATE inline UINT64 pmcfw_endian_efficient_swap_64(UINT64 x)
{
    return pmcfw_endian_asm_swap_64(pmc_low_32b(x), pmc_high_32b(x));
}


/**
* @brief
*   Convert UINT8[2] to UINT16.
*
* @param[in] array - array to convert.
*
* @return
*   array[0]*256 + array[1].
*
*/
PRIVATE inline UINT16 pmcfw_endian_convert_uint8_x2_to_uint16(const UINT8 array[2])
{
    return((*array<<8) + *(array+1));
}

/**
* @brief
*   Convert UINT8[3] to UINT32.
*
* @param[in] array - array to convert.
*
* @return
*   array[0]*65536 + array[1]*256 + array[2].
*
*/
PRIVATE inline UINT32 pmcfw_endian_convert_uint8_x3_to_uint32(const UINT8 array[3])
{
    return((*array<<16) + (*(array+1)<<8) + *(array+2));
}

/**
* @brief
*   Convert UINT8[4] to UINT32.
*
* @param[in] array - array to convert.
*
* @return
*   array[0]*16777216 + array[1]*65536 + array[2]*256 + array[3].
*
*/
PRIVATE inline UINT32 pmcfw_endian_convert_uint8_x4_to_uint32(const UINT8 array[4])
{
    return((*array<<24) + (*(array+1)<<16) + (*(array+2)<<8) + *(array+3));
}

/**
* @brief
*   Convert UINT16 to UINT8[2].
*
* @param[in] array - array to convert.
*
* @return 
*   Big endian representation of u16 value in array.
*
*/
PRIVATE inline void pmcfw_endian_convert_uint16_to_uint8_x2(UINT16 u16, UINT8 array[2])
{
    array[0] = u16 / 0x100;
    array[1] = u16 % 0x100;

    return;
}


/**
* @brief
*   Convert UINT32 to UINT8[3].
*
* @param[in] array - array to convert.
*
* @return 
*   Big endian representation of u32 value in array.
*
*/
PRIVATE inline void pmcfw_endian_convert_uint32_to_uint8_x3(UINT32 u32, UINT8 array[3])
{
    u32 %= 0x1000000;
    array[0] = u32 / 0x10000;
    u32 %= 0x10000;
    array[1] = u32 / 0x100;
    array[2] = u32 % 0x100;

    return;
}

/**
* @brief
*   Convert UINT32 to UINT8[4].
*
* @param[in] array - array to convert.
*
* @return 
*   Big endian representation of u32 value in array.
*
*/
PRIVATE inline void pmcfw_endian_convert_uint32_to_uint8_x4(UINT32 u32, UINT8 array[4])
{
    array[0] = u32 / 0x1000000;
    u32 %= 0x1000000;
    array[1] = u32 / 0x10000;
    u32 %= 0x10000;
    array[2] = u32 / 0x100;
    array[3] = u32 % 0x100;

    return;
}

/**
*
* @brief
**  Convert UINT64 to UINT8[8].
*
*   @param[in] array - array to convert. 
*
* @return 
*   Big endian representation of u64 value in array.
*
*/
PRIVATE inline void pmcfw_endian_convert_uint64_to_uint8_x8(UINT64 u64, UINT8 array[8])
{
    array[0] = u64 / (1LL << (UINT64_BITS - UINT8_BITS));
    u64 %= (1LL << (UINT64_BITS - UINT8_BITS));
    array[1] = u64 / (1LL << (UINT64_BITS - UINT8_BITS * 2));
    u64 %= (1LL << (UINT64_BITS - UINT8_BITS * 2));
    array[2] = u64 / (1LL << (UINT64_BITS - UINT8_BITS * 3));
    u64 %= (1LL << (UINT64_BITS - UINT8_BITS * 3));
    array[3] = u64 / (1LL << (UINT64_BITS - UINT8_BITS * 4));
    u64 %= (1LL << (UINT64_BITS - UINT8_BITS * 4));
    array[4] = u64 / (1LL << (UINT64_BITS - UINT8_BITS * 5));
    u64 %= (1LL << (UINT64_BITS - UINT8_BITS * 4));
    array[5] = u64 / (1LL << (UINT64_BITS - UINT8_BITS * 6));
    u64 %= (1LL << (UINT64_BITS - UINT8_BITS * 4));
    array[6] = u64 / (1LL << (UINT64_BITS - UINT8_BITS * 7));
    array[7] = u64 % (1LL << (UINT64_BITS - UINT8_BITS * 7));

    return;
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

#endif /* _PMCFW_ENDIAN_H */
/** @} end addtogroup */



