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
*   DESCRIPTION :
*     Definitions and declarations for OPSW APIs
*
*******************************************************************************/


#ifndef _OPSW_API_H
#define _OPSW_API_H

/*
** Include Files
*/
#include "pmc_hw_base.h"


/*
** Enumerated Types 
*/
/**
*  @brief
*   OPSW Scratchpad ID
*/
typedef enum
{
    OPSW_SCRATCHPAD_0 = 0,      /**< S/PAD 0 */
    OPSW_SCRATCHPAD_1,          /**< S/PAD 1 */
    OPSW_SCRATCHPAD_2,          /**< S/PAD 2 */
    OPSW_SCRATCHPAD_3,          /**< S/PAD 3 */
} opsw_scratchpad_enum;


/*
** Constants 
*/

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

EXTERN void opsw_scratchpad_set(opsw_scratchpad_enum id, UINT32 value);
EXTERN UINT32 opsw_scratchpad_get(opsw_scratchpad_enum id);
EXTERN void opsw_timer1_init(UINT32 time_base);
EXTERN void opsw_timer1_interval_update(UINT32 update_interval_ms);
EXTERN void opsw_outbound_doorbell_set(void);



#endif /* _OPSW_API_H */




