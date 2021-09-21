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
* @addtogroup ECH
* @{
* @file
* @brief
*   Explorer BOOT CONFIG Command Handler.
*
* @note
*/

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_api.h"
#include "pmc_profile.h"
#include "ech.h"
#include "ech_loc.h"
#include "mem.h"
#include "crc32.h"
#include "twi.h"
#include "twi_api.h"
#include "app_fw.h"
#include "top_exp_config_guide.h"
#include "ocmb_config_guide.h"
#include "ocmb_config_guide_mchp.h"
#include "serdes_plat.h"
#include "pmc_plat.h"

/*
* Local Enumerated Types
*/


/*
** Local Constants
*/
#define ECH_OCMB_TRAINING_COMPLETE          0x00100000
#define ECH_OCMB_TRAINING_SM_MASK           0x00007000
#define ECH_OCMB_TRAINING_SM_COMPLETE       0x00007000
#define ECH_OCMB_TRAINING_DELAY_500_US      500
#define ECH_REG_64_BIT_MASK                 0xFF000000
#define ECH_REG_64_BIT_ADDRESS              0xA8000000



/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/

EXTERN VOID ech_twi_pqm_set(BOOL setting);


/*
** Global Variables
*/


/*
** Local Variables
*/


/*
* Private Functions
*/

/*
* Public Functions
*/
                                                

/**
* @brief
*   Process TWI boot config command.
*
* @param
*   rx_buf[in] - received data to process
*   rx_index[in] - index in buffer of start of received command
*
* @return
*   EXP_TWI_SUCCESS for SUCCESS, otherwise error code
*
* @note
*/
PUBLIC UINT32 ech_twi_boot_config_proc(UINT8* rx_buf, UINT32 rx_index)
{

    UINT32 rc;
    UINT32 cfg_flags = (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] << 8) | rx_buf[EXP_TWI_CMD_DATA_OFFSET + 3];
    UINT8 ech_twi_extended_status = EXP_TWI_SUCCESS;

    /* 
    ** Do not increment the receive buffer index as
    ** this command is handled from VPE 0 as a deferred processing
    ** command.
    **
    */

    /* process the boot config command */


    /* record the raw boot config flags */
    ech_raw_boot_cfg_set(cfg_flags);

    bc_printf("TWI_BOOT_CONFIG:Received BOOT_CFG_FLAGS = 0x%x\n", cfg_flags);
    
#if EXPLORER_HOST_SET_PH_OFS_T_PRELOAD == 1 
#if 0
    /* 
    ** for testing PH_OFS_T_PRELOAD field in OBJECT_PRELOAD_VAL_5 register setting passed in with boot config command 
    ** register setting is passed in bits 23:16 of the boot config command 
    */
    /* one value in boot config command applied to all lanes */
    ech_ph_ofs_t_preload_set(rx_buf[EXP_TWI_CMD_DATA_OFFSET + 1]);
#else
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();

    /* 
    ** for testing PH_OFS_T_PRELOAD field in OBJECT_PRELOAD_VAL_5 register
    **
    ** assume that host has written per lane setting in extended data buffer
    ** byte 0 = 0, the default value will be applied 
    ** byte 0 = 1, the values in extended data buffer will be applied 
    ** bytes 1-8 = per lane 0-7 setting
    */

    /* record the use host settings flag */
    ech_ph_ofs_t_preload_use_host_set(ext_data_ptr[0]);

    /* record the value for each lane*/
    for (UINT32 lane = 0; lane < 8; lane++)
    {
        ech_ph_ofs_t_preload_set(lane, ext_data_ptr[lane + 1]);
    }
#endif
#endif

#if EXPLORER_HOST_SET_D_IQ_OFFSET == 1
    /* 
    ** for testing D_IQ_OFFSET field in TR_CONFIG_5 register 
    **  
    ** assume that host has written per lane setting in extended data buffer
    ** byte 9 = 0, the hardware calibration result for d_iq_offset will be used
    ** byte 9 = 1, the values in extended data buffer will be applied 
    ** bytes 10-17 = per lane 0-7 setting
    */

    /* record the use host settings flag */
    ech_d_iq_offset_use_host_set(ext_data_ptr[9]);

    /* record the value for each lane*/
    for (UINT32 lane = 0; lane < 8; lane++)
    {
        ech_d_iq_offset_set(lane, ext_data_ptr[lane + 10]);
    }
#endif

    if (EXP_TWI_BOOT_CFG_FW_PRODUCT_QUALIFICATION_MODE == ((cfg_flags & EXP_TWI_BOOT_CFG_FW_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_FW_MODE_BITOFF))
    {
        /* product qualification mode specified */
        
        bc_printf("TWI_BOOT_CONFIG:Enabling PQM Mode\n");
        /* initialize PQM mode */
        ech_twi_pqm_set(TRUE);        
        /*
        ** Set Adaptation enable/disable depending on bit set in the BOOT_CFG command. 
        */
        ech_adaptation_state_set((cfg_flags & EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_ENABLE_BITMSK) >> EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_BITOFF);
        return EXP_TWI_SUCCESS;
    }

    /*
    ** BOOT CONFIG command has been broken down into two steps
    ** to overcome a hardware bug and race condition.
    ** Step 0: SerDes will be initialized
    ** Step 1: DL train will be initiated
    ** HOST will send two BOOT CONFIG commands back to back
    ** First BOOT CONFIG command will initiate step 0 and 2nd BOOT CONFIG command
    ** will initiate step 1.
    */

    /* BOOT CONFIG Step 0 */
    if (((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK)  >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_BOOT_STEP_0)
    {
        
        bc_printf("TWI_BOOT_CONFIG:Stage 0\n");

        /* 
        ** Validate transport layer mode 
        ** Note: This has no impact on FW. 
        ** This can be used for debugging when customer reports an error
        */
        switch ((cfg_flags & EXP_TWI_BOOT_CFG_TL_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_TL_MODE_BITOFF)
        {
            case EXP_TWI_BOOT_CFG_TL_OCAPI_MODE:                
                bc_printf("TWI_BOOT_CONFIG:Enabling EXP_TWI_BOOT_CFG_TL_OCAPI_MODE Mode\n");
                ech_tl_mode_set(EXP_TWI_BOOT_CFG_TL_OCAPI_MODE);
                break;
        
            case EXP_TWI_BOOT_CFG_TL_TWI_MODE:
                bc_printf("TWI_BOOT_CONFIG:Enabling EXP_TWI_BOOT_CFG_TL_TWI_MODE Mode\n");
                ech_tl_mode_set(EXP_TWI_BOOT_CFG_TL_TWI_MODE);
                break;
        
            case EXP_TWI_BOOT_CFG_TL_JTAG_MODE:                
                bc_printf("TWI_BOOT_CONFIG:Enabling EXP_TWI_BOOT_CFG_TL_JTAG_MODE Mode\n");
                ech_tl_mode_set(EXP_TWI_BOOT_CFG_TL_JTAG_MODE);
                break;
        
            default:
                /* invalid transport layer mode specified */            
                bc_printf("TWI_BOOT_CONFIG:ERR!!! EXP_TWI_BOOT_CFG_UNSUPPORTED_TL_BITMSK\n");
                ech_twi_extended_status |= EXP_TWI_BOOT_CFG_UNSUPPORTED_TL_BITMSK;
                break;
        }

        /*
        ** Validate specified SerDes frequency 
        */
        switch ((cfg_flags & EXP_TWI_BOOT_CFG_SERDES_FREQ_BITMSK)  >> EXP_TWI_BOOT_CFG_SERDES_FREQ_BITOFF)
        {
            case EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS\n");
                ech_serdes_freq_set(EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS);
                break;

            case EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS\n");
                ech_serdes_freq_set(EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS);
                break;

            case EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS\n");
                ech_serdes_freq_set(EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS);
                break;

            default:                
                bc_printf("TWI_BOOT_CONFIG:Frequency: ERR!!! EXP_TWI_BOOT_CFG_UNSUPPORTED_SERDES_FREQ_BITMSK\n");
                /* invalid SerDes frequency */
                ech_twi_extended_status |= EXP_TWI_BOOT_CFG_UNSUPPORTED_SERDES_FREQ_BITMSK;
                break;
        }

        /* validate the lane configuration */
        switch ((cfg_flags & EXP_TWI_BOOT_CFG_LANE_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_LANE_MODE_BITOFF)
        {
            case EXP_TWI_BOOT_CFG_LANE_8:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Lane: EXP_TWI_BOOT_CFG_LANE_8\n");
                ech_lane_cfg_set(EXP_TWI_BOOT_CFG_LANE_8);
                break;

            case EXP_TWI_BOOT_CFG_LANE_4:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Lane: EXP_TWI_BOOT_CFG_LANE_4\n");
                ech_lane_cfg_set(EXP_TWI_BOOT_CFG_LANE_4);
                break;

            default:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Lane: ERR!!! EXP_TWI_BOOT_CFG_UNSUPPORT_LANE_CONFIG_BITMSK\n");
                /* invalid lane configuration */
                ech_twi_extended_status |= EXP_TWI_BOOT_CFG_UNSUPPORT_LANE_CONFIG_BITMSK;
                break;
        }

        /* implementation based on EXPLORER and OCMB configuration guides */
        if (EXP_TWI_SUCCESS != ech_twi_extended_status)
        {
            /* Set the extended error code*/
            ech_extended_error_code_set(ech_twi_extended_status);
            /* boot config parameters contain errors, command failed */
            return EXP_TWI_BOOT_CFG_FAILED_BITMSK;
        }

        /*
        ** Set DFE enable/disable depending on bit set in the BOOT_CFG command. 
        ** This MUST be done before intializing SERDES with serdes_plat_low_level_init(). 
        */
        ech_dfe_state_set((cfg_flags & EXP_TWI_BOOT_CFG_DFE_BITMSK) >> EXP_TWI_BOOT_CFG_DFE_BITOFF);

        /*
        ** Set Adaptation enable/disable depending on bit set in the BOOT_CFG command. 
        */
        ech_adaptation_state_set((cfg_flags & EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_ENABLE_BITMSK) >> EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_BITOFF);

        /*
        ** Check if SerDes Initialization has passed
        ** If there is an error, return immediately
        */
        
        bc_printf("TWI_BOOT_CONFIG: SerDes Init Start\n");
        rc = serdes_plat_low_level_init(ech_lane_cfg_bitmask_get(),
                                        ech_serdes_freq_get(),
                                        ech_dfe_state_get());

        /* 
        ** Set the flag app_fw_oc_ready to TRUE prior to error checking,
        ** which will enable command polling over OpenCapi.
        ** Since DLx was taken out of reset in serdes_plat_low_level_init,
        ** it is safe to enable command polling. This will enable a path over 
        ** I2C should host wants to use I2C path instead of OpenCapi.
        */
        app_fw_oc_ready = TRUE;
        
        if (rc != PMC_SUCCESS)
        {        
            
            bc_printf("TWI_BOOT_CONFIG: serdes_plat_low_level_init: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;        
        }

        bc_printf("TWI_BOOT_CONFIG: SerDes Init End\n");
        
       
        if (EXP_TWI_BOOT_CFG_FW_MANUFACTURING_MODE == ((cfg_flags & EXP_TWI_BOOT_CFG_FW_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_FW_MODE_BITOFF))
        {            
            bc_printf("TWI_BOOT_CONFIG: FW is in manufacturing mode \n");
            /* manufacturing mode specified */
            ech_mfg_mode_set();
        
            /* test for loopback mode */
            if (EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_MODE == ((cfg_flags & EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_BITMSK) >> EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_BITOFF))
            {                
                bc_printf("TWI_BOOT_CONFIG: Configuring SerDes in Loopback mode \n");
                /* loopback mode selected, set flag */
                ech_ocapi_loopback_set();

                /* Call this function to initialize SerDes so that we can support external loopback */
                rc = serdes_plat_low_level_standalone_init(ech_lane_cfg_bitmask_get(), ech_dfe_state_get());
                if (rc != PMC_SUCCESS)
                {                     
                    ech_extended_error_code_set(rc);
                    return EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_FAIL_BITMASK;                    
                }
                /* Run SerDes loopback test */
                rc = serdes_plat_loopback_test(ech_lane_cfg_bitmask_get());
                if (rc != PMC_SUCCESS)
                {                     
                    ech_extended_error_code_set(rc);
                    return EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_FAIL_BITMASK;                    
                }
            }
        
        }

        return EXP_TWI_SUCCESS;
    }
    
    /* BOOT CONFIG Step 1 */
    else if (((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK)  >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_BOOT_STEP_1)
    {
        bc_printf("TWI_BOOT_CONFIG: Stage 1\n");

        /* Wait for RX data and run RCLK alignment */
        if ( !ocmb_cfg_RxPatAorB(OCMB_REGS_BASE_ADDR, ech_lane_cfg_pattern_bitmask_get()) )
        {                        
            bc_printf("[ERROR] Cannot find pattern A or B\n");  
            ech_extended_error_code_set(EXP_TWI_BOOT_CFG_DLX_CONFIG_PATTERN_A_B_FAILED);
            return EXP_TWI_BOOT_CFG_DLX_CONFIG_FAIL_BITMSK;
        }

        bc_printf("TWI_BOOT_CONFIG: Found Rx Pattern A or B\n");

        /* Call following function to support lane inversion */
        rc =  serdes_plat_lane_inversion_config(ech_lane_cfg_bitmask_get(), ech_lane_cfg_pattern_bitmask_get());
        if (rc != PMC_SUCCESS)
        {        
            bc_printf("TWI_BOOT_CONFIG:  serdes_plat_lane_inversion_config: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_LANE_INVERSION_CONFIG_FAIL_BITMSK;        
        }
        
        rc = serdes_plat_iq_offset_calibration(ech_lane_cfg_bitmask_get());
        if (rc != PMC_SUCCESS)
        {            
            bc_printf("TWI_BOOT_CONFIG: serdes_plat_iq_offset_calibration: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;        
        }

        rc = serdes_plat_adapt_step1(ech_dfe_state_get(),ech_adaptation_state_get(), ech_lane_cfg_bitmask_get());
        if (rc != PMC_SUCCESS)
        {            
            bc_printf("TWI_BOOT_CONFIG: Serdes_plat_adapt_step1: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;        
        }

        /* apply OCMB DL reset */
        if (FALSE == ocmb_cfg_DLx_config_FW(OCMB_REGS_BASE_ADDR))
        {
            /* DLx Config FW failed */
            ech_extended_error_code_set(EXP_TWI_BOOT_CFG_DLX_CONFIG_FW_FAILED);
            bc_printf("[ERROR] OCMB DLx_config_FW FAILED\n");
            /* exit */
            return EXP_TWI_BOOT_CFG_DLX_CONFIG_FAIL_BITMSK;
        }

        rc = serdes_plat_adapt_step2(ech_dfe_state_get(), ech_adaptation_state_get(), ech_lane_cfg_bitmask_get());
        if (rc != PMC_SUCCESS)
        {            
            bc_printf("TWI_BOOT_CONFIG: serdes_plat_adapt_step2: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;        
        }
        return EXP_TWI_SUCCESS;
    }
    /* boot config command succeeded */
    return EXP_TWI_SUCCESS;

} /* ech_twi_boot_config_proc */


