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
* @addtogroup TOP_PLAT
* @{
* @file
* @brief
*   Platform specific TOP module functions
*
* @note
*/

/*
** Include Files
*/

#include "bc_printf.h"
#include "top_plat_cfg.h"
#include "top_plat.h"
#include "top.h"
#include <string.h>

/**
* @brief
*    returns the device id override
*
* @return
*    device id
*
* @note
*   Access to this routine is provide for debug logging purposes only.
*   Other modules are not expected to call this routine directly.
*   Instead, use the specific information routines below (or add a new
*   routine).
*
*/
PUBLIC PMCFW_ERROR top_device_id_override(UINT32* dev_id)
{
    /* Dummy function to so compiler doesn't complain about missing function */
    return PMC_SUCCESS;
}



