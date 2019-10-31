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
* @addtogroup SYS_TIMER
* @{ 
* @file 
* @brief
*   Public API for system timer functions
*
*   Refer to sys_timer_api.h for detailed of this module.
* @note 
*   None.
*/
#ifndef _TMR_SYS_H
#define _TMR_SYS_H

#include "pmcfw_types.h"
#include "tmr_sys_plat.h"
#include "tmr_sys_plat_cfg.h"
#if (SYS_TIMER_INT_SUPPORT == 1)
#include "cicint.h"
#endif
#include "sys_timer_api.h"

/*
*  These macros wrap the currently used timer hardware.  If alternate hardware
* is provided in the future, these need to change
*/
#define SYS_TIMER_OS_TICKS_PER_SECOND   100
#define TMR_ERR_INVALID_PARAMETER       (0x001)
#define TMR_ERR_REG_READ                (0x002)

typedef  UINT32    (*sys_timer_freq_get_fn_ptr)(void);
typedef  void      (*sys_timer_arm_fn_ptr)(UINT32);

#if (SYS_TIMER_INT_SUPPORT == 1)
EXTERN cicint_cback_fcn_ptr sys_timer_isr;
#endif

EXTERN void sys_timer_register(sys_timer_rd_fn_ptr rd_fn,
                               sys_timer_freq_get_fn_ptr freq_get_fn);

#if (SYS_TIMER_INT_SUPPORT == 1)
PUBLIC void sys_timer_isr_register(cicint_cback_fcn_ptr isr_fn,
                                   sys_timer_arm_fn_ptr arm_fn,
                                   UINT32 int_num,
                                   UINT32 int_is_local);

EXTERN void sys_timer_isr_init (UINT32 period_ms);
#endif

EXTERN sys_timer_freq_get_fn_ptr sys_timer_freq_get;

#endif

/** @} end addtogroup */


