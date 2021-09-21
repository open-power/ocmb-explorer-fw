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
* DESCRIPTION  :  Embedded command server engine interface definition
*
* NOTES        :
*
********************************************************************************/



#ifndef _CMDSVR_H
#define _CMDSVR_H

#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
 * Constant definitions
 */

/* Error codes */
#define CMDSVR_ERR_INVALID_CMD_LISTS_MAX (PMCFW_ERR_BASE_CMDSVR + 0)  /* Invalid maximum number of command lists */
#define CMDSVR_ERR_CMD_LIST_ARR_FULL     (PMCFW_ERR_BASE_CMDSVR + 1)  /* Command list array is full */
#define CMDSVR_ERR_INVALID_PARAMS_01     (PMCFW_ERR_BASE_CMDSVR + 2)  /* Invalid parameters */
#define CMDSVR_ERR_INVALID_PARAMS_02     (PMCFW_ERR_BASE_CMDSVR + 3)  /* Invalid parameters */
#define CMDSVR_ERR_INVALID_PARAMS_03     (PMCFW_ERR_BASE_CMDSVR + 4)  /* Invalid parameters */
#define CMDSVR_ERR_INVALID_PARAMS_04     (PMCFW_ERR_BASE_CMDSVR + 5)  /* Invalid parameters */
#define CMDSVR_ERR_INVALID_PARAMS_05     (PMCFW_ERR_BASE_CMDSVR + 6)  /* Invalid parameters */
#define CMDSVR_ERR_INVALID_PARAMS_06     (PMCFW_ERR_BASE_CMDSVR + 7)  /* Invalid parameters */
#define CMDSVR_ERR_INVALID_PARAMS_07     (PMCFW_ERR_BASE_CMDSVR + 8)  /* Invalid parameters */
#define CMDSVR_ERR_INVALID_FUNC_LIST     (PMCFW_ERR_BASE_CMDSVR + 9)  /* Invalid command function list */
#define CMDSVR_ERR_INVALID_FUNC_LIST_LEN (PMCFW_ERR_BASE_CMDSVR + 10) /* Invalid command function list length */
#define CMDSVR_ERR_INVALID_CMD_NAME      (PMCFW_ERR_BASE_CMDSVR + 11) /* Invalid command name */
#define CMDSVR_ERR_INVALID_FN_NAME       (PMCFW_ERR_BASE_CMDSVR + 12) /* Invalid command function description */
#define CMDSVR_ERR_INVALID_DO_CMD        (PMCFW_ERR_BASE_CMDSVR + 13) /* Invalid command function pointer */
#define CMDSVR_ERR_DUPLICATE_CMD         (PMCFW_ERR_BASE_CMDSVR + 14) /* Duplicate command */


EXTERN VOID cmdsvr_init(const UINT32 cmd_lists_max, const BOOL disable_internal);

EXTERN BOOL cmdsvr_command_exec(char * const command_str);

EXTERN BOOL cmdsvr_command_match(CHAR * args[],
                                 UINT8 num_args);

EXTERN BOOL cmdsvr_command_exists(const CHAR * const cmd_name);

#endif  /* _CMDSVR_H */


