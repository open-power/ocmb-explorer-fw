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
*     This file contains the reset module definitions used both in
*     assembler files and in C source files.
*
*   NOTES:
*     None.
*
*******************************************************************************/



/**
 * Reset exception codes.
 */
#define RESET_EXC_CODE_ID_MASK             0x00000fff
#define RESET_EXC_CODE_CPU_MASK            0xfffff000
#define RESET_EXC_CODE_NONE                0xfffff000
#define RESET_EXC_CODE_TLB_EXCEPTION       0xfffff001
#define RESET_EXC_CODE_GEN_EXCEPTION       0xfffff002
#define RESET_EXC_CODE_NMI_EXCEPTION       0xfffff003
#define RESET_EXC_CODE_CACHE_EXCEPTION     0xfffff004
#define RESET_EXC_CODE_TC_HALTED           0xfffff005

/**
 * Size of the per VPE crash stack.
 */
#define RESET_SIZEOF_CRASH_STACK  512


