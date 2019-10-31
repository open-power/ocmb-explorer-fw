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
* @addtogroup EXP_PCSE_TIMER
* @{
* @file
* @brief
*   Explorer PCSE timer definitions and declarations.
*
* @note
*/
#ifndef _EXP_PCSE_TIMER_H
#define _EXP_PCSE_TIMER_H


/*
* Include Files
*/
#include "pmcfw_types.h"

/*
** Constants
*/

/*
* Macro Definitions
*/

/*
* Enumerated Types
*/

/*
* Structures and Unions
*/

/*
** Function Prototypes
*/

EXTERN UINT32 exp_timer0_read(VOID);
EXTERN VOID exp_timer0_init(VOID);
EXTERN UINT32 exp_timer0_freq_get(VOID);
EXTERN BOOL exp_timer0_wait_for_value_sec(const UINT32 * const address, const UINT32 bit_mask, const UINT32 bit_pattern, const UINT32 timeout_sec);

/*
** Extern variables
*/


#endif /* _EXP_PCSE_TIMER_H */
/** @} end addtogroup */



