/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2020 Microchip Technology Inc. All rights reserved. 
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
*   DESCRIPTION : This file is the public module header file for the WDT.
*
*   NOTES:        None.
*
*******************************************************************************/

#ifndef _WDT_H
#define _WDT_H

/*
** Include Files
*/
#include "pmcfw_types.h"


/*
** Macro Definitions
*/

/**
 * This macro creates error codes for watchdog module.
 *
 * @param [in] err_suffix      The base error code
 *
 * @returns The complete error code, including module ID and error code.
 */
#define WDT_ERR_CODE_CREATE(err_suffix)     ((PMCFW_ERR_BASE_WDT) | (err_suffix))
#define WDT_ERR_PERIOD_EXCEED_MAX           WDT_ERR_CODE_CREATE(0x001)


/*
* Enumerated Types
*/


/*
** Global variables 
*/


/* 
** Function Prototypes
*/
EXTERN VOID wdt_hardware_tmr_isr(VOID *cback_arg);
EXTERN UINT32 wdt_hardware_tmr_period_get(VOID);
EXTERN UINT32 wdt_hardware_tmr_curr_cnt_read(VOID);
EXTERN VOID wdt_hardware_tmr_kick(VOID);
EXTERN VOID wdt_hardware_tmr_fatal_error_handler(VOID);
EXTERN VOID wdt_hardware_tmr_init(UINT32 timeout_ms);
EXTERN UINT32 wdt_interval_tmr_period_get(VOID);
EXTERN VOID wdt_interval_tmr_kick(VOID);
EXTERN BOOL wdt_interval_tmr_timeout(VOID);
EXTERN VOID wdt_interval_tmr_fatal_error_handler(VOID);
EXTERN VOID wdt_interval_tmr_init(UINT64 timeout_ms);

#endif /* _WDT_H */



