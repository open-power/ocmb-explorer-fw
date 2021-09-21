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
#include "ocmb_config_guide_mchp.h"
#include "ocmb_config_guide.h"
#include "bc_printf.h"


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

/* SERDES_PI_PHGEN_IQCOR_CFG2 data and timing selection */
#define SERDES_PI_PHGEN_IQCOR_CFG2_DATA_ENABLED_TIMING_DISABLED     0x01
#define SERDES_PI_PHGEN_IQCOR_CFG2_DATA_DISABLED_TIMING_ENABLED     0x02

#define SERDES_IQ_MAX_NEGATIVE      -16
#define SERDES_IQ_MAX_POSITIVE      15

/* SERDES rtrim_34_15 validity masks */
#define SERDES_RTRIM_34_15_BIT6_VALID       0x40
#define SERDES_RTRIM_34_15_MASK             0x00FF

PRIVATE BOOL serdes_initialized = FALSE;

/* SerDes FFE settings */
#define SERDES_FFE_DEFAULT_PRECURSOR    0
#define SERDES_FFE_MIN_PRECURSOR        0
#define SERDES_FFE_MAX_PRECURSOR        32
PRIVATE UINT32 serdes_ffe_precursor = SERDES_FFE_DEFAULT_PRECURSOR;

#define SERDES_FFE_DEFAULT_POSTCURSOR   0
#define SERDES_FFE_MIN_POSTCURSOR       0
#define SERDES_FFE_MAX_POSTCURSOR       32
PRIVATE UINT32 serdes_ffe_postcursor = SERDES_FFE_DEFAULT_POSTCURSOR;

#define SERDES_FFE_DEFAULT_CALIBRATION  0x28
#define SERDES_FFE_MIN_CALIBRATION      64
#define SERDES_FFE_MAX_CALIBRATION      80
PRIVATE UINT32 serdes_fuse_val_stat_1 = 0;
PRIVATE UINT32 serdes_ffe_calibration = SERDES_FFE_MIN_CALIBRATION;

#define SERDES_FFE_DISABLE_TX_PARALLEL_TERMINATION  0


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
    SERDES_FH_PGA_init(SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);

    /* determine if the host provided non-default FFE settings */
    if ((SERDES_FFE_DEFAULT_PRECURSOR == serdes_ffe_precursor) ||
        (SERDES_FFE_DEFAULT_POSTCURSOR == serdes_ffe_postcursor))
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
        /* host has provided non-default FFE pre- and/or post-cursor values
        ** calculate FFE settings based on code provided by mixed signal 
        */ 

        /* determine if a valid fuse value setting should be used */

        /* read rtrim settings */
        SERDES_FH_read_rtrim((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                             &rtrim_14_0,
                             &rtrim_34_15);

        if (SERDES_RTRIM_34_15_BIT6_VALID == (rtrim_34_15 & SERDES_RTRIM_34_15_BIT6_VALID))
        {
            /* valid setting in fuse register, use FUSE_VAL_STAT_1:RTRIM_34_15 */
            serdes_fuse_val_stat_1 = (rtrim_34_15 & ~SERDES_RTRIM_34_15_BIT6_VALID) & SERDES_RTRIM_34_15_MASK;
        }

        /* 
        ** set the FFE calibration value as per mixed signal:
        ** CAL (in decimal) = 64 + TX3_P3A_D1EN[5]*6 + TX3_P3A_D1EN[4]*4 + TX3_P3A_D1EN[3]*2
        **                    + TX3_P3A_D1EN[2]*2 + TX3_P3A_D1EN[1]*1 + TX3_P3A_D1EN[0]*1
        **  
        ** serdes_ffe_fuse_val_stat_1 can be set to TX3_P3A_D1EN but a value for TX3_P3A_D1EN 
        ** is re-calculated as one of the set of the 13 6-bit values. The calculated value is 
        ** usually the same as serdes_ffe_fuse_val_stat_1. 
        */ 
        serdes_ffe_calibration = SERDES_FFE_MIN_CALIBRATION +
                                 (((serdes_fuse_val_stat_1 & 0x20) >> 5) * 6) +
                                 (((serdes_fuse_val_stat_1 & 0x10) >> 4) * 4) +
                                 (((serdes_fuse_val_stat_1 & 0x08) >> 3) * 2) +
                                 (((serdes_fuse_val_stat_1 & 0x04) >> 2) * 2) +
                                 (((serdes_fuse_val_stat_1 & 0x02) >> 1) * 1) +
                                 (((serdes_fuse_val_stat_1 & 0x01) >> 0) * 1);


        /* 
        ** calculate the 13 6-bit words that control the TX amplitude
        ** the encoding scheme is set to have maxswing on the TX output
        */
        serdes_plat_txctrls_maxswing(serdes_ffe_precursor,
                                     serdes_ffe_postcursor,
                                     serdes_ffe_calibration,
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
*   Low Level SerDes Data and Timing IQ Offset Calibration
*
* @param [in] lanes: Number of lanes being configured
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
#if EXPLORER_SERDES_D_T_IQ_CALIBRATION_DEBUG == 0
PUBLIC UINT32 serdes_plat_iq_offset_calibration(UINT8 lane_bitmask)
{
    INT32 d_iq_offset;
    INT32 t_iq_offset;
    UINT32 iq_accum_reg_val;
    INT16 iq_accum;
    INT16 prev_iq_accum;
    UINT32 iq_done;
    BOOL first_measurement;
    UINT32 lane_offset = 0;

    for (UINT32 current_lane = 0; current_lane < SERDES_LANES; current_lane++)
    {
        if (lane_bitmask & (1 << current_lane))
        {
            bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_iq_offset_calibration()\n", current_lane);

            /* set the offset for the lane being configured */
            lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

            /* capture starting correction offsets */
            d_iq_offset = serdes_api_d_iq_offset_read(lane_offset);
            t_iq_offset = serdes_api_t_iq_offset_read(lane_offset);

            /* select current lane for offset correction */
            if (FALSE == SERDES_FH_IQ_Offset_Calibration_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                            SERDES_PI_PHGEN_IQCOR_CFG2_DATA_ENABLED_TIMING_DISABLED))
            {
                /* calibration1 failed */
                bc_printf("    SERDES_FH_IQ_Offset_Calibration_1() failed\n");
                return (EXP_SERDES_DATA_IQ_OFFSET_CALIBRATION_1_FAILED);
            }

#if 0
            /* 
            ** as per config guide initial value of iq_done = 1 
            ** not needed, see modifications to SERDES_FH_IQ_Offset_Calibration_2() 
            */
            iq_done = 1;
#endif

            /* do not check for convergence on first measurement */
            first_measurement = TRUE;

            while (TRUE)
            {
                /* set data and timing correction offsets, data calibration enabled, timing calibration disabled, and utility unused */
                if (FALSE == SERDES_FH_IQ_Offset_Calibration_3((SERDES_MDSP_PCBI_BASE_ADDR + lane_offset),
                                                               d_iq_offset,
                                                               TRUE,
                                                               t_iq_offset,
                                                               FALSE,
                                                               0))
                {
                    /* calibration3 failed */
                    bc_printf("    SERDES_FH_IQ_Offset_Calibration_3() failed\n");
                    return (EXP_SERDES_DATA_IQ_OFFSET_CALIBRATION_3_FAILED);
                }

                /* run calibration and find direction to move offset */
                if (FALSE == SERDES_FH_IQ_Offset_Calibration_2((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                               iq_done,
                                                               &iq_done,
                                                               &iq_accum_reg_val))
                {
                    /* calibration2 failed */
                    bc_printf("    SERDES_FH_IQ_Offset_Calibration_2() data IQ calibration timed out\n");
                    return (EXP_SERDES_DATA_IQ_OFFSET_CALIBRATION_2_FAILED);
                }

                /* convert the iq_accum register value from unsigend 32-bit to signed 16-bit signed */
                iq_accum = (INT16)iq_accum_reg_val;

                /* do not check for convergence on first measurement */
                if (FALSE == first_measurement)
                {
                    /* check for convergence, signaled by sign change */
                    if (((prev_iq_accum < 0) && (iq_accum > 0)) ||
                        ((prev_iq_accum > 0) && (iq_accum < 0)))
                    {
                        /* data IQ phase has converged */
                        break;
                    }
                }
                else
                {
                    /* first measurement taken */
                    first_measurement = FALSE;
                }

                /* move data offset accordingly */
                if (iq_accum > 0)
                {
                    d_iq_offset += -1;
                }
                else if (iq_accum < 0)
                {
                    d_iq_offset += 1;
                }

                /* check if new correction offset is out of range */
                if ((d_iq_offset < SERDES_IQ_MAX_NEGATIVE) ||
                    (d_iq_offset > SERDES_IQ_MAX_POSITIVE))
                {
                    bc_printf("    data IQ offset is out of range = %d\n", d_iq_offset);
                    return (EXP_SERDES_DATA_IQ_OFFSET_OUT_OF_RANGE);
                }

                /* record iq_accum for next comparison */
                prev_iq_accum = iq_accum;
            }

#if EXPLORER_HOST_SET_D_IQ_OFFSET == 1
            if (ech_d_iq_offset_use_host_get() != 0)
            {
                /* overwrite D_IQ phase convergence result with host value */
                bc_printf("    lane %u hardware converged data IQ phase offset = %d\n", 
                          current_lane, d_iq_offset);
                bc_printf("    lane %u setting host specified IQ phase offset  = %d\n", 
                          current_lane, ech_d_iq_offset_get(current_lane));

                /* use host values to set register field */
                serdes_api_d_iq_offset_write(lane_offset, ech_d_iq_offset_get(current_lane));
            }
#endif

            /* select timing for offset correction */
            if ( FALSE == SERDES_FH_IQ_Offset_Calibration_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                            SERDES_PI_PHGEN_IQCOR_CFG2_DATA_DISABLED_TIMING_ENABLED))
            {
                /* calibration failed */
                bc_printf("    SERDES_FH_IQ_Offset_Calibration_1() failed\n");
                return (EXP_SERDES_TIMING_IQ_OFFSET_CALIBRATION_1_FAILED);
            }

#if 0
            /* 
            ** as per config guide initial value of iq_done = 1 
            ** not needed, see modifications to SERDES_FH_IQ_Offset_Calibration_2() 
            */
            iq_done = 1;
#endif

            /* do not check for convergence on first measurement */
            first_measurement = TRUE;

            while (TRUE)
            {
                /* set data and timing correction offsets, timing calibration enabled, data calibration disabled, and utility unused */
                if (FALSE == SERDES_FH_IQ_Offset_Calibration_3((SERDES_MDSP_PCBI_BASE_ADDR + lane_offset),
                                                               d_iq_offset,
                                                               FALSE,
                                                               t_iq_offset,
                                                               TRUE,
                                                               0))
                {
                    /* calibration failed */
                    bc_printf("    SERDES_FH_IQ_Offset_Calibration_3() failed\n");
                    return (EXP_SERDES_TIMING_IQ_OFFSET_CALIBRATION_3_FAILED);
                }

                /* run calibration and find direction to move offset */
                if (FALSE == SERDES_FH_IQ_Offset_Calibration_2((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                               iq_done,
                                                               &iq_done,
                                                               &iq_accum_reg_val))
                {
                    /* calibration failed */
                    bc_printf("    SERDES_FH_IQ_Offset_Calibration_2() failed\n");
                    return (EXP_SERDES_TIMING_IQ_OFFSET_CALIBRATION_2_FAILED);
                }

                /* convert the iq_accum register value from unsigned 32-bit to signed 16-bit signed */
                iq_accum = (INT16)iq_accum_reg_val;

                /* do not check for convergence on first measurement */
                if (FALSE == first_measurement)
                {
                    /* check for convergence, signaled by sign change */
                    if (((prev_iq_accum < 0) && (iq_accum > 0)) ||
                        ((prev_iq_accum > 0) && (iq_accum < 0)))
                    {
                        /* timing IQ phase has converged */
                        break;
                    }
                }
                else
                {
                    /* first measurement taken */
                    first_measurement = FALSE;
                }

                /* move data offset accordingly */
                if (iq_accum > 0)
                {
                    t_iq_offset += -1;
                }
                else if (iq_accum < 0)
                {
                    t_iq_offset += 1;
                }

                /* check if new correction offset is out of range */
                if ((t_iq_offset < SERDES_IQ_MAX_NEGATIVE) ||
                    (t_iq_offset > SERDES_IQ_MAX_POSITIVE))
                {
                    bc_printf("    timing IQ offset is out of range = %d\n", t_iq_offset);
                    return (EXP_SERDES_TIMING_IQ_OFFSET_OUT_OF_RANGE);
                }

                /* record iq_accum for next comparison */
                prev_iq_accum = iq_accum;
            }

            if (FALSE == SERDES_FH_IQ_Offset_Calibration_4(SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset))
            {
                /* calibration2 failed */
                bc_printf("    SERDES_FH_IQ_Offset_Calibration_4() failed\n");
                return (EXP_SERDES_IQ_OFFSET_CALIBRATION_4_FAILED);
            }
        }
    }

    return (PMC_SUCCESS);
}
#else
PUBLIC UINT32 serdes_plat_iq_offset_calibration(UINT8 lane_bitmask)
{
    INT32 d_iq_offset;
    INT32 t_iq_offset;
    UINT32 iq_accum_reg_val;
    INT16 iq_accum;
    INT16 prev_iq_accum;
    UINT32 iq_done;
    BOOL first_measurement;
    UINT32 lane_offset = 0;
    UINT32 convergence_iteration;
    INT32 iq_field;

    for (UINT32 test_loop = 0; test_loop < 10; test_loop++)
    {
        bc_printf("\n\nTest Loop %d\n\n", test_loop);

        for (UINT32 current_lane = 0; current_lane < SERDES_LANES; current_lane++)
        {
            if (lane_bitmask & (1 << current_lane))
            {
                bc_printf("\n[%d] TWI_BOOT_CONFIG: serdes_plat_iq_offset_calibration()\n", current_lane);

                /* set the offset for the lane being configured */
                lane_offset = current_lane * SERDES_LANE_REG_OFFSET;

                /* capture starting correction offsets */
                d_iq_offset = serdes_api_d_iq_offset_read(lane_offset);
                t_iq_offset = serdes_api_t_iq_offset_read(lane_offset);

                bc_printf("    start d_iq_offset = 0x%02X = %d\n", d_iq_offset, d_iq_offset);
                bc_printf("    start t_iq_offset = 0x%02X = %d\n", t_iq_offset, t_iq_offset);

                /* select current lane data for offset correction */
                if (FALSE == SERDES_FH_IQ_Offset_Calibration_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                               SERDES_PI_PHGEN_IQCOR_CFG2_DATA_ENABLED_TIMING_DISABLED))
                {
                    /* calibration 1 failed */
                    bc_printf("    SERDES_FH_IQ_Offset_Calibration_1() failed\n");
                    return (EXP_SERDES_DATA_IQ_OFFSET_CALIBRATION_1_FAILED);
                }

#if 0
                /* 
                ** as per config guide initial value of iq_done = 1 
                ** not needed, see modifications to SERDES_FH_IQ_Offset_Calibration_2() 
                */
                iq_done = 1;
#endif

                /* do not check for convergence on first measurement */
                first_measurement = TRUE;

                convergence_iteration = 0;
                while (TRUE)
                {
                    /* set data and timing correction offsets, data calibration enabled, timing calibration disabled, and utility unused */
                    if (FALSE == SERDES_FH_IQ_Offset_Calibration_3((SERDES_MDSP_PCBI_BASE_ADDR + lane_offset),
                                                                   d_iq_offset,
                                                                   TRUE,
                                                                   t_iq_offset,
                                                                   FALSE,
                                                                   0))
                    {
                      /* calibration 3 failed */
                      bc_printf("    SERDES_FH_IQ_Offset_Calibration_3() failed\n");
                      return (EXP_SERDES_DATA_IQ_OFFSET_CALIBRATION_3_FAILED);
                    }

                    /* run calibration and find direction to move offset */
                    if (FALSE == SERDES_FH_IQ_Offset_Calibration_2((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                                   iq_done,
                                                                   &iq_done,
                                                                   &iq_accum_reg_val))
                    {
                        /* calibration 2 failed */
                        bc_printf("    SERDES_FH_IQ_Offset_Calibration_2() failed\n");
                        return (EXP_SERDES_DATA_IQ_OFFSET_CALIBRATION_2_FAILED);
                    }

                    /* read the data IQ offset field from the register */
                    iq_field = serdes_api_d_iq_offset_read(lane_offset);
    	 	 	
                    /* convert the iq_accum register value from unsigend 32-bit to signed 16-bit signed */
                    iq_accum = (INT16)iq_accum_reg_val;

                    bc_printf("    iteration = %d\n", convergence_iteration);
                    bc_printf("    TR_CONFIG5_REG d_iq_offset = %d\n", iq_field);
                    bc_printf("    iq_done = 0x%02X    ~iq_don = 0x%02X\n", iq_done, ~iq_done);
                    bc_printf("    iq_accum_reg_val = 0x%08X = %d    iq_accum = 0x%04X = %d\n", 
                              iq_accum_reg_val, iq_accum_reg_val, iq_accum, iq_accum);

                    /* do not check for convergence on first measurement */
                    if (FALSE == first_measurement)
                    {
                        bc_printf ("    prev_iq_accum = %d    iq_accum = %d\n", prev_iq_accum, iq_accum);

                        /* check for convergence, signaled by sign change */
                        if (((prev_iq_accum < 0) && (iq_accum > 0)) ||
                            ((prev_iq_accum > 0) && (iq_accum < 0)))
                        {
                            bc_printf("    data IQ converged\n");
                            bc_printf("    final lane [%d] d_iq_offset = %d\n", current_lane, d_iq_offset);
                            break;
                        }
                    }
                    else
                    {
                        /* first measurement taken */
                        first_measurement = FALSE;
                    }

                    /* move data offset accordingly */
                    if (iq_accum > 0)
                    {
                        d_iq_offset += -1;
                    }
                    else if (iq_accum < 0)
                    {
                        d_iq_offset += 1;
                    }
                    bc_printf("    d_iq_offset = %d\n", d_iq_offset);

                    /* check if new correction offset is out of range */
                    if ((d_iq_offset < SERDES_IQ_MAX_NEGATIVE) ||
                        (d_iq_offset > SERDES_IQ_MAX_POSITIVE))
                    {
                        bc_printf("    data IQ offset is out of range = %d\n", d_iq_offset);
                        return (EXP_SERDES_DATA_IQ_OFFSET_OUT_OF_RANGE);
                    }

                    /* record iq_accum for next comparison */
                    prev_iq_accum = iq_accum;
                    convergence_iteration++;
                }

#if EXPLORER_HOST_SET_D_IQ_OFFSET == 1
                if (ech_d_iq_offset_use_host_get() != 0)
                {
                    /* overwrite D_IQ phase convergence result with host value */
                    bc_printf("    lane %u hardware converged data IQ phase offset = %d\n", 
                              current_lane, d_iq_offset);
                    bc_printf("    lane %u setting host specified IQ phase offset  = %d\n", 
                              current_lane, ech_d_iq_offset_get(current_lane));

                    /* use host values to set register field */
                    serdes_api_d_iq_offset_write(lane_offset, ech_d_iq_offset_get(current_lane));
                }
#endif

                /* select timing for offset correction */
                if (FALSE == SERDES_FH_IQ_Offset_Calibration_1((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                               SERDES_PI_PHGEN_IQCOR_CFG2_DATA_DISABLED_TIMING_ENABLED))
                {
                    /* calibration 1 failed  */
                    bc_printf("    SERDES_FH_IQ_Offset_Calibration_1() failed\n");
                    return (EXP_SERDES_TIMING_IQ_OFFSET_CALIBRATION_1_FAILED);
                }

#if 0
                /* 
                ** as per config guide initial value of iq_done = 1 
                ** not needed, see modifications to SERDES_FH_IQ_Offset_Calibration_2() 
                */
                iq_done = 1;
#endif

                /* do not check for convergence on first measurement */
                first_measurement = TRUE;

                convergence_iteration = 0;
                while (TRUE)
                {
                    /* set data and timing correction offsets, timing calibration enabled, data calibration disabled, and utility unused */
                    if (FALSE == SERDES_FH_IQ_Offset_Calibration_3((SERDES_MDSP_PCBI_BASE_ADDR + lane_offset),
                                                                    d_iq_offset,
                                                                    FALSE,
                                                                    t_iq_offset,
                                                                    TRUE,
                                                                    0))
                    {
                      /* calibration 3 failed  */
                      bc_printf("    SERDES_FH_IQ_Offset_Calibration_3() failed\n");
                      return (EXP_SERDES_TIMING_IQ_OFFSET_CALIBRATION_3_FAILED);
                    }

                    /* run calibration and find direction to move offset */
                    if (FALSE == SERDES_FH_IQ_Offset_Calibration_2((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                                                   iq_done,
                                                                   &iq_done,
                                                                   &iq_accum_reg_val))
                    {
                        /* calibration 2 failed  */
                        bc_printf("    SERDES_FH_IQ_Offset_Calibration_2() failed\n");
                        return (EXP_SERDES_TIMING_IQ_OFFSET_CALIBRATION_2_FAILED);
                    }

                    /* read the timing IQ offset field from the register */
                    iq_field = serdes_api_t_iq_offset_read(lane_offset);
    	 	 	
                    /* convert the iq_accum register value from unsigend 32-bit to signed 16-bit signed */
                    iq_accum = (INT16)iq_accum_reg_val;

                    bc_printf("    iteration = %d\n", convergence_iteration);
                    bc_printf("    TR_CONFIG5_REG t_iq_offset = %d\n", iq_field);
                    bc_printf("    iq_done = 0x%02X    ~iq_done = 0x%02X\n", iq_done, ~iq_done);
                    bc_printf("    iq_accum_reg_val = 0x%08X = %d    iq_accum = 0x%04X = %d\n",
                              iq_accum_reg_val, iq_accum_reg_val, iq_accum, iq_accum);

                    /* do not check for convergence on first measurement */
                    if (FALSE == first_measurement)
                    {
                        bc_printf ("    prev_iq_accum = %d    iq_accum = %d\n", prev_iq_accum, iq_accum);

                        /* check for convergence, signaled by sign change */
                        if (((prev_iq_accum < 0) && (iq_accum > 0)) ||
                            ((prev_iq_accum > 0) && (iq_accum < 0)))
                        {
                            bc_printf("    timing IQ converged\n");
                            bc_printf("    final lane [%d] t_iq_offset = %d\n", current_lane, t_iq_offset);
                            break;
                        }
                    }
                    else
                    {
                        /* first measurement taken */
                        first_measurement = FALSE;
                    }

                    /* move data offset accordingly */
                    if (iq_accum > 0)
                    {
                        t_iq_offset += -1;
                    }
                    else if (iq_accum < 0)
                    {
                        t_iq_offset += 1;
                    }
                    bc_printf("    t_iq_offset = %d\n", t_iq_offset);

                    /* check if new correction offset is out of range */
                    if ((t_iq_offset < SERDES_IQ_MAX_NEGATIVE) ||
                        (t_iq_offset > SERDES_IQ_MAX_POSITIVE))
                    {
                        bc_printf("    timing IQ offset is out of range = %d\n", t_iq_offset);
                        return (EXP_SERDES_TIMING_IQ_OFFSET_OUT_OF_RANGE);
                    }

                    /* record iq_accum for next comparison */
                    prev_iq_accum = iq_accum;
                    convergence_iteration++;
                }
            }

            if (FALSE == SERDES_FH_IQ_Offset_Calibration_4(SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset))
            {
                /* calibration 4 failed  */
                bc_printf("    SERDES_FH_IQ_Offset_Calibration_4() failed\n");
                return (EXP_SERDES_IQ_OFFSET_CALIBRATION_4_FAILED);
            }
        }
    }

    return (PMC_SUCCESS);
}
#endif

/**
* @brief
*   Low Level SerDes adapt step 1
*
* @param [in] dfe_state:   TRUE=DFE enabled; FALSE=DFE disabled 
* @param [in] adapt_state: TRUE=Adaptation enabled; FALSE=Adaptation disabled 
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_adapt_step1(BOOL dfe_state, BOOL adpt_state, UINT8 lane_bitmask)
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
                bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable \n",i);
                rc = SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                      SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
            }

            else if( (FALSE == dfe_state)  && (TRUE == adpt_state) )
            {

                bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_dfe_disable \n",i);
                rc = SERDES_FH_TXRX_Adaptation1_FW_dfe_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                               SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
            }

            else if( (TRUE == dfe_state)  && (FALSE == adpt_state) )
            {

                bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable \n",i);
                rc = SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
                                                                      SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset);
            }
            else
            {
                bc_printf("[%d] Calling SERDES_FH_TXRX_Adaptation1_FW_normal \n",i);
                rc = SERDES_FH_TXRX_Adaptation1_FW_normal(SERDES_ADSP_PCBI_BASE_ADDR + lane_offset,
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
    UINT32 i;
    UINT32 rc;
    UINT32 lane_offset = 0;

    /* deassert serdes reset */
    top_exp_cfg_deassert_serdes_reset(TOP_XCBI_BASE_ADDR);

    serdes_plat_initialized_set(TRUE);

    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
        {
            bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_1\n", i);
            /* apply the first initialization sequence on all lanes */
            rc = serdes_plat_low_level_init_sequence_1(i, frequency);

            if (PMC_SUCCESS != rc)
            {
                return (rc);
            }
        }
    }

    /* apply the second initialization sequence on all lanes */
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
        {
            bc_printf("[%d] TWI_BOOT_CONFIG: serdes_plat_low_level_init_sequence_2\n", i);
            rc = serdes_plat_low_level_init_sequence_2(i, dfe_state);
            if (PMC_SUCCESS != rc)
            {
                bc_printf("[%d] serdes_plat_low_level_init_sequence_2 failed", i);
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
    
    /* initialize lane alignment part 1 to all lanes*/
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
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
    }
    
    for (i = 0; i < SERDES_LANES; i++)
    {
        if (lane_bitmask & (1 << i))
        {
            /* set the offset for the lane being configured */
            lane_offset = i * SERDES_LANE_REG_OFFSET;
            /* initialize lane alignment part 2 on all lanes except master lane 4 */
            if (i != SERDES_LANE_4)
            {
                rc = SERDES_FH_TX_alignment(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);
                if (TRUE != rc)
                {
                    bc_printf("[%d] EXP_SERDES_FH_TX_ALIGNMENT_FAIL\n", i);
                    return EXP_SERDES_FH_TX_ALIGNMENT_FAIL;
                }
            }
        }
    }
    return (PMC_SUCCESS);
}

/**
* @brief
*   FW sequence to support lane inversion 
*
* @param [in] lane_bitmask: Bitmask for the Serdes lanes, which will be initialized
* @param [in] lane_pattern_bitmask: Bitmask for the Serdes lanes to check rx pattern
*  
* @return
*   PMC_SUCCESS for SUCCESS, otherwise error codes.
* 
* @note
* 
*/
PUBLIC UINT32 serdes_plat_lane_inversion_config(UINT8 lane_bitmask, UINT8 lane_pattern_bitmask)
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
    /* Send Pattern B back to the HOST*/
    ocmb_cfg_SendPatB(OCMB_REGS_BASE_ADDR);      
    /* Making sure that HOST has enough time to detect Pattern B */
    sys_timer_busy_wait_us(10000);

    bc_printf(" serdes_plat_lane_inversion_config: waiting for Rx Pattern B ..\n");
    /* Wait for Pattern B*/
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
    UINT32 obj_en_pass3;
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

/** @} end group */
