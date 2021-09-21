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
#include "serdes_cg_supplement.h"
#include <string.h>
#include "serdes_plat.h"
#include "app_fw.h"
#include "crash_dump.h"
#include "serdes_dump.h"
#include "sys_timer_api.h"
#include "pmc_profile.h"
#include "serdes_api.h"
#include "ocmb_config_guide_mchp.h"
#include "ocmb_config_guide.h"
#include "bc_printf.h"
#include "top_plat.h"
#include "ocmb_erep.h"

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

/* Crash dump size empirically determined to be 0x65ca bytes. */
/*
** The crash dump size for the SerDes regsiters has been empirically determined 
** to be a size of 0x6bca bytes.  Setting it to be 0x7000 to provide some room 
** for growth. 
*/
#define SERDES_REG_CRASH_DUMP_SIZE 0x7000

/* SerDes periodic cal initialized flag */
PRIVATE BOOL serdes_initialized = FALSE;

/* Flag to disable SerDes periodic cal */
PRIVATE BOOL serdes_cal_timer_disable = FALSE;

/* SerDes FFE settings */
PRIVATE UINT32 serdes_ffe_precursor = SERDES_FFE_DEFAULT_PRECURSOR;
PRIVATE UINT32 serdes_ffe_postcursor = SERDES_FFE_DEFAULT_POSTCURSOR;
PRIVATE UINT32 serdes_ffe_calibration = SERDES_FFE_MIN_CALIBRATION;

#define SERDES_FFE_DISABLE_TX_PARALLEL_TERMINATION  0

/* Timer period in ms to update calibration */
#define SERDES_CAL_UPDATE_PERIOD_MS 5000

/* Init flag for periodic serdes cal */
PRIVATE BOOL serdes_cal_timer_init;

/* Variable to hold the last time that the periodic cal was run */
PRIVATE UINT_TIME sys_timer_last_cal;

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

    /* invoke FH_CSU_init_1 */
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
        return EXP_SERDES_TRAINING_CSU_FAILED_1;
    }

    /* invoke FH_CSU_init_2 */
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
        return EXP_SERDES_TRAINING_CSU_FAILED_2;
    }
    return (PMC_SUCCESS);
}

/**
* @brief
 *Sets the PX 6-bit words that control the TX amplitude.  
 *  
* @param [in] cursor_val: pre/postcursor value 
*  
* @return
*   returns an integer number that represents the TX*_P**_D*EN
*   code needed to have “val” worth of TX segments enabled
*   eg. to enable 6 TX segments, we would need to set
*   TX*_P**_D*EN to 32 (32==b100000), dec2pseudobin(6) = 32
* 
* @note 
*   Code based on functions provided by mixed signal   
*/  
PRIVATE UINT32 dec2pseudobin(UINT32 cursor_val)
{
    UINT32 retval = 0;
    
    /* 
    ** bit[5] == +6;  bit[4] == +4;  bit[3] == +2;     
    ** bit[2] == +2;  bit[1] == +1;  bit[0] == +1;     
    **   E.g.: 21 (dec) == 01 0101 (bin)     
    **   --> Weight of TX bank = +4 +2 +1 = +7. 
    **       Hence for a weight of +7, a PXX_DXEN[5:0]=21 is needed. 
    */ 
    
    /* 
    ** Case statement was easier to build for 16 values than
    ** a mathematical equation that does the same thing 
    */ 
    switch (cursor_val)
    {
        case 0 : retval=0 ; break;   //000000
        case 1 : retval=2  ; break;  //000010
        case 2 : retval=8  ; break;  //001000
        case 3 : retval=10 ; break;  //001010

        case 4 : retval=16 ; break;  //010000
        case 5 : retval=18 ; break;  //010010
        case 6 : retval=32 ; break;  //100000
        case 7 : retval=34 ; break;  //100010

        case 8 : retval=40 ; break;  //101000
        case 9 : retval=42 ; break;  //101010 
        case 10: retval=48 ; break;  //110000
        case 11: retval=50 ; break;  //110010

        case 12: retval=56 ; break;  //111000
        case 13: retval=58 ; break;  //111010
        case 14: retval=60 ; break;  //111100
        case 15: retval=62 ; break;  //111110

        case 16: retval=63 ; break;  //111111 
        default: retval=0 ;
    }
    
    return retval;
}

/**
* @brief 
*  Sets the PX 6-bit words that control the TX amplitude.
*  
* @param [in] cursor_val: pre/postcursor value 
*  
* @param [out] tx_pxa_d1en_ptr 
* @param [out] tx_pxa_d2en_ptr 
* @param [out] tx_pxb_d1en_ptr 
* @param [out] tx_pxb_d2en_ptr 
* @return
*   Nothing
* 
* @note 
*   Code based on functions provided by mixed signal   
*/  
PRIVATE VOID serdes_plat_setpx(UINT32  cursor_val, 
                               UINT32* tx_pxa_d1en_ptr, 
                               UINT32* tx_pxa_d2en_ptr,
                               UINT32* tx_pxb_d1en_ptr,
                               UINT32* tx_pxb_d2en_ptr)
{
    if (cursor_val > 16) 
    {
        *tx_pxa_d2en_ptr = 63;
        *tx_pxa_d1en_ptr = 0;
        *tx_pxb_d2en_ptr = dec2pseudobin(cursor_val - 16);
        *tx_pxb_d1en_ptr = 63 - (*tx_pxb_d2en_ptr);
    } 
    else 
    {
        *tx_pxa_d2en_ptr = dec2pseudobin(cursor_val);
        *tx_pxa_d1en_ptr = 63 - (*tx_pxa_d2en_ptr); 
        *tx_pxb_d2en_ptr = 0;   
        *tx_pxb_d1en_ptr = 63; 
    }
}

/**
* @brief
* Sets the 13 6-bit words that control the TX amplitude. The  
* encoding scheme is set to have maxswing on the TX output.  
*  
* @param [in] precursor: FFE precursor setting
* @param [in] postcursor: FFE postcursor setting
* @param [in] calibration: FFE calibration 
*  
* @param [out] tx_p1a_d1en_ptr 
* @param [out] tx_p1a_d2en_ptr 
* @param [out] tx_p1b_d1en_ptr 
* @param [out] tx_p1b_d2en_ptr 
* @param [out] tx_p2a_d1en_ptr 
* @param [out] tx_p2a_d2en_ptr 
* @param [out] tx_p2b_d1en_ptr 
* @param [out] tx_p2b_d2en_ptr 
* @param [out] tx_p1a_pten_ptr 
* @param [out] tx_p1b_pten_ptr 
* @param [out] tx_p2a_pten_ptr 
* @param [out] tx_p2b_pten_ptr 
* @param [out] tx_p3a_d1en_ptr 
*  
* @return
*   Nothing
* 
* @note 
*   Code based on functions provided by mixed signal   
*/  
PRIVATE VOID serdes_plat_txctrls_maxswing(UINT32  precursor, 
                                          UINT32  postcursor, 
                                          UINT32  calibration, 
                                          UINT32* tx_p1a_d1en_ptr, 
                                          UINT32* tx_p1a_d2en_ptr,    
                                          UINT32* tx_p1b_d1en_ptr, 
                                          UINT32* tx_p1b_d2en_ptr,          
                                          UINT32* tx_p2a_d1en_ptr, 
                                          UINT32* tx_p2a_d2en_ptr,    
                                          UINT32* tx_p2b_d1en_ptr, 
                                          UINT32* tx_p2b_d2en_ptr, 
                                          UINT32* tx_p1a_pten_ptr, 
                                          UINT32* tx_p1b_pten_ptr, 
                                          UINT32* tx_p2a_pten_ptr, 
                                          UINT32* tx_p2b_pten_ptr,
                                          UINT32* tx_p3a_d1en_ptr) 
{
    /* calculate the P2 settings */
    serdes_plat_setpx(precursor, tx_p2a_d1en_ptr, tx_p2a_d2en_ptr, tx_p2b_d1en_ptr, tx_p2b_d2en_ptr);

    /* calculate the P1 settings */
    serdes_plat_setpx(postcursor, tx_p1a_d1en_ptr, tx_p1a_d2en_ptr, tx_p1b_d1en_ptr, tx_p1b_d2en_ptr);

    /* calculate the P3 settings */
    *tx_p3a_d1en_ptr = dec2pseudobin(calibration - SERDES_FFE_MIN_CALIBRATION);

    /* max swing implies that PT=0 (i.e.: disable TX parallel termination */
    *tx_p1a_pten_ptr = SERDES_FFE_DISABLE_TX_PARALLEL_TERMINATION;
    *tx_p1b_pten_ptr = SERDES_FFE_DISABLE_TX_PARALLEL_TERMINATION;
    *tx_p2a_pten_ptr = SERDES_FFE_DISABLE_TX_PARALLEL_TERMINATION;
    *tx_p2b_pten_ptr = SERDES_FFE_DISABLE_TX_PARALLEL_TERMINATION;
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
*/
PRIVATE UINT32 serdes_plat_low_level_init_sequence_2(UINT32 lane, BOOL dfe_state)
{
    UINT32 rc;
    UINT32 tap_sel, udfe_mode;
    UINT32 rtrim_14_0;
    UINT32 rtrim_34_15;
    UINT32 tx_p1a_d1en;
    UINT32 tx_p1a_d2en;
    UINT32 tx_p1b_d1en;
    UINT32 tx_p1b_d2en;          
    UINT32 tx_p2a_d1en;
    UINT32 tx_p2a_d2en;
    UINT32 tx_p2b_d1en;
    UINT32 tx_p2b_d2en; 
    UINT32 tx_p1a_pten;
    UINT32 tx_p1b_pten;
    UINT32 tx_p2a_pten;
    UINT32 tx_p2b_pten;
    UINT32 tx_p3a_d1en;

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
    SERDES_FH_PGA_init((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                       (SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset));

    /* determine if the host provided non-default FFE settings */
    if ((SERDES_FFE_DEFAULT_PRECURSOR == serdes_plat_ffe_precursor_get()) ||
        (SERDES_FFE_DEFAULT_POSTCURSOR == serdes_plat_ffe_postcursor_get()))
    {
        /* default FFE values being used */

        /* determine if a valid fuse value setting should be used */

        /* read rtrim settings */
        SERDES_FH_read_rtrim((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                             &rtrim_14_0,
                             &rtrim_34_15);

        if (SERDES_RTRIM_34_15_BIT6_VALID == (rtrim_34_15 & SERDES_RTRIM_34_15_BIT6_VALID))
        {
            /* set up the near end transmitter settings using value from FUSE_VAL_STAT_1:RTRIM_34_15 */
            SERDES_FH_NETX_Settings_value((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                          ((rtrim_34_15 & ~SERDES_RTRIM_34_15_BIT6_VALID) & SERDES_RTRIM_34_15_MASK));
        }
        else
        {
            /* set up the near end transmitter using default settings */
            SERDES_FH_NETX_Settings_default(SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);        
        }
    }
    else
    {
        /* 
        ** host has provided non-default FFE pre- and post-cursor values
        ** calculate FFE settings based on code provided by mixed signal 
        */ 

        /* 
        ** calculate the 13 6-bit words that control the TX amplitude
        ** the encoding scheme is set to have maxswing on the TX output
        */
        serdes_plat_txctrls_maxswing(serdes_plat_ffe_precursor_get(),
                                     serdes_plat_ffe_postcursor_get(),
                                     serdes_plat_ffe_calibration_get(),
                                     &tx_p1a_d1en,
                                     &tx_p1a_d2en,
                                     &tx_p1b_d1en,
                                     &tx_p1b_d2en,
                                     &tx_p2a_d1en,
                                     &tx_p2a_d2en,
                                     &tx_p2b_d1en,
                                     &tx_p2b_d2en,
                                     &tx_p1a_pten,
                                     &tx_p1b_pten,
                                     &tx_p2a_pten,
                                     &tx_p2b_pten,
                                     &tx_p3a_d1en);

        /* apply the calculated settings */
        SERDES_FH_NETX_Settings_calcprepost((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                            tx_p2b_d2en,
                                            tx_p2b_d1en,
                                            tx_p2a_pten,
                                            tx_p2a_d2en,
                                            tx_p2a_d1en,
                                            tx_p1b_d2en,
                                            tx_p1b_d1en,
                                            tx_p1a_pten,
                                            tx_p1a_d2en,
                                            tx_p1a_d1en,
                                            tx_p3a_d1en,
                                            tx_p2b_pten,
                                            tx_p1b_pten);
    }

    /* read the parameters that got applied */
    serdes_api_ffe_prepost_settings_read(lane_offset,
                                         &tx_p2b_d2en,
                                         &tx_p2b_d1en,
                                         &tx_p2a_pten,
                                         &tx_p2a_d2en,
                                         &tx_p2a_d1en,
                                         &tx_p1b_d2en,
                                         &tx_p1b_d1en,
                                         &tx_p1a_pten,
                                         &tx_p1a_d2en,
                                         &tx_p1a_d1en,
                                         &tx_p3a_d1en,
                                         &tx_p2b_pten,
                                         &tx_p1b_pten);

    /* dump the settings for the current lane */
#if SERDES_CAL_VERBOSE == 1
    bc_printf("[%d] tx_p1a_d1en = 0x%02X\n", lane, tx_p1a_d1en);
    bc_printf("[%d] tx_p1a_d2en = 0x%02X\n", lane, tx_p1a_d2en);
    bc_printf("[%d] tx_p1b_d1en = 0x%02X\n", lane, tx_p1b_d1en);
    bc_printf("[%d] tx_p1b_d2en = 0x%02X\n", lane, tx_p1b_d2en);
    bc_printf("[%d] tx_p2a_d1en = 0x%02X\n", lane, tx_p2a_d1en);
    bc_printf("[%d] tx_p2a_d2en = 0x%02X\n", lane, tx_p2a_d2en);
    bc_printf("[%d] tx_p2b_d1en = 0x%02X\n", lane, tx_p2b_d1en);
    bc_printf("[%d] tx_p2b_d2en = 0x%02X\n", lane, tx_p2b_d2en);
    bc_printf("[%d] tx_p1a_pten = 0x%02X\n", lane, tx_p1a_pten);
    bc_printf("[%d] tx_p1b_pten = 0x%02X\n", lane, tx_p1b_pten);
    bc_printf("[%d] tx_p2a_pten = 0x%02X\n", lane, tx_p2a_pten);
    bc_printf("[%d] tx_p2b_pten = 0x%02X\n", lane, tx_p2b_pten);
    bc_printf("[%d] tx_p3a_d1en = 0x%02X\n", lane, tx_p3a_d1en);
#endif
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
                            ech_serdes_cdr_prop_gain_get(),
                            ech_serdes_cdr_integ_gain_get(),
                            tap_sel,
                            udfe_mode,
#if EXPLORER_HOST_SET_PH_OFS_T_PRELOAD == 1
                            ech_ph_ofs_t_preload_get(lane));
#else
                            SERDES_PH_OFS_T_PRELOAD_SETTING);
#endif

    return (PMC_SUCCESS);
}

/*
** Public Functions
*/

/**
* @brief
*   Initializes the serdes periodic calibration. The periodic
*   calibration is necessary for optimal performance with the
*   SerDes PRBS calibration sequence.
*  
* @param [in] serdes_cal_prbs: TRUE - SerDes PRBS cal sequence
*        has been run. FALSE otherwise.
*  
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID serdes_plat_periodic_cal_init(BOOL serdes_cal_prbs)
{

    serdes_cal_timer_init = FALSE;

    /* Not required on the non-PRBS sequence */
    if (!serdes_cal_prbs) 
    {
        return;
    }

    /* Start the periodic calibration */
    sys_timer_last_cal = sys_timer_read();
    serdes_cal_timer_init = TRUE;

    bc_printf("Configure SerDes PRBS Continuous IQ Offset cal to run every %lu ms\n", SERDES_CAL_UPDATE_PERIOD_MS);

}

/**
* @brief
*   Runs the periodic serdes calibration. This function should
*   be polled. Calibration will be run every 
*   SERDES_CAL_UPDATE_PERIOD_MS ms.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID serdes_plat_cal_update(VOID)
{

    UINT32 rc;
    UINT8 lane_bitmask;

    UINT_TIME curr_time = sys_timer_read();

    /*
    ** Check initialization. Run the calibration if it's 
    ** been longer than SERDES_CAL_UPDATE_PERIOD_MS.
    */
    if (serdes_cal_timer_init && !serdes_cal_timer_disable && 
        sys_timer_count_to_us(sys_timer_diff(sys_timer_last_cal,curr_time)) > SERDES_CAL_UPDATE_PERIOD_MS * 1000) 
    {
        sys_timer_last_cal = curr_time;

        lane_bitmask = ech_lane_active_pattern_bitmask_get();
        
        rc = SERDES_FH_IQ_Offset_Calibration(lane_bitmask);

        if (rc != PMC_SUCCESS)
        {
            /* Use doorbell 3 to indicate a failure to the host */
            bc_printf("SERDES_FH_IQ_Offset_Calibration failed: ERR!!! rc=0x%x\n",rc);
            ocmb_erep_db_ring(ocmb_erep_db_3);
        }

    }

}

/**
* @brief
*   Disables/re-enables the periodic serdes calibration.
* 
* @param [in] cal_disable: TRUE - Disable serdes periodic cal.
*        FALSE Enable periodic cal.
*  
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID serdes_plat_cal_disable(BOOL cal_disable)
{
    serdes_cal_timer_disable = cal_disable;
}

/**
* @brief
*   Low Level SerDes adapt step 1
*
* @param [in] dfe_state:   TRUE=DFE enabled; FALSE=DFE disabled 
* @param [in] adapt_state: TRUE=Adaptation enabled; FALSE=Adaptation disabled 
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized 
* @param [in] force_start: TRUE=Force the adaptation, used if
*        this function is called during boot config 0. FALSE=Do
*        not force the adaptation, used if this function is
*        called during boot config 1.
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_adapt_step1(BOOL dfe_state, BOOL adpt_state, UINT8 lane_bitmask, BOOL force_start)
{
    UINT32 i;
    UINT32 lane_offset  = 0;
    BOOL rc;
   
    for(i= 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
        {
            /* set the offset for the lane being configured */
            lane_offset = i * SERDES_LANE_REG_OFFSET;

            if( (FALSE == dfe_state)  && (FALSE == adpt_state) )
            {
                bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_start_adaptation_disable \n",i);
                rc = SERDES_FH_TXRX_Adaptation1_FW_start_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                            SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
            }

            else if( (FALSE == dfe_state)  && (TRUE == adpt_state) )
            {
                if (force_start) 
                {
                    bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_Force_start_dfe_disable \n", i);
                    rc = SERDES_FH_TXRX_Adaptation1_Force_start_dfe_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                   SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
                }
                else
                {
                    bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_start_dfe_disable \n", i);
                    rc = SERDES_FH_TXRX_Adaptation1_FW_start_dfe_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                         SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
                }

            }

            else if( (TRUE == dfe_state)  && (FALSE == adpt_state) )
            {

                bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_start_adaptation_disable \n",i);
                rc = SERDES_FH_TXRX_Adaptation1_FW_start_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                            SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
            }
            else
            {
                if (force_start) 
                {
                    bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_Force_start_normal \n", i);
                    rc = SERDES_FH_TXRX_Adaptation1_Force_start_normal(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                       SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
                }
                else
                {
                    bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_start_normal \n", i);
                    rc = SERDES_FH_TXRX_Adaptation1_FW_start_normal(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                    SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
                }
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
    }
    return (PMC_SUCCESS);

}

/**
* @brief
*   Low Level SerDes adaptation step 2
*  
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled 
* @param [in] adapt_state: TRUE=Adaptation enabled; FALSE=Adaptation disabled 
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_adapt_step2(BOOL dfe_state , BOOL adpt_state, UINT8 lane_bitmask)
{
    UINT32  i;  
    BOOL    rc;
    UINT32  lane_offset  = 0;
    UINT32  reg_val;

    /*
    **
    ** Query and report ADAPT_DONE_V0 for informational purposes.
    */
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
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
    }

    /* Apply SERDES_FH_TXRX_Adaptation2 to the all of the lanes. */          
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
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
    }
    return (PMC_SUCCESS);

}


/**
* @brief
*   Low Level SerDes init routine
*
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized
* @param [in] frequency: SerDes frequency
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled 
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_low_level_init(UINT8 lane_bitmask,
                                         UINT32 frequency,
                                         BOOL dfe_state)
{
    UINT32 current_lane;
    UINT32 rc;

    /* deassert serdes reset */
    top_exp_cfg_deassert_serdes_reset(TOP_XCBI_BASE_ADDR);

    serdes_plat_initialized_set(TRUE);

    for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
    {
        if (lane_bitmask & (1 << current_lane))
        {
            bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_1\n", current_lane);
            /* apply the first initialization sequence on all lanes */
            rc = serdes_plat_low_level_init_sequence_1(current_lane, frequency);

            if (PMC_SUCCESS != rc)
            {
                return (rc);
            }
        }
    }

    /* apply the second initialization sequence on all lanes */
    for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
    {
        if (lane_bitmask & (1 << current_lane))
        {
            bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_2\n", current_lane);
            rc = serdes_plat_low_level_init_sequence_2(current_lane, dfe_state);
            if (PMC_SUCCESS != rc)
            {
                bc_printf("[%d] serdes_plat_low_level_init_sequence_2 failed", current_lane);
                return (rc);
            }
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

    return (PMC_SUCCESS);
}

/**
* @brief
*   FW sequence to support lane inversion 
*
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized
* @param [in] lane_pattern_bitmask: Bitmask for the Serdes lanes to check rx pattern
* 
* @param [out] lane_pattern_bitmask: Bitmask of the lane
*        configuration
* 
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_lane_inversion_config(UINT8 lane_bitmask, UINT8 * lane_pattern_bitmask)
{   
    UINT32 lane_offset  = 0;    
    UINT32 i;    
    BOOL rc;
    top_plat_lock_struct lock_struct;

    bc_printf(" serdes_plat_lane_inversion_config: Sending Pattern A ..\n");
    
    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    /* Send Pattern A */
    ocmb_cfg_SendPatA(OCMB_REGS_BASE_ADDR);

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct); 

    /* Making sure that HOST has enough time to detect Pattern A */
    sys_timer_busy_wait_us(10000);

    bc_printf(" serdes_plat_lane_inversion_config: Running SERDES_FH_RX_alignment ..\n");
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
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
    }
    
    bc_printf(" serdes_plat_lane_inversion_config: Sending Pattern B to HOST ..\n");

    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    /* Send Pattern B back to the HOST*/
    ocmb_cfg_SendPatB(OCMB_REGS_BASE_ADDR);      

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);

    /* Making sure that HOST has enough time to detect Pattern B */
    sys_timer_busy_wait_us(10000);

    bc_printf(" serdes_plat_lane_inversion_config: waiting for Rx Pattern B ..\n");

    /* Wait for Pattern B. Pass the configured bitmask so that the
    ** same check can be performed as in ocmb_cfg_RxPatA
    */
    *lane_pattern_bitmask = lane_bitmask;
    rc = ocmb_cfg_RxPatB(OCMB_REGS_BASE_ADDR, lane_pattern_bitmask);

    if (TRUE != rc)
    {    
        bc_printf(" serdes_plat_lane_inversion_config:ocmb_cfg_RxPatB Timeout\n");
        return EXP_TWI_BOOT_CFG_SERDES_RX_PATB_TIMEOUT_FAIL;
    }
    
    return (PMC_SUCCESS);
}

/**
* @brief
 *  FW function to enable PRBS23 monitoring
 *  Check that the pattern monitor is locked. If it is not
 *  locked on a lane, try inverting it to compensate for boards
 *  where lanes could be inverted
*
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized 
* @param [in] ber_dwell_ms: Number of ms to accumulate bit  
*       errors
* @param [in] ber_per_ms_threshold: Threshold of bit errors per 
*       ms. If BER is less than this threshold, the pattern
*       monitor is considered locked
* @param [out] *pattmon_detected_bitmask: Pointer where a 
*       bitmask of the lanes which locked will be returned
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_pattmon_config(UINT8 lane_bitmask, UINT32 ber_dwell_ms, UINT32 ber_per_ms_threshold, UINT8 * pattmon_detected_bitmask)
{   

    UINT32 user_pattern[10] = { 0 };
    UINT32 meas_err_cnt[SERDES_LANES];
    UINT32 meas_err_cnt_per_ms[SERDES_LANES];
    UINT32 pattmon_detected_lanes = 0;
    UINT32 lane_offset;
    UINT32 current_lane;

    *pattmon_detected_bitmask = 0x00;

    /* Run two loops, inverting after the first if necessary */
    for (UINT32 pattmon_test = 0; pattmon_test < 2; pattmon_test++) 
    {
        /* Enable the pattern monitor */
        for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
        {
            if (lane_bitmask & (1 << current_lane))
            {
                /* set the offset for the lane being configured */
                lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                SERDES_FH_pattmon_en((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                                     5,
                                     user_pattern);
            }
        }

        /* Measure errors on the lanes */
        SERDES_FH_Meas_BER_Linking(lane_bitmask,
                                   ber_dwell_ms,
                                   meas_err_cnt,
                                   meas_err_cnt_per_ms);

        for (current_lane = 0; current_lane < SERDES_LANES; current_lane++)
        {
            if (lane_bitmask & (1 << current_lane))
            {
                if (meas_err_cnt_per_ms[current_lane] > ber_per_ms_threshold && pattmon_test > 0) 
                {
                    /* The pattern monitor did not detect PRBS in non-inverted or inverted */
                    bc_printf("    WARNING: Pattern monitor did not detect PRBS on lane %lu\n", current_lane);
                }
                else if (meas_err_cnt_per_ms[current_lane] > SERDES_BER_21G33_PER_MS_1e_4 && 0 == pattmon_test) 
                {
                    /* Invert the pattern monitor and try again */
                    bc_printf("    Inverting PRBS monitor on lane %lu\n", current_lane);

                    lane_offset = current_lane * SERDES_LANE_REG_OFFSET;
                    serdes_api_lane_invert_set(lane_offset);
                }
                else if (meas_err_cnt_per_ms[current_lane] < SERDES_BER_21G33_PER_MS_1e_4 && pattmon_test > 0) 
                {
                    *pattmon_detected_bitmask |= (1 << current_lane);
                    pattmon_detected_lanes++;
                }
            }
        }
    }

    bc_printf("    PRBS monitor detecting PRBS on %lu lanes 0x%02x\n", pattmon_detected_lanes, *pattmon_detected_bitmask);

    return PMC_SUCCESS;

}

/**
* @brief
*   Low Level SerDes standalone init routine for Loopback test
*
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized
* @param [in] dfe_state: TRUE=DFE enabled; FALSE=DFE disabled 
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
*   Assumption is SerDes will be put in external loopback mode
*/
PUBLIC UINT32 serdes_plat_low_level_standalone_init(UINT8 lane_bitmask, BOOL dfe_state)
{
    UINT32 lane_offset  = 0;
    UINT32 i;    
    UINT32 patt[10];
    UINT32 rc;
    memset(&patt[0], 0, sizeof(patt));

    /* enable PRBS generation across supported lanes */
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
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
    }    
    /* Start adaption for lane 4 only.*/
    if (dfe_state)
    {
        rc = SERDES_FH_TXRX_Adaptation1_Force_start_normal(SERDES_ADSP_PCBI_BASE_ADDR + (4 * SERDES_LANE_REG_OFFSET),
                                                           SERDES_MTSB_CTRL_PCBI_BASE_ADDR + (4 * SERDES_LANE_REG_OFFSET));
    }
    else
    {
        rc = SERDES_FH_TXRX_Adaptation1_Force_start_dfe_disable(SERDES_ADSP_PCBI_BASE_ADDR + (4 * SERDES_LANE_REG_OFFSET),
                                                                SERDES_MTSB_CTRL_PCBI_BASE_ADDR + (4 * SERDES_LANE_REG_OFFSET));
    }

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
        if (lane_bitmask & (1 << i))
        {
            if(i == SERDES_LANE_4)
            {
                bc_printf("Skipping lane 4\n");
                continue;
            }
            bc_printf("Lane ID= %d\n",i);

            /* set the offset for the lane being configured */
            lane_offset = i * SERDES_LANE_REG_OFFSET;

            rc = SERDES_FH_TXRX_Adaptation1_Force_start_normal(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                               SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
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
    }

    return PMC_SUCCESS;

}

/**
* @brief
*  C function for SerDes Loopback test
*
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
*   Assumption is SerDes will be put in external loopback mode
*/
PUBLIC UINT8 serdes_plat_loopback_test(UINT8 lane_bitmask)
{
    UINT32 i;    
    UINT32 lane_offset  = 0;    
    UINT32 patt[10];
    BOOL rc;
    UINT8 return_code = PMC_SUCCESS;

    /* Initialize PRBS user pattern array to 0 */
    memset(&patt[0], 0, sizeof(patt));
    /* enable PRBS monitoring across supported lanes */
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
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
    }

    /*
    ** PRBS-23 pattern has been enabled in serdes_plat_low_level_standalone_init.
    ** In this function, pattern monitor has been enabled first. SERDES_FH_pattmon is called
    ** to check that pattern is recived or not.
    */
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
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
    }    
    return return_code;
}

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
    crash_dump_register(CRASH_DUMP_SET_0, "SERDES_REGS", &serdes_dump_debug_info, CRASH_DUMP_ASCII, SERDES_REG_CRASH_DUMP_SIZE);
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

/**
* @brief
*    Get SERDES FFE pre-cursor value.
*
* @return
*   Nothing
*
* @note
*
*/
PUBLIC UINT32 serdes_plat_ffe_precursor_get(VOID)
{
    return serdes_ffe_precursor;
}

/**
* @brief
*    Set SERDES FFE pre-cursor value.
*
* @return
*   Nothing
*
* @note
*
*/
PUBLIC VOID serdes_plat_ffe_precursor_set(UINT32 precursor)
{
    serdes_ffe_precursor = precursor;
}

/**
* @brief
*    Get SERDES FFE post-cursor value.
*
* @return
*   Nothing
*
* @note
*
*/
PUBLIC UINT32 serdes_plat_ffe_postcursor_get(VOID)
{
    return serdes_ffe_postcursor;
}

/**
* @brief
*    Set SERDES FFE post-cursor value.
*
* @return
*   Nothing
*
* @note
*
*/
PUBLIC VOID serdes_plat_ffe_postcursor_set(UINT32 postcursor)
{
    serdes_ffe_postcursor = postcursor;
}

/**
* @brief
*    Get SERDES FFE post-cursor value.
*
* @return
*   Nothing
*
* @note
*
*/
PUBLIC UINT32 serdes_plat_ffe_calibration_get(VOID)
{
    return serdes_ffe_calibration;
}

/**
* @brief
*    Set SERDES FFE post-cursor value.
*
* @return
*   Nothing
*
* @note
*
*/
PUBLIC VOID serdes_plat_ffe_calibration_set(UINT32 calibration)
{
    serdes_ffe_calibration = calibration;
}

/** @} end group */
