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
*     Platform specific header for DCSU subsystem
*******************************************************************************/



#ifndef _DCSU_PLAT_H
#define _DCSU_PLAT_H

/*
** Include Files
*/
#include "pmc_profile.h"
#include "dcsu_plat_cfg.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Clock signals on emulation platforms are hardwired to a fixed frequency.
** In addition, pboot will use fixed, default clock frequencies.
** The following DCSU_<clk_domain>_FIXED_DEFAULT_CLK_FREQ #defines should
** match the default clock frequencies on the platform.
*/ 
#if (DCSU_USE_FIXED_DEFAULT_FREQS == 1)

#define DCSU_CPU_FIXED_DEFAULT_CLK_FREQ     DCSU_400_MHZ
#define DCSU_SYS_FIXED_DEFAULT_CLK_FREQ     DCSU_400_MHZ
#define DCSU_DDR_FIXED_DEFAULT_CLK_FREQ     DCSU_800_MHZ
#define DCSU_SERIAL_FIXED_DEFAULT_CLK_FREQ  DCSU_300_MHZ
#define DCSU_ONFI_FIXED_DEFAULT_CLK_FREQ    DCSU_330_MHZ

#endif /* DCSU_USE_FIXED_DEFAULT_FREQS == 1 */

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


#endif /* _DCSU_PLAT_H */



