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
* @addtogroup ocmb
* @{
* @file
* @brief
*   OCMB library
*
* @note
*/

#include "top_api.h"
#include "ocmb_config_guide.h"
#include "target_platform.h"

/*
** Constants
*/

/*
** Local Variables
*/

/*
** Global Variables
*/

/*
** Local Functions
*/

/*
** Public Functions
*/

/****************************************************************************
*


    if(ocmb_cfg_DLx_config_FW(OCMB_REGS_BASE_ADDR) == FALSE)
    {
        bc_printf("WZ: DLx config timed out\n");
    }
* FUNCTION: ocmb_init
* __________________________________________________________________________
*
* DESCRIPTION:
*   Initialize OCMB.
*
* INPUTS:
*   None.
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   TRUE if successful.
*
* NOTES:
*   None.
*
*****************************************************************************/
PUBLIC BOOL ocmb_init(void)
{
    /* Execute DLx config guide. */
    if (ocmb_cfg_DLx_config_FW(OCMB_REGS_BASE_ADDR) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}


