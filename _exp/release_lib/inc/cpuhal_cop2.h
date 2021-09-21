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
*       This file defines COP2 operations for MIPS cores. The COP2 provides native
*       64b math operations for the 32b MIPS cpu. Native 64 operations provide
*       a significant benefit for multiplication. A comparison of CPU cycles is 
*       below with COP2 in PIPELINE mode.
*       
*    Time to do 100 of COP2 op ADDU REFERENCE = 2645 cycles, 26 cycles per OP 
*    Time to do 100 of COP2 op ADDU VALUE = 3467 cycles, 34 cycles per OP  
*    Time to do 100 of COMPILER op ADDU = 2512 cycles, 25 cycles per OP
* 
*    Time to do 100 of COP2 op SUBU REFERENCE = 2366 cycles, 23 cycles per OP 
*    Time to do 100 of COP2 op SUBU VALUE = 3471 cycles, 34 cycles per OP  
*    Time to do 100 of COMPILER op SUBU = 2277 cycles, 22 cycles per OP
* 
*    Time to do 100 of COP2 op MULT REFERENCE = 2667 cycles, 26 cycles per OP 
*    Time to do 100 of COP2 op MULT VALUE = 3768 cycles, 37 cycles per OP  
*    Time to do 100 of COMPILER op MULT = 6669 cycles, 66 cycles per OP
* 
*    Time to do 100 of COP2 op DIVU REFERENCE = 4215 cycles, 42 cycles per OP 
*    Time to do 100 of COP2 op DIVU VALUE = 5215 cycles, 52 cycles per OP  
*    Time to do 100 of COMPILER op DIVU = 9475 cycles, 94 cycles per OP
* 
*    Time to do 100 of COP2 op MODU REFERENCE = 4204 cycles, 42 cycles per OP 
*    Time to do 100 of COP2 op MODU VALUE = 5169 cycles, 51 cycles per OP  
*    Time to do 100 of COMPILER op MODU = 11000 cycles, 110 cycles per OP 
* 
*   Even assuming worst case values, SUBU/ADDU are always worse using the COP2.
*   These macros should not be used. 
* 
*   MULTU always had performance increases. Using the COP2 for MULTU will provide
*   a performance benefit. 
* 
*   DIVU/MODU should only be used if values are > max UINT32. The cycles above assume 
*   64b values that are > max UINT32. If the values are < max UINT32, the compiler 
*   will optimize to save cycles while the COP2 cycles will remain the same. 
* 
* 
* @note
*    
*       Each TC in the MIPS cluster has exclusive access to 4 CP0 registers,
*       therefore HW exclusion is not necessary.
*       To ensure SW mutual exclusion, interrupts are atomically disabled prior
*       to loading the CP2 registers. This will prevent a context switch until
*       the CP2 has finished the operation.
*       These macros make no assumptions of coherency. If coherency is
*       required, a sync must be done after the macro to ensure the data has been
*       pushed out of cache.
*/ 

#ifndef _CPUHAL_COP2_H
#define _CPUHAL_COP2_H

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
*   Configure CU2 in status register to allow access to coprocessor 2. This must
*   be called by every thread context that wishes to use COP2.
*/
__asmleaf void cpuhal_cop2_enable(void)
{
    mfc0    $2, $12, 0 
    lui     $3, 0x4000
    or      $2, $2, $3
    mtc0    $2, $12
    ehb
}

/**
* @brief
*   Execute a 64B arithmetic operation ADDU in COP2
* 
*   result = op1 + op2
*
* @param[in] op1 - Operand 1
* @param[in] op2 - Operand 2
* 
*
* @return 
*   Result
*
*/
__asmleaf UINT64 cpuhal_cop2_addu(UINT64 op1, UINT64 op2)
{
%reg op1 %reg op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %reg op2

    /* Enter critical section */
    di      $1

    /* OP1 is an address */
    ldc2    $1, op1($gp)  
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %mem op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %mem op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp)
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %mem op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %con op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp) 
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %con op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %reg op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %con op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    li      $3, op2
    mtc2    $3, $2 
    mthc2   $0, $2    

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation SUBU in COP2
* 
*   result = op1 - op2
*
* @param[in] op1 - Operand 1
* @param[in] op2 - Operand 2
* 
*
* @return 
*   Result
*
*/
__asmleaf UINT64 cpuhal_cop2_subu(UINT64 op1, UINT64 op2)
{
%reg op1 %reg op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %reg op2

    /* Enter critical section */
    di      $1

    /* OP1 is an address */
    ldc2    $1, op1($gp)  
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %mem op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %mem op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp)
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %mem op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %con op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp) 
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %con op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %reg op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %con op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    li      $3, op2
    mtc2    $3, $2 
    mthc2   $0, $2    

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation MULTU in COP2
* 
*   result = op1 * op2
*
* @param[in] op1 - Operand 1
* @param[in] op2 - Operand 2
* 
*
* @return 
*   Result
*
*/
__asmleaf UINT64 cpuhal_cop2_multu(UINT64 op1, UINT64 op2)
{
%reg op1 %reg op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %reg op2

    /* Enter critical section */
    di      $1

    /* OP1 is an address */
    ldc2    $1, op1($gp)  
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %mem op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %mem op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp)
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %mem op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %con op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp) 
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %con op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %reg op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %con op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    li      $3, op2
    mtc2    $3, $2 
    mthc2   $0, $2    

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation DIVU in COP2
* 
*   result = op1 / op2
*
* @param[in] op1 - Operand 1
* @param[in] op2 - Operand 2
* 
*
* @return 
*   Result
*
*/
__asmleaf UINT64 cpuhal_cop2_divu(UINT64 op1, UINT64 op2)
{
%reg op1 %reg op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %reg op2

    /* Enter critical section */
    di      $1

    /* OP1 is an address */
    ldc2    $1, op1($gp)  
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %mem op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %mem op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp)
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %mem op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %con op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp) 
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %con op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %reg op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %con op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    li      $3, op2
    mtc2    $3, $2 
    mthc2   $0, $2    

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation MODU (Remainder) in COP2
* 
*   result = op1 % op2
*
* @param[in] op1 - Operand 1
* @param[in] op2 - Operand 2
* 
*
* @return 
*   Result
*
*/
__asmleaf UINT64 cpuhal_cop2_modu(UINT64 op1, UINT64 op2)
{
%reg op1 %reg op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %reg op2

    /* Enter critical section */
    di      $1

    /* OP1 is an address */
    ldc2    $1, op1($gp)  
    mtc2    op2, $2
    mthc2   (op2 + 1), $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %mem op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %mem op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp)
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %mem op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    ldc2    $2, op2($gp)  

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%mem op1 %con op2

    /* Enter critical section */
    di      $1

    ldc2    $1, op1($gp) 
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%reg op1 %con op2

    /* Enter critical section */
    di      $1

    mtc2    op1, $1 
    mthc2   (op1 + 1), $1
    li      $2, op2
    mtc2    $2, $2
    mthc2   $0, $2 

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %reg op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    mtc2    op2, $2
    mthc2   (op2 + 1), $2

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%con op1 %con op2

    /* Enter critical section */
    di      $1

    li      $2, op1
    mtc2    $2, $1 
    mthc2   $0, $1    
    li      $3, op2
    mtc2    $3, $2 
    mthc2   $0, $2    

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    mfc2    $2, $3
    mfhc2   $3, $3

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation ADDU in COP2 using memory transactions.
* 
*   result = op1 + op2
*
* @param[in] op1_ptr - Pointer to memory location of operand 1 64b
* @param[in] op2_ptr - Pointer to memory location of operand 2 64b
* @param[in] tgt_ptr - Pointer to memory location to put resultant 64b
* 
*      Resultant into tgt_ptr
*
* @return 
*    None
*
*/
__asmleaf void cpuhal_cop2_addu_mem(UINT64 *op1_ptr, UINT64 *op2_ptr, UINT64 *tgt_ptr)
{
%reg op1_ptr %reg op2_ptr %reg tgt_ptr

    /* Enter critical section */
    di      $1

    /* Load operands into COP2 F1 = Op1, F2 = Op1 */
    ldc2    $1, 0x0(op1_ptr)
    ldc2    $2, 0x0(op2_ptr)

    /* Perform COP2 operation F3 = F1 + F2 */
    cop2    0x110c0 

    /* Copy result to memory Tgt = F3 */
    sdc2    $3, 0x0(tgt_ptr)

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation SUBU in COP2 using memory transactions
* 
*   result = op1 - op2
*
* @param[in] op1_ptr - Pointer to memory location of operand 1 64b
* @param[in] op2_ptr - Pointer to memory location of operand 2 64b
* @param[in] tgt_ptr - Pointer to memory location to put resultant 64b
* 
*      Resultant into tgt_ptr
*
* @return 
*    None
*
*/
__asmleaf void cpuhal_cop2_subu_mem(UINT64 *op1_ptr, UINT64 *op2_ptr, UINT64 *tgt_ptr)
{
%reg op1_ptr %reg op2_ptr %reg tgt_ptr

    /* Enter critical section */
    di      $1

    /* Load operands into COP2 F1 = Op1, F2 = Op2 */
    ldc2    $1, 0x0(op1_ptr)
    ldc2    $2, 0x0(op2_ptr)

    /* Perform COP2 operation F3 = F1 - F2 */
    cop2    0x2208c0 

    /* Copy result to memory Tgt = F3 */
    sdc2    $3, 0x0(tgt_ptr)

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation MULTU in COP2 using memory transactions
* 
*   result = op1 * op2
*
* @param[in] op1_ptr - Pointer to memory location of operand 1 64b
* @param[in] op2_ptr - Pointer to memory location of operand 2 64b
* @param[in] tgt_ptr - Pointer to memory location to put resultant 64b
* 
*      Resultant into tgt_ptr
*
* @return 
*    None
*
*/
__asmleaf void cpuhal_cop2_multu_mem(UINT64 *op1_ptr, UINT64 *op2_ptr, UINT64 *tgt_ptr)
{
%reg op1_ptr %reg op2_ptr %reg tgt_ptr

    /* Enter critical section */
    di      $1

    /* Load operands into COP2 F1 = Op1, F2 = Op2 */
    ldc2    $1, 0x0(op1_ptr)
    ldc2    $2, 0x0(op2_ptr)

    /* Perform COP2 operation HI/LO = F1 * F2 */
    cop2    0x8110c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    /* Copy result to memory Tgt = F3 */
    sdc2    $3, 0x0(tgt_ptr)

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation DIVU in COP2 using memory transactions
* 
*   result = op1 / op2
*
* @param[in] op1_ptr - Pointer to memory location of operand 1 64b
* @param[in] op2_ptr - Pointer to memory location of operand 2 64b
* @param[in] tgt_ptr - Pointer to memory location to put resultant 64b
* 
*      Resultant into tgt_ptr
*
* @return 
*    None
*
*/
__asmleaf void cpuhal_cop2_divu_mem(UINT64 *op1_ptr, UINT64 *op2_ptr, UINT64 *tgt_ptr)
{
%reg op1_ptr %reg op2_ptr %reg tgt_ptr

    /* Enter critical section */
    di      $1

    /* Load operands into COP2 F1 = Op1, F2 = Op2 */
    ldc2    $1, 0x0(op1_ptr)
    ldc2    $2, 0x0(op2_ptr)

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = LO */ 
    cop2    0x12000c0

    /* Copy result to memory Tgt = F3 */
    sdc2    $3, 0x0(tgt_ptr)

    /* Exit critical section */
    ei      $1
%error
}

/**
* @brief
*   Execute a 64B arithmetic operation MODU (Remainder) in COP2 using memory
*      transactions
* 
*   result = op1 % op2
*
* @param[in] op1_ptr - Pointer to memory location of operand 1 64b
* @param[in] op2_ptr - Pointer to memory location of operand 2 64b
* @param[in] tgt_ptr - Pointer to memory location to put resultant 64b
* 
*      Resultant into tgt_ptr
*
* @return 
*    None
*
*/
__asmleaf void cpuhal_cop2_modu_mem(UINT64 *op1_ptr, UINT64 *op2_ptr, UINT64 *tgt_ptr)
{
%reg op1_ptr %reg op2_ptr %reg tgt_ptr

    /* Enter critical section */
    di      $1

    /* Load operands into COP2 F1 = Op1, F2 = Op2 */
    ldc2    $1, 0x0(op1_ptr)
    ldc2    $2, 0x0(op2_ptr)

    /* Perform COP2 operation LO = F1 / F2  & HI = F1 % F2 */
    cop2    0xC208c0
 
    /* Perform COP2 operation F3 = HI */ 
    cop2    0x10000c0

    /* Copy result to memory Tgt = F3 */
    sdc2    $3, 0x0(tgt_ptr)

    /* Exit critical section */
    ei      $1
%error
}

/*
* Constants
*/

/*
* Function Prototypes
*/

#endif /* _CPUHAL_COP2_H */
/** @} end addtogroup */


