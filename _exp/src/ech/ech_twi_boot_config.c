/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*
* Copyright (c) 2021 Microchip Technology Inc. All rights reserved.
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
#include "top_plat.h"
#include "ocmb.h"
#include "top.h"
#include "spi.h"
#include "serdes_config_guide.h"
#include "serdes_cg_supplement.h"
#include "serdes_api.h"

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
    UINT32 cfg_flags = (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 1] << 16) | (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] << 8) | rx_buf[EXP_TWI_CMD_DATA_OFFSET + 3];
    UINT8  ech_twi_extended_status = EXP_TWI_SUCCESS;
    UINT32 revision_id;
    UINT32 err_reg_offset;
    UINT32 current_lane;
    UINT32 lane_offset;
    UINT32 lane_bitmask;
    top_plat_lock_struct lock_struct;

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
    for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
    {
        ech_ph_ofs_t_preload_set(current_lane, ext_data_ptr[current_lane + 1]);
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
    for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
    {
        ech_d_iq_offset_set(current_lane, ext_data_ptr[current_lane + 10]);
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
    ** Step 1: DL train will be initialized
    ** HOST will send two BOOT CONFIG commands back to back
    ** First BOOT CONFIG command will initiate step 0 and 2nd BOOT CONFIG command
    ** will initiate step 1.
    */

    /* BOOT CONFIG Step 0 */
    if (((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK) >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_BOOT_STEP_0 
        || ((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK) >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_SERDES_PRBS_CAL_STEP_0)
    {
        /* 
        ** Validate SerDes calibration algorithm selection
        */ 
        if (((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK) >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_BOOT_STEP_0) 
        {
            ech_serdes_prbs_cal_state_set(FALSE);
            bc_printf("TWI_BOOT_CONFIG:Stage 0\n");
        }
        else
        {
            ech_serdes_prbs_cal_state_set(TRUE);
            bc_printf("TWI_BOOT_CONFIG:Stage 0 SerDes PRBS Calibration Algorithm\n");
        }
        

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
            case EXP_SERDES_21_33_GBPS:
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS\n");
                ech_serdes_freq_set(EXP_SERDES_21_33_GBPS);
                break;

            case EXP_SERDES_23_46_GBPS:
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS\n");
                ech_serdes_freq_set(EXP_SERDES_23_46_GBPS);
                break;

            case EXP_SERDES_25_60_GBPS:
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS\n");
                ech_serdes_freq_set(EXP_SERDES_25_60_GBPS);
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

        /* 
        ** Validate the loopback cal mode settings. This loopback is for running the PRBS 
        ** calibration sequence with a OMI loopback.
        */
        if (EXP_TWI_BOOT_CFG_OMI_NO_LOOPBACK_MODE == ((cfg_flags & EXP_TWI_BOOT_CFG_OMI_LOOPBACK_BITMSK) >> EXP_TWI_BOOT_CFG_OMI_LOOPBACK_BITOFF))
        {
            ech_serdes_loopback_set(FALSE);
            ech_serdes_loopback_csu_offset_mask_set(0x00);
            bc_printf("TWI_BOOT_CONFIG:Disabling OMI Loopback Calibration Mode\n");
        }
        else
        {
            /* This mode is only applicable to the PRBS calibration sequence */
            if (!ech_serdes_prbs_cal_state_get()) 
            {
                bc_printf("TWI_BOOT_CONFIG:Enabling OMI Loopback Calibration: ERR!!! Only applicable to PRBS Calibration mode\n");
                ech_twi_extended_status |= EXP_TWI_BOOT_CFG_UNSUPPORT_LOOPBACK_CONFIG_BITMSK;
            }
            else
            {
                ech_serdes_loopback_set(TRUE);
                ech_serdes_loopback_csu_offset_mask_set((cfg_flags & EXP_TWI_BOOT_CFG_OMI_LOOPBACK_BITMSK) >> EXP_TWI_BOOT_CFG_OMI_LOOPBACK_BITOFF);
                bc_printf("TWI_BOOT_CONFIG:Enabling OMI Loopback Calibration Mode (CSU offset on lanes: 0x%02x)\n", ech_serdes_loopback_csu_offset_mask_get());
            }
        }

        /* get the enabled lanes bit mask */
        lane_bitmask = ech_lane_cfg_bitmask_get();
        bc_printf("\n    lane_bitmask = 0x%02X\n", lane_bitmask);

        /* implementation based on EXPLORER and OCMB configuration guides */
        if (EXP_TWI_SUCCESS != ech_twi_extended_status)
        {
            /* Set the extended error code*/
            bc_printf("TWI_BOOT_CONFIG: FAILED ech_twi_extended_status=0x%08x\n", ech_twi_extended_status);
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

        /*
        ** config guide sequences: 
        **     FH_CSU_init_1
        **     FH_CSU_init_2
        **     FH_TXRX_Calibration1
        **     FH_PGA_init
        **     FH_NETX_Settings
        **     FH_TXRX_Datapath
        */
        bc_printf("TWI_BOOT_CONFIG: SerDes Init Start\n");
        rc = serdes_plat_low_level_init(ech_lane_cfg_bitmask_get(),
                                        ech_serdes_freq_get(),
                                        ech_dfe_state_get());
        if (rc != PMC_SUCCESS)
        {
            bc_printf("TWI_BOOT_CONFIG: serdes_plat_low_level_init: ERR!!! rc=0x%x\n",rc);
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
        }

        /*
        ** Run the PRBS calibration sequence
        */
        if (TRUE == ech_serdes_prbs_cal_state_get()) 
        {

            /*
            ** config guide sequence FH_SetCSCU_PPM
            */

            UINT32 serdes_freq = ech_serdes_freq_get();
        
            bc_printf("TWI_BOOT_CONFIG: FH_SetCSU_ppm\n");

            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                /*
                ** In loopback cal mode, the mask defines which lanes are to have ppm 
                ** offset applied. Each Tx and Rx must be offset relative to eachother. 
                ** Without loopback, apply the ppm offset to all lanes. 
                */
                if ((lane_bitmask & (1 << current_lane)) &&
                    (!ech_serdes_loopback_get() || (ech_serdes_loopback_csu_offset_mask_get() & (1 << current_lane))))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                    switch (serdes_freq)
                    {
                        case EXP_SERDES_21_33_GBPS: 
                        {
                            SERDES_FH_SetCSU_ppm_21g328(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
                        }
                        break;

                        case EXP_SERDES_23_46_GBPS:
                        {
                            SERDES_FH_SetCSU_ppm_23g46(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
                        }
                        break;

                        case EXP_SERDES_25_60_GBPS:
                        {
                            SERDES_FH_SetCSU_ppm_25g6(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
                        }
                        break;
                    }
                }
            }

            /* 
            ** Run FH_alignment_init_1 here prior to in boot config 1. Unclear if 
            ** this is required or only the write to RX_ALIGN_EN = FALSE
            ** initialize lane alignment part 1 to all lanes 
            ** config guide FH_alignment_init_1 
            */ 
            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                if (lane_bitmask & (1 << current_lane))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;
                    rc = SERDES_FH_alignment_init_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                    (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                                                    FALSE);
                    if (TRUE != rc)
                    {
                        bc_printf("[%d] EXP_SERDES_FH_ALIGNMENT1_FAILED\n", current_lane);
                        ech_extended_error_code_set(EXP_SERDES_FH_ALIGNMENT_INIT_1_FAIL);
                        return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
                    }
                }
            }

            /* 
            ** Enable pattern generator in loopback cal mode
            */
            if (ech_serdes_loopback_get()) 
            {
                bc_printf("TWI_BOOT_CONFIG: Enable pattern generators for loopback calibration mode\n");

                UINT32 gen_usr_patt[10] = {0};

                for (current_lane = 0; current_lane < SERDES_LANES; current_lane++) 
                {
                    if (lane_bitmask & (1 << current_lane))
                    {
                        lane_offset = current_lane * SERDES_LANE_REG_OFFSET;
                        SERDES_FH_pattgen_en((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                                             0x5,
                                             gen_usr_patt);
                    }
                }
            }

            /*
            ** config guide sequence FH_TXRX_Adaptation1_FW
            */
            bc_printf("\nTWI_BOOT_CONFIG: FH_TXRX_Adaptation1_FW\n");
            rc = serdes_plat_adapt_step1(ech_dfe_state_get(),
                                         ech_adaptation_state_get(), 
                                         lane_bitmask,
                                         TRUE);

            if (rc != PMC_SUCCESS)
            {
                bc_printf("TWI_BOOT_CONFIG: serdes_plat_adapt_step1: ERR!!! rc=0x%x\n",rc);
                ech_extended_error_code_set(rc);
                return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
            }

            /*
            ** Enable PRBS23 monitoring and check if any lanes need to be inverted 
            ** Get a bitmask of the lanes which achieve pattern lock. 
            ** The remaining CDR offset cal cannot be run on lanes where the pattern monitor 
            ** is not locked.
            */
            bc_printf("\nTWI_BOOT_CONFIG: Enable pattern monitors\n");
            UINT8 pattmon_detected_bitmask;
            rc = serdes_plat_pattmon_config(lane_bitmask, SERDES_CDR_CAL_DWELL_10_MS, SERDES_BER_25G6_PER_MS_1e_4, &pattmon_detected_bitmask);

            if (rc != PMC_SUCCESS) 
            {
                ech_extended_error_code_set(rc);
                return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
            }

            /* 
            ** calibrate DIQ and TIQ offsets 
            ** config guide FH_IQ_Offset_Calibration 
            */ 
            rc = SERDES_FH_IQ_Offset_Calibration(lane_bitmask);

            if (rc != PMC_SUCCESS)
            {
                bc_printf("TWI_BOOT_CONFIG: SERDES_FH_IQ_Offset_Calibration: ERR!!! rc=0x%x\n",rc);
                ech_extended_error_code_set(rc);
                return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
            }

            /*
            ** Run 10 mission mode cycles 
            ** FH_ForceCDRadaptDFE with FORCE_ADAPT_VALUE_EN=0x0, OBJECTS_EN_PASS11=0x1e68, ADAPT_LOOPS_PASS11=0x10 
            */
            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                if (lane_bitmask & (1 << current_lane))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;
                    
                    SERDES_FH_ForceCDRadaptDFE((SERDES_ADSP_PCBI_BASE_ADDR + lane_offset),
                                                 (SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                 SERDES_MISSION_MODE_FORCE_ADAPT_VALUE_EN,
                                                 SERDES_MISSION_MODE_ADAPT_LOOPS_PASS11,
                                                 SERDES_MISSION_MODE_OBJECTS_EN_PASS11);

                }
            }

            /* Select the BER per ms threshold based on the rate */
            UINT32 serdes_ber_fast_sweep_cnt;
            UINT32 serdes_ber_slow_sweep_cnt;
            switch (serdes_freq)
            {
                case EXP_SERDES_21_33_GBPS: 
                {
                    serdes_ber_fast_sweep_cnt = SERDES_BER_21G33_PER_MS_1e_4;
                    serdes_ber_slow_sweep_cnt = SERDES_BER_21G33_PER_MS_1e_6;
                }
                break;
            
                case EXP_SERDES_23_46_GBPS:
                {
                    serdes_ber_fast_sweep_cnt = SERDES_BER_23G46_PER_MS_1e_4;
                    serdes_ber_slow_sweep_cnt = SERDES_BER_23G46_PER_MS_1e_6;
                }
                break;
            
                case EXP_SERDES_25_60_GBPS:
                {
                    serdes_ber_fast_sweep_cnt = SERDES_BER_25G6_PER_MS_1e_4;
                    serdes_ber_slow_sweep_cnt = SERDES_BER_25G6_PER_MS_1e_6;
                }
                break;
            }

            /*
            ** calibrate CDR offsets 
            ** config guide FH_CDR_Offset_Calibration 
            */ 
            UINT8 converged_lane_bitmask;
            rc = SERDES_FH_CDR_Offset_Calibration_Centered(lane_bitmask & pattmon_detected_bitmask,
                                                  SERDES_IMPAIRMENT_PJ_SIN0,
                                                  SERDES_CDR_CAL_DWELL_10_MS,
                                                  SERDES_CDR_SLO_DWELL_100_MS,
                                                  serdes_ber_fast_sweep_cnt,
                                                  serdes_ber_slow_sweep_cnt,
                                                  SERDES_FORCE_ADAPT_VALUE_EN,
                                                  SERDES_OBJECTS_EN_PASS11,
                                                  SERDES_ADAPT_LOOPS_PASS11,
                                                  &converged_lane_bitmask);

            if (rc != PMC_SUCCESS)
            {
                bc_printf("TWI_BOOT_CONFIG: SERDES_FH_CDR_Offset_Calibration: ERR!!! rc=0x%x\n",rc);
                ech_extended_error_code_set(rc);
                return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
            }

            /* Store pattmon_detected_bitmask and converged_lane_bitmask */
            ech_serdes_prbs_cal_data_set(pattmon_detected_bitmask, converged_lane_bitmask);

            /*
            ** config guide FH_ClearCSU_ppm
            */
            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                if (lane_bitmask & (1 << current_lane))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                    SERDES_FH_ClearCSU_ppm(SERDES_CSU_PCBI_BASE_ADDR + lane_offset);
                }
            }
        }

        if (FALSE == ech_serdes_prbs_cal_state_get()) 
        {
        
            /* initialize lane alignment part 1 to all lanes*/
            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                if (lane_bitmask & (1 << current_lane))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                    rc = SERDES_FH_alignment_init_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                           (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset), TRUE);

                    if (TRUE != rc)
                    {
                        bc_printf("[%d] EXP_SERDES_FH_ALIGNMENT1_FAILED\n", current_lane);
                        ech_extended_error_code_set(EXP_SERDES_FH_ALIGNMENT_INIT_1_FAIL);
                        return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
                    }
                }
            }

            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                if (lane_bitmask & (1 << current_lane))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                    /* initialize lane alignment part 2 on all lanes except master lane 4 */
                    if (current_lane != SERDES_LANE_4)
                    {
                        rc = SERDES_FH_TX_alignment(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);
                        if (TRUE != rc)
                        {
                            bc_printf("[%d] EXP_SERDES_FH_TX_ALIGNMENT_FAIL\n", current_lane);
                            ech_extended_error_code_set(EXP_SERDES_FH_TX_ALIGNMENT_FAIL);
                            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
                        }
                    }
                }
            }
        }

        /*
        ** Set the flag app_fw_oc_ready to TRUE prior to error checking,
        ** which will enable command polling over OpenCapi.
        ** Since DLx was taken out of reset in serdes_plat_low_level_init,
        ** it is safe to enable command polling. This will enable a path over
        ** I2C should host wants to use I2C path instead of OpenCapi.
        */
        app_fw_oc_ready = TRUE;

        bc_printf("TWI_BOOT_CONFIG: SerDes Init End\n");

        if (EXP_SERDES_4_LANE_BITMASK == ech_lane_cfg_bitmask_get())
        {
            /*
            ** operating in x4 mode:
            ** configure the disabled lanes in OCMB and
            ** take appropriate lanes out of reset at the top level
            */

            /* disable interrupts and disable multi-VPE operation */
            top_plat_critical_region_enter(&lock_struct);

            ocmb_cfg_x4LaneMode(OCMB_REGS_BASE_ADDR);

            /* restore interrupts and enable multi-VPE operation */
            top_plat_critical_region_exit(lock_struct);

            top_exp_cfg_serdes_link_width_x4(TOP_XCBI_BASE_ADDR);
        }
        else if (EXP_SERDES_8_LANE_BITMASK == ech_lane_cfg_bitmask_get())
        {
            /*
            ** operating in x8 mode:
            ** take all lanes out of reset at the top level
            */
            top_exp_cfg_serdes_link_width_x8(TOP_XCBI_BASE_ADDR);
        }

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
                    bc_printf("TWI_BOOT_CONFIG: FAILED to init Loopback mode (0x%08x) \n", rc);
                    ech_extended_error_code_set(rc);
                    return EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_FAIL_BITMASK;
                }
                /* Run SerDes loopback test */
                rc = serdes_plat_loopback_test(ech_lane_cfg_bitmask_get());
                if (rc != PMC_SUCCESS)
                {
                    bc_printf("TWI_BOOT_CONFIG: FAILED to init Loopback mode (0x%08x) \n", rc);
                    ech_extended_error_code_set(rc);
                    return EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_FAIL_BITMASK;
                }
            }

        }

        return EXP_TWI_SUCCESS;
    }

    /* BOOT CONFIG Step 1 */
    else if (((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK) >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_BOOT_STEP_1)
    {
        /* Rely on calibration algorithm selection passed in during boot config 0 */
        if (FALSE == ech_serdes_prbs_cal_state_get()) 
        {
            bc_printf("TWI_BOOT_CONFIG:Stage 1\n");
        }
        else
        {
            bc_printf("TWI_BOOT_CONFIG:Stage 1 SerDes PRBS Calibration Algorithm\n");
        }

        lane_bitmask = ech_lane_cfg_bitmask_get();
        bc_printf("\n    lane_bitmask = 0x%02X\n", lane_bitmask);
        /* 
        ** get lane bitmask, w/o lane 4 (internal clock) if in x4 mode 
        ** default x4 lanes = 7/5/2/0 
        */ 
        UINT8 active_lane_bitmask = ech_lane_cfg_pattern_bitmask_get();

        if (TRUE == ech_serdes_prbs_cal_state_get()) 
        {
            /* initialize lane alignment part 1 to all lanes*/
            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                if (lane_bitmask & (1 << current_lane))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                    rc = SERDES_FH_alignment_init_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                           (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset), TRUE);

                    if (TRUE != rc)
                    {
                        bc_printf("[%d] EXP_SERDES_FH_ALIGNMENT1_FAILED\n", current_lane);
                        ech_extended_error_code_set(EXP_SERDES_FH_ALIGNMENT_INIT_1_FAIL);
                        return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
                    }
                }
            }

            for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
            {
                if (lane_bitmask & (1 << current_lane))
                {
                    /* set the offset for the lane being configured */
                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                    /* initialize lane alignment part 2 on all lanes except master lane 4 */
                    if (current_lane != SERDES_LANE_4)
                    {
                        rc = SERDES_FH_TX_alignment(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);
                        if (TRUE != rc)
                        {
                            bc_printf("[%d] EXP_SERDES_FH_TX_ALIGNMENT_FAIL\n", current_lane);
                            ech_extended_error_code_set(EXP_SERDES_FH_TX_ALIGNMENT_FAIL);
                            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
                        }
                    }
                }
            }
        }

        /* Wait for RX data and run RCLK alignment */
        if (!ocmb_cfg_RxPatAorB(OCMB_REGS_BASE_ADDR, &active_lane_bitmask) )
        {
            bc_printf("[ERROR] Cannot find pattern A or B\n");
            ech_extended_error_code_set(EXP_TWI_BOOT_CFG_DLX_CONFIG_PATTERN_A_B_FAILED);
            return EXP_TWI_BOOT_CFG_DLX_PAT_A_B_FAIL_BITMASK;
        }

        bc_printf("TWI_BOOT_CONFIG: Found Rx Pattern A or B on lanes = 0x%02X\n", active_lane_bitmask);

        /* Call following function to support lane inversion */
        rc =  serdes_plat_lane_inversion_config(ech_lane_cfg_pattern_bitmask_get(), &active_lane_bitmask);
        if (rc != PMC_SUCCESS)
        {
            bc_printf("TWI_BOOT_CONFIG:  serdes_plat_lane_inversion_config: ERR!!! rc=0x%x\n",rc);
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_LANE_INVERSION_CONFIG_FAIL_BITMSK;
        }

        /* 
        ** record the lanes that successfully trained 
        ** may differ from configured lanes if operating in degraded mode 
        */ 
        ech_lane_active_pattern_bitmask_set(active_lane_bitmask);

        /* Run calibrate at this point for the non-PRBS algorithm */
        if (FALSE == ech_serdes_prbs_cal_state_get()) 
        {
            rc = SERDES_FH_IQ_Offset_Calibration(active_lane_bitmask);

            if (rc != PMC_SUCCESS)
            {
                bc_printf("TWI_BOOT_CONFIG: SERDES_FH_IQ_Offset_Calibration: ERR!!! rc=0x%x\n",rc);
                ech_extended_error_code_set(rc);
                return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
            }

            rc = serdes_plat_adapt_step1(ech_dfe_state_get(),ech_adaptation_state_get(), active_lane_bitmask, FALSE);

            if (rc != PMC_SUCCESS)
            {
                bc_printf("TWI_BOOT_CONFIG: Serdes_plat_adapt_step1: ERR!!! rc=0x%x\n",rc);
                ech_extended_error_code_set(rc);
                return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
            }

        }

        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);

        /* 
        ** configure the DL layer and enable automatic training 
        ** this function unconditionally returns TRUE (passed) 
        */ 
        ocmb_cfg_DLx_config_FW(OCMB_REGS_BASE_ADDR);

        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        bc_printf("OCMB DLx_config_FW PASSED\n");

        /* wait for OCMB DL training to complete */
        if (FALSE == ocmb_cfg_DLx_train_wait_FW(OCMB_REGS_BASE_ADDR))
        {
            /* DLx Train Wait FW failed */
            bc_printf("[ERROR] OCMB DLx_train_wait FAILED\n");
            ech_extended_error_code_set(EXP_TWI_BOOT_CFG_DLX_CONFIG_FW_FAILED);

            /* exit */
            return EXP_TWI_BOOT_CFG_DLX_CONFIG_FAIL_BITMSK;
        }
        bc_printf("OCMB DLx_train_wait PASSED\n");

        /* Run adaptation2 */
        rc = serdes_plat_adapt_step2(ech_dfe_state_get(), ech_adaptation_state_get(), active_lane_bitmask);
        if (rc != PMC_SUCCESS)
        {
            bc_printf("TWI_BOOT_CONFIG: serdes_plat_adapt_step2: ERR!!! rc=0x%x\n",rc);
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;
        }

#if SERDES_CAL_VERBOSE == 1
        /* Print adapted objects now that mission mode is running */
        bc_printf("Objects after adaptation 2\n");
        SERDES_FH_Print_Adapted_Obj(lane_bitmask);
#endif

        /*
        ** Propagate the REVISION ID from the EFUSE to the OCMB
        ** registers MB_SIM_MMIO_O1CCD_RIGHT and MB_SIM_MMIO_O0CCD_RIGHT
        ** for IBM.
        */
        revision_id = top_efuse_revision_id_get();
        bc_printf("TWI_BOOT_CONFIG: Propagating rev_id=%d to OCMB O1CCD and O0CCD rev ID registers\n", revision_id);
        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);
        ocmb_api_revision_id_set(revision_id);
        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        /*
        ** Now setup serdes fatal for normal runtime and check to see if there were
        ** any serdes fatals occurred during the BOOT_CONFIG sequence.
        */
        serdes_fatal_init(ech_lane_cfg_bitmask_get());
        if (serdes_fatal_get(ech_lane_cfg_bitmask_get(), &err_reg_offset))
        {
            /* SerDes fatal error detected */

            /*
            ** err_reg_offset contains register in which the error was detected
            ** 2nd nibble contains SerDes sub-block identifier of the register
            ** set the sub-block as the extended error code byte
            */
            bc_printf("TWI_BOOT_CONFIG: SerDes Hardware error found. (0x%08x)", err_reg_offset);
            ech_extended_error_code_set((err_reg_offset & 0x00000F00) >> 8);

            return EXP_TWI_BOOT_CFG_SERDES_FATAL_FOUND_BITMASK;
        }

        /*
        ** After BOOT_CONFIG Stage 1 completes successfully, then the OMI link is up. 
        ** As per EBCF-10894, enable 1-bit SPI ECC interrupts.
        */
        spi_fatal_cecc_enable();

        /*
        ** Start the periodic serdes calibration as necessary
        */
        serdes_plat_periodic_cal_init(ech_serdes_prbs_cal_state_get());

        return EXP_TWI_SUCCESS;
    }
    /* unsupported boot stage, command failed */
    ech_extended_error_code_set(EXP_TWI_BOOT_CFG_UNSUPPORTED_BOOT_STAGE);
    return EXP_TWI_BOOT_CFG_FAILED_BITMSK;

} /* ech_twi_boot_config_proc */


