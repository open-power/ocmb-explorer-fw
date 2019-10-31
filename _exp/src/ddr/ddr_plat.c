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
* @addtogroup DDR
*
* @remarks
*   DDR Platform Functions
*
* @note
*
*/

/*
** Include Files
*/

#include "pmcfw_types.h"
#include "top.h"
#include "ddr_phy_plat.h"

/*
** Constants
*/

/*
** Macros
*/

/*
* Forward References
*/

/*
** Private Variables
*/

/*
** Public Variables
*/

/*
** Private Functions
*/

/*
** Public Functions
*/


/**
* @brief
*   Reset the DDR PHY from top level.
*
* @return
*   none
*
*/
PUBLIC void ddr_plat_phy_reset(void)
{
    ddr_phy_plat_initialized_set(FALSE);

    top_ddr_phy_reset(FALSE);

    ddr_phy_plat_initialized_set(TRUE);
}

/** @} end addtogroup */


