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
EXTERN UINT32 ech_twi_boot_config_proc(UINT8* rx_buf, UINT32 rx_index);


/*
** Global Variables
*/


/*
** Local Variables
*/


/*
* Private Functions
*/
                                                                       

/**
* @brief
*   Get the PQM enable/disable setting
*   
* @return
*   The PQM enable (TRUE) or disable (FALSE) status
*
* @note
*/
PRIVATE BOOL ech_twi_pqm_get(VOID)
{
    return (ech_pqm_mode);
}


/*
* Public Functions
*/


/**
* @brief
*   Set the Product Qualification Mode
* 
* @param[in] setting - TRUE to enable PQM, FALSE to disable PQM
*  
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_pqm_set(BOOL setting)
{
    ech_pqm_mode = setting;
}

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
                (void *)(&ech_twi_deferred_cmd_buf[0]),
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
        case EXP_FW_PQM_PRBS_ERR_COUNT_START:
        case EXP_FW_PQM_PRBS_ERR_COUNT_READ:
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
                ech_twi_status_byte = EXP_TWI_UNSUPPORTED;
            }
        }
        break;

        /* 
        ** Following 4 PQM commands are made avaiable outside of PQM mode
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
            /* process the command */
            ech_pqm_cmd_proc(rx_buf_ptr, rx_index);
        break;
        /* 
        ** Even though EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE is a PQM command, 
        ** it needs to be made available outside PQM mode.
        **/
       
        case EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE:
            /* Force Delay Line update PQM received, process it */
            memcpy(&ech_twi_deferred_cmd_buf[0], &rx_buf_ptr[rx_index], EXP_TWI_DELAY_LINE_UPDATE_CMD_LEN);
            ech_twi_status_byte_set(EXP_TWI_BUSY);
            ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE,
                &ech_pqm_force_delay_line_update, 
                (void *)(&ech_twi_deferred_cmd_buf[0]),
                &ech_twi_status_byte_set,
                EXP_TWI_DELAY_LINE_UPDATE_CMD_LEN,
                rx_index);
            
        break;
        case EXP_FW_TWI_CMD_FW_DOWNLOAD:
        case EXP_FW_TWI_CMD_NULL:
        default:
            bc_printf("COMMAND : 0x%x\n", rx_buf_ptr[rx_index]);
            bc_printf("twi_activity 0x%08x\n: ", twi_activity);
            bc_printf("ech_twi_rx_len : 0x%08x'n", ech_twi_rx_len);
            bc_printf("ech_twi_rx_index : 0x%08x\n",  ech_twi_rx_index);
            PMCFW_ASSERT(FALSE, EXP_TWI_INVALID_CMD);
        break;
    }
 }

   
   
