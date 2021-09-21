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
*   This file defines atomic operations for use on MIPS
*   processors that support the LL/SC instructions
* @note 
*/ 

#ifndef _CPUHAL_ATOMIC_H
#define _CPUHAL_ATOMIC_H

/*
* Include Files
*/
#include "pmcfw_types.h"

/*
* Global Variables 
*/

/*
* Enumerated Types
*/

/*
* Macro Definitions
*/

/**
* @brief
*   Perform atomic 32-bit signed add operation:
*     *value_ptr = *value_ptr + operand
* 
* @brief
*   Subtraction can be performed using negative values of 'operand'.
*
* @param[in]   value_ptr - Pointer to value to be modified.  May be in CPU register or
*                          memory.
* @param[in]   operand - Amount to adjust value by.  May be in CPU register, memory, or
*                        a compile time constant.
* 
* RETURNS
*   UINT32 - Resulting new value of *value_ptr
*   
* 
* @note
*     __asmleaf allows the usage of $2 and $3 without restriction.  $1 may also
*   be used if no synthesized instructions are used while the __asmleaf
*   is using $1.
* 
* @note
*   Assembly macros use patterns that define supported forms that the arguments
*   to the macro may take.  The assembly of the matching pattern will be used at
*   compile time.  Some supported patterns are:
*     %reg - argument is stored in a register
*     %mem - argument is stored in memory, which must be loaded
*     %con - argument is a constant specified at compile time
*     %error - Form of arguments does not match any specified patterns, issue
*              a compile error.  All assembly macros should specify this as the
*              last pattern.
*
*/
__asmleaf UINT32 cpuhal_atomic_add(INT32 *value_ptr, INT32 operand)
{
%reg value_ptr %reg operand
1:  ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    add     $3, $3, operand  // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %con operand
1:  li      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    add     $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %mem operand
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    add     $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %reg operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    add     $3, $3, operand  // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %con operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  li      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    add     $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %mem operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    add     $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%error
}


/**
* @brief
*   Perform atomic 32-bit unsigned add operation:
*     *value_ptr = *value_ptr + operand
* 
* @param[in]   value_ptr - Pointer to value to be modified.  May be in CPU register or
*                          memory.
* @param[in]   operand - Amount to adjust value by.  May be in CPU register, memory, or
*                        a compile time constant.
* 
* RETURNS
*   UINT32 - Resulting new value of *value_ptr
*   
* 
* @note
*   __asmleaf allows the usage of $2 and $3 without restriction.  $1 may also
*   be used if no synthesized instructions are used while the __asmleaf
*   is using $1.
* 
* @note
*   Assembly macros use patterns that define supported forms that the arguments
*   to the macro may take.  The assembly of the matching pattern will be used at
*   compile time.  Some supported patterns are:
*     %reg - argument is stored in a register
*     %mem - argument is stored in memory, which must be loaded
*     %con - argument is a constant specified at compile time
*     %error - Form of arguments does not match any specified patterns, issue
*              a compile error.  All assembly macros should specify this as the
*              last pattern.
*
*/
__asmleaf UINT32 cpuhal_atomic_addu(UINT32 *value_ptr, UINT32 operand)
{
%reg value_ptr %reg operand
1:  ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    addu    $3, $3, operand  // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %con operand
1:  li      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    addu    $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %mem operand
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    addu    $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %reg operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    addu    $3, $3, operand  // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %con operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  li      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    addu    $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %mem operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    addu    $3, $3, $2       // temp_value = temp_value + operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%error
}


/**
* @brief
*   Perform atomic 32-bit unsigned subtraction operation:
*     *value_ptr = *value_ptr - operand
* 
* @brief
*   For signed variables, use cpuhal_atomic_add() instead.
* 
* @param[in] value_ptr - Pointer to value to be modified.  May be in CPU register or
*                        memory.
* @param[in] operand    - Amount to adjust value by.  May be in CPU register, memory, or
*                         a compile time constant.
* 
* RETURNS
*   UINT32 - Resulting new value of *value_ptr
*   
* 
* @note
*   __asmleaf allows the usage of $2 and $3 without restriction.  $1 may also
*   be used if no synthesized instructions are used while the __asmleaf
*   is using $1.
* 
* @note
*   Assembly macros use patterns that define supported forms that the arguments
*   to the macro may take.  The assembly of the matching pattern will be used at
*   compile time.  Some supported patterns are:
*     %reg - argument is stored in a register
*     %mem - argument is stored in memory, which must be loaded
*     %con - argument is a constant specified at compile time
*     %error - Form of arguments does not match any specified patterns, issue
*              a compile error.  All assembly macros should specify this as the
*              last pattern.
*
*/
__asmleaf UINT32 cpuhal_atomic_subu(UINT32 *value_ptr, UINT32 operand)
{
%reg value_ptr %reg operand
1:  ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    subu    $3, $3, operand  // temp_value = temp_value - operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %con operand
1:  li      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    subu    $3, $3, $2       // temp_value = temp_value - operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %mem operand
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    subu    $3, $3, $2       // temp_value = temp_value - operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %reg operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    subu    $3, $3, operand  // temp_value = temp_value - operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %con operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  li      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    subu    $3, $3, $2       // temp_value = temp_value - operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %mem operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    subu    $3, $3, $2       // temp_value = temp_value - operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%error
}


/**
* @brief
*   Perform atomic 32-bit bitwise AND operation.
*     *value_ptr = *value_ptr & operand
* 
* @param[in] value_ptr - Pointer to value to be modified.  May be in CPU register or
*                        memory.
* @param[in] operand     - Value to AND with.  May be in CPU register, memory, or a compile
*                        time constant.
* 
* RETURNS
*   UINT32 - Resulting new value of *value_ptr
*   
* 
* @note
*   __asmleaf allows the usage of $2 and $3 without restriction.  $1 may also
*   be used if no synthesized instructions are used while the __asmleaf
*   is using $1.
* 
* @note
*   Assembly macros use patterns that define supported forms that the arguments
*   to the macro may take.  The assembly of the matching pattern will be used at
*   compile time.  Some supported patterns are:
*     %reg - argument is stored in a register
*     %mem - argument is stored in memory, which must be loaded
*     %con - argument is a constant specified at compile time
*     %error - Form of arguments does not match any specified patterns, issue
*              a compile error.  All assembly macros should specify this as the
*              last pattern.
*
*/
__asmleaf UINT32 cpuhal_atomic_and(UINT32 *value_ptr, UINT32 operand)
{
%reg value_ptr %reg operand
1:  ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    and     $3, $3, operand  // temp_value = temp_value & operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %con operand
1:  li      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    and     $3, $3, $2       // temp_value = temp_value & operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %mem operand
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    and     $3, $3, $2       // temp_value = temp_value & operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %reg operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    and     $3, $3, operand  // temp_value = temp_value & operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %con operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  li      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    and     $3, $3, $2       // temp_value = temp_value & operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %mem operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    and     $3, $3, $2       // temp_value = temp_value & operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%error
}


/**
* @brief
*   Perform atomic 32-bit bitwise OR operation.
*     *value_ptr = *value_ptr | operand
* 
* @param[in] value_ptr - Pointer to value to be modified.  May be in CPU register or
*                        memory.
* @param[in] operand - Value to OR with.  May be in CPU register, memory, or a compile
*                      time constant.
* 
* RETURNS
*   UINT32 - Resulting new value of *value_ptr
*   
* 
* @note
*   __asmleaf allows the usage of $2 and $3 without restriction.  $1 may also
*   be used if no synthesized instructions are used while the __asmleaf
*   is using $1.
* 
* @note
*   Assembly macros use patterns that define supported forms that the arguments
*   to the macro may take.  The assembly of the matching pattern will be used at
*   compile time.  Some supported patterns are:
*     %reg - argument is stored in a register
*     %mem - argument is stored in memory, which must be loaded
*     %con - argument is a constant specified at compile time
*     %error - Form of arguments does not match any specified patterns, issue
*              a compile error.  All assembly macros should specify this as the
*              last pattern.
*
*/
__asmleaf UINT32 cpuhal_atomic_or(UINT32 *value_ptr, UINT32 operand)
{
%reg value_ptr %reg operand
1:  ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    or      $3, $3, operand  // temp_value = temp_value | operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %con operand
1:  li      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    or      $3, $3, $2       // temp_value = temp_value | operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%reg value_ptr %mem operand
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0(value_ptr) // temp_value = *value_ptr, begin atomic sequence.
    or      $3, $3, $2       // temp_value = temp_value | operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0(value_ptr) // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %reg operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    or      $3, $3, operand  // temp_value = temp_value | operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %con operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  li      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    or      $3, $3, $2       // temp_value = temp_value | operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%mem value_ptr %mem operand
    lw      $1, value_ptr    // $1 = value_ptr
1:  lw      $2, operand      // $2 = operand
    ll      $3, 0($1)        // temp_value = *value_ptr, begin atomic sequence.
    or      $3, $3, $2       // temp_value = temp_value | operand
    or      $2, $3, $0       // return_value = temp_value
    sc      $3, 0($1)        // *value_ptr = temp_value, temp_value = SC result
    beq     $3, 0, 1b        // Check if LL-SC sequence was atomic
    nop                      // if not, try again.
%error
}

#define cpuhal_atomic_inc(x) (cpuhal_atomic_addu(&(x), 1))
#define cpuhal_atomic_dec(x) (cpuhal_atomic_subu(&(x), 1))

/*
* Constants
*/

/*
* Function Prototypes
*/

#endif /* _CPUHAL_ATOMIC_H */
/** @} end addtogroup */


