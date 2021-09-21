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


#ifndef _LOG_PLAT_H
#define _LOG_PLAT_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "log_plat_cfg.h"

/*
** Enumerated Types
*/

/*
** Typedefs
*/

/*
** Constants
*/

/*
** Macro Definitions
*/

/**
* @brief
*   Expands the label and text_string input into an enum.  The text_string will
*   not be used in this case.
*
* @param[in] label          - Enum label.
* @param[in] text_string    - String associated with the label.
*
* @return
*   None.
*
* @note
*
* @hideinitializer
*/
#define LOG_PLAT_EXPAND_AS_ENUM(label, text_string) label,


/**
* @brief
*   Expands the label and text_string input into a string.  The label will not
*   be used in this case.
*
* @param[in] label          - Enum label.
* @param[in] text_string    - String associated with the label.
*
* @return
*   None.
*
* @note
*
* @hideinitializer
*/
#define LOG_PLAT_EXPAND_AS_STRING(label, text_string) text_string,


/**
* @brief 
*   There are two versions of this macro.  The programmer sets
*   LOG_PLAT_USE_STRINGS to 1 to enable the ability to print strings to the HW
*   stdout, or the output buffer, or both.  By setting LOG_PLAT_USE_STRINGS to
*   0, Basecode will output a log code to the output buffer.
*  
*   If LOG_PLAT_USE_STRINGS == 1, the LOG_PLAT_APP_EVENT macro checks the
*   severity level of the event. If the severity is lower than the filter level,
*   logging will be skipped. The second severity check is for whether to print
*   to the hardware stdout or not. If yes, log_app_string_event() will be
*   called; otherwise, log_app_string_event_ccb_only() will be called to insert
*   the string into the log buffer.
*  
*   If LOG_PLAT_USE_STRINGS == 0, the LOG_PLAT_APP_EVENT calls the LOG_APP_EVENT
*   macro.  Refer to log_app_api.h for a description of the LOG_APP_EVENT macro.
*
* @param[in] event_sev          - Event severity, defined in log_sev_enum.
* @param[in] mod_id             - 16-bit module ID, one of PMCFW_MID_XXX.
* @param[in] log_str_array_ptr  - Array containing the module's log strings
* @param[in] log_code           - The unique code assigned to the log.  To be 
*                                 used as an index if using strings.
* @param[in] log_word0          - Generic 32-bit log word 0.
* @param[in] log_word1          - Generic 32-bit log word 1.
* @param[in] log_word2          - Generic 32-bit log word 2.
* @param[in] log_word3          - Generic 32-bit log word 3.
* @param[in] log_word4          - Generic 32-bit log word 4.
*
* @return
*   None.
*
* @note
*   This MUST be implemented as a macro, NOT an inline function. This ensures
*   that functions calls in parameters of the macro are also eliminated when
*   logging is disabled.
*
* @hideinitializer
*/
#if (LOG_PLAT_LOGGING_ENABLED == 0)
#define LOG_PLAT_APP_EVENT(event_sev,mod_id,log_str_array_ptr,log_code,log_word0,log_word1,log_word2,log_word3,log_word4)
#elif (LOG_PLAT_USE_STRINGS == 1)
#define LOG_PLAT_APP_EVENT(event_sev,mod_id,log_str_array_ptr,log_code,log_word0,log_word1,log_word2,log_word3,log_word4)   \
    do{\
            if ((event_sev) <= LOG_COMPILE_LEVEL)\
            {\
                if ((mod_id) < PMCFW_MOD_NUM)\
                {\
                    if (((event_sev) <= log_sev_filter_level_setting[mod_id]) && ((event_sev) > log_sev_disable))\
                    {\
                        if((event_sev) <= log_hw_sev_filter_level_setting[mod_id])\
                        {\
                            log_app_string_event(log_str_array_ptr[log_code], \
                                                 log_word0,\
                                                 log_word1,\
                                                 log_word2,\
                                                 log_word3,\
                                                 log_word4);\
                        }\
                        else\
                        {\
                            log_app_string_event_ccb_only(log_str_array_ptr[log_code], \
                                                          log_word0,\
                                                          log_word1,\
                                                          log_word2,\
                                                          log_word3,\
                                                          log_word4);\
                        }\
                    }\
                }\
            }\
        } while(0)
#else
#define LOG_PLAT_APP_EVENT(event_sev,mod_id,log_str_array_ptr,log_code,log_word0,log_word1,log_word2,log_word3,log_word4)   \
    LOG_APP_EVENT(event_sev,mod_id,log_code,log_word0,log_word1,log_word2,log_word3,log_word4)
#endif


/**
* @brief
*   Expands the label and text_string input into a string.  The label will not
*   be used in this case.
*
* @param[in] str_array  - Pointer to string array.
* @param[in] array_sz   - Number of strings in the string array.
*
* @return
*   None.
*
* @note
*
* @hideinitializer
*/
#if LOG_PLAT_USE_STRINGS == 1
#define LOG_PLAT_STR_LEN_CHECK(str_array, array_sz) \
    PMCFW_ASSERT(PMC_SUCCESS == log_app_string_len_check(str_array, array_sz), PMCFW_ERR_NUMBER_OUT_OF_RANGE)
#else
#define LOG_PLAT_STR_LEN_CHECK(str_array, array_sz)
#endif


/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN void log_mem_clear(UINT32 log_mem_size,
                          void  *log_mem_addr);
EXTERN VOID log_plat_init(VOID);
EXTERN PMCFW_ERROR log_spi_flash_store(VOID);
EXTERN VOID log_plat_ram_code_ptr_adjust(UINT32 offset);

#endif /* _LOG_PLAT_H */


