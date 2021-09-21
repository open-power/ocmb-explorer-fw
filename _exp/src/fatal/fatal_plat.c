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
* @addtogroup FATAL_PLAT
* @{
* @file
* @brief
*   This file contains the FATAL platform specific functions.
*
*/ 


/*
** Include Files
*/
#include "pmcfw_common.h"
#include "cicint_api.h"
#include "exp_gic.h"
#include "fatal_error_handle.h"
#include "gpbce_fatal.h"
#include "spi_plat.h"
#include "ddr_phy_plat.h"
#include "top_plat.h"
#include "pcse_fatal.h"

/*
** Global Variables
*/

/*
** Local Structures and Unions
*/


/*
** Forward Reference
*/


/*
** Private Data
*/

/*
** Private Functions
*/

/*
** Public Functions
*/

/**
* @brief
*    Initializes the FATAL module for the Explorer platform.
*
* @return
*    Nothing
*
* @note
*
*/
PUBLIC void fatal_plat_init(void)
{

    /**
     * Initialize fatal and non-fatal interrupts for each hardware 
     * block. 
     */
    gpbce_fatal_init();
    pcse_irq_plat_fatal_init();
    top_non_fatal_init();
    top_fatal_init();
    spi_fatal_init();

    /**
     * Configure fatal interrupt handling for all required blocks.
     */
    cicint_int_register(FOXHOUND_FATAL_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void*)FOXHOUND_FATAL_INT);
    cicint_int_register(DDR4_PHY_FATAL_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void*)DDR4_PHY_FATAL_INT);
    cicint_int_register(GPBC_FATAL_ERROR_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void*)GPBC_FATAL_ERROR_INT);
    cicint_int_register(TOP_FATAL_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void*)TOP_FATAL_INT);
    cicint_int_register(PCSE_IRQ_0_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void*)PCSE_IRQ_0_INT);
    cicint_int_register(OPSW_CR_I_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void *)OPSW_CR_I_INT);
    cicint_int_register(OPSW_CR_I1_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void *)OPSW_CR_I1_INT);
    cicint_int_register(WDT_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void *)WDT_INT);
    cicint_int_register(TIMER_2_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void *)TIMER_2_INT);

    /**
     * Configure non-fatal interrupt handling for all required 
     * blocks. 
     */
    cicint_int_register(DDR4_PHY_NON_FATAL_INT, (cicint_cback_fcn_ptr)non_fatal_error_handler, (void*)DDR4_PHY_NON_FATAL_INT);
    cicint_int_register(TOP_NON_FATAL_INT, (cicint_cback_fcn_ptr)non_fatal_error_handler, (void*)TOP_NON_FATAL_INT);

    /**
     * Enable fatal interrupts.
     */
    cicint_int_enable(FOXHOUND_FATAL_INT);
    cicint_int_enable(DDR4_PHY_FATAL_INT);
    cicint_int_enable(GPBC_FATAL_ERROR_INT);
    cicint_int_enable(TOP_FATAL_INT);
    cicint_int_enable(PCSE_IRQ_0_INT);
    cicint_int_enable(OPSW_CR_I_INT);
    cicint_int_enable(OPSW_CR_I1_INT);
    cicint_int_enable(WDT_INT);
    cicint_int_enable(TIMER_2_INT);

    /**
     * Enable non-fatal interrupts.
     */
    cicint_int_enable(DDR4_PHY_NON_FATAL_INT);
    cicint_int_enable(TOP_NON_FATAL_INT);

    /**
     * Configure and enable for FAIL_n. 
     *  
     */
    cicint_int_register(FAIL_N_INT, (cicint_cback_fcn_ptr)fatal_error_handler, (void*)FAIL_N_INT);
    cicint_int_enable(FAIL_N_INT);
    

    return;

} /*END: ddr_phy_fatal_error_handler*/


/** @} end group */



