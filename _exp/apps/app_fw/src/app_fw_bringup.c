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
*   DESCRIPTION : This file contains platform specific APIs needed by
*                 various other modules. For example, exp_plat_ram_size_get
*                 will return on-chip RAM size in bytes.
*
*
*   NOTES:        None.
*
*
*******************************************************************************/



/*
** Include Files
*/

#include "app_fw.h"
#include "top_exp_config_guide.h"
#include "pmc_hw_base.h"
#include "app_fw_ddr.h"
#include "serdes_plat.h"

/*
** Local Constants
*/

/*
** Public Variables  
*/

// volatile UINT32 my_loop = 0;

/*
** Local Variables
*/

/*
** Forward References
*/

/*
** Public Functions
*/

/**
* @brief
*  This function is used for Explorer FCA bring and mimics the
*  BOOT_CONFIG command.
*  
*
* @return
*   Nothing
*
* @note
*   None.
*
*/
PUBLIC void expl_fca_bringup(void)
{
    UINT32  rc;

//    bc_printf("Waiting in the Loop \n...");
//    while(my_loop==0);

#if (EXPLORER_PE_BUILD == 0)
    bc_printf("Configuring SerDes for 8 lanes, 21.33 GBPS, DFE = 1\n");
    /* For standard builds run serdes at 2133 GBPS */
    rc = serdes_plat_low_level_init(EXP_SERDES_8_LANE_BITMASK, EXP_SERDES_21_33_GBPS, 1);
#else
    bc_printf("Waiting for 8s...\n");
    /* For PE wait 8 seconds so their power supplies can come up */
    sys_timer_busy_wait_us(8000000);

    /* For PE run serdes at 2560 GBPS and only initialize lane 4 */
    rc = serdes_plat_low_level_init(0x10, EXP_SERDES_25_60_GBPS, 1);
#endif

    bc_printf("Serdes Error_code = %x\n", rc);

    /* Configure the DDLL. */
    if (TRUE == top_exp_cfg_DDLL(DDLL_REGS_BASE_ADDR, TOP_XCBI_BASE_ADDR))
    {
        bc_printf("expl_fca_bringup(): DLL clock locked\n");
    }
    else
    {
        bc_printf("expl_fca_bringup(): DLL clock did not lock\n");
    }

    /* Reset OCMB and DDR_PHY */
    top_exp_cfg_deassert_phy_ocmb_reset(TOP_XCBI_BASE_ADDR);

    /* Initialize DDR */
    app_fw_ddr_bringup_init();

    /* set hardware initialization flag, this leads to DI being enabled */
    app_fw_plat_hw_init_set(TRUE);

    /* Enable OC command handler POLLING loop as DLX*/
    app_fw_oc_ready = TRUE;
}

