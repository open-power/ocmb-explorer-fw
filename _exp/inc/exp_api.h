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
* @addtogroup EXP_API
* @{
* @file
* @brief
*   Explorer Command Handler definitions and declarations.
*
* @note
*/
#ifndef _EXP_API_H
#define _EXP_API_H


/*
* Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
** Constants
*/

/* TWI interfaces */
#define EXP_TWI_MASTER_PORT                     0
#define EXP_TWI_SLAVE_PORT                      1
#define EXP_TWI_MAX_BUF_SIZE                    EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_LEN
#define EXP_TWI_SLAVE_ADDR                      0x20
#define EXP_TWI_ADDR_SIZE                       TWI_ADDR_SIZE_7BIT
#define EXP_TWI_STRETCH_TIMEOUT_MS              5

/* TWI command packet offsets */
#define EXP_TWI_CMD_OFFSET                      0
#define EXP_TWI_CMD_LEN_OFFSET                  1
#define EXP_TWI_CMD_DATA_OFFSET                 2

/* TWI response packet offsets */
#define EXP_TWI_RSP_LEN_OFFSET                  0
#define EXP_TWI_RSP_DATA_OFFSET                 1

/* TWI extended response packet offsets, used for larger PQM data transfers */
#define EXP_TWI_EXT_RSP_LEN_HIGH_OFFSET         0
#define EXP_TWI_EXT_RSP_LEN_LOW_OFFSET          1
#define EXP_TWI_EXT_RSP_DATA_OFFSET             2

/* TWI command success value */
#define EXP_TWI_SUCCESS                         0x00

/* TWI error values */
#define EXP_TWI_ERROR                           0xC0
#define EXP_TWI_UNSUPPORTED                     0xD0

/* TWI Busy Status*/
#define EXP_TWI_BUSY                            0xFE

/* TWI boot config command length */
#define EXP_TWI_BOOT_CFG_CMD_DATA_LEN           4
#define EXP_TWI_BOOT_CFG_CMD_LEN                (EXP_TWI_BOOT_CFG_CMD_DATA_LEN + 2)

/* Top level TWI boot config error bit masks */
#define EXP_TWI_BOOT_CFG_FAILED_BITMSK                             0x01
#define EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_FAIL_BITMASK              0x02
#define EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK                  0x04
#define EXP_TWI_BOOT_CFG_DLX_CONFIG_FAIL_BITMSK                    0x08
#define EXP_TWI_BOOT_CFG_SERDES_LANE_INVERSION_CONFIG_FAIL_BITMSK  0x10

/* Extended Error code for EXP_TWI_BOOT_CFG_FAILED_BITMSK */
#define EXP_TWI_BOOT_CFG_UNSUPPORTED_SERDES_FREQ_BITMSK    0x01
#define EXP_TWI_BOOT_CFG_UNSUPPORTED_TL_BITMSK             0x02
#define EXP_TWI_BOOT_CFG_UNSUPPORT_LANE_CONFIG_BITMSK      0x04


/* Extended Error code for EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK */
#define EXP_SERDES_TRAINING_FREQ_UNSUPPORTED               0x01
#define EXP_SERDES_TRAINING_CSU_FAILED                     0x02
#define EXP_SERDES_TRAINING_CALIB_FAILED                   0x03
#define EXP_SERDES_TRAINING_ADAPT1_FAILED                  0x04
#define EXP_SERDES_FH_ALIGNMENT1_FAILED                    0x05
#define EXP_SERDES_FH_ALIGNMENT2_FAILED                    0x06
#define EXP_SERDES_DDLL_LOCK_FAIL                          0x07
#define EXP_SERDES_DEASSERT_PHY_OCMB_RESET_FAIL            0x08
#define EXP_SERDES_FH_ALIGNMENT_INIT_1_FAIL                0x09
#define EXP_SERDES_FH_TX_ALIGNMENT_FAIL                    0x0a
#define EXP_SERDES_FH_PATTERNGEN_ENABLE_FAIL               0x0b
#define SERDES_FH_TXRX_ADAPTATION_1_PE_FAIL                0x0c
#define EXP_SERDES_FH_PATTERNMON_ENABLE_FAIL               0x0d
#define EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_LANE_ID_FAIL      0x0e
#define EXP_SERDES_TRAINING_ADAPT2_FAILED                  0x0f   

/* Extended Error code for EXP_TWI_BOOT_CFG_DLX_CONFIG_FAIL_BITMSK */
#define EXP_TWI_BOOT_CFG_DLX_CONFIG_PATTERN_A_B_FAILED     0x01
#define EXP_TWI_BOOT_CFG_DLX_CONFIG_FW_FAILED              0x02

/* Extended Error code for EXP_TWI_BOOT_CFG_SERDES_LANE_INVERSION_CONFIG_FAIL_BITMSK */
#define EXP_TWI_BOOT_CFG_SERDES_FH_RX_ALIGNMENT_FAIL       0x01
#define EXP_TWI_BOOT_CFG_SERDES_RX_PATB_TIMEOUT_FAIL       0x02       



/* TWI boot config Adaptation Enable */
#define EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_ENABLE_BITMSK                 0x8000
#define EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_BITOFF                        15
#define EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_ENABLE                        0x1
#define EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_DISABLE                       0x0

/* TWI boot config firmare mode */
#define EXP_TWI_BOOT_CFG_FW_MODE_BITMSK                 0x6000
#define EXP_TWI_BOOT_CFG_FW_MODE_BITOFF                 13
#define EXP_TWI_BOOT_CFG_FW_NORMAL_MODE                 0x0
#define EXP_TWI_BOOT_CFG_FW_MANUFACTURING_MODE          0x1
#define EXP_TWI_BOOT_CFG_FW_PRODUCT_QUALIFICATION_MODE  0x2
#define EXP_TWI_BOOT_CFG_FW_UNSUPPORTED_MODE            0x3

/* TWI boot config OpenCAPI mode */
#define EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_BITMSK  0x1000
#define EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_BITOFF  12
#define EXP_TWI_BOOT_CFG_OCAPI_NO_LOOPBACK_MODE 0
#define EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_MODE    1

/* TWI boot config transport layer mode */
#define EXP_TWI_BOOT_CFG_TL_MODE_BITMSK         0x0C00
#define EXP_TWI_BOOT_CFG_TL_MODE_BITOFF         10
#define EXP_TWI_BOOT_CFG_TL_OCAPI_MODE          0
#define EXP_TWI_BOOT_CFG_TL_TWI_MODE            1
#define EXP_TWI_BOOT_CFG_TL_JTAG_MODE           2

/* TWI boot config datalink layer mode */
#define EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK       0x0300
#define EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF       8
#define EXP_TWI_BOOT_CFG_BOOT_STEP_0            0
#define EXP_TWI_BOOT_CFG_BOOT_STEP_1            1

/* TWI boot config bootmode */
#define EXP_TWI_BOOT_CFG_DFE_BITMSK             0x0080
#define EXP_TWI_BOOT_CFG_DFE_BITOFF             7
#define EXP_TWI_BOOT_CFG_DFE_DISABLE            1

/* TWI boot config lanemode */
#define EXP_TWI_BOOT_CFG_LANE_MODE_BITMSK       0x0070
#define EXP_TWI_BOOT_CFG_LANE_MODE_BITOFF       4
#define EXP_TWI_BOOT_CFG_LANE_ERROR             0
#define EXP_TWI_BOOT_CFG_LANE_8                 1
#define EXP_TWI_BOOT_CFG_LANE_4                 2

/* TWI boot config SerDes frequency */
#define EXP_TWI_BOOT_CFG_SERDES_FREQ_BITMSK     0x000F
#define EXP_TWI_BOOT_CFG_SERDES_FREQ_BITOFF     0
#define EXP_TWI_BOOT_CFG_SERDES_FREQ_ERROR      0
#define EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS  1
#define EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS  2
#define EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS  3

/* TWI boot stage */
#define EXP_TWI_STATUS_BOOT_ROM                 0x01
#define EXP_TWI_STATUS_FW_UPGRADE               0x02
#define EXP_TWI_STATUS_RUN_TIME                 0x03

/* TWI register read/write errors */
#define EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE        0x01
#define EXP_TWI_REG_RW_ADDR_PROHIBITED          0x02

/* TWI Status command/response lengths */
#define EXP_TWI_STATUS_CMD_LEN                  1
#define EXP_TWI_STATUS_RSP_LEN                  5
#define EXP_TWI_STATUS_RSP_DATA_LEN             4

/* TWI register address latch command length */
#define EXP_TWI_REG_ADDR_LATCH_CMD_DATA_LEN     4
#define EXP_TWI_REG_ADDR_LATCH_CMD_LEN          (EXP_TWI_REG_ADDR_LATCH_CMD_DATA_LEN + 2)

/* TWI register read command length */
#define EXP_TWI_REG_READ_CMD_DATA_LEN           4
#define EXP_TWI_REG_READ_CMD_LEN                (EXP_TWI_REG_READ_CMD_DATA_LEN + 2)
#define EXP_TWI_REG_READ_RSP_DATA_LEN           4
#define EXP_TWI_REG_READ_RSP_LEN                (EXP_TWI_REG_READ_RSP_DATA_LEN + 1)

/* TWI register write command length */
#define EXP_TWI_REG_WRITE_CMD_DATA_LEN          8
#define EXP_TWI_REG_WRITE_CMD_LEN               (EXP_TWI_REG_WRITE_CMD_DATA_LEN + 2)

/* TWI firmware download command length */
#define EXP_TWI_FW_DOWNLOAD_CMD_LEN             1

/* TWI PHY INIT command parms length */
#define EXP_TWI_PHY_INIT_PARMS_INIT_MODE_LEN    1
#define EXP_TWI_PHY_INIT_PARMS_RESERVED_LEN     EXP_OC_RESP_PARMS_LEN - EXP_TWI_PHY_INIT_PARMS_INIT_MODE_LEN

/* TWI pass-through temperature read command parms */
#define EXP_TWI_TEMP_READ_RESPONSE_DATA_LEN     24
#define EXP_TWI_TEMP_READ_CMD_FLAG_STOP_BITMSK  0x1
#define EXP_TWI_TEMP_READ_CMD_FLAG_STOP_BITOFF  0

/* TWI pass-through temperature write command parms */
#define EXP_TWI_TEMP_WRITE_COMMAND_DATA_LEN     24
#define EXP_TWI_TEMP_WRITE_CMD_FLAG_STOP_BITMSK 0x1
#define EXP_TWI_TEMP_WRITE_CMD_FLAG_STOP_BITOFF 0

/* TWI interval temperature read command parms */
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_1_BYTE         0
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_2_BYTES        1
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_BITMSK         0x18
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_BITOFF         3
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_REG_OFFSET_BITMSK   0x04
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_REG_OFFSET_BITOFF   2
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_NUM_READ_OP_BITMSK  0x02
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_NUM_READ_OP_BITOFF  1
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_SENSOR_TYPE_BITMSK  0x01
#define EXP_TWI_TEMP_INTERVAL_READ_CMD_SENSOR_TYPE_BITOFF  0

/* TWI continuous read command length */
#define EXP_TWI_CONT_READ_CMD_DATA_LEN          4
#define EXP_TWI_CONT_READ_CMD_LEN               (EXP_TWI_CONT_READ_CMD_DATA_LEN + 2)

/* TWI continuous write command length */
#define EXP_TWI_CONT_WRITE_CMD_DATA_LEN         4
#define EXP_TWI_CONT_WRITE_CMD_LEN              (EXP_TWI_CONT_WRITE_CMD_DATA_LEN + 2)

/* TWI bypass start-up timeout */
#define EXP_TWI_BYPASS_4SEC_TIMEOUT             1

/* 
** TWI PQM command/response lengths 
** For commands containing command ID and length, '2' is added to
** the data length to get the actual command length.
*/
#define EXP_TWI_PQM_LANE_SET_CMD_DATA_LEN                       1
#define EXP_TWI_PQM_LANE_SET_CMD_LEN                            (EXP_TWI_PQM_LANE_SET_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_LANE_GET_CMD_LEN                            1
#define EXP_TWI_PQM_LANE_GET_RSP_DATA_LEN                       1
#define EXP_TWI_PQM_LANE_GET_RSP_LEN                            (EXP_TWI_PQM_LANE_GET_RSP_DATA_LEN + 1)
#define EXP_TWI_PQM_FREQ_SET_CMD_DATA_LEN                       1
#define EXP_TWI_PQM_FREQ_SET_CMD_LEN                            (EXP_TWI_PQM_FREQ_SET_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_FREQ_GET_CMD_LEN                            1
#define EXP_TWI_PQM_FREQ_GET_RSP_DATA_LEN                       1
#define EXP_TWI_PQM_FREQ_GET_RSP_LEN                            (EXP_TWI_PQM_FREQ_GET_RSP_DATA_LEN + 1)
#define EXP_TWI_PQM_LANE_TRAIN_CMD_DATA_LEN                     2
#define EXP_TWI_PQM_LANE_TRAIN_CMD_LEN                          (EXP_TWI_PQM_LANE_TRAIN_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_LANE_RETRAIN_CMD_LEN                        1
#define EXP_TWI_PQM_RX_ADAPAT_OBJ_START_DATA_LEN                1
#define EXP_TWI_PQM_RX_ADAPAT_OBJ_START_CMD_LEN                 (EXP_TWI_PQM_RX_ADAPAT_OBJ_START_DATA_LEN + 2)
#define EXP_TWI_PQM_RX_ADAPAT_OBJ_READ_CMD_LEN                  1
#define EXP_TWI_PQM_RX_ADAPT_OBJ_READ_RSP_DATA_LEN              sizeof(exp_pqm_rx_adapt_obj_struct)
#define EXP_TWI_PQM_RX_ADAPT_OBJ_READ_RSP_LEN                   (EXP_TWI_PQM_RX_ADAPT_OBJ_READ_RSP_DATA_LEN + 1)
#define EXP_TWI_PQM_RX_CALIB_VALUE_START_DATA_LEN               1
#define EXP_TWI_PQM_RX_CALIB_VALUE_START_CMD_LEN                (EXP_TWI_PQM_RX_CALIB_VALUE_START_DATA_LEN + 2)
#define EXP_TWI_PQM_RX_CALIB_VALUE_READ_CMD_LEN                 1
#define EXP_TWI_PQM_RX_CALIB_VALUE_READ_RSP_DATA_LEN            sizeof(exp_pqm_rx_calib_value_struct)
#define EXP_TWI_PQM_RX_CALIB_VALUE_READ_RSP_LEN                 (EXP_TWI_PQM_RX_CALIB_VALUE_READ_RSP_DATA_LEN + 1)
#define EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_START_DATA_LEN       1
#define EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_START_CMD_LEN        (EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_START_DATA_LEN + 2)
#define EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_CMD_LEN         1
#define EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_RSP_DATA_LEN    sizeof(exp_pqm_csu_calib_value_status_struct)
#define EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_RSP_LEN         (EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_RSP_DATA_LEN + 1)
#define EXP_TWI_PQM_PRBS_PATTERN_MODE_SET_CMD_DATA_LEN          sizeof(exp_pqm_prbs_pattern_struct)
#define EXP_TWI_PQM_PRBS_PATTERN_MODE_SET_CMD_LEN               (EXP_TWI_PQM_PRBS_PATTERN_MODE_SET_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_PRBS_USER_PATTERN_SET_CMD_DATA_LEN          sizeof(exp_pqm_prbs_user_pattern_struct)
#define EXP_TWI_PQM_PRBS_USER_PATTERN_SET_CMD_LEN               (EXP_TWI_PQM_PRBS_USER_PATTERN_SET_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_PRBS_MONITOR_CONTROL_CMD_DATA_LEN           2
#define EXP_TWI_PQM_PRBS_MONITOR_CONTROL_CMD_LEN                (1 + EXP_TWI_PQM_PRBS_MONITOR_CONTROL_CMD_DATA_LEN)
#define EXP_TWI_PQM_PRBS_GENERATOR_CONTROL_CMD_DATA_LEN         2
#define EXP_TWI_PQM_PRBS_GENERATOR_CONTROL_CMD_LEN              (1 + EXP_TWI_PQM_PRBS_GENERATOR_CONTROL_CMD_DATA_LEN)
#define EXP_TWI_PQM_PRBS_ERR_COUNT_START_CMD_LEN                1
#define EXP_TWI_PQM_PRBS_ERR_COUNT_READ_CMD_LEN                 1
#define EXP_TWI_PQM_PRBS_ERR_COUNT_READ_RSP_DATA_LEN            sizeof(exp_pqm_prbs_error_count_struct)
#define EXP_TWI_PQM_PRBS_ERR_COUNT_READ_RSP_LEN                 (EXP_TWI_PQM_PRBS_ERR_COUNT_READ_RSP_DATA_LEN + EXP_TWI_PQM_PRBS_ERR_COUNT_READ_CMD_LEN)
#define EXP_TWI_PQM_HORZ_BATHTUB_GET_CMD_DATA_LEN               8
#define EXP_TWI_PQM_HORZ_BATHTUB_GET_START_CMD_LEN              (EXP_TWI_PQM_HORZ_BATHTUB_GET_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_HORZ_BATHTUB_GET_READ_CMD_LEN               1
#define EXP_TWI_PQM_HORZ_BATHTUB_GET_RSP_DATA_LEN               sizeof(exp_pqm_horz_bt_struct)
#define EXP_TWI_PQM_HORZ_BATHTUB_GET_RSP_LEN                    (EXP_TWI_PQM_HORZ_BATHTUB_GET_RSP_DATA_LEN + 1)
#define EXP_TWI_PQM_VERT_BATHTUB_GET_CMD_DATA_LEN               9
#define EXP_TWI_PQM_VERT_BATHTUB_GET_START_CMD_LEN              (EXP_TWI_PQM_VERT_BATHTUB_GET_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_VERT_BATHTUB_GET_READ_CMD_LEN               1
#define EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN               sizeof(exp_pqm_vert_bt_struct)
#define EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_LEN                    (EXP_TWI_PQM_VERT_BATHTUB_GET_RSP_DATA_LEN + 2)
#define EXP_TWI_PQM_TWOD_BATHTUB_GET_CMD_DATA_LEN               10
#define EXP_TWI_PQM_TWOD_BATHTUB_GET_START_CMD_LEN              (EXP_TWI_PQM_TWOD_BATHTUB_GET_CMD_DATA_LEN + 2)
#define EXP_TWI_PQM_TWOD_BATHTUB_GET_READ_CMD_LEN               1
#define EXP_TWI_DELAY_LINE_UPDATE_CMD_LEN                       1

/* API command flags */
#define EXP_FW_NO_EXTENDED_DATA         0
#define EXP_FW_EXTENDED_DATA            1
#define EXP_FW_EXTENDED_DATA_BIT_MASK   0x01

/* API response codes */
#define EXP_FW_API_SUCCESS              0
#define EXP_FW_API_FAILURE              1

/* API error conditions */
#define EXP_FW_API_CMD_ERR              1
#define EXP_FW_API_CMD_CRC_ERR          2
#define EXP_FW_API_CMD_DATA_CRC_ERR     3

/* API flags */
#define EXP_FW_NO_EXTENDED_DATA         0
#define EXP_FW_EXTENDED_DATA            1
#define EXP_FW_EXTENDED_DATA_BITMSK     0x01
#define EXP_FW_EXTENDED_DATA_BITMOFF    0

/* Temperature interval read API command flags */
#define EXP_FW_TEMP_DIMM0_NOT_PRESENT     0
#define EXP_FW_TEMP_DIMM0_PRESENT         1
#define EXP_FW_TEMP_DIMM0_PRESENT_BITMSK  0x02
#define EXP_FW_TEMP_DIMM0_PRESENT_BITOFF  1

#define EXP_FW_TEMP_DIMM1_NOT_PRESENT     0
#define EXP_FW_TEMP_DIMM1_PRESENT         1
#define EXP_FW_TEMP_DIMM1_PRESENT_BITMSK  0x04
#define EXP_FW_TEMP_DIMM1_PRESENT_BITOFF  2

#define EXP_FW_TEMP_ONCHIP_NOT_PRESENT    0
#define EXP_FW_TEMP_ONCHIP_PRESENT        1
#define EXP_FW_TEMP_ONCHIP_PRESENT_BITMSK 0x8
#define EXP_FW_TEMP_ONCHIP_PRESENT_BITOFF 3

/* OpenCAPI command structure field lengths */
#define EXP_OC_CMD_PADDING_LEN          12
#define EXP_OC_CMD_PARMS_LEN            28

/* OpenCAPI response structure field lengths */
#define EXP_OC_RESP_PADDING_LEN         16
#define EXP_OC_RESP_PARMS_LEN           28

/* DDR SPD and PHY data lengths */
#define ECH_SPD_LEN                     512
#define ECH_DDR_PHY_LEN                 sizeof(user_input_msdg_t)

/* firmware log size */
#define EXP_FW_LOG_SIZE_4KB             (4 * 1024)

/* SerDes support lane configurations */
#define EXP_SERDES_1_LANE               1
#define EXP_SERDES_4_LANE               4
#define EXP_SERDES_8_LANE               8

/* SerDes lane configuration errors */
#define EXP_SERDES_LANE_OOR             1
#define EXP_SERDES_LANE_UNSUPPORTED     2
#define EXP_SERDES_LANE_SERDES_INIT_ERR 3
#define EXP_SERDES_LANE_PHY_RESET_ERR   4
#define EXP_SERDES_LANE_DL_RESET_ERR    5

/* SerDes support frequency configurations */
#define EXP_SERDES_21_33_GBPS           1
#define EXP_SERDES_23_46_GBPS           2
#define EXP_SERDES_25_60_GBPS           3


/* SerDes read adaptation object error */
#define EXP_SERDES_READ_ADAPT_OBJ_ERROR 1

/* SerDes calibration errors */
#define EXP_SERDES_RX_CALIB_ERROR       1
#define EXP_SERDES_CSU_CALIB_ERROR      1

/* frequency configuration errors */
#define EXP_SERDES_FREQ_NA              1

/* PRBS configuration error */
#define EXP_PRBS_PATTERN_NA             1

/* PRBS error count errors */
#define EXP_PRBS_MONITOR_DISABLED       1
#define EXP_PRBS_MONITOR_NOT_LOCKED     2
#define EXP_PRBS_MONITOR_ALL_ZEROES     3

/* PRBS pattern modes */
#define ECH_PQM_PRBS_PATTERN_5          0x00
#define ECH_PQM_PRBS_PATTERN_7          0x01
#define ECH_PQM_PRBS_PATTERN_11         0x02
#define ECH_PQM_PRBS_PATTERN_15         0x03
#define ECH_PQM_PRBS_PATTERN_20         0x04
#define ECH_PQM_PRBS_PATTERN_23         0x05
#define ECH_PQM_PRBS_PATTERN_31         0x06
#define ECH_PQM_PRBS_PATTERN_9          0x07
#define ECH_PQM_PRBS_PATTERN_USER       0x0F
#define ECH_PQM_PRBS_PATTERN_CJPAT      0x08
#define ECH_PQM_PRBS_PATTERN_CJTPAT     0x09

/* Horizontal Bathtub data capture failure */
#define EXP_HORZ_BATHTUB_FAILURE        1

/* Vertical Bathtub data capture failure */
#define EXP_VERT_BATHTUB_FAILURE        1

/* 2 Dimensional data capture failure */
#define EXP_2D_BATHTUB_FAILURE          1

/* phases for 2D bathtub */
#define EXP_2D_BATHTUB_PHASES           32
#define EXP_2D_BATHTUB_PHASE_MIN        -16
#define EXP_2D_BATHTUB_PHASE_MAX        15


/*
* Macro Definitions
*/

/**
* @brief
*   This macro shall be used to creates error codes for the ECH module.
*   Each ECH error code has 32 bits and is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | ECH error base (16-bit) | error code suffix (12-bit)]
*   where
*     ECH error base    - 16-bit PMCFW_ERR_BASE_ECH, defined in "pmcfw_mid.h".
*     error code suffix - 12-bit specific error code suffix input to the macro.
*
* @param [in] err_suffix - 12-bit error suffix.  See above for details.
*
* @return 
*   Error code in format described above.
*
* @note
*   For success, the return code = PMC_SUCCESS.
*
* @hideinitializer
*/
#define ECH_ERR_CODE_CREATE(err_suffix)     ((PMCFW_ERR_BASE_ECH) | (err_suffix))

/* Error code definitions */
#define EXP_SUCCESS                         ECH_ERR_CODE_CREATE(0x0000) /* Explorer success indication */
#define EXP_TWI_MEM_ALLOC_ERR               ECH_ERR_CODE_CREATE(0x0001) /* could not allocate memory for ECH module*/
#define EXP_TWI_SLAVE_PORT_CFG_ERR          ECH_ERR_CODE_CREATE(0x0002) /* invalid TWI slave port configuration */
#define EXP_INVALID_CTRL_LIST_ERR           ECH_ERR_CODE_CREATE(0x0003) /* invalid control list pointer */
#define EXP_INVALID_FUNC_PTR_ERR            ECH_ERR_CODE_CREATE(0x0004) /* invalid function pointer */
#define EXP_INVALID_FUNC_CMD_ERR            ECH_ERR_CODE_CREATE(0x0005) /* invalid function command */
#define EXP_FW_LOG_OP_NO_ACTIVE_LOG         ECH_ERR_CODE_CREATE(0x0006) /* no active firmware log */ 
#define EXP_FW_LOG_OP_NO_STORED_LOG         ECH_ERR_CODE_CREATE(0x0007) /* no stored firmware log */ 
#define EXP_TWI_INVALID_CMD                 ECH_ERR_CODE_CREATE(0x0008) /* invalid exp_twi_cmd_enum command. */

/*
* Enumerated Types
*/

/**
*  @brief
*   Explorer TWI Commands
*/
typedef enum
{
    EXP_FW_TWI_CMD_NULL = 0,                        /**< Null command */
    EXP_FW_TWI_CMD_BOOT_CONFIG,                     /**< Boot config */
    EXP_FW_TWI_CMD_STATUS,                          /**< Status */
    EXP_FW_TWI_CMD_REG_ADDR_LATCH,                  /**< Address latch */
    EXP_FW_TWI_CMD_REG_READ,                        /**< Register read */
    EXP_FW_TWI_CMD_REG_WRITE,                       /**< Register write */
    EXP_FW_TWI_CMD_FW_DOWNLOAD,                     /**< Firmware download */
    EXP_FW_CONT_REG_READ,                           /**< Continuous read */
    EXP_FW_CONT_REG_WRITE,                          /**< Continuous write */
    EXP_FW_BYPASS_4SEC_TIMEOUT,                     /**< Skip the 4 sec wait */
    EXP_FW_PQM_LANE_SET,                            /**< Set SerDes lane configuration */ 
    EXP_FW_PQM_LANE_GET,                            /**< Get SerDes lane configuration */
    EXP_FW_PQM_FREQ_SET,                            /**< Set SerDes clock frequency */
    EXP_FW_PQM_FREQ_GET,                            /**< Get SerDes clock frequency */
    EXP_FW_PQM_LANE_TRAINING,                       /**< Initiate SerDes training */
    EXP_FW_PQM_TRAINING_RESET,                      /**< Reset SerDes training */
    EXP_FW_PQM_RX_ADAPTATION_OBJ_START,             /**< Start RX adaptation object command */
    EXP_FW_PQM_RX_ADAPTATION_OBJ_READ,              /**< Step 2, read the data */
    EXP_FW_PQM_RX_CALIBRATION_VALUE_START,          /**< Start RX calibration read command */
    EXP_FW_PQM_RX_CALIBRATION_VALUE_READ,           /**< Step 2, read the data */
    EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_START,  /**< Start CSU calibration status command */
    EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_READ,   /**< Step 2, read the data */
    EXP_FW_PQM_PRBS_PATTERN_MODE_SET,               /**< Set SerDes pattern generator mode */
    EXP_FW_PQM_PRBS_USER_DEFINED_PATTERN_SET,       /**< Set user defined SerDes pattern generator */
    EXP_FW_PQM_PRBS_MONITOR_CONTROL,                /**< Enable SerDes PRBS monitoring engine */
    EXP_FW_PQM_PRBS_GENERATOR_CONTROL,              /**< Enable SerDes PRBS generation engine */
    EXP_FW_PQM_PRBS_ERR_COUNT_START,                /**< Read SerDes error count register */
    EXP_FW_PQM_PRBS_ERR_COUNT_READ,                 /**< Step 2, read the data */
    EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_START,        /**< Read horizontal bathtub values */
    EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_READ,
    EXP_FW_PQM_VERTICAL_BATHTUB_GET_START,          /**< Read vertical bathtub values */
    EXP_FW_PQM_VERTICAL_BATHTUB_GET_READ,
    EXP_FW_PQM_2D_BATHTUB_GET_START,                /**< Read 2d bathtub values */
    EXP_FW_PQM_2D_BATHTUB_GET_READ,
    EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE,             /**< Command to force DDR PHY delay line update */
    EXP_FW_TWI_CMD_MAX

} exp_twi_cmd_enum;

/**
* @brief
*  Explorer Commands
*/
typedef enum
{
    EXP_FW_NULL_CMD = 0,                    /**< Null command */
    EXP_FW_PHY_STEP_BY_STEP_INIT,           /**< Initialize DDR controller step-by-step */
    EXP_FW_DDR_PHY_INIT,                    /**< Initialize DDR PHY interface */
    EXP_FW_TEMP_SENSOR_PASS_THROUGH_READ,   /**< Initialize the temperature sensor */
    EXP_FW_TEMP_SENSOR_PASS_THROUGH_WRITE,  /**< Initialize the temperature sensor */
    EXP_FW_TEMP_SENSOR_CONFIG_INTERVAL_READ,/**< Initialize the temperature sensor */
    EXP_FW_GO_COMMAND,                      /**< Execute firmware */
    EXP_FW_ADAPTER_PROPERTIES_GET,          /**< Get adapter properties */
    EXP_FW_BINARY_UPGRADE,                  /**< Firmware upgrade with binary image */
    EXP_FW_FLASH_LOADER_VERSION_INFO,       /**< Get flash loader version information */
    EXP_FW_LOG,                             /**< Access and manage firmware logs */
    EXP_FW_MAX_CMD 

} exp_cmd_enum;

/**
* @brief
*  Explorer Log Command Operands
*/
typedef enum
{
    EXP_FW_LOG_OP_NULL = 0,                 /**< Null operand */
    EXP_FW_LOG_OP_READ_ACTIVE_LOG,          /**< Read from active firmware logfile */
    EXP_FW_LOG_OP_READ_SAVED_LOG,           /**< Read from saved firmware logfile */
    EXP_FW_LOG_OP_ACTIVE_CLR,               /**< Clear active logfile */
    EXP_FW_LOG_OP_SAVED_CLR                 /**< Clear saved logfile */
} exp_fw_log_cmd_ops;

/**
* @brief
*  Explorer PHY INIT Command Operands
*/
typedef enum
{
    EXP_FW_PHY_INIT_DEFAULT_TRAIN   = 0,   /**< Standard PHY training with response */
    EXP_FW_PHY_INIT_READ_EYE_TRAIN  = 1,   /**< Train and return 2D read eye */
    EXP_FW_PHY_INIT_WRITE_EYE_TRAIN = 2,   /**< Train and return 2D write eye */
} exp_phy_init_cmd_ops;

/*
* Structures and Unions
*/

/**
* @brief
*  Explorer command structure
*/
typedef struct
{
    UINT8  id;              /**< Command ID*/
    UINT8  flags;           /**< Command flags */
    UINT16 req_id;          /**< Request identifier */
    UINT32 ext_data_len;    /**< Length in bytes of data in extended data buffer */
    UINT32 ext_data_crc;    /**< CRC of data in extended data buffer */
    UINT32 host_spad_area;  /**< Host scratchpad area */
    UINT32 ech_spad_area;   /**< Explorer scratchpad area */
    UINT8  padding[EXP_OC_CMD_PADDING_LEN];  /**< Reserved, aligns to cache line */
    UINT8  parms[EXP_OC_CMD_PARMS_LEN];      /**< Additional command parameters */
    UINT32 crc;             /**< CRC of command */

} exp_cmd_struct;

/**
* @brief
*  Explorer response structure
*/
typedef struct
{
    UINT8  id;              /**< Response ID (same as command) */
    UINT8  flags;           /**< Response flags */
    UINT16 req_id;          /**< Request identifier (same as commmand) */
    UINT32 ext_data_len;    /**< Length in bytes of data in extended data buffer */
    UINT32 ext_data_crc;    /**< CRC of data in extended data buffer */
    UINT32 host_spad_area;  /**< Host scratchpad area */
    UINT8  padding[EXP_OC_RESP_PADDING_LEN];  /**< Reserved, aligns to cache line */
    UINT8  parms[EXP_OC_RESP_PARMS_LEN];      /**< Additional response parameters */
    UINT32 crc;             /**< CRC of response */

} exp_rsp_struct;

/**
*  @brief
*   ECH TWI Deferred Command Handler Structure
*/
typedef struct
{
    volatile BOOL       deferred_cmd_flag;                      /**< When Set, VPE0 will process this command */
    exp_twi_cmd_enum    command_id;                             /**< Command ID */
    UINT32              (*deferred_cmd_handler)(UINT8 *, UINT32);       /**< Command Handler */
    UINT8               *cmd_buf;                               /**< Command Buffer required for command handler */
    UINT32               cmd_buf_idx;                           /**< Command Buffer index required for command handler */
    VOID                (*callback_handler) (UINT8);            /**< Callback Handler */
} ech_twi_deferred_cmd_handler_struct;

/** 
*  @brief 
*   Explorer Log command operands 
*/
typedef struct
{
    UINT8  op;          /**< Firmware log operation, enumerator from exp_fw_log_cmd_ops */

} exp_fw_log_cmd_parms_struct;

/** 
*  @brief 
*   Explorer Log response operands 
*/
typedef struct
{
    UINT8  op;                 /**< Firmware log operation, enumerator from exp_fw_log_cmd_ops */
    UINT8  status;             /**< Operation pass/fail status */
    UINT32 err_code;           /**< Specific error code if operation failed */
    UINT32 num_bytes_returned; /**< Number of bytes returned */

} exp_fw_log_rsp_parms_struct;

/** 
*  @brief 
*   Explorer phy init response operands
*/
typedef __packed struct
{
    UINT8  status;             /**< Operation pass/fail status */
    UINT32 err_code;           /**< Specific error code if operation failed */
} exp_fw_phy_init_rsp_parms_struct;

/**
*  @brief
*   Explorer phy init cmd operands
*/
typedef __packed struct
{
    UINT8  phy_init_mode;                                  /**< Phy initialization mode */
    UINT8  reserved[EXP_TWI_PHY_INIT_PARMS_RESERVED_LEN];  /**< Reserved for future use */
} exp_fw_phy_init_cmd_parms_struct;

/**
*  @brief
*   Explorer pass-through temperature read response operands
*/
typedef __packed struct
{
    UINT8  status;             /**< Operation pass/fail status */
    UINT16 err_code;           /**< Specific error code if operation failed */
    UINT8  reserved;           /**< Reserved for future use */
    UINT8  resp_data[EXP_TWI_TEMP_READ_RESPONSE_DATA_LEN];  /** Data read from the starting offset */
} exp_fw_temp_read_rsp_parms_struct;

/**
*  @brief
*   Explorer pass-through temperature read cmd operands
*/
typedef __packed struct
{
    UINT8  twi_address;   /**< I2C address */
    UINT8  twi_cmd_flags; /**< I2C command flags */
    UINT16 num_bytes;     /**< Number of bytes to read */
} exp_fw_temp_read_cmd_parms_struct;

/**
*  @brief
*   Explorer pass-through temperature write response operands
*/
typedef __packed struct
{
    UINT8  status;             /**< Operation pass/fail status */
    UINT16 err_code;           /**< Specific error code if operation failed */
} exp_fw_temp_write_rsp_parms_struct;

/**
*  @brief
*   Explorer pass-through temperature write cmd operands
*/
typedef __packed struct
{
    UINT8  twi_address;   /**< I2C address */
    UINT8  twi_cmd_flags; /**< I2C command flags */
    UINT16 num_bytes;     /**< Number of bytes to write */
    UINT8  data_bytes[EXP_TWI_TEMP_WRITE_COMMAND_DATA_LEN]; /**< Data bytes */
} exp_fw_temp_write_cmd_parms_struct;

/**
*  @brief
*   Explorer interval temperature write cmd operands
*/
typedef __packed struct
{
    UINT8  onboard_dimm0_twi_address;     /**< onboard dimm0 I2C address */
    UINT8  onboard_dimm0_twi_reg_offset;  /**< onboard dimm0 I2C register offset */
    UINT8  onboard_dimm0_twi_reg_length;  /**< onboard dimm0 I2C address register length */
    UINT8  onboard_dimm1_twi_address;     /**< onboard dimm1 I2C address */
    UINT8  onboard_dimm1_twi_reg_offset;  /**< onboard dimm1 I2C register offset */
    UINT8  onboard_dimm1_twi_reg_length; /**< onboard dimm1 I2C address register offset size */
    UINT8  onchip_config_flags;           /**< onchip configuration flags */
    UINT8  onchip_twi_address;            /**< onchip I2C address */
    UINT16 onchip_twi_reg_offset_0;       /**< onchip I2C register offset 0 */
    UINT16 onchip_twi_reg_offset_1;       /**< onchip I2C register offset 1 */
    UINT16 read_interval_ms;              /**< temperature sensor read interval ms */
    UINT8  reserved[14];                  /**< 14 bytes reserved */
} exp_fw_temp_interval_read_cmd_parms_struct;

/**
*  @brief
*   Explorer PQM Lane Set/Get command/response
*/
typedef struct
{
    UINT8  cfg;     /**< number of lanes */

} exp_pqm_lanes_struct;

/** 
*  @brief 
*   Explorer PQM Frequency Set/Get command/response
*/
typedef struct
{
    UINT8  cfg;     /**< frequency */

} exp_pqm_freq_struct;

/** 
*  @brief 
*   Explorer PQM Receive Adapation Object
*/
typedef struct
{
    UINT32 iq_offset;
    UINT32 afir_offset;
    UINT32 afir_c1;
    UINT32 afir_c2;
    UINT32 afir_c3;
    UINT32 dc_offset;
    UINT32 udfe_thr_00;
    UINT32 udfe_thr_11;
    UINT32 median_amp;

} exp_pqm_rx_adapt_obj_struct;

/** 
*  @brief 
*   Explorer PQM Receive Calibration Value
*/
typedef struct
{
    UINT32 phgen_state_obs;
    UINT32 phgen_dac_code_n_obs;
    UINT32 phgen_dac_code_p_obs;
    UINT32 phgen_ct_n_obs;
    UINT32 phgen_ct_p_obs;
    UINT32 phgen_update_dir_n_obs;
    UINT32 phgen_update_dir_p_obs;
    UINT32 phgen_sign;
    UINT32 phgen_unenc;
    UINT32 ocdac_d_sign;
    UINT32 ocdac_d_unenc;

} exp_pqm_rx_calib_value_struct;

/** 
*  @brief 
*   Explorer PQM CSU Calibration Value Status
*/
typedef struct
{
    UINT32 ppm_offset;
    UINT32 vco_mom_val;
    UINT32 ic_ctrl_curr_addr;
    UINT32 acc_val_msb;
    UINT32 acc_val_lsb;
    UINT32 csu_status;

} exp_pqm_csu_calib_value_status_struct;

/** 
*  @brief 
*   Explorer PQM PRBS Pattern Mode
*/
typedef struct
{
    UINT8 mode;

} exp_pqm_prbs_pattern_struct;

/** 
*  @brief 
*   Explorer PQM PRBS User Defined Pattern
*/
typedef struct
{
    UINT8 patt[10];

} exp_pqm_prbs_user_pattern_struct;

/** 
*  @brief 
*   Explorer PQM PRBS Error Count
*/
typedef struct
{
    UINT32 count[EXP_SERDES_8_LANE];

} exp_pqm_prbs_error_count_struct;

/** 
*  @brief 
*   Explorer PQM Horizontal Bathtub
*/
typedef struct
{
    UINT32 bin_0;
    UINT32 bin_1;
    UINT32 bin_2;
    UINT32 bin_3;
    UINT32 bin_4;
    UINT32 bin_5;
    UINT32 bin_6;
    UINT32 bin_7;
    UINT32 bin_8;
    UINT32 bin_9;
    UINT32 bin_10;
    UINT32 bin_11;
    UINT32 bin_12;
    UINT32 bin_13;
    UINT32 bin_14;
    UINT32 bin_15;
    UINT32 bin_16;
    UINT32 bin_17;
    UINT32 bin_18;
    UINT32 bin_19;
    UINT32 bin_20;
    UINT32 bin_21;
    UINT32 bin_22;
    UINT32 bin_23;
    UINT32 bin_24;
    UINT32 bin_25;
    UINT32 bin_26;
    UINT32 bin_27;
    UINT32 bin_28;
    UINT32 bin_29;
    UINT32 bin_30;
    UINT32 bin_31;

} exp_pqm_horz_bt_struct;

/** 
*  @brief 
*   Explorer PQM Vertical Bathtub
*/
typedef struct
{
    UINT32 bin_0;
    UINT32 bin_1;
    UINT32 bin_2;
    UINT32 bin_3;
    UINT32 bin_4;
    UINT32 bin_5;
    UINT32 bin_6;
    UINT32 bin_7;
    UINT32 bin_8;
    UINT32 bin_9;
    UINT32 bin_10;
    UINT32 bin_11;
    UINT32 bin_12;
    UINT32 bin_13;
    UINT32 bin_14;
    UINT32 bin_15;
    UINT32 bin_16;
    UINT32 bin_17;
    UINT32 bin_18;
    UINT32 bin_19;
    UINT32 bin_20;
    UINT32 bin_21;
    UINT32 bin_22;
    UINT32 bin_23;
    UINT32 bin_24;
    UINT32 bin_25;
    UINT32 bin_26;
    UINT32 bin_27;
    UINT32 bin_28;
    UINT32 bin_29;
    UINT32 bin_30;
    UINT32 bin_31;
    UINT32 bin_32;
    UINT32 bin_33;
    UINT32 bin_34;
    UINT32 bin_35;
    UINT32 bin_36;
    UINT32 bin_37;
    UINT32 bin_38;
    UINT32 bin_39;
    UINT32 bin_40;
    UINT32 bin_41;
    UINT32 bin_42;
    UINT32 bin_43;
    UINT32 bin_44;
    UINT32 bin_45;
    UINT32 bin_46;
    UINT32 bin_47;
    UINT32 bin_48;
    UINT32 bin_49;
    UINT32 bin_50;
    UINT32 bin_51;
    UINT32 bin_52;
    UINT32 bin_53;
    UINT32 bin_54;
    UINT32 bin_55;
    UINT32 bin_56;
    UINT32 bin_57;
    UINT32 bin_58;
    UINT32 bin_59;
    UINT32 bin_60;
    UINT32 bin_61;
    UINT32 bin_62;
    UINT32 bin_63;

} exp_pqm_vert_bt_struct;

/** 
*  @brief 
*   Explorer PQM Data Union
*/
typedef union
{
    exp_pqm_lanes_struct                  lanes;        /**< lane configuration */
    exp_pqm_freq_struct                   freq;         /**< frequency configuration */
    exp_pqm_rx_adapt_obj_struct           rx_adapt;     /**< receiver adaptation data */
    exp_pqm_rx_calib_value_struct         rx_calib;     /**< receiver calibration data */
    exp_pqm_csu_calib_value_status_struct csu_calib;    /**< CSU calibration value and status */
    exp_pqm_prbs_pattern_struct           prbs_pat;     /**< PRBS standard pattern */
    exp_pqm_prbs_user_pattern_struct      user_pat;     /**< PRBS user defined pattern */
    exp_pqm_prbs_error_count_struct       prbs_err;     /**< PRBS error count */
    exp_pqm_horz_bt_struct                horz_bt;      /**< horizontal bathtub data */
    exp_pqm_vert_bt_struct                vert_bt;      /**< vertical bathtub data */

} exp_pqm_data_struct;


/*
** Function Prototypes
*/

/*
** Extern variables
*/

EXTERN ech_twi_deferred_cmd_handler_struct ech_def_handler;


#endif /* _ECH_API_H */
/** @} end addtogroup */



