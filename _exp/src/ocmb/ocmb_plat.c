/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2018, 2019, 2020 Microchip Technology Inc. All rights reserved. 
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
#include "exp_opencapi_dump.h"
#include "crash_dump.h"
#include "ocmb_plat.h"
#include "top_plat.h"

/*
** Constants
*/

/*
** Empirically determined that the size is 0x22c6 bytes. 
** Therefore set this to 9KB for some room to grow in the future. 
*/
#define OCMB_REG_CRASH_DUMP_SIZE    (9*1024)

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

/**
* @brief
*   Register OCMB register dump with the crash dump module
*
* @return
*   None.
*
* @note
*
*/
PUBLIC void ocmb_plat_crash_dump_register(void)
{
    crash_dump_register(CRASH_DUMP_SET_0, "OCMB_REGS", &exp_opencapi_dump_debug_info, CRASH_DUMP_ASCII, OCMB_REG_CRASH_DUMP_SIZE);
}

/****************************************************************************
*
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
    top_plat_lock_struct lock_struct;

    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    /* Execute DLx config guide. */
    if (ocmb_cfg_DLx_config_FW(OCMB_REGS_BASE_ADDR) == FALSE)
    {
        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        return FALSE;
    }

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);

    return TRUE;
}

