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
*     This file contains the reset module public definitions.
*
*   NOTES:
*     None.
*
*******************************************************************************/


#ifndef _RESET_H
#define _RESET_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "pmcfw_common.h"

/*
** Enumerated Types
*/

/***************************************************************************
* ENUM: reset_genexccode_enum
* __________________________________________________________________________
*
* DESCRIPTION:
*   This enumeration specifies all possible MIPS exceptions.  The numbers
*   correspond to the value in the ExcCode field of the co-processor
*   Cause register.
*
* ELEMENTS:
*   RESET_GENEXCCODE_INT      - Interrupt, only for non-Vectored, non-EIC
*                               mode
*   RESET_GENEXCCODE_MOD      - TLB modification exception
*   RESET_GENEXCCODE_TLBL     - TLB exception (load or instruction fetch)
*   RESET_GENEXCCODE_TLBS     - TLB exception (store)
*   RESET_GENEXCCODE_ADEL     - Address error exception (load or instruction
*                               fetch)
*   RESET_GENEXCCODE_ADES     - Address error exception (store)
*   RESET_GENEXCCODE_IBE      - Bus error exception (instruction fetch)
*   RESET_GENEXCCODE_DBE      - Bus error exception (data reference: load
*                               or store)
*   RESET_GENEXCCODE_SYS      - Syscall exception
*   RESET_GENEXCCODE_BP       - Breakpoint exception
*   RESET_GENEXCCODE_RI       - Reserved instruction exception
*   RESET_GENEXCCODE_CPU      - Coprocessor Unusable exception
*   RESET_GENEXCCODE_OV       - Arithmetic Overflow exception
*   RESET_GENEXCCODE_TR       - Trap exception
*   RESET_GENEXCCODE_FPE      - Floating point exception
*   RESET_GENEXCCODE_IS1      - Coprocessor 2 implementation specific
*                               exception
*   RESET_GENEXCCODE_CEU      - CorExtend Unusable
*   RESET_GENEXCCODE_C2E      - Precise Coprocessor 2 exception
*   RESET_GENEXCCODE_WATCH    - Reference to WatchHi/WatchLo address
*   RESET_GENEXCCODE_MCHECK   - Machine check
*   RESET_GENEXCCODE_MT       - MIPS MT ASP interrupt
*                               (See CP0_VPEControl[EXCPT])
*   RESET_GENEXCCODE_CACHEERR - Cache error for debug mode
*
****************************************************************************/
typedef enum 
{
    RESET_GENEXCCODE_INT          = 0,
    RESET_GENEXCCODE_MOD          = 1,
    RESET_GENEXCCODE_TLBL         = 2,
    RESET_GENEXCCODE_TLBS         = 3,
    RESET_GENEXCCODE_ADEL         = 4,
    RESET_GENEXCCODE_ADES         = 5,
    RESET_GENEXCCODE_IBE          = 6,
    RESET_GENEXCCODE_DBE          = 7,
    RESET_GENEXCCODE_SYS          = 8,
    RESET_GENEXCCODE_BP           = 9,
    RESET_GENEXCCODE_RI           = 10,
    RESET_GENEXCCODE_CPU          = 11,
    RESET_GENEXCCODE_OV           = 12,
    RESET_GENEXCCODE_TR           = 13,
    RESET_GENEXCCODE_FPE          = 15,
    RESET_GENEXCCODE_IS1          = 16,
    RESET_GENEXCCODE_CEU          = 17,
    RESET_GENEXCCODE_C2E          = 18,
    RESET_GENEXCCODE_WATCH        = 23,
    RESET_GENEXCCODE_MCHECK       = 24,
    RESET_GENEXCCODE_MT           = 25,
    RESET_GENEXCCODE_CACHEERR     = 30,

    RESET_GENEXCCODE_NUM          = 32

} reset_genexccode_enum;

typedef void (*reset_nmi_cback_fcn_ptr)(void);

typedef void (*reset_assertion_cback_fcn_ptr)(void *cback_arg);

typedef void (*reset_cback_fcn_ptr)(UINT32 error_code,
                                    UINT32 num_reg,
                                    UINT32* reg_array,
                                    UINT32 call_stack_size,
                                    UINT32* call_stack_ptr);

typedef void (*reset_cli_cback_fcn_ptr)(void);
/*
** Constants
*/

/*
 * Exception handler stack size. If this changes, update stack offset calculation in
 * cache_exc_error_handler()
 */
#define RESET_UNCACHED_STACK_SIZE   512

/* Error codes */
#define RESET_ERR_INITIALIZED               (PMCFW_ERR_BASE_RESET+0)      /* already initialized */
#define RESET_ERR_UNINITIALIZED             (PMCFW_ERR_BASE_RESET+1)      /* not initialized yet */
#define RESET_ERR_REGISTERED                (PMCFW_ERR_BASE_RESET+2)      /* already registered */
#define RESET_ERR_UNREGISTERED              (PMCFW_ERR_BASE_RESET+3)      /* not registered yet */
#define RESET_ERR_INVALID_PARAM             (PMCFW_ERR_BASE_RESET+4)      /* invalid parameter */
#define RESET_ERR_NMI_FAILURE               (PMCFW_ERR_BASE_RESET+5)      /* nmi failed to complete */
#define RESET_ERR_HOST_NMI                  (PMCFW_ERR_BASE_RESET+6)      /* Host generated NMI */
#define RESET_ERR_APPLICATION_ASSERT        (PMCFW_ERR_BASE_RESET+7)      /* Application assertion */

/*
** Macro Definitions
*/
/*
** Structures and Unions
*/

/*
** Global Variables
*/

/*
** Function prototypes
*/

/******************************************************************************
* STRUCTURE: reset_parms_struct
* _____________________________________________________________________________
*
* DESCRIPTION:
*   reset parameters structure
*
* ELEMENTS:
*   num_of_vpe              - number of vpe in the system
*   num_of_tc               - number of tc in the system
*   reset_on_fatal_error    - reset upon fatal error 
*   fatal_cli_cback_ptr     - fatal error cli function pointer
*   nmi_cleanup_cback_ptr   - Function to cleanup SMP context before executing NMI
*
******************************************************************************/
typedef struct 
{
    UINT32                  num_of_vpe;
    UINT32                  num_of_tc;
    BOOL                    reset_on_fatal_error;
    reset_cli_cback_fcn_ptr fatal_cli_cback_ptr;

} reset_parms_struct;

/*
** Global Variables
*/

/*
** Function prototypes
*/
EXTERN void reset_init(reset_parms_struct *parms_ptr);

EXTERN void reset_genexc_register(reset_genexccode_enum   exccode,
                                  reset_cli_cback_fcn_ptr user_exc_handler);

EXTERN void reset_genexc_unregister(reset_genexccode_enum  exccode);

EXTERN void reset_default_gen_exc_handler(void);

EXTERN void reset_fw(void);

EXTERN void reset_nmi_cback_asm(void *cback_arg);

EXTERN void reset_gen_exc_vec(void *cback_arg);

EXTERN const CHAR * reset_cpu_exc_string_get(const UINT32 error_code);

EXTERN void reset_info_print(void);

EXTERN void reset_assertion_cback_register (reset_assertion_cback_fcn_ptr fptr);

EXTERN void reset_fatal_process_callback_set(reset_cback_fcn_ptr callback_ptr);

#endif /* _RESET_H */


