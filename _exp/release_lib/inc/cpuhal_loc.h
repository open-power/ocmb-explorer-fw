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
* @addtogroup CPUHAL
* @{ 
* @file 
* @brief
*       This file contains the local hardware-abstraction layer (M1004KHAL)
*       definitions.
*.
* @note 
*/ 

/*---- Compile Options -----------------------------------------------------*/
#ifndef _HAL_LOC_H
#define _HAL_LOC_H

/*---- Include Files -------------------------------------------------------*/
#include "cpuhal.h"
#include "cpuhal_plat_cfg.h"
#include "pmc_hw.h"
#include "pmc_plat.h"
#include "pmcfw_common.h"

/*---- Constant and Type Declarations --------------------------------------*/

/*---- Public Variable Declarations ----------------------------------------*/

/*---- Inline Function Declarations ----------------------------------------*/

/*---- Function Declarations -----------------------------------------------*/
void cpuhal_timer_isr(void *ignore);

#endif /* _HAL_LOC_H */
/** @} end addtogroup */



