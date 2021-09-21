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

/*
** Constants
*/

#define OCMB_REG_CRASH_DUMP_SIZE 0x80000

#define INT_SR_REG_INT_ENABLE_BIT   0x00000001

/*
** Local Variables
*/

/*
** Global Variables
*/

/* used for storing/restoring interrupt status */
PRIVATE UINT32 ocmb_plat_int_status;

/* used for storing/restoring MTC status */
PRIVATE UINT32 ocmb_plat_mtc_status;

/* used for storing/restoring VPE status */
PRIVATE UINT32 ocmb_plat_vpe_status;

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
    crash_dump_register("OCMB_REGS", &exp_opencapi_dump_debug_info, CRASH_DUMP_ASCII, OCMB_REG_CRASH_DUMP_SIZE);
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
    /* disable interrupts and disable multi-VPE operation */
    ocmb_plat_critical_region_enter();

    /* Execute DLx config guide. */
    if (ocmb_cfg_DLx_config_FW(OCMB_REGS_BASE_ADDR) == FALSE)
    {
        /* restore interrupts and enable multi-VPE operation */
        ocmb_plat_critical_region_exit();

        return FALSE;
    }

    /* restore interrupts and enable multi-VPE operation */
    ocmb_plat_critical_region_exit();

    return TRUE;
}

/**
* @brief
*   Prepare to enter critical region so 64-bit OCMB registers
*   can be accessed without interference from other VPEs or
*   interrupts.
*
*   Disable interrupts and multi-VPE operation.
*
* @return
*   None.
*
* @note
*
*/
PUBLIC void ocmb_plat_critical_region_enter(void)
{
    /* disable interrupts */
    ocmb_plat_int_status = hal_int_global_disable();

    /* disable VPE */
    ocmb_plat_vpe_status = hal_disable_mvpe();

    /* disable MTC */
    ocmb_plat_mtc_status = hal_disable_mtc();
}

/**
* @brief
*   Prepare to exit critical region so 64-bit OCMB registers can
*   be accessed without interference from other VPEs or
*   interrupts.
*
*   Enable interrupts and multi-VPE operation.
*
* @return
*   None.
*
* @note
*
*/
PUBLIC void ocmb_plat_critical_region_exit(void)
{
    /* restore MTC */
    hal_restore_mtc(ocmb_plat_mtc_status);

    /* resume other VPE */
    hal_restore_mvpe(ocmb_plat_vpe_status);

    /* 
    ** if interrupts were previously enabled, re-enable interrupts 
    ** NOTE: do not use hal_int_global_restore() as it is not PIC compliant 
    */ 
    if (INT_SR_REG_INT_ENABLE_BIT == (ocmb_plat_int_status & INT_SR_REG_INT_ENABLE_BIT))
    {
        /* interrupts were previously enabled */

        /* re-enable interrupts */
        hal_int_global_enable();
    }
}

