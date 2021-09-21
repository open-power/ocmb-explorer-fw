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
*   DESCRIPTION :
*     Definitions and declarations for OPSW timers
*
*******************************************************************************/


#ifndef _OPSW_TIMER_H
#define _OPSW_TIMER_H

/*
** Include Files
*/


/*
** Enumerated Types 
*/


/*
** Constants 
*/

/* configuration values for timer frequency. */
#define OPSW_CPU_INTVL_TIMER_01HZ_TIMEBASE   0x0
#define OPSW_CPU_INTVL_TIMER_1HZ_TIMEBASE    0x01
#define OPSW_CPU_INTVL_TIMER_10HZ_TIMEBASE   0x02
#define OPSW_CPU_INTVL_TIMER_100_HZ_TIMEBASE 0x03
#define OPSW_CPU_INTVL_TIMER_1KHZ_TIMEBASE   0x04
#define OPSW_CPU_INTVL_TIMER_10KHZ_TIMEBASE  0x05
#define OPSW_CPU_INTVL_TIMER_100KHZ_TIMEBASE 0x06
#define OPSW_CPU_INTVL_TIMER_1MHZ_TIMEBASE   0x07


/*
** Macro Definitions
*/


/*
** Structures and Unions
*/


/*
** Global variables
*/


/*
** Function Prototypes
*/

EXTERN UINT32 opsw_timer0_read(VOID);
EXTERN VOID opsw_timer0_system_seconds_init(VOID);
EXTERN UINT32 opsw_timer0_freq_get(VOID);
EXTERN VOID opsw_timer1_temp_polling_init(VOID);
EXTERN VOID opsw_timer1_interval_update(UINT32 update_interval_ms);
EXTERN UINT32 opsw_timer2_read(VOID);
EXTERN UINT32 opsw_timer2_period_get(VOID);
EXTERN VOID opsw_timer2_reload(VOID);
EXTERN VOID opsw_timer2_vpe1_wdt_init(UINT32 timeout_ms);

#endif /* _OPSW_TIMER_H */




