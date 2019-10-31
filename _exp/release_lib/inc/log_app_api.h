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
* @defgroup LOG_APP LOG_APP: Event logging
* @ingroup PlatformServicesModule
* @brief
*     This module implements event logging functions to be used by applications.
*
*     The application log entries are each 32-byte (8 32-bit word)
*     long in the following format:
*     <table>
*     <tr><th>Application log entries</th></tr>
*     <tr><td> 32-bit timestamp (upper 32-bit of 64-bit timestamp)  </td></tr>
*     <tr><td> 32-bit timestamp (lower 32-bit of 64-bit timestamp) </td></tr>
*     <tr><td> 32-bit log code (shall be in the format [16-bit module ID | 16-bit user defined field]) </td></tr>
*     <tr><td> 32-bit log word 0 </td></tr>
*     <tr><td> 32-bit log word 1 </td></tr>
*     <tr><td> 32-bit log word 2 </td></tr>
*     <tr><td> 32-bit log word 3 </td></tr>
*     <tr><td> 32-bit log word 4 </td></tr>
*     </table>
*
*     The user allocates a chunk of memory to be used by the application log and passes
*     that to the LOG module in log_app_init(). If the log memory size and address
*     meet expected criteria (see log_app_init() for details), the application log
*     is enabled.
*
*     log_app_init() creates a log header at the top of memory passed into the log_app_init(),
*     the remaining memory will be allocated for log array to store the log entries.
*     See log_cfg_struct definition for details of the log header.
*
*     User inserted entries are logged into the log based on its module's log severity
*     threshold configured by log_app_global_sev_filter_level_set() or
*     log_app_module_sev_filter_level_set().
*
*     Log entries include a 64-bit timestamp. If log_app_init() detects that a
*     previous log already exists in the given memory, it saves the timestamp of
*     the last log entry. This saved timestamp will be added as an offset to all
*     new log entries, which ensures that timestamps are consistently increasing
*     across all entries in the log.
*
*     The application log is stored as a circular buffer in the form of an array of
*     "log_app_entry_struct".  The LOG module maintains a write index for the
*     application log, but does not maintain a read index.  It is expected that
*     multiple users can be reading the application log, and each user should
*     maintain its own read index to the application log.
*
*     The log access rules are:
*       - Log entries are accessed by using the (write index).
*
*     ------------------
*     A normal sequence of starting up the LOG module's application log is as follows:
*     - log_app_init()  - Called at system startup. Responsible
*                         for setting up the application log and the
*                         log filters to their default settings:
*                       - Application severity filter level is set to
*                         disable logging to the application log.
*
*     If the application log is enabled, logging of the application log may be
*     enabled through:
*     - log_app_global_sev_filter_level_set()   - Sets the application severity
*                                                 filter level for all modules.
*     - log_app_module_sev_filter_level_set()   - Sets the application severity
*                                                 filter level for a module.

* @{
* @file
* @brief
*     This file contains the public logging (LOG) definitions.
*
*     User applications should include this file in order to use the LOG
*     services.
*
* @note
*     None.
*
*/

/*---- Compile Options -----------------------------------------------------*/
#ifndef _LOG_APP_API_H
#define _LOG_APP_API_H


/*---- Common Include Files ------------------------------------------------*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "log_plat_cfg.h"
#include "pmcfw_common.h"

/*---- Common Enumerations -------------------------------------------------*/

/**
* @brief
*   This enumeration specifies event severity levels.  The log_sev_disable
*   level allows logging to be disabled.
*/
typedef enum
{
    log_sev_disable,       /**< Disables logging. */
    log_sev_highest,       /**< Only filters in events of highest severity. */
    log_sev_high,          /**< Filters in events of high severity and up. */
    log_sev_medium,        /**< Filters in events of medium severity and up. */
    log_sev_low,           /**< Filters in events of low severity and up. */
    log_sev_lowest,        /**< Filters in events of lowest severity and up. */
    log_sev_num            /**< Maximum number of severity levels. Do NOT hard-code this number. */

} log_sev_enum;

/**
* @brief
*   This enumeration specifies cache operation level.
*/
typedef enum
{
    log_cache_ops_level_none,        /**< No cache operation */
    log_cache_ops_level_L1,          /**< Cache operation on L1 cache */
    log_cache_ops_level_L1_L2,       /**< Cache operation on L1 and L2 cache */
    log_cache_ops_level_num          /**< Maximum number of cache operation levels */

} log_cache_ops_level_enum;

/*---- Common Structures -----------------------------------------------------*/

/**
* @brief
*   Structure defines an application log entry.
*
* @note
*   The element order matches the application log format and shall NOT
*   be swapped.
*   Update the version of the LOG module if there are any changes to this structure.
*/
typedef struct
{
    UINT32 ts_u;        /**< Upper 32-bit of 64-bit timestamp. */
    UINT32 ts_l;        /**< Lower 32-bit of 64-bit timestamp. */
    UINT32 log_code;    /**<
                        * 32-bit log code, shall be in the format:
                        * [16-bit module ID | 16-bit user defined field]
                        */
    UINT32 log_word0;   /**< Generic 32-bit log word */
    UINT32 log_word1;   /**< Generic 32-bit log word */
    UINT32 log_word2;   /**< Generic 32-bit log word */
    UINT32 log_word3;   /**< Generic 32-bit log word */
    UINT32 log_word4;   /**< Generic 32-bit log word */
} log_app_entry_struct;


/**
* @brief
*   Structure defining the application log configuration.
*
** @note
*     Update the version of the LOG module if there are any changes to this structure.
*/
PMC_BEGIN_CACHEALIGN_STRUCT
typedef struct
{
    UINT32 signature_start;  /**<
                             * Must be 1st element in log_cfg_struct Start signature
                             * for the application log.  It is setup to be a fixed
                             * magic number used to positively identify the log header.
                             */

    UINT32 version;          /**<
                             * Version of the LOG module corresponding to the log
                             * entries. Must be 2nd element. Do not move.
                             */
    UINT32 size_in_entries;  /**< Size of application log in number of entries */
    UINT32 wr_idx;           /**< Current write index for the application log */
    UINT32 frequency_hz;     /**< Frequency for the 64-bit timestamp in Hertz */
    log_app_entry_struct *app_log_array;  /**< Application log (array of app log entries) */
    UINT16  header_size;     /**< Size of memory reserved for log header. */
    UINT16  wr_idx_wrap;     /**< index wrap around counter */

    UINT32 signature_end;    /**<
                             * Must be last element in log_cfg_struct. End signature
                             * for the application log. It is setup to be a fixed magic
                             * number used to positively identify the end of the log header
                             */
} log_cfg_struct;
PMC_END_CACHEALIGN_STRUCT

/*---- Common Typedefs -----------------------------------------------------*/

/*---- Common Macro Definition -----------------------------------------------------*/

/**
* @brief
*   This macro will be called by all modules' log macros. After being called by a log macro,
*   the LOG_APP_EVENT macro checks the severity level of the event. If the
*   severity is lower than required, logging will be skipped. Otherwise, log_app_event()
*   will be called to insert the event into the application log.
*
* @param[in] event_sev   - Event severity, defined in log_sev_enum.
* @param[in] mod_id      - 16-bit module ID, one of PMCFW_MID_XXX.
* @param[in] log_idx     - 16-bit log index.
* @param[in] log_word0   - Generic 32-bit log word 0.
* @param[in] log_word1   - Generic 32-bit log word 1.
* @param[in] log_word2   - Generic 32-bit log word 2.
* @param[in] log_word3   - Generic 32-bit log word 3.
* @param[in] log_word4   - Generic 32-bit log word 4.
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

#define LOG_APP_EVENT(event_sev,mod_id,log_idx,log_word0,log_word1,log_word2,log_word3,log_word4)   \
    do{\
            if ((event_sev) <= LOG_COMPILE_LEVEL)\
            {\
                if ((mod_id) < PMCFW_MOD_NUM)\
                {\
                    if (((event_sev) <= log_sev_filter_level_setting[mod_id]) && ((event_sev) > log_sev_disable))\
                    {\
                        log_app_event((((mod_id) << 16) | ((log_idx) & UINT16_MAX)),\
                                      log_word0,\
                                      log_word1,\
                                      log_word2,\
                                      log_word3,\
                                      log_word4);\
                    }\
                }\
            }\
        } while(0)

/*---- Include Files -------------------------------------------------------*/

/*---- Constant and Type Declarations --------------------------------------*/

/* LOG module signatures to be stored in the log_cfg_struct in memory */
#define LOG_SIGNATURE_START 0xa0a1a2a3
#define LOG_SIGNATURE_END   0xa4a5a6a7


/*---- Public Variable Declarations ----------------------------------------*/
/** Public variable used in macro function LOG_APP_EVENT, its size is defined
 * internally and user cannot modify. */
EXTERN log_sev_enum            log_sev_filter_level_setting[];
EXTERN log_sev_enum            log_hw_sev_filter_level_setting[];

/*---- Inline Function Declarations ----------------------------------------*/

/*---- Function Declarations -----------------------------------------------*/

/* Logging functions */
EXTERN void log_app_init(UINT32 log_mem_size,
                         void  *log_mem_addr,
                         log_cache_ops_level_enum cache_ops_level);

EXTERN void log_app_module_sev_filter_level_set(UINT32         mod_id,
                                                log_sev_enum   app_sev_filter_level);

EXTERN void log_app_global_sev_filter_level_set(log_sev_enum app_sev_filter_level);

EXTERN void log_app_module_hw_sev_filter_level_set(UINT32         mod_id,
                                                log_sev_enum   app_sev_filter_level);

EXTERN void log_app_global_hw_sev_filter_level_set(log_sev_enum app_sev_filter_level);

EXTERN log_sev_enum log_app_module_sev_filter_level_get(UINT32 mod_id);

EXTERN UINT32 log_app_info_get(void   **addr_pptr);

/*
* Although this function prototype is public, users should NOT use this function
* directly, but use LOG_APP_EVENT() macro instead
*/
EXTERN void log_app_event(UINT32            log_code,
                          UINT32            log_word0,
                          UINT32            log_word1,
                          UINT32            log_word2,
                          UINT32            log_word3,
                          UINT32            log_word4);

#endif /* _LOG_APP_API_H */

/** @} end addtogroup */


