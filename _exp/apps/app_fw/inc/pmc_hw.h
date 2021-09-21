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
*  DESCRIPTION :
*    Hardware definitions for the Explorer platform. Each platform
*    has their own copy of pmc_hw.h  These definitions are low level
*    enough that almost all modules include pmc_hw.h. IMPORTANT: This
*    header must not contain any typedefs or C constructs as it
*    may be included by assembly files.
*
*    In contrast, pmc_plat.h contains higher level platform definitions
*    not necessarily generic enough to be included by all modules.n
*    pmc_plat.h CAN contain typedefs and C constructs.
*
*******************************************************************************/


#ifndef _PMC_HW_H
#define _PMC_HW_H

/*
** Include Files
*/
#include "pmc_hw_base.h"

/*
** Constants
*/

#define PMC_MAX_VPE_PER_CPU                  (2)
#define NUM_CPUS                             (1)
#define PMC_MAX_TC_PER_CPU                   (2)

/* UART Definitions =========================================================*/
#define UART_COUNT                           (2)

/* Serial EEPROM Definitions ================================================*/
#define BOOTSTRAP_SEEPROM_PORT               (0)

#endif /* define _PMC_HW_H */



