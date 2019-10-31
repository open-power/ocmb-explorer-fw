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
*   DESCRIPTION: This file implements high level APIs to initialize
*   SerDes
*
*
*   NOTES:       None.
*
******************************************************************************/


/**
* @addtogroup SERDES
* @{
* @file
* @brief
*   This file contains high level APIs to initialize SerDes
*
*/ 


/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_api.h"
#include "ech.h"
#include "pmc_hw_base.h"
#include "top_exp_config_guide.h"
#include "serdes_config_guide.h"
#include <string.h>
#include "serdes_plat.h"
#include "app_fw.h"
#include "crash_dump.h"
#include "serdes_dump.h"
#include "sys_timer_api.h"
#include "pmc_profile.h"
#include "serdes_api.h"
#include "ocmb_config_guide.h"



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

#define SERDES_REG_CRASH_DUMP_SIZE 0x80000

PRIVATE BOOL serdes_initialized = FALSE;

/*
** Private Functions
*/

/**
* @brief
*   Low Level SerDes init sequence 1
*
* @param [in] lane: lane being configured
* @param [in] frequency: SerDes frequency
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PRIVATE UINT32 serdes_plat_low_level_init_sequence_1(UINT32 lane, UINT32 frequency)
{
    BOOL rc;

    /* set the offset for the lane being configured */
    UINT32 lane_offset = lane * SERDES_LANE_REG_OFFSET;

    /* initialize termination on the lane */
    SERDES_FH_fw_init(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);

    /* Do CSU_init_1 */
    switch (frequency)
    {
        case EXP_SERDES_21_33_GBPS: 
        {
            rc = SERDES_FH_CSU_init_1_21g328(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
        }
        break;
    
        case EXP_SERDES_23_46_GBPS:
        {
            rc = SERDES_FH_CSU_init_1_23g46(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
        }
        break;
    
        case EXP_SERDES_25_60_GBPS:
        {
            rc = SERDES_FH_CSU_init_1_25g6(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
        }
        break;
    
        default:
        {
            bc_printf("[%d] SERDES_FH_CSU_init_1_2XgXX FAILED, invalid frequency\n", lane);
            return EXP_SERDES_TRAINING_FREQ_UNSUPPORTED;
        }
    }

    if (TRUE != rc)
    {
        /* CSU_1 initialization failed */
        bc_printf("[%d] SERDES_FH_CSU_init_1_2XgXX FAILED\n", lane);
        return EXP_SERDES_TRAINING_CSU_FAILED;
    }

    switch (frequency)
    {
        case EXP_SERDES_21_33_GBPS: 
        {
            rc = SERDES_FH_CSU_init_2_21g328(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
        }
        break;

        case EXP_SERDES_23_46_GBPS:
        {
            rc = SERDES_FH_CSU_init_2_23g46(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
        }
        break;

        case EXP_SERDES_25_60_GBPS:
        {
            rc = SERDES_FH_CSU_init_2_25g6(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
        }
        break;

        default:
        {
            bc_printf("[%d] SERDES_FH_CSU_init_1_2XgXX FAILED, invalid frequency\n", lane);
            return EXP_SERDES_TRAINING_FREQ_UNSUPPORTED;
        }
    }

    if (TRUE != rc)
    {
        /* CSU_2 initialization failed */
        bc_printf("[%d] SERDES_FH_CSU_init_2_2XgXXX FAILED\n", lane);
        return EXP_SERDES_TRAINING_CSU_FAILED;
    }
    return (PMC_SUCCESS);
}

/**
* @brief
*   Low Level SerDes init sequence 2
*
* @param [in] lane: lane being configured
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled 
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PRIVATE UINT32 serdes_plat_low_level_init_sequence_2(UINT32 lane, BOOL dfe_state)
{
    UINT32 rc;
    UINT32 tap_sel, udfe_mode;

    /* set the offset for the lane being configured */
    UINT32 lane_offset = lane * SERDES_LANE_REG_OFFSET;

    /* start and monitor TxRx lane calibration */
    rc = SERDES_FH_TXRX_Calibration1((SERDES_MDSP_PCBI_BASE_ADDR + lane_offset),
                                     (SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                     (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset));
        
    if (TRUE != rc)
    {
        bc_printf("[%d] EXP_SERDES_TRAINING_CALIB_FAILED\n", lane);
        return EXP_SERDES_TRAINING_CALIB_FAILED;
    }
    else
    {
        bc_printf("[%d] EXP_SERDES_TRAINING_CALIB_PASSED\n", lane);
    }

    /* initialize PGA */        
    SERDES_FH_PGA_init(SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);

    /* set up the near end transmitter settings */
    SERDES_FH_NETX_Settings_default(SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);

    /**
     * From the config guide: 
     *  
     *  SERDES_FH_TXRX_datapath:
     *  ------------------------
     *  $TAP_SEL: 0x6 = No DFE, 0x7 = 1 tap DFE
     *  $UDFE_MODE: 0x0 = No DFE, 0x1 = 1 tap DFE
     *  
     *  SERDES_FH_TXRX_Adaptation:
     *  --------------------------
     *  $OBJ_EN_PASS3: With DFE = 0xa6c, No DFE = 0x80c
     *  $OBJ_EN_PASS11: With DFE = 0xa48, No DFE = 0x808
     *  
     */
    if (dfe_state)
    {
        /* DEFAULT is with DFE enabled */

        /*
        * SERDES_FH_TXRX_datapath
        */
        tap_sel         = 0x7;
        udfe_mode       = 0x1;
    }
    else
    {
        /*
        * SERDES_FH_TXRX_datapath
        */
        tap_sel         = 0x6;
        udfe_mode       = 0x0;
    }

    /* set up the datapath for the desired rate */
    SERDES_FH_TXRX_datapath((SERDES_ADSP_PCBI_BASE_ADDR + lane_offset),
                            (SERDES_MDSP_PCBI_BASE_ADDR + lane_offset),
                            (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                            (SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                            tap_sel,
                            udfe_mode);

    return (PMC_SUCCESS);
}

/*
** Public Functions
*/

/**
* @brief
*   Low Level SerDes adapt step 1
*
* @param [in] dfe_state:   TRUE=DFE enabled; FALSE=DFE disabled 
* @param [in] adapt_state: TRUE=Adaptation enabled; FALSE=Adaptation disabled 
* @param [in] lanes: Number of lanes being configured
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_adapt_step1(BOOL dfe_state, BOOL adpt_state, UINT32 lanes)
{
    UINT32 i;
    UINT32 lane_offset  = 0;
    BOOL rc;
   
    for(i= 0; i < lanes; i++)
    {
        /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
      
        if( (FALSE == dfe_state)  && (FALSE == adpt_state) )
        {
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable \n",i);
            rc = SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                               SERDES_MDSP_PCBI_BASE_ADDR + lane_offset,
                                               SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
        }
            
        else if( (FALSE == dfe_state)  && (TRUE == adpt_state) )
        {
            
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_dfe_disable \n",i);
            rc = SERDES_FH_TXRX_Adaptation1_FW_dfe_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MDSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
        }

        else if( (TRUE == dfe_state)  && (FALSE == adpt_state) )
        {
            
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable \n",i);
            rc = SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MDSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
        }
        else
        {
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_normal \n",i);
            rc = SERDES_FH_TXRX_Adaptation1_FW_normal(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MDSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
        }
        if (TRUE != rc)
        {
            bc_printf("[%d] EXP_SERDES_TRAINING_ADAPT1_FAILED\n", i);
            return EXP_SERDES_TRAINING_ADAPT1_FAILED;
        }
        else
        {
            bc_printf("[%d] EXP_SERDES_TRAINING_ADAPT_PASSED (step 1)\n", i);
        }
    }
    return (PMC_SUCCESS);

}

/**
* @brief
*   Low Level SerDes adaptation step 2
*  
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled 
* @param [in] adapt_state: TRUE=Adaptation enabled; FALSE=Adaptation disabled 
* @param [in] lanes: Number of lanes being configured
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_adapt_step2(BOOL dfe_state , BOOL adpt_state, UINT32 lanes)
{
    UINT32  i;  
    BOOL    rc;
    UINT32  lane_offset  = 0;
    UINT32  reg_val;

    /*
    **
    ** Query and report ADAPT_DONE_V0 for informational purposes.
    */
    for(i= 0; i < lanes; i++)
    {
        /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;

        reg_val = serdes_api_adpt_done_get(lane_offset);
        
        if (reg_val == 1)
        {
            bc_printf("[ADAPT_DONE_V0] Success for lane %d.\n", i);
        }
        else
        {
            bc_printf("[ADAPT_DONE_V0] FAILURE for lane %d.\n", i);
        }
    }

    /* Apply SERDES_FH_TXRX_Adaptation2 to the all of the lanes. */          
    for(i= 0; i < lanes; i++)
    {
        /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;

        if( (FALSE == dfe_state)  && (FALSE == adpt_state) )
        {
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation2_adaptation_disable \n",i);
            rc = SERDES_FH_TXRX_Adaptation2_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                        SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
        }
            
        else if( (FALSE == dfe_state)  && (TRUE == adpt_state) )
        {
            
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation2_dfe_disable \n",i);
            rc = SERDES_FH_TXRX_Adaptation2_dfe_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                        SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
        }

        else if( (TRUE == dfe_state)  && (FALSE == adpt_state) )
        {
            
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation2_adaptation_disable \n",i);
            rc = SERDES_FH_TXRX_Adaptation2_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                    SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);

        }
        else
        {
            bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation2_normal \n",i);
            rc =SERDES_FH_TXRX_Adaptation2_normal(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                        SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
        }
        
        if (TRUE != rc)
        {
            bc_printf("[%d] EXP_SERDES_TRAINING_ADAPT2_FAILED (step 2)\n", i);
            return EXP_SERDES_TRAINING_ADAPT2_FAILED;
        }
        else
        {
            bc_printf("[%d] EXP_SERDES_TRAINING_ADAPT2_PASSED (step 2)\n", i);
        }
    }
    return (PMC_SUCCESS);

}


/**
* @brief
*   Low Level SerDes init routine
*
* @param [in] lanes: Number of Serdes lanes, which will be initialized
* @param [in] frequency: SerDes frequency
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled 
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_low_level_init(UINT32 lanes, 
                                         UINT32 frequency,
                                         BOOL dfe_state)
{
    UINT32 i;
    UINT32 rc;
    UINT32 lane_offset = 0;

    /* deassert serdes reset */
    top_exp_cfg_deassert_serdes_reset(TOP_XCBI_BASE_ADDR);

    serdes_plat_initialized_set(TRUE);

    for (i = 0; i < lanes; i++)
    {
        
        bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_1\n", i);
        /* apply the first initialization sequence on all lanes */
        rc = serdes_plat_low_level_init_sequence_1(i, frequency);

        if (PMC_SUCCESS != rc)
        {
            return (rc);
        }
    }

    /* apply the second initialization sequence on all lanes */
    for (i = 0; i < lanes; i++)
    {
            
        bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_2\n", i);
        rc = serdes_plat_low_level_init_sequence_2(i, dfe_state);
        if (PMC_SUCCESS != rc)
        {
            bc_printf("[%d] serdes_plat_low_level_init_sequence_2 failed", i);
            return (rc);
        }
    }
    
    bc_printf("TWI_BOOT_CONFIG: DDLL Init Start\n");

    /* Configure the DDLL. */
    rc = top_exp_cfg_DDLL(DDLL_REGS_BASE_ADDR, TOP_XCBI_BASE_ADDR);        

    if( rc == FALSE)
    {
        bc_printf("TWI_BOOT_CONFIG: DDLL Lock error \n");
        return EXP_SERDES_DDLL_LOCK_FAIL;
        
    }
    bc_printf("TWI_BOOT_CONFIG: DDLL Init End\n");

    
    bc_printf("Deasserting OCMB...\n");
    
    /* apply top-level OCMB PHY reset */
    if (FALSE == top_exp_cfg_deassert_phy_ocmb_reset(TOP_XCBI_BASE_ADDR))
    {
        bc_printf("Deasserting OCMB...FAILED\n");
        /* 
        ** OCMB reset deassertion failed 
        */            
        return EXP_SERDES_DEASSERT_PHY_OCMB_RESET_FAIL;
    }
    
    bc_printf("Deasserting OCMB...DONE\n");
    
    /* initialize lane alignment part 1 to all lanes*/
    for (i = 0; i < lanes; i++)
    {
       /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
        rc = SERDES_FH_alignment_init_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                               (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset), TRUE);
        if (TRUE != rc)
        {
            bc_printf("[%d] EXP_SERDES_FH_ALIGNMENT1_FAILED\n", i);
            return EXP_SERDES_FH_ALIGNMENT_INIT_1_FAIL;
        }
    }
    
    for (i = 0; i < lanes; i++)
    {
       /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
        /* initialize lane alignment part 2 on all lanes except master lane 4 */
        if (i != 4)
        {
            rc = SERDES_FH_TX_alignment((SERDES_CSU_PCBI_BASE_ADDR + lane_offset) , (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset));
            if (TRUE != rc)
            {
                bc_printf("[%d] EXP_SERDES_FH_TX_ALIGNMENT_FAIL\n", i);
                return EXP_SERDES_FH_TX_ALIGNMENT_FAIL;
            }
        }
    }
    return (PMC_SUCCESS);
}

/**
* @brief
*   FW sequence to support lane inversion 
*
* @param [in] lanes: Number of Serdes lanes, which will be initialized
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32  serdes_plat_lane_inversion_config(UINT32 lanes)
{   
    UINT32 lane_offset  = 0;    
    UINT32 i;    
    BOOL rc;

    bc_printf(" serdes_plat_lane_inversion_config: Sending Pattern A ..\n");
    
    /* Send Pattern A */
    ocmb_cfg_SendPatA(OCMB_REGS_BASE_ADDR);
    /* Making sure that HOST has enough time to detect Pattern A */
    sys_timer_busy_wait_us(10000);

    bc_printf(" serdes_plat_lane_inversion_config: Running SERDES_FH_RX_alignment ..\n");
    for (i = 0; i < lanes; i++)
    {
        /* Run FH RX alignment */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
        rc = SERDES_FH_RX_alignment((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset));        
        if (TRUE != rc)
        {
            bc_printf("[%d] SERDES_FH_RX_alignment Error!!!\n", i);
            return EXP_TWI_BOOT_CFG_SERDES_FH_RX_ALIGNMENT_FAIL;
        }
    }

    
    bc_printf(" serdes_plat_lane_inversion_config: Sending Pattern B to HOST ..\n");
    /* Send Pattern B back to the HOST*/
    ocmb_cfg_SendPatB(OCMB_REGS_BASE_ADDR);      
    /* Making sure that HOST has enough time to detect Pattern B */
    sys_timer_busy_wait_us(10000);

    bc_printf(" serdes_plat_lane_inversion_config: waiting for Rx Pattern B ..\n");
    /* Wait for Pattern B*/
    rc = ocmb_cfg_RxPatB(OCMB_REGS_BASE_ADDR);
    if (TRUE != rc)
    {    
        bc_printf(" serdes_plat_lane_inversion_config:ocmb_cfg_RxPatB Timeout\n");
        return EXP_TWI_BOOT_CFG_SERDES_RX_PATB_TIMEOUT_FAIL;
    }
    
    return (PMC_SUCCESS);
}


/**
* @brief
*   Low Level SerDes standalone init routine for Loopback test
*
* @param [in] lanes: Number of Serdes lanes, which will be initialized
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled 
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
*   Assumption is SerDes will be put in external loopback mode
*/
PUBLIC UINT32 serdes_plat_low_level_standalone_init(UINT32 lanes, BOOL dfe_state)
{
    UINT32 lane_offset  = 0;
    UINT32 obj_en_pass3;
    UINT32 i;    
    UINT8 patt[10];
    UINT32 rc;
    memset(&patt[0], 0, sizeof(patt));

    /* enable PRBS generation across supported lanes */
    for (i = 0; i < lanes; i++)
    {
        /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
    
        rc= SERDES_FH_pattgen_en((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                             ECH_PQM_PRBS_PATTERN_23,
                             &patt[0]);
        if (TRUE !=rc)
        {
            bc_printf("[%d] EXP_SERDES_FH_PATTERNGEN_ENABLE_FAIL \n", i);
            return EXP_SERDES_FH_PATTERNGEN_ENABLE_FAIL;
        }
    }    
    if (dfe_state)
    {
        /* DFE enabled. */
        obj_en_pass3 = 0xa6c;
    }
    else
    {
        /* DFE disabled. */
        obj_en_pass3 = 0x80c;
    }
        
    /* Start adaption for lane 4 only.*/
    rc = SERDES_FH_TXRX_Adaptation1_PE(SERDES_ADSP_PCBI_BASE_ADDR + 4 * SERDES_LANE_REG_OFFSET,
                                       SERDES_MDSP_PCBI_BASE_ADDR + 4 * SERDES_LANE_REG_OFFSET,
                                       SERDES_MTSB_CTRL_PCBI_BASE_ADDR + 4 * SERDES_LANE_REG_OFFSET,
                                       obj_en_pass3);
    if (TRUE != rc)
    {
        bc_printf("[%d] SERDES_FH_TXRX_ADAPTATION_1_PE_FAIL \n", 4);
        return SERDES_FH_TXRX_ADAPTATION_1_PE_FAIL;
    }
    else
    {
        bc_printf("[%d] SERDES_FH_TXRX_ADAPTATION_1_PE_PASSED (step 1)\n", 4);
    }

    for(i=0; i < SERDES_LANES; i++)
    {
        if(i == 4)
        {
            bc_printf("Skipping lane 4\n");
            continue;
        }
        bc_printf("Lane ID= %d\n",i);

        /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
        
        rc = SERDES_FH_TXRX_Adaptation1_PE(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MDSP_PCBI_BASE_ADDR + lane_offset,
                                           SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset,
                                           obj_en_pass3);
        if (TRUE != rc)
        {
            bc_printf("[%d] SERDES_FH_TXRX_ADAPTATION_1_PE_FAIL (step 1)\n", i);
            return SERDES_FH_TXRX_ADAPTATION_1_PE_FAIL;
        }
        else
        {
            bc_printf("[%d] SERDES_FH_TXRX_ADAPTATION_1_PE_PASSED (step 1)\n", i);
        }
    }

    return PMC_SUCCESS;

}

/**
* @brief
*  C function for SerDes Loopback test
*
* @param [in] lanes: Number of Serdes lanes, which will be initialized
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
*   Assumption is SerDes will be put in external loopback mode
*/
PUBLIC UINT8 serdes_plat_loopback_test(UINT32 lanes)
{
    UINT32 i;    
    UINT32 lane_offset  = 0;    
    UINT8 patt[10];
    BOOL rc;
    UINT8 return_code = PMC_SUCCESS;

    /* Initialize PRBS user pattern array to 0 */
    memset(&patt[0], 0, sizeof(patt));
    /* enable PRBS monitoring across supported lanes */
    for (i = 0; i < lanes; i++)
    {
        /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
    
        rc = SERDES_FH_pattmon_en((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                             ECH_PQM_PRBS_PATTERN_23,
                             &patt[0]);
        if (TRUE !=rc)
        {
            bc_printf("[%d] EXP_SERDES_FH_PATTERNMON_ENABLE_FAIL \n", i);
            return EXP_SERDES_FH_PATTERNMON_ENABLE_FAIL;
        }
    }

    /*
    ** PRBS-23 pattern has been enabled in serdes_plat_low_level_standalone_init.
    ** In this function, pattern monitor has been enabled first. SERDES_FH_pattmon is called
    ** to check that pattern is recived or not.
    */
    for (i = 0; i < lanes; i++)
    {
        /* set the offset for the lane being configured */
        lane_offset = i * SERDES_LANE_REG_OFFSET;
        /* Read first time to clear any error*/
        SERDES_FH_pattmon(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);  
        /* Wait 1 ms*/
        sys_timer_busy_wait_us(1000);
        /* Read second time to get actual data*/
        rc = SERDES_FH_pattmon(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);
        if (rc == FALSE)
        {
            bc_printf("Serdes Loopback test failed for lane = %d\n", i);
            return_code = EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_LANE_ID_FAIL;
        }
        else
        {
            bc_printf("Serdes Loopback test PASSED for lane = %d\n", i);
        }
    }    
    return return_code;
}

#if (EXPLORER_PE_BUILD == 1)
/**
* @brief
*   Low Level SerDes init routine to initialize a single serdes lane
*
* @param [in] lane: A single lane to initialize
* @param [in] frequency: SerDes frequency
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled
*
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
*
* @note
*
*/
PUBLIC UINT32 serdes_plat_low_level_single_lane_init(UINT32 lane,
                                                     UINT32 frequency,
                                                     BOOL dfe_state)
{
    BOOL    rc;
    UINT32  lane_offset;

    /* deassert serdes reset */
    top_exp_cfg_deassert_serdes_reset(TOP_XCBI_BASE_ADDR);

    /* Run initial config guide initialization. */

    /* set the offset for the lane being configured */
    lane_offset = lane * SERDES_LANE_REG_OFFSET;

    /* initialize termination on supported lanes */
    SERDES_FH_fw_init(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);

    
    bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_1\n", lane);
    /* apply the first initialization sequence on all lanes */
    rc = serdes_plat_low_level_init_sequence_1(lane, frequency);

    if (PMC_SUCCESS != rc)
    {
        return (rc);
    }
            
    bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_2\n", lane);;
    
    rc = serdes_plat_low_level_init_sequence_2(lane, dfe_state);
    if (PMC_SUCCESS != rc)
    {
        bc_printf("[%d] serdes_plat_low_level_init_sequence_2 failed", lane);
        return (rc);
    }
    
    bc_printf("TWI_BOOT_CONFIG: DDLL Init Start\n");

    /* Configure the DDLL. */
    rc = top_exp_cfg_DDLL(DDLL_REGS_BASE_ADDR, TOP_XCBI_BASE_ADDR);        

    if( rc == FALSE)
    {
        bc_printf("TWI_BOOT_CONFIG: DDLL Lock error \n");
        return EXP_SERDES_DDLL_LOCK_FAIL;
        
    }
    bc_printf("TWI_BOOT_CONFIG: DDLL Init End\n");

    
    bc_printf("Deasserting OCMB...\n");
    
    /* apply top-level OCMB PHY reset */
    if (FALSE == top_exp_cfg_deassert_phy_ocmb_reset(TOP_XCBI_BASE_ADDR))
    {
        bc_printf("Deasserting OCMB...FAILED\n");
        /* 
        ** OCMB reset deassertion failed 
        */            
        return EXP_SERDES_DEASSERT_PHY_OCMB_RESET_FAIL;
    }
    
    bc_printf("Deasserting OCMB...DONE\n");
    
    /* initialize lane alignment part 1 to all lanes*/
   /* set the offset for the lane being configured */
    lane_offset = lane * SERDES_LANE_REG_OFFSET;
    rc = SERDES_FH_alignment_init_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                           (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset), TRUE);
    if (TRUE != rc)
    {
        bc_printf("[%d] EXP_SERDES_FH_ALIGNMENT1_FAILED\n", lane);
        return EXP_SERDES_FH_ALIGNMENT_INIT_1_FAIL;
    }
    
    /* initialize lane alignment part 2 on all lanes except master lane 4 */
    if (lane != 4)
    {
        
        /* set the offset for the lane being configured */
        lane_offset = lane * SERDES_LANE_REG_OFFSET;
        rc = SERDES_FH_TX_alignment((SERDES_CSU_PCBI_BASE_ADDR + lane_offset) , (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset));        
        if (TRUE != rc)
        {
            bc_printf("[%d] EXP_SERDES_FH_TX_ALIGNMENT_FAIL\n", lane);
            return EXP_SERDES_FH_TX_ALIGNMENT_FAIL;
        }
    }


    return PMC_SUCCESS;
}
#endif

/**
* @brief
*   Register SERDES register dump with the crash dump module
*
* @return
*   None.
*
* @note
*
*/
PUBLIC VOID serdes_plat_crash_dump_register(VOID)
{
    crash_dump_register("SERDES_REGS", &serdes_dump_debug_info, CRASH_DUMP_ASCII, SERDES_REG_CRASH_DUMP_SIZE);
}

/**
* @brief
*    Set SERDES initialization state.
*
* @param[in] is_intialized - Update SERDES module to initialized or uninitialized state.
*
* @return
*   None.
*
* @note
*   We must keep track of the initialized state of the SERDES module so we do not
*   try to dump registers when the SERDES is in reset.
*
*/
PUBLIC VOID serdes_plat_initialized_set(BOOL is_initialized)
{
    serdes_initialized = is_initialized;
}

/**
* @brief
*    Get SERDES initialization state.
*
* @return
*   TRUE if SERDES is initialized.
*
* @note
*
*/
PUBLIC BOOL serdes_plat_initialized_get(VOID)
{
    return serdes_initialized;
}

/** @} end group */
