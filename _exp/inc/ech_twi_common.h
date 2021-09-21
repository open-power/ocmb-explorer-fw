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
* @addtogroup ECH_TWI_COMMON
* @{
* @file
* @brief
*   Explorer Command Handler definitions and declarations, which are used across flashloader and main FW code
*
* @note
*/
#ifndef _ECH_TWI_COMMON_H
#define _ECH_TWI_COMMON_H


/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_api.h"
#include "ech.h"


/*
** Constants
*/


/*
* Macro Definitions
*/


/*
* Enumerated Types
*/


/*
* Structures and Unions
*/
/**
*  @brief
*   ECH TWI Deferred Command Handler Structure
*/
typedef struct
{
    volatile BOOL       deferred_cmd_flag;                      /**< When Set, VPE0 will process this command */
    exp_twi_cmd_enum    command_id;                             /**< Command ID */
    UINT32              (*deferred_cmd_handler)(UINT8*, UINT32);/**< Command Handler */
    UINT8               *cmd_buf;                               /**< Command Buffer required for command handler */
    UINT32               cmd_buf_idx;                           /**< Command Buffer index required for command handler */
    VOID                (*callback_handler) (UINT8);            /**< Callback Handler */
} ech_twi_deferred_cmd_handler_struct;


/*
** Global variable
*/
EXTERN ech_twi_deferred_cmd_handler_struct ech_def_handler;
EXTERN UINT32 g_count_exp_fw_twi_cmd_status;
EXTERN UINT32 g_exp_fw_twi_cmd_boot_config;
EXTERN UINT32 g_exp_fw_twi_cmd_reg_addr_latch;
EXTERN UINT32 g_exp_fw_twi_cmd_reg_read;
EXTERN UINT32 g_exp_fw_twi_cmd_reg_write;
EXTERN UINT8 twi_cmd_id;
EXTERN UINT8 ech_twi_fw_mode_byte;
EXTERN UINT8 ech_twi_deferred_cmd_buf[EXP_TWI_MAX_BUF_SIZE];
EXTERN UINT32 ech_twi_rx_len;
EXTERN UINT32 ech_twi_rx_index;
EXTERN UINT32 twi_activity;


/*
* Function Prototypes
*/

EXTERN VOID ech_cmd_rxd_clr(VOID);
EXTERN ech_ctrl_struct* ech_ctrl_ptr_get(VOID);
EXTERN VOID ech_ctrl_ptr_set(ech_ctrl_struct* ctrl_ptr);
EXTERN BOOL ech_reg_addr_validate(UINT32 addr);
EXTERN VOID ech_raw_boot_cfg_set(UINT32 boot_cfg_flags);
EXTERN VOID ech_mfg_mode_set(VOID);
EXTERN VOID ech_ocapi_loopback_set(VOID);
EXTERN VOID ech_tl_mode_set(UINT32 mode);
EXTERN VOID ech_dl_boot_mode_set(UINT32 mode);
EXTERN VOID ech_host_boot_mode_set(VOID);
EXTERN VOID ech_serdes_freq_set(UINT32 freq);
EXTERN VOID ech_lane_cfg_set(UINT32 cfg);
EXTERN VOID ech_latched_reg_addr_set(UINT32 reg_addr);
EXTERN VOID ech_twi_status_proc(UINT32 port_id);
EXTERN VOID ech_twi_reg_read_proc(UINT32 port_id, UINT8* rx_buf);
EXTERN VOID ech_twi_reg_write_proc(UINT8* rx_buf);
EXTERN VOID ech_twi_reg_addr_latch_proc(UINT8* rx_buf);
EXTERN VOID ech_twi_init(UINT32 mst_port, UINT32 slv_port, UINT32 slv_addr);
EXTERN VOID ech_oc_init(VOID);
EXTERN VOID ech_twi_poll_abort_flag_set(BOOL abort_flag);
EXTERN BOOL ech_twi_poll_abort_flag_get(VOID);
EXTERN UINT8* ech_twi_tx_buf_get(VOID);
EXTERN VOID ech_twi_status_byte_set(UINT8 status_byte);
EXTERN VOID ech_twi_rx_index_inc(UINT32 bytes_processed);
EXTERN VOID ech_twi_slave_proc(UINT32 port_id);
EXTERN VOID ech_twi_fw_mode_set(UINT8 fw_mode_byte);
EXTERN UINT8 ech_twi_fw_mode_get(VOID);
EXTERN VOID ech_twi_cdr_offset_from_cal_set(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id);
EXTERN VOID ech_twi_cdr_bandwidth_set(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id);
EXTERN VOID ech_twi_continuous_serdes_cal_disable(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id);
EXTERN VOID ech_twi_prbs_cal_status(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id);
EXTERN VOID ech_twi_read_active_logs(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id);
EXTERN VOID ech_twi_read_saved_ddr_params(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id);
EXTERN UINT32 ech_twi_boot_config_proc(UINT8* rx_buf, UINT32 rx_index);

EXTERN VOID ech_twi_deferred_cmd_processing_struct_set(exp_twi_cmd_enum cmd_id, 
                                                       UINT32 (*func_ptr)(UINT8*, UINT32), 
                                                       VOID *buf, 
                                                       VOID (*callback_func_ptr)(UINT8),
                                                       UINT32 len,
                                                       UINT32 rx_index);

#endif /* _ECH_TWI_COMMON_H */
/** @} end addtogroup */



