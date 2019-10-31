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
* @defgroup SYS_TIMER SYS_TIMER: system timer
* @brief
*   This module provides time  functions to busy wait and
*   timeout.
*
*   A function to access the HW timer must be registers with this
*   module during system boot. The hardware timer can be 32 bits or
*   64 bits.
*
* @{
* @file
* @brief
*   Public API for system timer
*
* @note
*   These functions are used for small delays usually less than
*   1 millisecond. The os general timer should be used for
*   delays greater than 10 milliseconds. Don't use these functions for
*   greater than 1 second delay because of possible HW overflow issues.
*/
#ifndef _TMR_SYSTEM_API_H
#define _TMR_SYSTEM_API_H

#include "pmcfw_types.h"
#include "tmr_sys_plat.h"

#ifndef UINT_TIME
#error "UINT_TIME is not defined - this must be defined in platform-specific header file (tmr_sys_plat.h)"
#endif

/*
** Function Prototypes and Pointers to Functions in RAM
**
** To accommodate PIC code executing in SPI flash and non-PIC code executing in
** RAM, the functions in RAM are accessed through pointers. The changes that were made:
**
**      - original function name:     $type func_name($type, $type)
**        changed with prepended '_': $type _func_name($type, $type)
**      - define a typedef for the function pointer: typedef $type (_func_name_fn_ptr)($type, $type)
**      - public function pointers initialized in source code files:
**        PUBLIC _func_name_fn_ptr (*func_name_ptr) =  _func_name;
**      - private function pointers intitialized in source code files:
**        PRIVATE _func_name_fn_ptr (*func_name_ptr) = _func_name;
**      - for public functions new define for original function name in header file:
**        #define func_name (*func_name_ptr)
**      - for private functions new define for original function name in source code file:
**        #define func_name (*func_name_ptr)
*/
typedef void (*sys_timer_busy_wait_us_fn_ptr)(UINT32 wait_time_ns);
EXTERN sys_timer_busy_wait_us_fn_ptr sys_timer_busy_wait_us_ptr;
#define sys_timer_busy_wait_us (*sys_timer_busy_wait_us_ptr)

typedef void (*sys_timer_busy_wait_ns_fn_ptr)(UINT32 wait_time_ns);
EXTERN sys_timer_busy_wait_ns_fn_ptr sys_timer_busy_wait_ns_ptr;
#define sys_timer_busy_wait_ns (*sys_timer_busy_wait_ns_ptr)

typedef UINT_TIME (*sys_timer_us_to_count_fn_ptr)(UINT32 time_us);
EXTERN sys_timer_us_to_count_fn_ptr sys_timer_us_to_count_ptr;
#define sys_timer_us_to_count (*sys_timer_us_to_count_ptr)

typedef UINT_TIME (*sys_timer_count_to_us_fn_ptr)(UINT_TIME count);
EXTERN sys_timer_count_to_us_fn_ptr sys_timer_count_to_us_ptr;
#define sys_timer_count_to_us (*sys_timer_count_to_us_ptr)

typedef UINT_TIME (*sys_timer_count_to_ns_fn_ptr)(UINT_TIME count);
EXTERN sys_timer_count_to_ns_fn_ptr sys_timer_count_to_ns_ptr;
#define sys_timer_count_to_ns (*sys_timer_count_to_ns_ptr)

typedef UINT_TIME (*sys_timer_diff_fn_ptr)(UINT_TIME time1, UINT_TIME time2);
EXTERN sys_timer_diff_fn_ptr sys_timer_diff_ptr;
#define sys_timer_diff (*sys_timer_diff_ptr)

/*
** Function Protoypes
*/
typedef  UINT_TIME (*sys_timer_rd_fn_ptr)(void);

EXTERN BOOL sys_timer_wait_for_value_us(const UINT32 * const address, const UINT32 bit_mask, const UINT32 bit_pattern, const UINT32 timeout_us);
EXTERN BOOL sys_timer_wait_for_value_sec(const UINT32 * const address, const UINT32 bit_mask, const UINT32 bit_pattern, const UINT32 timeout_sec);

EXTERN sys_timer_rd_fn_ptr sys_timer_read;

/* macro to ensure if sys_timer_read64 is called, UINT_TIME is 64 bit */
#define COMPILE_TIME_ASSERT(condition) ((UINT_TIME)sizeof(char[1 - 2*!(condition)]))
#define sys_timer_read64() (sys_timer_read() * (COMPILE_TIME_ASSERT(sizeof(UINT_TIME) == sizeof(UINT64))))

EXTERN VOID sys_timer_ram_code_ptr_adjust(UINT32 offset);

#endif /* _TMR_SYSTEM_API_H */

/** @} end addtogroup */


