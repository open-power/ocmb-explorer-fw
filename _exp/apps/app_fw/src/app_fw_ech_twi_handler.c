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
*   Explorer Platform specific Command Handler.
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
#include "ech_twi_common.h"
#include "ech_pqm.h"
#include "pmc_profile.h"
#include "ech.h"
#include "mem.h"
#include "crc32.h"
#include "twi.h"
#include "twi_api.h"
#include "app_fw.h"
#include "top_exp_config_guide.h"
#include "ocmb_config_guide.h"
#include <string.h>
#include "serdes_plat.h"
#include "pmc_plat.h"
#include "serdes_api.h"

/*
* Local Enumerated Types
*/


/*
** Local Constants
*/


/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/


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
*   Process TWI slave interface to the host.
*
* @param [in]  port_id    - TWI port to check for activity
* @param [in]  rx_buf_ptr - Pointer to TWI rx buffer
* @param [out] rx_index   - Current index to rx buffer
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_plat_slave_proc(UINT32 port_id, UINT8 *rx_buf_ptr, UINT8 rx_index)
{
    switch (rx_buf_ptr[rx_index])
    {
        case EXP_FW_TWI_CMD_BOOT_CONFIG:
        {
            /* Boot Config command received, process it */
            memcpy(&ech_twi_deferred_cmd_buf[0],&rx_buf_ptr[rx_index],EXP_TWI_BOOT_CFG_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_TWI_CMD_BOOT_CONFIG,
                                                       &ech_twi_boot_config_proc, 
                                                       (VOID*)(&ech_twi_deferred_cmd_buf[0]),
                                                       &ech_twi_status_byte_set,
                                                       EXP_TWI_BOOT_CFG_CMD_LEN,
                                                       rx_index);
            g_exp_fw_twi_cmd_boot_config++;
        }
        break;

        case EXP_FW_TWI_CMD_STATUS:
        {
            /* Status command received, process it */
            ech_twi_status_proc(port_id);
            g_count_exp_fw_twi_cmd_status++;
        }
        break;
        

        case EXP_FW_TWI_CMD_REG_ADDR_LATCH:
        {
            /* Register Address Latch command received, process it */
            ech_twi_reg_addr_latch_proc(&rx_buf_ptr[rx_index]);
            g_exp_fw_twi_cmd_reg_addr_latch++;
        }
        break;

        case EXP_FW_TWI_CMD_REG_READ:
        {
            /* Register Read command received, process it */
            ech_twi_reg_read_proc(port_id, &rx_buf_ptr[rx_index]);
            g_exp_fw_twi_cmd_reg_read++;
        }
        break;

        case EXP_FW_TWI_CMD_REG_WRITE:
        {
            /* Register Write command received, process it */
            ech_twi_reg_write_proc(&rx_buf_ptr[rx_index]);
            g_exp_fw_twi_cmd_reg_write++;
        }
        break;

        /* Following PQM commands are only available in PQM mode */
        case EXP_FW_PQM_LANE_SET:
        case EXP_FW_PQM_LANE_GET:
        case EXP_FW_PQM_FREQ_SET:
        case EXP_FW_PQM_FREQ_GET:
        case EXP_FW_PQM_LANE_TRAINING:
        case EXP_FW_PQM_TRAINING_RESET:
        case EXP_FW_PQM_RX_CALIBRATION_VALUE_START:
        case EXP_FW_PQM_RX_CALIBRATION_VALUE_READ:
        case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_START:
        case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_READ:
        case EXP_FW_PQM_PRBS_PATTERN_MODE_SET:
        case EXP_FW_PQM_PRBS_USER_DEFINED_PATTERN_SET:
        case EXP_FW_PQM_PRBS_MONITOR_CONTROL:
        case EXP_FW_PQM_PRBS_GENERATOR_CONTROL:
        {
            /* Product Qualification Mode Command received */
    
            if (TRUE == ech_twi_pqm_get())
            {
                /* PQM has been selected */

                /* process the command */
                ech_pqm_cmd_proc(rx_buf_ptr, rx_index);
            }
            else
            {
                /* PQM has not been selected, increment command pointer */
                ech_pqm_cmd_rx_index_increment(rx_buf_ptr, rx_index);

                /* set the Status register */
                ech_twi_status_byte_set(EXP_TWI_UNSUPPORTED);
            }
        }
        break;

        /* 
        ** Following PQM commands are made available outside of PQM mode
        ** Please refer to JIRA EDBC-417
        ** Assumption is: SerDes will be initialzied by the HOST prior to the 
        ** execution of the following commands
        */ 
        case EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_START:
        case EXP_FW_PQM_VERTICAL_BATHTUB_GET_START:
        case EXP_FW_PQM_2D_BATHTUB_GET_START:
        case EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_READ:
        case EXP_FW_PQM_VERTICAL_BATHTUB_GET_READ:
        case EXP_FW_PQM_2D_BATHTUB_GET_READ:
        case EXP_FW_PQM_RX_ADAPTATION_OBJ_START:
        case EXP_FW_PQM_RX_ADAPTATION_OBJ_READ:                
        case EXP_FW_PQM_PRBS_ERR_COUNT_START:
        case EXP_FW_PQM_PRBS_ERR_COUNT_READ:
        {
            /* process the command */
            ech_pqm_cmd_proc(rx_buf_ptr, rx_index);
        }
        break;

        /* 
        ** EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE needs to be supported outside of PQM mode
        */
        case EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE:
        {    
            /* Force Delay Line update PQM received, process it */
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_PQM_DELAY_LINE_UPDATE_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE,
                                                       &ech_pqm_force_delay_line_update, 
                                                       (VOID*)(&ech_twi_deferred_cmd_buf[0]),
                                                       &ech_twi_status_byte_set,
                                                       EXP_TWI_PQM_DELAY_LINE_UPDATE_CMD_LEN,
                                                       rx_index);
        }
        break;

        case EXP_FW_TWI_POLL_ABORT:
        {
            bc_printf("Poll Abort command received\n");

            if (FALSE == ech_twi_poll_abort_flag_get())
            {
                /* abort flag is not set so code on VPE0 is in infinite loop */

                /* set the poll abort flag to exit polling loop */
                ech_twi_poll_abort_flag_set(TRUE);
            }

            /* increment receive buffer index */
            ech_twi_rx_index_inc(EXP_TWI_POLL_ABORT_CMD_LEN);
        }
        break;

        case EXP_FW_CDR_OFFSET_FROM_CAL_SET:
        {
            ech_twi_cdr_offset_from_cal_set(rx_buf_ptr, rx_index, port_id);
        }
        break;

        case EXP_FW_CDR_BANDWIDTH_SET:
        {
            ech_twi_cdr_bandwidth_set(rx_buf_ptr, rx_index, port_id);
        }
        break;

        case EXP_FW_CONT_SERDES_CAL_DISABLE:
        {
            ech_twi_continuous_serdes_cal_disable(rx_buf_ptr, rx_index, port_id);
        }
        break;

        case EXP_FW_PRBS_CAL_STATUS_READ:
        {
            ech_twi_prbs_cal_status(rx_buf_ptr, rx_index, port_id);
        }
        break;

        case EXP_FW_READ_ACTIVE_LOGS:
        {
            ech_twi_read_active_logs(rx_buf_ptr, rx_index, port_id);
        }
        break;

        case EXP_FW_READ_SAVED_DDR_PARAMS:
        {
            ech_twi_read_saved_ddr_params(rx_buf_ptr, rx_index, port_id);
        }
        break;

        case EXP_FW_TWI_FFE_SETTINGS:
        {
            /* 
            ** host is providing FFE pre- and post-cursor values
            ** validate these settings as well as calibration value based on 
            ** efuse register setting 
            */ 
            UINT32 fuse_val_stat_1 = serdes_api_fuse_val_stat_1_read();
            UINT8  extended_error_code = EXP_TWI_SUCCESS;

            /* 
            ** set the FFE calibration value as per mixed signal:
            ** CAL (in decimal) = 64 + TX3_P3A_D1EN[5]*6 + TX3_P3A_D1EN[4]*4 + TX3_P3A_D1EN[3]*2
            **                    + TX3_P3A_D1EN[2]*2 + TX3_P3A_D1EN[1]*1 + TX3_P3A_D1EN[0]*1
            **  
            ** serdes_ffe_fuse_val_stat_1 can be set to TX3_P3A_D1EN but a value for TX3_P3A_D1EN 
            ** is re-calculated as one of the set of the 13 6-bit values. The calculated value is 
            ** usually the same as serdes_ffe_fuse_val_stat_1. 
            */ 
            UINT32 calibration = SERDES_FFE_MIN_CALIBRATION +
                                 (((fuse_val_stat_1 & 0x20) >> 5) * 6) +
                                 (((fuse_val_stat_1 & 0x10) >> 4) * 4) +
                                 (((fuse_val_stat_1 & 0x08) >> 3) * 2) +
                                 (((fuse_val_stat_1 & 0x04) >> 2) * 2) +
                                 (((fuse_val_stat_1 & 0x02) >> 1) * 1) +
                                 (((fuse_val_stat_1 & 0x01) >> 0) * 1);


            /* extract the pre and post cursor and record the values */
            UINT32 precursor = (rx_buf_ptr[rx_index + 2] << 24) |
                               (rx_buf_ptr[rx_index + 3] << 16) |
                               (rx_buf_ptr[rx_index + 4] << 8)  |
                               (rx_buf_ptr[rx_index + 5] << 0);

            UINT32 postcursor = (rx_buf_ptr[rx_index + 6] << 24) |
                                (rx_buf_ptr[rx_index + 7] << 16) |
                                (rx_buf_ptr[rx_index + 8] << 8)  |
                                (rx_buf_ptr[rx_index + 9] << 0);

            /* validate the FFE precursor, postcursor, and calibration values */
            if ((precursor <= SERDES_FFE_MAX_PRECURSOR) &&
                (postcursor <= SERDES_FFE_MAX_POSTCURSOR) &&
                ((SERDES_FFE_MIN_CALIBRATION <= calibration) && (calibration <= SERDES_FFE_MAX_CALIBRATION)))
            {
                /* valid FFE parameter settings */

                /* record the precursor, postcursor and calibration values */
                serdes_plat_ffe_precursor_set(precursor);
                serdes_plat_ffe_postcursor_set(postcursor);
                serdes_plat_ffe_calibration_set(calibration);

                /* clear extended error code */
                ech_extended_error_code_set(EXP_TWI_SUCCESS);

                /* set success response */
                ech_twi_status_byte_set(EXP_TWI_SUCCESS);
            }
            else
            {
                if (precursor > SERDES_FFE_MAX_PRECURSOR)
                {
                    /* invalid precursor value */
                    extended_error_code |= EXP_TWI_FFE_ERROR_INVALID_PRECURSOR_BIT_FLAG; 
                }

                if (postcursor > SERDES_FFE_MAX_POSTCURSOR)
                {
                    /* invalid postcursor value */
                    extended_error_code |= EXP_TWI_FFE_ERROR_INVALID_POSTCURSOR_BIT_FLAG; 
                }

                if ((calibration < SERDES_FFE_MIN_CALIBRATION) || (calibration > SERDES_FFE_MAX_CALIBRATION))
                {
                    /* invalid calibration value */
                    extended_error_code |= EXP_TWI_FFE_ERROR_INVALID_CALIBRATION_BIT_FLAG;
                }

                /* set extended error code */
                bc_printf("TWI_FFE_SETTING: Invalid input values (0x%08x)\n", extended_error_code);
                ech_extended_error_code_set(extended_error_code);

                /* set error response */
                ech_twi_status_byte_set(EXP_TWI_ERROR);
            }

            /* increment receive buffer index */
            ech_twi_rx_index_inc(EXP_TWI_FFE_SETTINGS_CMD_LEN);
        }
        break;

        case EXP_FW_TWI_CMD_FW_DOWNLOAD:
        case EXP_FW_TWI_CMD_NULL:
        default:
        {
            bc_printf("COMMAND : 0x%x\n", rx_buf_ptr[rx_index]);
            bc_printf("twi_activity 0x%08x\n: ", twi_activity);
            bc_printf("ech_twi_rx_len : 0x%08x\n", ech_twi_rx_len);
            bc_printf("ech_twi_rx_index : 0x%08x\n",  ech_twi_rx_index);
            ech_twi_status_byte_set(EXP_TWI_UNSUPPORTED);
        }
        break;
    }
 }

   
   
