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
* @brief
*   PMC Firmware generic bit operations.
*
* @note
*     None.
*
*/ 
#ifndef _PMCFW_BITOPS_H_
#define _PMCFW_BITOPS_H_

#ifndef CPU_EAMP
#include "cpuhal.h"
#include "string.h"
#endif

/*#############################################################################
#
#
#                               M A C R O S
#
#
#############################################################################*/
/*
*      Bit manipulation MACROS
*/
/*
*      Any bit set in Y will get set in X
*          i.e. Y:n == 1 ? X:n = 1 else X:n = X:n
*/
#define BITOPS_SET_BITS( x, y )       ( x ) |= ( y );
/*
*      Any bit set in Y will get cleared in X
*          i.e. Y:n == 1 ? X:n = 0 else X:n = X:n
*/
#define BITOPS_CLR_BITS( x, y )       ( x ) &= ~( y );
/*
*      Any bit cleared in Y will get cleared in X
*          i.e. Y:n == 0 ? X:n = 0 else X:n = X:n
*/
#define BITOPS_MSK_BITS( x, y )       ( x ) &= ( y );
/*
*      Any bit set in Y will toggle the bit in X
*          i.e. Y:n == 1 ? X:n = !X:n else X:n = X:n
*/
#define BITOPS_XOR_BITS( x, y )       ( x ) ^= ( y );
/*
*      Bit test MACROS
*/
/*
*      TRUE if all of the bits that are 1 in 'y' are 0 in 'x', else FALSE.
*/
#define BITOPS_ALL_BITS_OFF( x, y )   ( 0 == ( ( x ) & ( y ) ) )
/*
*      TRUE if any of the bits that are 1 in 'y' are 0 in 'x', else FALSE.
*/
#define BITOPS_ANY_BITS_OFF( x, y )   ( ( y ) != ( ( x ) & ( y ) ) )
/*
*      TRUE if all of the bits that are 1 in 'y' are 1 in 'x', else FALSE.
*/
#define BITOPS_ALL_BITS_ON( x, y )    ( ( y ) == ( ( x ) & ( y ) ) )
/*
*      TRUE if any of the bits that are 1 in 'y' are 1 in 'x', else FALSE.
*/
#define BITOPS_ANY_BITS_ON( x, y )    ( 0 != ( ( x ) & ( y ) ) )


/*
* Bit extraction macros
*/

/* Get mask of least significant bit set in x */
#define BITOPS_LSB_SET_MASK_GET(x) ((x) & ~((x) - 1))



#ifndef CPU_EAMP
/*#############################################################################
#
#
#                        I n l i n e   F u n c t i o n s
#
#
#############################################################################*/
/**
* @brief
*   find last bit set.
*   It returns the position of the last (the most significant) bit in the
*   word w. The least significant bit is position 1 and the most significant
*   bit 32.
*
* @param[in] w   - the word to be searched.
*
* @return
*   returns the position of the last bit set, or 0 if no bits are set.
*
*/
static inline INT32 fls(INT32 w)
{
    return (32 - hal_count_lead_zeroes(w));
}

/**
* @brief
*   find last bit set with long long type.
*   It returns the position of the last (the most significant) bit in the
*   long long w. The least significant bit is position 1 and the most significant
*   bit 32.
*
* @param[in] w   - the word to be searched.
*
* @return
*   returns the position of the last bit set, or 0 if no bits are set.
*
*/
static inline INT32 fls_64(INT64 w)
{
    INT32 ret;

    if ( (w >> 32) & 0xffffffff)
    {
        ret = fls((INT32)(w >> 32));

        ret += 32;
    }
    else
    {
        ret = fls((INT32)w);
    }

    return ret;
}

/**
* @brief
*   find first bit set. 
*   It returns the position of the first bit in the
*   word w. The least significant bit is position 1 and the most significant
*   bit 32.
*
* @param[in] w   - the word to be searched.
*
* @return
*    returns the position of the first bit set, or 0 if no bits are set.
*
*/
static inline INT32 find_first_bit_set(INT32 w)
{
    return fls(w & -w);
}

/**
* @brief
*   find first bit set with long long type.
*   It returns the position of the first bit in the
*   long long w. The least significant bit is position 1 and the most significant
*   bit 64.
*
* @param[in] w   - the long long to be searched.
*
* @return
*   returns the position of the first bit set, or 0 if no bits are set.
*
*/
PRIVATE inline INT32 find_first_bit_set_64(INT64 w)
{
    INT32 ret;

    if (w & 0xffffffff)
    {
        ret = find_first_bit_set((INT32)w);
    }
    else
    {
        ret = find_first_bit_set((INT32)(w >> 32));

        if (ret)
        {
            ret += 32;
        }
    }

    return ret;
}

/**
* @brief
*   find the mask for the specified size. 
*   It returns the mask of the size good for PCIe size/mask registers. The mask
*   complies with the rules set by PCI BAR Mask registers. For example,
*      size          mask
*   0x00100000   0x000fffff
*   0x00200000   0x003fffff
*   0x00300000   0x003fffff
*   0x00400000   0x003fffff
*   0x00500000   0x007fffff
*   0x00800000   0x007fffff
*   0x00900000   0x00ffffff
*
* @param[in] size   - the size
*
* @return
*   returns the mask of the size according to the ruls of PCI BAR Mask 
*   registers.
*
* @note
*   If size is 0, 1, 2, the returned mask doesn't comply with the rules.
*   However the PCI bar size won't be so small. Forewarmed!
*/
static inline UINT32 find_size_mask(UINT32 size)
{
    INT32 mask, ls, fs;

    if (!size)
        return 0;

    ls = fls((INT32)size);
    fs = ffs((INT32)size);

    if (ls == fs)
        ls--;

    mask = 1 << ls;

    return --mask;
}

/**
* @brief
*   This function determins if the given integer v is a power of 2.
*
* @param[in] v - the input number.
*
* @return 
*   1 - TRUE
* @return 
*   0 - FALSE
*
* 
* @note
*   0 is not a power of 2. The algorithm comes from here
*   http://www-graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
*/
static inline INT32 is_powerof_two(INT32 v)
{
    return v && !(v & (v - 1));
}

//MIT HAKMEM Count
/**
* @brief
*   This is the MIT HAKMEM Count algorithm counting the number
*   of 1s in a 32-bit data.
*
* @param[in] n - the input number.
*
* @param[out] None.
*
* @return 
*   The number of 1s.
*
* 
* @note
*   This works only for a 32-bit data. It needs modification to work with
*   a 64-bit data.
*
*/
static inline UINT32 bitCount32(UINT32 n)
{
   /* works for 32-bit numbers only    */
   /* fix last line for 64-bit numbers */

   UINT32 tmp;

   tmp = n - ((n >> 1) & 033333333333)
           - ((n >> 2) & 011111111111);
   return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}

/**
* @brief
*   Writes a bit in a multi-UINT32 bit array
*
* @param[in]
*   bit_array_base - address of base of bit array
*   bit_array_size - size of bit array (number of 32-bit words)
*   bit_id - id of bit within bit field. LSB of first UINT32 in array
*       corresponds to bit zero. LSB of second UINT32 corresponds to bit 32
*       and so on.
*   bit_val - 0 or 1.
*  
*/
static inline void bitmap_set(UINT32 * const bit_array_base,
                              const UINT32 bit_array_size,
                              const UINT32 bit_id,
                              const UINT32 bit_val)
{
    const UINT32 array_index = bit_id / 32;
    const UINT32 bit_offset_within_word = bit_id - (array_index * 32);
    const UINT32 mask = 1 << bit_offset_within_word;
    const UINT32 val = (bit_val & 1) << bit_offset_within_word;

    PMCFW_ASSERT(array_index < bit_array_size, PMCFW_ERR_NUMBER_OUT_OF_RANGE);

    bit_array_base[array_index] = (bit_array_base[array_index] & ~mask) | val;
}

/**
* @brief
*   Return the bit value of specified bit ID in a bit array
*
* @param[in]
*   bit_array_base - address of base of bit array
*   bit_array_size - size of bit array (number of 32-bit words)
*   bit_id - id of bit within bit field. LSB of first UINT32 in array
*       corresponds to bit zero. LSB of second UINT32 corresponds to bit 32
*       and so on.
* 
* @return
*   bit_val - 0 or 1
*/
static inline BOOL bitmap_get(UINT32 * const bit_array_base,
                              const UINT32 bit_array_size,
                              const UINT32 bit_id)
{
    const UINT32 array_index = bit_id / 32;
    const UINT32 bit_offset_within_word = bit_id - (array_index * 32);
    const UINT32 mask = 1 << bit_offset_within_word;

    PMCFW_ASSERT(array_index < bit_array_size, PMCFW_ERR_NUMBER_OUT_OF_RANGE);

    return ((bit_array_base[array_index] & mask) != 0);
}


#endif /* !CPU_EAMP */
#endif /* _PMCFW_BITOPS_H_ */



