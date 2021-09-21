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
* @addtogroup LOG_APP
* @{
* @file
* @brief
*    This module implements event logging functions.
*
*    Refer to log_app_api.h for details of this module.
*
* @note
*     None.
*
*/

/*---- Compile Options -----------------------------------------------------*/
#ifndef _LOG_APP_H
#define _LOG_APP_H

/*---- Common Include Files ------------------------------------------------*/

#include "log_app_api.h"

/*---- Common Enumerations -------------------------------------------------*/

/*---- Common Structures ---------------------------------------------------*/

/*---- Common Typedefs -----------------------------------------------------*/

typedef PMCFW_ERROR (*log_app_string_len_check_fn_ptr)(const char ** log_str_array_ptr, const UINT32 array_sz);

/*---- Common Macro Definition ---------------------------------------------*/

/*---- Include Files -------------------------------------------------------*/

/*---- Constant and Type Declarations --------------------------------------*/

/*---- Public Variable Declarations ----------------------------------------*/

/*---- Inline Function Declarations ----------------------------------------*/

/*---- Function Declarations -----------------------------------------------*/
EXTERN UINT32 log_app_log_search(UINT32 search_start_address,
                                 UINT32 search_end_address,
                                 UINT32 *log_buffer_base);

EXTERN void log_app_string_event(const CHAR    *format, 
                                 UINT32        log_word0,
                                 UINT32        log_word1,
                                 UINT32        log_word2, 
                                 UINT32        log_word3,
                                 UINT32        log_word4);

EXTERN void log_app_string_event_ccb_only(const CHAR    *format,
                                          UINT32        log_word0,
                                          UINT32        log_word1,
                                          UINT32        log_word2,
                                          UINT32        log_word3,
                                          UINT32        log_word4);

#define log_app_string_len_check (*log_app_string_len_check_ptr)
EXTERN log_app_string_len_check_fn_ptr log_app_string_len_check_ptr;

#endif /* _LOG_APP_H */
/** @} end addtogroup */


