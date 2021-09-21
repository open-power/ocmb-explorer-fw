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
* @defgroup CMDSVR_FUNC CMDSVR_FUNC: Interface and tools for creating cmdsvr cmds
*            
* @brief
*   Function prototypes and defines common to all users creating command
*   functions to be registered with the command server infrastucture.
*   Optional tools to help parse command function arguements.
* @{  
* @file 
* @brief 
*   Interface and tools for creating commands to be registered with cmdsvr 
*
* @note
*   The command function interface is separate from the command server implementation
*   to allow flexibility in the command server implementation. This header
*   contains all that is necessary for a module to register command functions.
*/ 

#ifndef _CMDSVR_FUNC_H
#define _CMDSVR_FUNC_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
* Enumerated Types
*/

/*
* Constants
*/

/*
* Macro Definitions
*/

/*
* Structures and Unions
*/

/**
* @brief
*   This typedef is used to declare the function interface for the cmdsvr cmds. 
*
* @param [in] args      - List of input parameter strings. 
* @param [in] num_args  - Number of parameters in list 
*  
* @return
*   PMC_SUCCESS - command parsed successfully
*   !PMC_SUCCESS - command parsing failed and the caller should print further
*                  help info. 
* @note
*   None.
*/
typedef PMCFW_ERROR (*cmdsvr_do_cmd_t)(CHAR * args[], UINT8 num_args);

/**
* @brief 
*   This structure is used to describe a command object, which includes the
*   command name, short description of the command, function to run, and
*   detailed description of the command.
*
*   Example command definition:
*
*   PRIVATE cmdsvr_cmd_def_struct cmdsvr_cmd_set[] = {
*     {
*        .cmd_name = "help",
*        .fn_name  = "Help containing menu of commands",
*        .do_cmd   = cmdsvr_help,
*        .internal = FALSE,
*        .desc     = "Help containing menu of commands\n"
*                    "Cmd Usage: help [cmd_name]\n"
*                    "           help          - List all commands\n"
*                    "           help cmd_name - Display command help\n"
*     }  
*   };
*
* @note
*   None
*
*/
typedef struct cmdsvr_cmd_def_struct
{   
    CHAR *cmd_name;      /**< String description of the command */    
    CHAR *fn_name;       /**< Short description of the function to run */    
    cmdsvr_do_cmd_t do_cmd; /**< Function pointer to run */    
    CHAR *desc;          /**< Long description of the command.*/    
    BOOL internal;       /**< TRUE = Cmd is internal (hidden); FALSE = Cmd is external (visible) */
} cmdsvr_cmd_def_struct;

/*
* Global Variables
*/

/*
* Function Prototypes
*/

/**
* @brief
*   Register a list of command functions with the command server
*
* @param[in] func_list     - command function list
* @param[in] func_list_len - length of the list
*
* @return
*   none
*
* @note
*   PMC_SUCCESS  - successfully registered command function list
*   !PMC_SUCCESS - failed to register command function list
*
*/
EXTERN PMCFW_ERROR cmdsvr_func_list_register(const cmdsvr_cmd_def_struct * const func_list,
                                             const UINT32 func_list_len);

#endif  /* _CMDSVR_FUNC_H */
/** @} end addtogroup */



