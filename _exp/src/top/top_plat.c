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
#include "cpuhal.h"

/*
** Constants
*/

#define INT_SR_REG_INT_ENABLE_BIT   0x00000001

/*
** Local Variables
*/

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

/**
* @brief
*   Prepare to enter critical region. Disable so 64-bit OCMB
*   registers or SPI flash can be accessed without interference
*   from other VPEs or interrupts.
*
*   Disable interrupts and multi-VPE operation.
*  
* @param[out] lock_struct_ptr - Pointer to the lock structure.
*
* @return
*   None.
*
* @note
*
*/
PUBLIC void top_plat_critical_region_enter(top_plat_lock_struct * lock_struct_ptr)
{
    /* disable interrupts */
    lock_struct_ptr->int_status = hal_int_global_disable();

    /* disable VPE */
    lock_struct_ptr->vpe_status = hal_disable_mvpe();

    /* disable MTC */
    lock_struct_ptr->mtc_status = hal_disable_mtc();
}

/**
* @brief
*   Prepare to exit critical region so 64-bit OCMB registers or
*   SPI flash can be accessed without interference from other
*   VPEs or interrupts.
*
*   Enable interrupts and multi-VPE operation.
* 
* @param[out] lock_struct - Lock structure.
*
* @return
*   None.
*
* @note
*
*/
PUBLIC void top_plat_critical_region_exit(top_plat_lock_struct lock_struct)
{
    /* restore MTC */
    hal_restore_mtc(lock_struct.mtc_status);

    /* resume other VPE */
    hal_restore_mvpe(lock_struct.vpe_status);

    /* 
    ** if interrupts were previously enabled, re-enable interrupts 
    ** NOTE: do not use hal_int_global_restore() as it is not PIC compliant 
    */ 
    if (INT_SR_REG_INT_ENABLE_BIT == (lock_struct.int_status & INT_SR_REG_INT_ENABLE_BIT))
    {
        /* interrupts were previously enabled */

        /* re-enable interrupts */
        hal_int_global_enable();
    }
}

