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
* @addtogroup tsh
* @{  
* @file 
* @brief 
*   This is the public header file for users of the Tiny Shell (tsh) module. The
*   module implements a simple command shell which:
*    -prints a prompt to a UART
*    -gathers a command line string from the UART with support for basic
*     editing features such as backspace, delete, and echo
*    -passes the command line string to a registered command server module for
*     execution
*
*   The upper layer can register multiple command servers with the shell using
*   tsh_cmdsvr_register(). The shell will pass commands to each command server
*   in order of registration until a command server has handled the command.
* 
*   Multiple instances of the Tiny Shell can be created. This can be useful
*   for systems with multiple UARTs, as a separate shell can be instantiated
*   for each UART.
*
* @note
*/
#ifndef _TSH_H
#define _TSH_H

/*
** Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "tsh_plat_cfg.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/* Error codes */
#define TSH_ERR_INVALID_PARM_PTR_01     (PMCFW_ERR_BASE_TSH + 0) /* Bad parameter pointer */
#define TSH_ERR_INVALID_PARM_PTR_02     (PMCFW_ERR_BASE_TSH + 1) /* Bad parameter pointer */
#define TSH_ERR_INVALID_PARM_PTR_03     (PMCFW_ERR_BASE_TSH + 2) /* Bad parameter pointer */
#define TSH_ERR_INVALID_PARM_PTR_04     (PMCFW_ERR_BASE_TSH + 3) /* Bad parameter pointer */
#define TSH_ERR_COMMAND_EXEC_LIST_FULL  (PMCFW_ERR_BASE_TSH + 4) /* Command Execute function pointer list is full */
#define TSH_ERR_INVALID_SHELL_IDX_01    (PMCFW_ERR_BASE_TSH + 5) /* Invalid shell index */
#define TSH_ERR_INVALID_SHELL_IDX_02    (PMCFW_ERR_BASE_TSH + 6) /* Invalid shell index */
#define TSH_ERR_INVALID_SHELL_IDX_03    (PMCFW_ERR_BASE_TSH + 7) /* Invalid shell index */
#define TSH_ERR_INVALID_SHELL_IDX_04    (PMCFW_ERR_BASE_TSH + 8) /* Invalid shell index */
#define TSH_ERR_INVALID_SHELL_IDX_05    (PMCFW_ERR_BASE_TSH + 9) /* Invalid shell index */
#define TSH_ERR_INVALID_SHELL_IDX_06    (PMCFW_ERR_BASE_TSH + 10) /* Invalid shell index */
#define TSH_ERR_INVALID_CMDSVR_REG_MAX  (PMCFW_ERR_BASE_TSH + 11) /* Invalid max. number of registered command servers */

/* maximum number of characters in a tsh thread (will be named "tsh_xx") */
#define TSH_THREAD_NAME_CHARS_MAX   7

#define TSH_CMD_BUFF_LEN  256 /**< maximum command string length */

/*
** Structure and Union types
*/

/*
** typedef for command server "command_exec" functions that can be registered
** with the shell
*/
typedef BOOL (*tsh_cmdsvr_command_exec_fptr)(char * const command_str);

/**
* @brief
*   This structure contains the parameters which do not change after the
*   tsh_create() function call.
*/
typedef struct
{
    UINT32 shell_idx;              /**< Shell index */ 
#if (TSH_THREAD_SUPPORT == 1)
    CHAR thread_name[TSH_THREAD_NAME_CHARS_MAX]; /**< Thread name */
    UINT32 thread_stack_size;      /**< Stack size for thread in bytes */
    UINT32 thread_priority;        /**< tsh thread priority */
    UINT32 thread_affinity_mask;   /**< tsh thread affinity mask */
#endif
    UINT32 cmdsvr_reg_max;         /**<
                                   * maximum number of command servers that can
                                   * be registered with the shell
                                   */
    CHAR * prompt_str;             /**< shell prompt string */
} tsh_cfg_struct;

/**
* @brief
*   This structure contains the public, cfg, and pointer to private
*   parameters which are used by the module and thread.
*/
typedef struct
{
    tsh_cfg_struct cfg;     /**<
                            * configuration parameters which do not change after
                            * create()
                            */
    BOOL prompt_en;         /**< enable/disable for the prompt */
    struct tsh_parms_prv_struct *parms_prv_ptr;
                            /**< per-instance private data used by module */
    UINT8 uart_port;        /**< UART port tsh used */
    BOOL echo_en;           /**< enable/disable for echo feature */
} tsh_parms_struct;

/*
** Function Prototypes
*/

EXTERN tsh_parms_struct *tsh_parms_get(const UINT32 shell_idx);

EXTERN VOID tsh_create(tsh_parms_struct * const parms_ptr);

EXTERN VOID tsh_init(const tsh_parms_struct * const parms_ptr);

EXTERN PMCFW_ERROR tsh_cmdsvr_register(const UINT32 shell_idx,
                                       const tsh_cmdsvr_command_exec_fptr command_exec_fptr);

EXTERN VOID tsh_main_loop(const UINT32 shell_idx,
                          const BOOL threaded);

EXTERN UINT8 tsh_uart_port_get(const UINT32 shell_idx);

EXTERN VOID tsh_uart_port_set(const UINT32 shell_idx,
                              const UINT8 uart_port);

#endif /* _TSH_H */

/** @} end addtogroup */



