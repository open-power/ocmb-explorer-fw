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
*   Explorer Command Handler definitions and declarations.
*
* @note
*/
#ifndef _ECH_H
#define _ECH_H


/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_api.h"
#include "tmr_sys_plat.h"


/*
** Constants
*/


/*
* Enumerated Types
*/


/*
* Structures and Unions
*/

/**
* @brief 
*   This typedef is used to declare the ECH ISR function.
*  
*/
typedef void (*ech_isr_fcn_ptr)(UINT);

/**
* @brief 
*   This typedef is used to declare the function interface for
*   Open CAPI callback functions registered with the ECH module.
*  
*/
typedef VOID (*ech_cmd_fn_ptr)(VOID);

/**
* @brief
*   Explorer Command Handler control structure.
*/
typedef struct _ech_ctrl_struct
{
    exp_cmd_enum    cmd;            /**< Explorer command */
    ech_cmd_fn_ptr  api_fn_ptr;     /**< API function handler */

} ech_ctrl_struct;


/*
* Global Variables
*/


/*
* Function Prototypes
*/
EXTERN VOID ech_init(VOID);
EXTERN VOID ech_api_func_register(exp_cmd_enum cmd, ech_cmd_fn_ptr fn_ptr);
EXTERN exp_cmd_struct* ech_cmd_ptr_get(VOID);
EXTERN UINT32 ech_cmd_size_get(VOID);
EXTERN exp_rsp_struct* ech_rsp_ptr_get(VOID);
EXTERN UINT32 ech_rsp_size_get(VOID);
EXTERN UINT8* ech_ext_data_ptr_get(VOID);
EXTERN UINT32 ech_ext_data_size_get(VOID);
EXTERN VOID ech_cmd_rxd_flag_set(VOID);
EXTERN BOOL ech_cmd_rxd_flag_get(VOID);
EXTERN VOID ech_cmd_txd_flag_set(VOID);
EXTERN BOOL ech_cmd_txd_flag_get(VOID);
EXTERN UINT32 ech_raw_boot_cfg_get(VOID);
EXTERN BOOL ech_mfg_mode_get(VOID);
EXTERN BOOL ech_ocapi_loopback_get(VOID);
EXTERN UINT32 ech_tl_mode_get(VOID);
EXTERN UINT32 ech_dl_boot_mode_get(VOID);
EXTERN BOOL ech_host_boot_mode_get(VOID);
EXTERN UINT32 ech_serdes_freq_get(VOID);
EXTERN UINT32 ech_lane_cfg_get(VOID);
EXTERN UINT8 ech_lane_cfg_bitmask_get(VOID);
EXTERN UINT8 ech_lane_cfg_pattern_bitmask_get(VOID);
EXTERN UINT32 ech_latched_reg_addr_get(VOID);
EXTERN VOID ech_dfe_state_set(UINT32 dfe_state_val);
EXTERN BOOL ech_dfe_state_get(VOID);
EXTERN VOID ech_adaptation_state_set(UINT32 adaptation_state_val);
EXTERN BOOL ech_adaptation_state_get(VOID);
EXTERN VOID ech_serdes_prbs_cal_state_set(BOOL serdes_prbs_cal_state_val);
EXTERN BOOL ech_serdes_prbs_cal_state_get(VOID);
EXTERN VOID ech_serdes_prbs_cal_data_set(UINT8 pattmon_detected_bitmask, UINT8 cal_converged_bitmask);
EXTERN VOID ech_serdes_prbs_cal_data_get(UINT8 * pattmon_detected_bitmask, UINT8 * cal_converged_bitmask);
EXTERN VOID ech_serdes_loopback_set(BOOL serdes_loopback_state);
EXTERN BOOL ech_serdes_loopback_get(VOID);
EXTERN VOID ech_serdes_loopback_csu_offset_mask_set(BOOL serdes_csu_offset_mask);
EXTERN BOOL ech_serdes_loopback_csu_offset_mask_get(VOID);
EXTERN BOOL ech_oc_cmd_proc(VOID);
EXTERN VOID ech_oc_rsp_proc(VOID);
EXTERN UINT8 ech_extended_error_code_get(VOID);
EXTERN VOID ech_extended_error_code_set(UINT32 error_code);
EXTERN UINT8 ech_ph_ofs_t_preload_use_host_get(VOID);
EXTERN VOID ech_ph_ofs_t_preload_use_host_set(UINT8 use_host_flag);
EXTERN VOID ech_ph_ofs_t_preload_set(UINT32 lane, UINT8 reg_val);
EXTERN UINT8 ech_ph_ofs_t_preload_get(UINT32 lane);
EXTERN UINT8 ech_d_iq_offset_use_host_get(VOID);
EXTERN VOID ech_d_iq_offset_use_host_set(UINT8 use_host_flag);
EXTERN UINT8 ech_d_iq_offset_get(UINT32 lane);
EXTERN VOID ech_d_iq_offset_set(UINT32 lane, UINT8 reg_val);
EXTERN BOOL ech_serdes_cdr_prop_gain_set(UINT8 prop_gain);
EXTERN BOOL ech_serdes_cdr_integ_gain_set(UINT8 integ_gain);
EXTERN UINT8 ech_serdes_cdr_prop_gain_get(VOID);
EXTERN UINT8 ech_serdes_cdr_integ_gain_get(VOID);
PUBLIC UINT8 ech_lane_active_pattern_bitmask_get(VOID);
PUBLIC VOID ech_lane_active_pattern_bitmask_set(UINT8 lane_bitmask);


#endif /* _ECH_H */
/** @} end addtogroup */



