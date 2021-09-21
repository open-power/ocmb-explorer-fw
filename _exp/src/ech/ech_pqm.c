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
*   Explorer Command Handler.
*
* @note
*/

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_api.h"
#include "ech.h"
#include "twi_api.h"
#include "pmc_hw_base.h"
#include "top_exp_config_guide.h"
#include "serdes_config_guide.h"
#include <string.h>
#include "serdes_plat.h"
#include "ddr_phy.h"
#include "ech_loc.h"
#include "ocmb_config_guide.h"
#include "ocmb_config_guide_mchp.h"
#include "app_fw.h"

/*
* Local Enumerated Types
*/


/*
** Local Constants
*/

#define ECH_PBRS_USER_PATTERNS          10

/* offset of data in received TWI packets */
#define ECH_PQM_LANE_SET_DATA_OFFSET            2
#define ECH_PQM_FREQ_SET_DATA_OFFSET            2
#define ECH_PQM_PRBS_PATTERN_SET_DATA_OFFSET    2

/**
 * This is the maximum value for the bathtub capture as advised 
 * by the hardware team in PREP 379190. 
 */
#define ECH_PQM_BATHTUB_CAPTURE_TIME_LIMIT_MAX_PREP379190   0xFFFFFFFFF0

/*
** Local Structures and Unions
*/

/** 
* @brief 
*   structure for local record of host set config params
*/  
typedef struct
{
    UINT8  lanes;        /**< number of lanes to enable: 1, 4, or 8 */
    UINT8  freq;         /**< operating frequency: 1 = 21.33, 2 = 23.46, 3 = 25.6 GBPS */
    UINT8  lane_bitmask; /**< bitmask for the enabled lanes */
    UINT8  lane_rx_pattern_bitmask; /**< bitmask for the lanes to check pattern */
} ech_pqm_cfg_struct;


/*
** Forward declarations
*/
EXTERN UINT8 ech_twi_deferred_cmd_buf[EXP_TWI_MAX_BUF_SIZE];

/*
** Global Variables
*/


/*
** Local Variables
*/

PRIVATE ech_pqm_cfg_struct ech_pqm_cfg;
PRIVATE exp_pqm_data_struct ech_pqm_data;
PRIVATE exp_pqm_prbs_pattern_struct ech_pqm_prbs;
PRIVATE exp_pqm_prbs_user_pattern_struct ech_pqm_user_prbs;
PRIVATE exp_pqm_prbs_error_count_struct ech_pqm_err;
PRIVATE exp_pqm_horz_bt_struct horz_bt;
PRIVATE exp_pqm_vert_bt_struct vert_bt;

/*
** Private Functions
*/

/**
* @brief
*   Process TWI PQM Lane Set command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_lane_set(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    if (rx_buf_ptr[rx_index + ECH_PQM_LANE_SET_DATA_OFFSET] > EXP_SERDES_8_LANE)
    {
        /* lane configuration is out of range */
        bc_printf("ERROR: PQM lane_set lane configuration is out of range\n");
        ech_twi_status_byte_set(EXP_SERDES_LANE_OOR);
    }
    else if ((rx_buf_ptr[rx_index + ECH_PQM_LANE_SET_DATA_OFFSET] != EXP_SERDES_1_LANE) &&
             (rx_buf_ptr[rx_index + ECH_PQM_LANE_SET_DATA_OFFSET] != EXP_SERDES_4_LANE) &&
             (rx_buf_ptr[rx_index + ECH_PQM_LANE_SET_DATA_OFFSET] != EXP_SERDES_8_LANE))
    {
        /* unsupported lane configuration */
        bc_printf("ERROR: PQM lane_set unsupported lane configuration %d\n",
            rx_buf_ptr[rx_index + ECH_PQM_LANE_SET_DATA_OFFSET]);
        ech_twi_status_byte_set(EXP_SERDES_LANE_UNSUPPORTED);
    }
    else 
    {
        /* record the lane configuration */
        ech_pqm_cfg.lanes = rx_buf_ptr[rx_index + ECH_PQM_LANE_SET_DATA_OFFSET];

        /* set the lane bitmasks */
        if (ech_pqm_cfg.lanes == EXP_SERDES_8_LANE)
        {
            ech_pqm_cfg.lane_bitmask = EXP_SERDES_8_LANE_BITMASK;
            ech_pqm_cfg.lane_rx_pattern_bitmask = EXP_SERDES_8_LANE_PAT_BITMASK;
        }
        else
        {
            ech_pqm_cfg.lane_bitmask = EXP_SERDES_4_LANE_BITMASK;
            ech_pqm_cfg.lane_rx_pattern_bitmask = EXP_SERDES_4_LANE_PAT_BITMASK;
        }

        bc_printf("INFO: PQM lane_set lane = %d\n", ech_pqm_cfg.lanes);
        ech_twi_status_byte_set(EXP_TWI_SUCCESS);
    }

    /* increment the bytes processed from the receive buffer */
    ech_twi_rx_index_inc(EXP_TWI_PQM_LANE_SET_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM Lane Get command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_lane_get(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

    tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET] = 1;
    tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET] = ech_pqm_cfg.lanes;

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                     tx_buf_ptr,                 
                     EXP_TWI_PQM_LANE_GET_RSP_LEN);

    /* set status byte */
    ech_twi_status_byte_set(EXP_TWI_SUCCESS);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_LANE_GET_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM Frequency Set command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM
*   command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_freq_set(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    if ((rx_buf_ptr[rx_index + ECH_PQM_FREQ_SET_DATA_OFFSET] != EXP_SERDES_21_33_GBPS) &&
        (rx_buf_ptr[rx_index + ECH_PQM_FREQ_SET_DATA_OFFSET] != EXP_SERDES_23_46_GBPS) &&
        (rx_buf_ptr[rx_index + ECH_PQM_FREQ_SET_DATA_OFFSET] != EXP_SERDES_25_60_GBPS))
    {
        /* unsupported frequency configuration */
        bc_printf("ERROR: PQM freq set unsupported frequency configuration %d\n",
            rx_buf_ptr[rx_index + ECH_PQM_FREQ_SET_DATA_OFFSET]);
        ech_twi_status_byte_set(EXP_SERDES_FREQ_NA);
    }
    else
    {
        /* record the lane configuration */
        ech_pqm_cfg.freq = rx_buf_ptr[rx_index + ECH_PQM_LANE_SET_DATA_OFFSET];
        bc_printf("INFO: PQM freq set freq = %d\n", ech_pqm_cfg.freq);
        ech_twi_status_byte_set(EXP_TWI_SUCCESS);
    }

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_FREQ_SET_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM Frequency Get command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM
*   command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_freq_get(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

    tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET] = 1;
    tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET] = ech_pqm_cfg.freq;

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                     tx_buf_ptr,                 
                     EXP_TWI_PQM_FREQ_GET_RSP_LEN);

    /* set status byte */
    ech_twi_status_byte_set(EXP_TWI_SUCCESS);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_FREQ_GET_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM Lane Training command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer rx_index
*   rx_index[in] - buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*/
PRIVATE UINT32 ech_pqm_lane_training(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT32 rc;

    if ((ech_pqm_cfg.lanes != EXP_SERDES_1_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_4_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_8_LANE))
    {
        bc_printf("INFO: PQM lane training unsupported lane configuration %d\n", ech_pqm_cfg.lanes);
        return EXP_SERDES_LANE_UNSUPPORTED;
    }
    else
    {
        UINT8 step = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 1];

        bc_printf("INFO: PQM lane training starts step %d...\n", step);

        if (step == 0)
        {
            UINT32 enable_dfe = (UINT32)rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET];
            ech_dfe_state_set(enable_dfe);
            bc_printf("INFO: PQM lane training set DFE state to %s\n",
                enable_dfe ? "enable" : "disable");

            /* Initialize SerDes*/
            rc = serdes_plat_low_level_init(ech_pqm_cfg.lane_bitmask, ech_pqm_cfg.freq, ech_dfe_state_get());
            bc_printf("INFO: PQM lane training initialized SerDes lanes=%d freq=%d with return status=%d\n",
                ech_pqm_cfg.lanes,
                ech_pqm_cfg.freq,
                rc);

            if (rc != PMC_SUCCESS)
            {
                ech_extended_error_code_set(rc);
                bc_printf("ERROR: PQM lane training SerDes INIT failed\n");
                return EXP_SERDES_LANE_SERDES_INIT_ERR;
            }

            /* set hardware initialization flag, this leads to DI being enabled */
            app_fw_plat_hw_init_set(TRUE);
        }
        else if (step == 1)
        {
            if ( !ocmb_cfg_RxPatAorB(OCMB_REGS_BASE_ADDR, ech_pqm_cfg.lane_rx_pattern_bitmask) )
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
        }

        bc_printf("INFO: PQM lane training step %d is done\n", step);        
        return EXP_TWI_SUCCESS;
    }
}

/**
* @brief
*   Process TWI PQM Training Reset
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_training_reset(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* assert serdes reset */
    serdes_plat_initialized_set(FALSE);

    top_exp_cfg_assert_serdes_reset(TOP_XCBI_BASE_ADDR);

    bc_printf("INFO: PQM reset lane trainning\n");

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_LANE_RETRAIN_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM Receiver Adaptation Object Start command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*   This command is running on VPE 0
*/
PRIVATE UINT32 ech_pqm_rx_adapatation_obj_start(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* extract lane ID from command */
    UINT8  lane_id = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET];
    
    /* calculate the offset for the lane being analyzed */
    UINT32 lane_offset = lane_id * SERDES_LANE_REG_OFFSET;

    bc_printf("INFO: ech_pqm_rx_adapatation_obj_start for Lane ID = %d\n", lane_id);

    if (FALSE == SERDES_FH_read_adapt(SERDES_ADSP_PCBI_BASE_ADDR+lane_offset,
                                      SERDES_MTSB_CTRL_PCBI_BASE_ADDR+lane_offset,
                                      &ech_pqm_data.rx_adapt))
    {
        /* hardware error */
        bc_printf("ERROR: PQM rx_adapatation_obj_start HW error\n");
        return EXP_SERDES_READ_ADAPT_OBJ_ERROR;
    }

    bc_printf("INFO: PQM rx_adapatation_obj_start command executed\n");
    return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM Receiver Adaptation Object Read command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_rx_adapatation_obj_read(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

     /* set the response length */
    tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_PQM_RX_ADAPT_OBJ_READ_RSP_DATA_LEN;

    /* copy the data to the transmit buffer */
    memcpy((VOID*)&tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET],
           (VOID*)&ech_pqm_data.rx_adapt, 
           EXP_TWI_PQM_RX_ADAPT_OBJ_READ_RSP_DATA_LEN);

    bc_printf("INFO: PQM rx_adapatation_obj_read sending %d Bytes to HOST\n",
        tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET]);

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                     tx_buf_ptr,                 
                     EXP_TWI_PQM_RX_ADAPT_OBJ_READ_RSP_LEN);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_RX_ADAPAT_OBJ_READ_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM Receiver Calibration Value Start command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*   This command is running on VPE 0
*/
PRIVATE UINT32 ech_pqm_rx_calibration_value_start(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* extract lane ID from command */
    UINT8  lane_id = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET];
    
    /* calculate the offset for the lane being analyzed */
    UINT32 lane_offset = lane_id * SERDES_LANE_REG_OFFSET;

    bc_printf("INFO: ech_pqm_rx_calibration_value_start for Lane ID = %d\n", lane_id);

    if (FALSE == SERDES_FH_read_calib(SERDES_MTSB_CTRL_PCBI_BASE_ADDR+lane_offset,
                                      &ech_pqm_data.rx_calib))
    {
        /* hardware error */
        bc_printf("ERROR: PQM rx_calibration_value_start HW error\n");
        return EXP_SERDES_RX_CALIB_ERROR;
    }

    bc_printf("INFO: PQM rx_calibration_value_start command executed\n");
    return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM Receiver Calibration Value Read command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_rx_calibration_value_read(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

    /* set the response length */
    tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_PQM_RX_CALIB_VALUE_READ_RSP_DATA_LEN;

    /* copy the data to the transmit buffer */
    memcpy((VOID*)&tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET],
            (VOID*)&ech_pqm_data.rx_calib, 
            sizeof(exp_pqm_rx_adapt_obj_struct));

    bc_printf("INFO: PQM rx_calibration_value_read sending %d Bytes to HOST\n",
        tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET]);

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                     tx_buf_ptr,                 
                     EXP_TWI_PQM_RX_CALIB_VALUE_READ_RSP_LEN);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_RX_CALIB_VALUE_READ_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM CSU Calibration Value Status Start command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*   This command is running on VPE 0
*/
PRIVATE UINT32 ech_pqm_csu_calibration_value_status_start(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* extract lane ID from command */
    UINT8  lane_id = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET];
    
    /* calculate the offset for the lane being analyzed */
    UINT32 lane_offset = lane_id * SERDES_LANE_REG_OFFSET;

    bc_printf("INFO: ech_pqm_csu_calibration_value_status_start for Lane ID = %d\n", lane_id);

    if (FALSE == SERDES_FH_read_CSU_status(SERDES_CSU_PCBI_BASE_ADDR + lane_offset,
                                           &ech_pqm_data.csu_calib))
    {
        /* hardware error */
        bc_printf("ERROR: PQM csu_calibration_value_status_start HW error\n");
        return EXP_SERDES_CSU_CALIB_ERROR;
    }

    bc_printf("INFO: PQM csu_calibration_value_status_start command executed\n");
    return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM CSU Calibration Value Status Read command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_csu_calibration_value_status_read(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

     /* set the response length */
    tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_RSP_DATA_LEN;

    /* copy the data to the transmit buffer */
    memcpy((VOID*)&tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET],
            (VOID*)&ech_pqm_data.csu_calib, 
            sizeof(exp_pqm_rx_adapt_obj_struct));

    bc_printf("INFO: PQM csu_calibration_value_status_read sending %d Bytes to HOST\n",
        tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET]);

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                     tx_buf_ptr,                 
                     EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_RSP_LEN);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM PRBS Pattern Mode Set command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_prbs_pattern_mode_set(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8 mode = rx_buf_ptr[rx_index + ECH_PQM_PRBS_PATTERN_SET_DATA_OFFSET];

    switch (mode)
    {
        case ECH_PQM_PRBS_PATTERN_5:
        case ECH_PQM_PRBS_PATTERN_7:
        case ECH_PQM_PRBS_PATTERN_11:
        case ECH_PQM_PRBS_PATTERN_15:
        case ECH_PQM_PRBS_PATTERN_20:
        case ECH_PQM_PRBS_PATTERN_23:
        case ECH_PQM_PRBS_PATTERN_31:
        case ECH_PQM_PRBS_PATTERN_9:
        case ECH_PQM_PRBS_PATTERN_USER:
        case ECH_PQM_PRBS_PATTERN_CJPAT:
        case ECH_PQM_PRBS_PATTERN_CJTPAT:
        {
            /* record the PRBS pattern mode */
            ech_pqm_prbs.mode = rx_buf_ptr[rx_index + ECH_PQM_PRBS_PATTERN_SET_DATA_OFFSET];

            bc_printf("INFO: PRBS pattern mode is set to %d\n", mode);

            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_SUCCESS);
        }
        break;

        default:
        {
            bc_printf("INFO: Unsupported PRBS pattern mode\n");

            /* unsupported configuration, set the status byte  */
            ech_twi_status_byte_set(EXP_PRBS_PATTERN_NA);
        }
        break;
    }

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_PATTERN_MODE_SET_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM PRBS User Defined Pattern Set command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI_SUCCESS. No other error code is possible.
*
* @note
*/
PRIVATE UINT32 ech_pqm_prbs_user_defined_pattern_set(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* record the PRBS user defined pattern */
    for (UINT32 i= 0; i < ECH_PBRS_USER_PATTERNS; i++)
    {
        ech_pqm_user_prbs.patt[i] = rx_buf_ptr[rx_index + EXP_TWI_CMD_DATA_OFFSET + (i*4) + 0] << 24 |
                                    rx_buf_ptr[rx_index + EXP_TWI_CMD_DATA_OFFSET + (i*4) + 1] << 16 |
                                    rx_buf_ptr[rx_index + EXP_TWI_CMD_DATA_OFFSET + (i*4) + 2] << 8  |
                                    rx_buf_ptr[rx_index + EXP_TWI_CMD_DATA_OFFSET + (i*4) + 3] << 0 ;
        bc_printf("INFO: PRBS user defined pattern[%d] = 0x%02x\n", i, ech_pqm_user_prbs.patt[i]);
    }
    
   return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM PRBS Monitor Control command, enable or
*   disable monitoring
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_prbs_monitor_control(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT32 i;
    UINT32 lane_offset;

    if ((ech_pqm_cfg.lanes != EXP_SERDES_1_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_4_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_8_LANE))
    {
        /* unsupported lane configuration */
        bc_printf("ERROR: PRBS monitor control invalid lane configuration %d\n", ech_pqm_cfg.lanes);

        ech_twi_status_byte_set(EXP_SERDES_LANE_UNSUPPORTED);
    }
    else if (TRUE == rx_buf_ptr[rx_index + EXP_TWI_CMD_DATA_OFFSET])
    {
        /* enable PRBS monitoring across supported lanes */
        for (i = 0; i < ech_pqm_cfg.lanes; i++)
        {
            if (ech_pqm_cfg.lane_bitmask & (1 << i))
            {
                /* set the offset for the lane being configured */
                lane_offset = i * SERDES_LANE_REG_OFFSET;

                SERDES_FH_pattmon_en((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                                     ech_pqm_prbs.mode,
                                     &ech_pqm_user_prbs.patt[0]);
            }
        }

        bc_printf("INFO: Enabled PRBS monitoring control from lane 0 to lane %d\n",
            ech_pqm_cfg.lanes - 1);
    }
    else
    {
        /* disable PRBS monitoring across supported lanes */
        for (i = 0; i < ech_pqm_cfg.lanes; i++)
        {
            if (ech_pqm_cfg.lane_bitmask & (1 << i))
            {
                /* set the offset for the lane being configured */
                lane_offset = i * SERDES_LANE_REG_OFFSET;

                SERDES_FH_pattmon_dis(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);
            }
        }

        bc_printf("INFO: Disabled PRBS monitoring control from lane 0 to lane %d\n",
            ech_pqm_cfg.lanes - 1);
    }

    /* set status byte */
    ech_twi_status_byte_set(EXP_TWI_SUCCESS);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_MONITOR_CONTROL_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM PRBS Generator Control command, enable or
*   disable generation
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_prbs_generator_control(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT32 i;
    UINT32 lane_offset;
    
    if ((ech_pqm_cfg.lanes != EXP_SERDES_1_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_4_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_8_LANE))
    {
        /* invalid lane configuration */
        bc_printf("ERROR: PRBS generator control invalid lane configuration %d\n", ech_pqm_cfg.lanes);

        /* set status byte */
        ech_twi_status_byte_set(EXP_SERDES_LANE_UNSUPPORTED);
    }
    else if (TRUE == rx_buf_ptr[rx_index + EXP_TWI_CMD_DATA_OFFSET])
    {
        /* enable PRBS generation across supported lanes */
        for (i = 0; i < ech_pqm_cfg.lanes; i++)
        {
            if (ech_pqm_cfg.lane_bitmask & (1 << i))
            {
                /* set the offset for the lane being configured */
                lane_offset = i * SERDES_LANE_REG_OFFSET;

                SERDES_FH_pattgen_en((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                                     ech_pqm_prbs.mode,
                                     &ech_pqm_user_prbs.patt[0]);
            }
        }

        bc_printf("INFO: Enabled PRBS generator control from lane 0 to lane %d\n",
            ech_pqm_cfg.lanes - 1);
    }
    else
    {
        /* disable PRBS generation across supported lanes */
        for (i = 0; i < ech_pqm_cfg.lanes; i++)
        {
            if (ech_pqm_cfg.lane_bitmask & (1 << i))
            {
                /* set the offset for the lane being configured */
                lane_offset = i * SERDES_LANE_REG_OFFSET;

                SERDES_FH_pattgen_dis(SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset);
            }
        }

        bc_printf("INFO: Disabled PRBS generator control from lane 0 to lane %d\n",
            ech_pqm_cfg.lanes - 1);
    }

    /* set status byte */
    ech_twi_status_byte_set(EXP_TWI_SUCCESS);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_GENERATOR_CONTROL_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM PRBS Error Count Start command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*   This command is running on VPE 0
*/
PRIVATE UINT32 ech_pqm_prbs_err_count_start(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    if ((ech_pqm_cfg.lanes != EXP_SERDES_1_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_4_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_8_LANE))
    {
        /* invalid lane configuration */
        bc_printf("ERROR: PRBS error count start invalid lane configuration %d\n", ech_pqm_cfg.lanes);
        return EXP_SERDES_LANE_UNSUPPORTED;
    }

    /* read the error count across supported lanes */
    for (UINT32 i = 0; i < ech_pqm_cfg.lanes; i++)
    {
        if (ech_pqm_cfg.lane_bitmask & (1 << i))
        {
            /* set the offset for the lane being configured */
            UINT32 lane_offset = i * SERDES_LANE_REG_OFFSET;

            SERDES_FH_pattgen_chkrd((SERDES_CHANNEL_PCBI_BASE_ADDR + lane_offset),
                                    &ech_pqm_err.count[i]);
        }
    }

    bc_printf("INFO: Read PRBS error count for lanes: ");
    for (UINT32 i = 0; i < ech_pqm_cfg.lanes; i++)
    {
        if (ech_pqm_cfg.lane_bitmask & (1 << i))
        {
            bc_printf("%d ", i);
        }
    }
    bc_printf("\n");

    return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM PRBS Error Count Read command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_prbs_err_count_read(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    if ((ech_pqm_cfg.lanes != EXP_SERDES_1_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_4_LANE) &&
        (ech_pqm_cfg.lanes != EXP_SERDES_8_LANE))
    {
        bc_printf("ERROR: PRBS error count read invalid lane configuration %d, do not send any response\n", ech_pqm_cfg.lanes);
        
        ech_twi_status_byte_set(EXP_SERDES_LANE_UNSUPPORTED);
        
        /* increment the received buffer index */
        ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_ERR_COUNT_READ_CMD_LEN);
        return;
    }
    
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

    /* set the response length */
    tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET] = ech_pqm_cfg.lanes * sizeof(UINT32);
    /*bc_printf("INFO: PRBS error count before memcpy, ech_pqm_cfg.lanes=%d, size byte=%d\n",
        ech_pqm_cfg.lanes, ech_pqm_cfg.lanes * sizeof(UINT32));*/

    /* copy the data to the transmit buffer */
    memcpy((VOID*)&tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET],
            (VOID*)&ech_pqm_err.count[0], 
            tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET]);

    bc_printf("INFO: PRBS error count sending %d Bytes to HOST, %d out of %d Bytes in the package contains useful information\n",
        EXP_TWI_PQM_PRBS_ERR_COUNT_READ_RSP_LEN,
        tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET],
        EXP_TWI_PQM_PRBS_ERR_COUNT_READ_RSP_LEN);

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                     tx_buf_ptr,                 
                     EXP_TWI_PQM_PRBS_ERR_COUNT_READ_RSP_LEN);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_ERR_COUNT_READ_CMD_LEN);
}

/**
* @brief
*   Process TWI PQM Horizontal Bathtub Get Start command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*   This command is running on VPE 0
*/
PRIVATE UINT32 ech_pqm_horizontal_bathtub_get_start(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* extract bathtub parameters from command */
    UINT8  lane_id = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET];
    UINT32 fast_acq_enable = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 1];
    UINT32 time_limit_high = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 2];
    UINT32 time_limit_low = (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 3] << 24) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 4] << 16) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 5] << 8) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 6] << 0);
    UINT32 event_limit = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 7];
    UINT64 time_limit_u64;

    /* calculate the offset for the lane being analyzed */
    UINT32 lane_offset = lane_id * SERDES_LANE_REG_OFFSET;

    bc_printf("INFO: HZ Bathtub capture: Lane ID = %d fast_acq_end = %d time_limit_high = %d time_limit_low = %d event_limit = %d lane_offset = %d SERDES_MTSB_CTRL_PCBI_BASE_ADDR = 0x%x SERDES_DIAG_PCBI_BASE_ADDR = 0x%x \n",                                         
                                            lane_id, 
                                            fast_acq_enable,
                                            time_limit_high,
                                            time_limit_low,
                                            event_limit,
                                            lane_offset,
                                            SERDES_MTSB_CTRL_PCBI_BASE_ADDR,
                                            SERDES_DIAG_PCBI_BASE_ADDR);

    /**
     * PREP 379190 - the time limit cannot exceed 0xFF_FFFF_FFF0.
     */
    time_limit_u64 = (UINT64)time_limit_high << 32 | time_limit_low;
    if (ECH_PQM_BATHTUB_CAPTURE_TIME_LIMIT_MAX_PREP379190 < time_limit_u64)
    {
        time_limit_high = (ECH_PQM_BATHTUB_CAPTURE_TIME_LIMIT_MAX_PREP379190 >> 32) & 0xFFFFFFFF;
        time_limit_low = ECH_PQM_BATHTUB_CAPTURE_TIME_LIMIT_MAX_PREP379190 & 0xFFFFFFFF;
    }

    /* capture the bathtub data */
    if (FALSE == SERDES_FH_Diag_HBT((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                    (SERDES_DIAG_PCBI_BASE_ADDR + lane_offset),
                                    fast_acq_enable,
                                    time_limit_high,
                                    time_limit_low,
                                    event_limit,
                                    &horz_bt))
    {
        /* hardware error */
        bc_printf("ERROR: HZ Bathtub capture returned HW error \n");
        return EXP_HORZ_BATHTUB_FAILURE;
    }

    return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM Horizontal Bathtub Get Read command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_horizontal_bathtub_get_read(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

    /* set the response length */
    tx_buf_ptr[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_PQM_HORZ_BATHTUB_GET_RSP_DATA_LEN;

    /* copy the data to the transmit buffer */
    memcpy((VOID*)&tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET],
            (VOID*)&horz_bt, 
            EXP_TWI_PQM_HORZ_BATHTUB_GET_RSP_DATA_LEN);

    bc_printf("INFO: HZ capture sending %d Bytes to HOST \n", EXP_TWI_PQM_HORZ_BATHTUB_GET_RSP_LEN);

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                        tx_buf_ptr,                 
                        EXP_TWI_PQM_HORZ_BATHTUB_GET_RSP_LEN);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_HORZ_BATHTUB_GET_READ_CMD_LEN);
}


/**
* @brief
*   Process TWI PQM Vertical Bathtub Get Start command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*   This command is running on VPE 0
*/
PRIVATE UINT32 ech_pqm_vertical_bathtub_get_start(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* extract bathtub parameters from command */
    UINT8  lane_id = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET];
    UINT32 outer_eye_enable = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 1];
    UINT32 fast_acq_enable = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 2];
    UINT32 time_limit_high = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 3];
    UINT32 time_limit_low = (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 4] << 24) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 5] << 16) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 6] << 8) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 7] << 0);
    UINT32 event_limit = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 8];

    /* calculate the offset for the lane being analyzed */
    UINT32 lane_offset = lane_id * SERDES_LANE_REG_OFFSET;

    bc_printf("INFO: VT Bathtub capture: Lane ID = %d Outer EYE = %d fast_acq_end = %d time_limit_high = %d time_limit_low = %d event_limit = %d lane_offset = %d SERDES_MTSB_CTRL_PCBI_BASE_ADDR = 0x%x SERDES_DIAG_PCBI_BASE_ADDR = 0x%x \n",                                         
                                         lane_id, 
                                         outer_eye_enable,
                                         fast_acq_enable,
                                         time_limit_high,
                                         time_limit_low,
                                         event_limit,
                                         lane_offset,
                                         SERDES_MTSB_CTRL_PCBI_BASE_ADDR,
                                         SERDES_DIAG_PCBI_BASE_ADDR);

    /* capture the bathtub data */
    if (FALSE == SERDES_FH_Diag_VBT((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                    (SERDES_DIAG_PCBI_BASE_ADDR + lane_offset),
                                    FALSE,
                                    FALSE,
                                    outer_eye_enable,
                                    fast_acq_enable,
                                    time_limit_high,
                                    time_limit_low,
                                    event_limit,
                                    &vert_bt))
    {
        /* hardware failure */
        bc_printf("ERROR: VT Bathtub capture returned HW error \n");
        return EXP_VERT_BATHTUB_FAILURE;
    }

    return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM Vertical Bathtub Get Read command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_vertical_bathtub_get_read(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

    /* set the response length */
    tx_buf_ptr[EXP_TWI_EXT_RSP_LEN_HIGH_OFFSET] = (UINT8)((EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN & 0xFF00) >> 8);
    tx_buf_ptr[EXP_TWI_EXT_RSP_LEN_LOW_OFFSET] = (UINT8)(EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN & 0x00FF);

    /* copy the data to the transmit buffer */
    memcpy((VOID*)&tx_buf_ptr[EXP_TWI_RSP_DATA_OFFSET],
            (VOID*)&vert_bt, 
            EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN);

    bc_printf("INFO: VT Bathtub capture sending %d Bytes to HOST \n", EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_LEN);

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT, tx_buf_ptr, EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_LEN);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_VERT_BATHTUB_GET_READ_CMD_LEN);
}


/**
* @brief
*   Process TWI PQM 2-Dimensional Bathtub Get Start command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   EXP_TWI status code
*
* @note
*   This command is running on VPE 0
*/
PRIVATE UINT32 ech_pqm_2d_bathtub_get_start(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    /* extract bathtub parameters from command */
    UINT8  lane_id = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET];
    INT32  phase = (INT32)rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 1];
    UINT32 outer_eye_enable = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 2];
    UINT32 fast_acq_enable = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 3];
    UINT32 time_limit_high = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 4];
    UINT32 time_limit_low = (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 5] << 24) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 6] << 16) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 7] << 8) |
                            (rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 8] << 0);
    UINT32 event_limit = rx_buf_ptr[EXP_TWI_CMD_DATA_OFFSET + 9];

    /* calculate the offset for the lane being analyzed */
    UINT32 lane_offset = lane_id * SERDES_LANE_REG_OFFSET;

    bc_printf("INFO: 2D Bathtub capture: Lane ID = %d Phase = %d Outer EYE = %d fast_acq_end = %d time_limit_high = %d time_limit_low = %d event_limit = %d lane_offset = %d SERDES_MTSB_CTRL_PCBI_BASE_ADDR = 0x%x SERDES_DIAG_PCBI_BASE_ADDR = 0x%x \n",                                         
                                         lane_id, 
                                         phase,
                                         outer_eye_enable,
                                         fast_acq_enable,
                                         time_limit_high,
                                         time_limit_low,
                                         event_limit,
                                         lane_offset,
                                         SERDES_MTSB_CTRL_PCBI_BASE_ADDR,
                                         SERDES_DIAG_PCBI_BASE_ADDR);

    /* capture the Vertical Bathtub data for the specified phase */
    if (FALSE == SERDES_FH_Diag_VBT((SERDES_MTSB_CTRL_PCBI_BASE_ADDR + lane_offset),
                                    (SERDES_DIAG_PCBI_BASE_ADDR + lane_offset),
                                    TRUE,
                                    phase,
                                    outer_eye_enable,
                                    fast_acq_enable,
                                    time_limit_high,
                                    time_limit_low,
                                    event_limit,
                                    &vert_bt))
    {
        /* hardware error */
        bc_printf("ERROR: 2D Bathtub capture returned HW error \n");
        return EXP_VERT_BATHTUB_FAILURE;
    }

    return EXP_TWI_SUCCESS;
}

/**
* @brief
*   Process TWI PQM 2-Dimensional Bathtub Get Read command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_pqm_2d_bathtub_get_read(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    UINT8* tx_buf_ptr = ech_twi_tx_buf_get();

    /* set the extended response length */
    tx_buf_ptr[EXP_TWI_EXT_RSP_LEN_HIGH_OFFSET] = (UINT8)((EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN & 0xFF00) >> 8);
    tx_buf_ptr[EXP_TWI_EXT_RSP_LEN_LOW_OFFSET] = (UINT8)(EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN & 0x00FF);

    /* copy the data to the transmit buffer */
    memcpy((VOID*)&tx_buf_ptr[EXP_TWI_EXT_RSP_DATA_OFFSET],
            (VOID*)&vert_bt, 
            EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN);

    bc_printf("INFO: 2D Bathtub capture sending %d Bytes to HOST \n", EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_LEN);

    /* send the response */
    twi_slv_data_put(EXP_TWI_SLAVE_PORT,
                    tx_buf_ptr,                 
                    EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_LEN);

    /* increment the received buffer index */
    ech_twi_rx_index_inc(EXP_TWI_PQM_TWOD_BATHTUB_GET_READ_CMD_LEN);
}

/**
* @brief
*   Process TWI Force Delay Line update command
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   PMC_SUCCESS or EXP_TWI_ERROR. Extended error field 
*   of the status command is also set to indicate the error
*
* @note
*/
PUBLIC UINT32 ech_pqm_force_delay_line_update(UINT8* rx_buf_ptr, UINT32 rx_index)

{
    ddr_bist_setup_t bistSetup;
    UINT32 status = ddrBistInit(0, &bistSetup);

    if(status != PMC_SUCCESS)
    {
        bc_printf("INFO: PQM cannot initialize BIST mode, status = 0x%08x\n", status);
        /* Set the extended error code*/
        ech_extended_error_code_set(status);
        return EXP_TWI_ERROR;
    }

    ddrBistSetup(&bistSetup);
    ddrEnterPubMode();
    status = ddrphy_force_dl_update();

    /* Set BIST_ENABLE = 0 and exit pub mode */    
    ddrExitBistCmd();

    /* Call DDR PHY Toolbox to update the delay line register*/
    if(status != PMC_SUCCESS)
    {   
        bc_printf("INFO: PQM cannot force delay line update, status = 0x%08x\n", status);
        /* Set the extended error code*/
        ech_extended_error_code_set(status);
        return EXP_TWI_ERROR;
    }

    bc_printf("INFO: PQM forced delay line update\n");

    /* 
    ** Do not increment the receive buffer index as
    ** this command is handled from VPE 0 as a deferred processing
    ** command.
    **
    */

    return EXP_TWI_SUCCESS;
}



/*
** Public Functions
*/

/**
* @brief
*   Process TWI PQM commands.
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_pqm_cmd_proc(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    switch (rx_buf_ptr[rx_index])
    {
        case EXP_FW_PQM_LANE_SET:
        {
            ech_pqm_lane_set(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_LANE_GET:
        {
            ech_pqm_lane_get(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_FREQ_SET:
        {
            ech_pqm_freq_set(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_FREQ_GET:
        {
            ech_pqm_freq_get(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_LANE_TRAINING:
        {      
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_LANE_TRAIN_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_LANE_TRAINING,
                &ech_pqm_lane_training, 
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_LANE_TRAIN_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_TRAINING_RESET:
        {
            ech_pqm_training_reset(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_RX_ADAPTATION_OBJ_START:
        {
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_RX_ADAPAT_OBJ_START_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_RX_ADAPTATION_OBJ_START,
                &ech_pqm_rx_adapatation_obj_start, 
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_RX_ADAPAT_OBJ_START_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_RX_ADAPTATION_OBJ_READ:
        {
            ech_pqm_rx_adapatation_obj_read(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_RX_CALIBRATION_VALUE_START:
        {
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_RX_CALIB_VALUE_START_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_RX_CALIBRATION_VALUE_START,
                &ech_pqm_rx_calibration_value_start,
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_RX_CALIB_VALUE_START_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_RX_CALIBRATION_VALUE_READ:
        {
            ech_pqm_rx_calibration_value_read(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_START:
        {
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_START_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_START,
                &ech_pqm_csu_calibration_value_status_start,
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_START_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_READ:
        {
            ech_pqm_csu_calibration_value_status_read(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_PRBS_PATTERN_MODE_SET:
        {
            ech_pqm_prbs_pattern_mode_set(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_PRBS_USER_DEFINED_PATTERN_SET:
        {        
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_PRBS_USER_PATTERN_SET_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_PRBS_USER_DEFINED_PATTERN_SET,
                &ech_pqm_prbs_user_defined_pattern_set,
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_PRBS_USER_PATTERN_SET_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_PRBS_MONITOR_CONTROL:
        {
            ech_pqm_prbs_monitor_control(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_PRBS_GENERATOR_CONTROL:
        {
            ech_pqm_prbs_generator_control(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_PRBS_ERR_COUNT_START:
        {
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_PRBS_ERR_COUNT_START_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_PRBS_ERR_COUNT_START,
                &ech_pqm_prbs_err_count_start,
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_PRBS_ERR_COUNT_START_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_PRBS_ERR_COUNT_READ:
        {
            ech_pqm_prbs_err_count_read(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_START:
        {
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_HORZ_BATHTUB_GET_START_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_START,
                &ech_pqm_horizontal_bathtub_get_start,
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_HORZ_BATHTUB_GET_START_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_READ:
        {
            ech_pqm_horizontal_bathtub_get_read(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_VERTICAL_BATHTUB_GET_START:
        {
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_VERT_BATHTUB_GET_START_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_VERTICAL_BATHTUB_GET_START,
                &ech_pqm_vertical_bathtub_get_start,
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_VERT_BATHTUB_GET_START_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_VERTICAL_BATHTUB_GET_READ:
        {
            ech_pqm_vertical_bathtub_get_read(rx_buf_ptr, rx_index);
        }
        break;

        case EXP_FW_PQM_2D_BATHTUB_GET_START:
        {
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_TWOD_BATHTUB_GET_START_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_2D_BATHTUB_GET_START,
                &ech_pqm_2d_bathtub_get_start,
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_PQM_TWOD_BATHTUB_GET_START_CMD_LEN,
                rx_index);
        }
        break;

        case EXP_FW_PQM_2D_BATHTUB_GET_READ:
        {
            ech_pqm_2d_bathtub_get_read(rx_buf_ptr, rx_index);
        }
        break;

    }
} /* ech_pqm_cmd_proc() */

/**
* @brief
*   Increment I2C command pointer when FW is not in PQM mode
*   Otherwise, subsequent commands will not be processed.
*
* @param
*   rx_buf_ptr[in] - reference to received data buffer
*   rx_index[in] - index in buffer of start of received PQM command
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_pqm_cmd_rx_index_increment(UINT8* rx_buf_ptr, UINT32 rx_index)
{
    switch (rx_buf_ptr[rx_index])
    {
        case EXP_FW_PQM_LANE_SET:
        {            
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_LANE_SET_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_LANE_GET:
        {            
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_LANE_GET_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_FREQ_SET:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_FREQ_SET_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_FREQ_GET:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_FREQ_GET_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_LANE_TRAINING:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_LANE_TRAIN_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_TRAINING_RESET:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_LANE_RETRAIN_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_RX_ADAPTATION_OBJ_START:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_RX_ADAPAT_OBJ_START_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_RX_ADAPTATION_OBJ_READ:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_RX_ADAPAT_OBJ_READ_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_RX_CALIBRATION_VALUE_START:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_RX_CALIB_VALUE_START_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_RX_CALIBRATION_VALUE_READ:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_RX_CALIB_VALUE_READ_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_START:
        {            
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_START_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_READ:
        {            
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_PRBS_PATTERN_MODE_SET:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_PATTERN_MODE_SET_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_PRBS_USER_DEFINED_PATTERN_SET:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_USER_PATTERN_SET_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_PRBS_MONITOR_CONTROL:
        {
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_MONITOR_CONTROL_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_PRBS_GENERATOR_CONTROL:
        {            
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_GENERATOR_CONTROL_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_PRBS_ERR_COUNT_START:
        {            
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_ERR_COUNT_START_CMD_LEN);
        }
        break;

        case EXP_FW_PQM_PRBS_ERR_COUNT_READ:
        {            
            /* increment the received buffer index */
            ech_twi_rx_index_inc(EXP_TWI_PQM_PRBS_ERR_COUNT_READ_CMD_LEN);
        }
        break;

        default:
            bc_printf("Unsupported PQM Command ID \n");
    }
} /* ech_pqm_cmd_proc() */
