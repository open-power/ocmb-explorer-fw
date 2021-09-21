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
#include "ech_loc.h"
#include "mem.h"
#include "crc32.h"
#include "twi.h"
#include "twi_api.h"
#include "app_fw.h"
#include "bc_printf.h"
#include "top_exp_config_guide.h"
#include <string.h>
#include "top.h"
#include "crash_dump.h"
#include "ocmb.h"
#include "opsw_api.h"
#include "serdes_plat.h"
#include "top_plat.h"


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


/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/
EXTERN UINT8 __ghsbegin_cmd_buf[];
EXTERN UINT8 __ghsend_cmd_buf[];
EXTERN UINT8 __ghsbegin_rsp_buf[];
EXTERN UINT8 __ghsend_rsp_buf[];
EXTERN UINT8 __ghsbegin_ext_data_buf[];
EXTERN UINT8 __ghsend_ext_data_buf[];
EXTERN UINT8 __ghsbegin_fw_boot_cfg[];


/*
** Global Variables
*/

/*
** Local Variables
*/

/* pointer to Explorer command control structures */
PRIVATE ech_ctrl_struct* ech_ctrl = NULL;

/* accessor to boot configuration data */
PRIVATE ech_boot_cfg_struct* ech_boot_cfg_ptr = (ech_boot_cfg_struct*)__ghsbegin_fw_boot_cfg;

/* TWI Register Address Latch and Address Validity*/
PRIVATE UINT32 ech_latched_reg_addr;

/* This field gets copied into FW Status BIT24-BIT31*/
PRIVATE UINT8  extended_error_code = 0x0;

/* PH_OFS_T_PRELOAD use host values flag */
UINT8 ech_use_host_ph_ofs_t_preload = 0;

/* PH_OFS_T_PRELOAD values from host */
PRIVATE UINT8 ech_ph_ofs_t_preload_val[8];

/* D_IQ_OFFSET use host values flag */
UINT8 ech_use_host_d_iq_offset = 0;

/* D_IQ_OFFSET values from host */
PRIVATE UINT8 ech_d_iq_offset_val[8];

/* 
** active lane bit mask
** may vary from configured if training results in degraded mode 
** x8 degrades to x4 
** x4 degrades to x2 
** x8 to x2 is not supported 
*/
PRIVATE UINT8 ech_active_lane_bitmask = 0x00; 

/* 
** Bitmasks to store status during PRBS calibration which can 
** be returned to the host.
*/ 
PRIVATE UINT8 ech_pattmon_detected_bitmask = 0x00;
PRIVATE UINT8 ech_cal_converged_bitmask = 0x00;

/*
** CDR bandwidth settings
*/
#define CDR_PROP_GAIN_MAX   0x7F
#define CDR_INTEG_GAIN_MAX  0xFF

#define CDR_PROP_GAIN_DEFAULT   0xA
#define CDR_INTEG_GAIN_DEFAULT  0x0

PRIVATE UINT8 serdes_cdr_prop_gain = CDR_PROP_GAIN_DEFAULT;
PRIVATE UINT8 serdes_cdr_integ_gain = CDR_INTEG_GAIN_DEFAULT;

/*
* Private Functions
*/
/**
* @brief
*   Dump boot config to crash dump
*
* @return
*   None
*
* @note
*/
PRIVATE VOID ech_boot_config_crash_dump(VOID)
{
    crash_dump_put(sizeof(ech_boot_cfg_struct), (void*) ech_boot_cfg_ptr);
}

/**
* @brief
*   Dump command buffer to crash dump
*
* @return
*   None
*
* @note
*/
PRIVATE void ech_cmd_buffer_crash_dump(VOID)
{
    crash_dump_put(ech_cmd_size_get(), (void*) ech_cmd_ptr_get());
}

/**
* @brief
*   Dump response buffer to crash dump
*
* @return
*   None
*
* @note
*/
PRIVATE void ech_rsp_buffer_crash_dump(VOID)
{
    crash_dump_put(ech_rsp_size_get(), (void*) ech_rsp_ptr_get());
}

/*
* Public Functions
*/

/**
* @brief
*   Register API function with ECH module.
*
* @return
*   TRUE - ECH initialized
*   FALSE - ECH not initialized
*
* @note
*/
PUBLIC VOID ech_init(VOID)
{
    /* initialize the OpenCAPI interface handler */
    ech_oc_init();

    /* clear the configuration flags */
    memset((VOID*)ech_boot_cfg_ptr, 0x00, sizeof(ech_boot_cfg_struct));

    /* Default is for the DFE state to be enabled. */
    ech_boot_cfg_ptr->dfe_state = TRUE;

    crash_dump_register(CRASH_DUMP_SET_0, "ECH_BOOT_CFG", &ech_boot_config_crash_dump, CRASH_DUMP_RAW, sizeof(ech_boot_cfg_struct));
    crash_dump_register(CRASH_DUMP_SET_0, "ECH_CMD_BUFFER", &ech_cmd_buffer_crash_dump, CRASH_DUMP_RAW, ech_cmd_size_get());
    crash_dump_register(CRASH_DUMP_SET_0, "ECH_RSP_BUFFER", & ech_rsp_buffer_crash_dump, CRASH_DUMP_RAW, ech_rsp_size_get());

} /* ech_init */

/**
* @brief
*   Get access to ECH command structures.
*
* @return
*   reference to ECH control structures
*
* @note
*/
PUBLIC ech_ctrl_struct* ech_ctrl_ptr_get(VOID)
{
    return (ech_ctrl);
}

/**
* @brief
*   Set control structure memory reference.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_ctrl_ptr_set(ech_ctrl_struct* ctrl_ptr)
{
    ech_ctrl = ctrl_ptr;
}

/**
* @brief
*   Validate address to ensure that the memory location is
*   accessible and access is not restricted
*
* @param
*   addr the memory location
*
* @return
*   TRUE - the memory address is accessible
*   FALSE - the memory location is not accessible or is
*   restricted
*
* @note
*/
PUBLIC BOOL ech_reg_addr_validate(UINT32 addr)
{
    /*
    ** TO DO: determine accessible and restricted memory locations
    ** ensure the address is not out-of-range (EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE)
    ** ensure the address is not a protected region (EXP_TWI_REG_RW_ADDR_PROHIBITED)
    ** any need to distinguish between out-of-range and protected
    ** if so a boolean return value won't be sufficient
    */
    return (TRUE);
}

/**
* @brief
*   Clear host command received flag.
*
* @return
*   reference to ECH control structures
*
* @note
*/
PUBLIC VOID ech_cmd_rxd_clr(VOID)
{
    top_plat_lock_struct lock_struct;

    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    ocmb_api_rxd_clr();

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);
}

/**
* @brief
*   Set host command received flag.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_cmd_rxd_flag_set(VOID)
{
    top_plat_lock_struct lock_struct;

    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    ocmb_api_rxd_flag_set();

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);
}

/**
* @brief
*   Get host command received flag.
*
* @return
*   host command received flag
*
* @note
*/
PUBLIC BOOL ech_cmd_rxd_flag_get(VOID)
{
    return ocmb_api_rxd_flag_get();
}

/**
* @brief
*   Set host command transmit flag.
*
* @return
*   host command received fla
*
* @note
*/
PUBLIC VOID ech_cmd_txd_flag_set(VOID)
{
    opsw_outbound_doorbell_set();
}

/**
* @brief
*   Get pointer to command buffer.
*
* @return
*   Pointer to command buffer
*
* @note
*/
PUBLIC exp_cmd_struct* ech_cmd_ptr_get(VOID)
{
    /* 
    ** Return uncached address of command buffer 
    ** Host deposits command in RAM, if the address is cached
    ** code will read stale data unless entries are invalidated 
    ** after every command execution. 
    ** Just to make things simple, using uncached address
    */
    return ((exp_cmd_struct*)(MIPS_KSEG1 (__ghsbegin_cmd_buf)));
}

/**
* @brief
*   Get size of command buffer.
*
* @return
*   Size of command buffer
*
* @note
*/
PUBLIC UINT32 ech_cmd_size_get(VOID)
{
    return ((UINT32)__ghsend_cmd_buf - (UINT32)__ghsbegin_cmd_buf);
}

/**
* @brief
*   Get pointer to response buffer.
*
* @return
*   Pointer to response buffer
*
* @note
*/
PUBLIC exp_rsp_struct* ech_rsp_ptr_get(VOID)
{    
    /* 
    ** Return uncached address of response buffer 
    ** Host deposits command in RAM, if the address is cached
    ** code will read stale data unless entries are invalidated 
    ** after every command execution. 
    ** Just to make things simple, using uncached address
    */
    return ((exp_rsp_struct*)(MIPS_KSEG1 (__ghsbegin_rsp_buf)));
}

/**
* @brief
*   Get size of response buffer.
*
* @return
*   Size of response buffer
*
* @note
*/
PUBLIC UINT32 ech_rsp_size_get(VOID)
{
    return ((UINT32)__ghsend_rsp_buf - (UINT32)__ghsbegin_rsp_buf);
}

/**
* @brief
*   Get pointer to extended data buffer.
*
* @return
*   Pointer to extended data buffer
*
* @note
*/
PUBLIC UINT8* ech_ext_data_ptr_get(VOID)
{    
    /* 
    ** Return uncached address of data buffer 
    ** Host deposits command in RAM, if the address is cached
    ** code will read stale data unless entries are invalidated 
    ** after every command execution. 
    ** Just to make things simple, using uncached address
    */
    return ((UINT8*)(MIPS_KSEG1 (__ghsbegin_ext_data_buf)));
}

/**
* @brief
*   Get size of extended data buffer.
*
* @return
*   Size of extended data buffer
*
* @note
*/
PUBLIC UINT32 ech_ext_data_size_get(VOID)
{
    return ((UINT32)__ghsend_ext_data_buf - (UINT32)__ghsbegin_ext_data_buf);
}

/**
* @brief
*   Returns a pointer to the boot configuration data structure
*
* @return
*   pointer to boot config data
*
* @note
*/
PUBLIC ech_boot_cfg_struct* ech_boot_cfg_ptr_get(VOID)
{
    return(ech_boot_cfg_ptr);
}

/**
* @brief
*   Set configuration sent by host.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_raw_boot_cfg_set(UINT32 boot_cfg_flags)
{
    ech_boot_cfg_ptr->raw_cfg_flags = boot_cfg_flags;
}

/**
* @brief
*   Returns the manufacturing boot mode.
*
* @return
*   TRUE - manufacturing mode is enabled
*   FALSE - manufacturing mode is not enabled
*
* @note
*/
PUBLIC UINT32 ech_raw_boot_cfg_get(VOID)
{
    return (ech_boot_cfg_ptr->raw_cfg_flags);
}

/**
* @brief
*   Set OpenCAPI loopback configuration, used for manufacturing
*   boot mode.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_mfg_mode_set(VOID)
{
    ech_boot_cfg_ptr->mfg_mode = TRUE;
}

/**
* @brief
*   Returns the manufacturing boot mode.
*
* @return
*   TRUE - manufacturing mode is enabled
*   FALSE - manufacturing mode is not enabled
*
* @note
*/
PUBLIC BOOL ech_mfg_mode_get(VOID)
{
    return (ech_boot_cfg_ptr->mfg_mode);
}

/**
* @brief
*   Set OpenCAPI loopback configuration, used for manufacturing
*   boot mode.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_ocapi_loopback_set(VOID)
{
    ech_boot_cfg_ptr->ocapi_loopback = TRUE;
}

/**
* @brief
*   Returns the OpenCAPI loopback setting, used for
*   manufacturing boot mode.
*
* @return
*   TRUE - OpenCAPI loopback is enabled
*   FALSE - OpenCAPI loopback is not enabled
*
* @note
*/
PUBLIC BOOL ech_ocapi_loopback_get(VOID)
{
    return (ech_boot_cfg_ptr->ocapi_loopback);
}

/**
* @brief
*   Set Transport Layer mode, used for manufacturing boot mode.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_tl_mode_set(UINT32 mode)
{
    ech_boot_cfg_ptr->tl_mode = mode;
}

/**
* @brief
*   Returns the transport layer selection setting.
*
* @return
*   EXP_TWI_BOOT_CFG_TL_OCAPI_MODE - OpenCAPI TL
*   EXP_TWI_BOOT_CFG_TL_TWI_MODE - TWI TL
*   EXP_TWI_BOOT_CFG_TL_JTAG_MODE - JTAG TL
*
* @note
*/
PUBLIC UINT32 ech_tl_mode_get(VOID)
{
    return (ech_boot_cfg_ptr->tl_mode);
}

/**
* @brief
*   Set Data Link Layer boot mode, used for manufacturing boot
*   mode.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_dl_boot_mode_set(UINT32 mode)
{
    ech_boot_cfg_ptr->dl_boot_mode = mode;
}

/**
* @brief
*   Returns data link layer boot mode setting.
*
* @return
*   EXP_TWI_BOOT_CFG_TL_OCAPI_MODE - OpenCAPI TL
*   EXP_TWI_BOOT_CFG_TL_TWI_MODE - TWI TL
*   EXP_TWI_BOOT_CFG_TL_JTAG_MODE - JTAG TL
*
* @note
*/
PUBLIC UINT32 ech_dl_boot_mode_get(VOID)
{
    return (ech_boot_cfg_ptr->dl_boot_mode);
}

/**
* @brief
*   Set host step-by-step boot mode.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_host_boot_mode_set(VOID)
{
    ech_boot_cfg_ptr->host_boot_mode = TRUE;
}

/**
* @brief
*   Returns the configured boot mode.
*
* @return
*   EXP_TWI_BOOT_CFG_FULL_BOOT_MODE - full boot
*   EXP_TWI_BOOT_CFG_HOST_BOOT_MODE - host step-by-step
*
* @note
*/
PUBLIC BOOL ech_host_boot_mode_get(VOID)
{
    return (ech_boot_cfg_ptr->host_boot_mode);
}

/**
* @brief
*   Set the SerDes frequency.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_serdes_freq_set(UINT32 freq)
{
    ech_boot_cfg_ptr->serdes_freq = freq;
}

/**
* @brief
*   Returns the SerDes frequency configuration.
*
* @return
*   The configured SerDes frequency:
*      EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS
*      EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS
*      EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS
*
* @note
*/
PUBLIC UINT32 ech_serdes_freq_get(VOID)
{
    return (ech_boot_cfg_ptr->serdes_freq);
}

/**
* @brief
*   Set lane configuration.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_lane_cfg_set(UINT32 cfg)
{
    ech_boot_cfg_ptr->lane_cfg = cfg;
}

/**
* @brief
*   Returns the lane configuration.
*
* @return
*   The lane copnfiguration:
*      EXP_TWI_BOOT_CFG_LANE_8
*      EXP_TWI_BOOT_CFG_LANE_4
*
* @note
*/
PUBLIC UINT32 ech_lane_cfg_get(VOID)
{
    return (ech_boot_cfg_ptr->lane_cfg == EXP_TWI_BOOT_CFG_LANE_8 ? EXP_SERDES_8_LANE : EXP_SERDES_4_LANE);
}

/**
* @brief
*   Returns the lane bitmask for the lanes to configure.
*
* @return
*   The lane configuration bitmask
*
* @note
*/
PUBLIC UINT8 ech_lane_cfg_bitmask_get(VOID)
{
    return (ech_boot_cfg_ptr->lane_cfg == EXP_TWI_BOOT_CFG_LANE_8 ? EXP_SERDES_8_LANE_BITMASK : EXP_SERDES_4_LANE_BITMASK);
}

/**
* @brief
*   Returns the lane pattern bitmask for the lanes to configure.
*
* @return
*   The lane pattern check bitmask
*
* @note
*/
PUBLIC UINT8 ech_lane_cfg_pattern_bitmask_get(VOID)
{
    return (ech_boot_cfg_ptr->lane_cfg == EXP_TWI_BOOT_CFG_LANE_8 ? EXP_SERDES_8_LANE_PAT_BITMASK: EXP_SERDES_4_LANE_PAT_BITMASK);
}

/**
* @brief
*   Returns the lane pattern bitmask for the lanes that
*   successfully trained.
*
* @return
*   The lane pattern check bitmask
*
* @note
*/
PUBLIC UINT8 ech_lane_active_pattern_bitmask_get(VOID)
{
    return (ech_active_lane_bitmask);
}

/**
* @brief
*   Records the lane pattern bitmask for the lanes that
*   successfully trained.
*  
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_lane_active_pattern_bitmask_set(UINT8 lane_bitmask)
{
    ech_active_lane_bitmask = lane_bitmask;
}

/**
* @brief
*   Returns the latched register address
*
* @return
*   latched register address
*
* @note
*/
PUBLIC VOID ech_latched_reg_addr_set(UINT32 reg_addr)
{
    ech_latched_reg_addr = reg_addr;
}

/**
* @brief
*   Returns the latched register address
*
* @return
*   latched register address
*
* @note
*/
PUBLIC UINT32 ech_latched_reg_addr_get(VOID)
{
    return (ech_latched_reg_addr);
}

/**
* @brief
*   Setting extended error code
* 
* @param [in] extended_error_code - extended error code for FW status command
*
* @return
*   None
*
* @note
*/
PUBLIC VOID ech_extended_error_code_set(UINT32 error_code)
{
    extended_error_code = (UINT8)error_code;
}

/**
* @brief
*   Return extended error code
* 
* @param 
*   None
*
* @return
*   Extended Error Code
*
* @note
*/
PUBLIC UINT8 ech_extended_error_code_get(VOID)
{
    return extended_error_code;
}

/**
* @brief
*   Set the DFE state for internal firmware tracking.
* 
* @param [in] dfe_state_val - The state of DFE:
*                               1=enabled 0=disabled
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_dfe_state_set(UINT32 dfe_state_val)
{
    /* Default to TRUE. */
    ech_boot_cfg_ptr->dfe_state = TRUE;
    if (0 == dfe_state_val)
    {
        ech_boot_cfg_ptr->dfe_state = FALSE;
    }
}

/**
* @brief
*   Get the DFE state.
* 
* @param 
*   None
*
* @return
*   TRUE    = DFE enabled
*   FALSE   = DFE disabled.
*
* @note
*/
PUBLIC BOOL ech_dfe_state_get(VOID)
{
    return (ech_boot_cfg_ptr->dfe_state);
}

/**
* @brief
*   Set the Adaptation state for internal firmware tracking.
* 
* @param [in] adaptation_state_val - The state of Adaptation:
*                               1=enabled 0=disabled
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_adaptation_state_set(UINT32 adaptation_state_val)
{
    /* Default to TRUE. */
    ech_boot_cfg_ptr->adapt_state = TRUE;
    if (0 == adaptation_state_val)
    {
        ech_boot_cfg_ptr->adapt_state = FALSE;
    }
}

/**
* @brief
*   Get the adaptation state.
* 
* @param 
*   None
*
* @return
*   TRUE    = Adaptation enabled
*   FALSE   = Adaptation disabled.
*
* @note
*/
PUBLIC BOOL ech_adaptation_state_get(VOID)
{
    return (ech_boot_cfg_ptr->adapt_state);
}

/**
* @brief
*   Set the improved serdes algorithm enable state for internal
*   firmware tracking.
* 
* @param [in] serdes_prbs_cal_state_val - Enable to use the
*        improved PRBS SERDES calibration algorithm:
*        TRUE=enabled FALSE=disabled
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_serdes_prbs_cal_state_set(BOOL serdes_prbs_cal_state_val)
{
    ech_boot_cfg_ptr->serdes_prbs_cal_enable = serdes_prbs_cal_state_val;
}

/**
* @brief
*   Get the improved serdes algorithm enable state.
* 
* @param 
*   None
*
* @return
*   TRUE = SerDes PRBS calibration algorithm enabled
*   FALSE = SerDes PRBS calibration algorithm disabled.
*
* @note
*/
PUBLIC BOOL ech_serdes_prbs_cal_state_get(VOID)
{
    return (ech_boot_cfg_ptr->serdes_prbs_cal_enable);
}

/**
* @brief
*   Store the CDR proportional gain.
*  
* @param [in] prop_gain: 	LF_PROP_CTRL_GAIN to apply in boot 
*        config
* @return
*   TRUE for success, FALSE for parameter out of range
* 
* @note 
* 
*/
PUBLIC BOOL ech_serdes_cdr_prop_gain_set(UINT8 prop_gain)
{
    if (prop_gain > CDR_PROP_GAIN_MAX) 
    {
        return FALSE;
    }

    serdes_cdr_prop_gain = prop_gain;

    return TRUE;
}

/**
* @brief
*   Store the CDR integral path gain.
*  
* @param [in] integ_gain: 	LF_INTEG_CTRL_GAIN to apply in boot 
*        config
* @return
*   TRUE for success, FALSE for parameter out of range
*    
* @note 
* 
*/
PUBLIC BOOL ech_serdes_cdr_integ_gain_set(UINT8 integ_gain)
{
    if (integ_gain > CDR_INTEG_GAIN_MAX) 
    {
        return FALSE;
    }

    serdes_cdr_integ_gain = integ_gain;

    return TRUE;
}

/**
* @brief
*   Get the CDR proportional gain.
*  
* @param 
*   None
*  
* @return
*   CDR prop gain setting
* 
* @note 
* 
*/
PUBLIC UINT8 ech_serdes_cdr_prop_gain_get()
{
    return serdes_cdr_prop_gain;
}

/**
* @brief
*   Get the CDR integral gain.
*  
* @param 
*   None
*  
* @return
*   CDR integ gain setting
* 
* @note 
* 
*/
PUBLIC UINT8 ech_serdes_cdr_integ_gain_get()
{
    return serdes_cdr_integ_gain;
}

/**
* @brief
*   Store status information gathered during SerDes
*   PRBS calibration
* 
* @param [in] pattmon_detected_bitmask - Bitmask of the lanes
*        where the pattern monitor detected PRBS before
*        calibration.
* @param [in] cal_converged_bitmask - Bitmask of the lanes where
*        the pattern monitor detected PRBS after calibration.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_serdes_prbs_cal_data_set(UINT8 pattmon_detected_bitmask, UINT8 cal_converged_bitmask)
{
    ech_pattmon_detected_bitmask = pattmon_detected_bitmask;
    ech_cal_converged_bitmask = cal_converged_bitmask;
}

/**
* @brief
*   Get status information gathered during SerDes
*   PRBS calibration
* 
* @param [out] pattmon_detected_bitmask - Bitmask of the lanes
*        where the pattern monitor detected PRBS before
*        calibration.
* @param [out] cal_converged_bitmask - Bitmask of the lanes
*        where the pattern monitor detected PRBS after
*        calibration.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_serdes_prbs_cal_data_get(UINT8 * pattmon_detected_bitmask, UINT8 * cal_converged_bitmask)
{
    *pattmon_detected_bitmask = ech_pattmon_detected_bitmask;
    *cal_converged_bitmask = ech_cal_converged_bitmask;
}


/**
* @brief
*   Set the SerDes loopback state.
* 
* @param [in] serdes_loopback_state - Enable to use the SerDes
*        loopback mode with PRBS calibration algorithm.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_serdes_loopback_set(BOOL serdes_loopback_state)
{
    ech_boot_cfg_ptr->serdes_loopback_state = serdes_loopback_state;
}

/**
* @brief
*   Get the SerDes loopback state.
* 
* @param 
*   None
*
* @return
*   TRUE = SerDes loopback mode selected
*   FALSE = Normal operation
*
* @note
*/
PUBLIC BOOL ech_serdes_loopback_get(VOID)
{
    return (ech_boot_cfg_ptr->serdes_loopback_state);
}

/**
* @brief
*   Set the SerDes CSU offset mask in loopback mode
* 
* @param [in] serdes_csu_offset_mask - Lane bitmask of which
*        lanes should have a CSU ppm offset applied.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_serdes_loopback_csu_offset_mask_set(BOOL serdes_csu_offset_mask)
{
    ech_boot_cfg_ptr->serdes_csu_offset_mask = serdes_csu_offset_mask;
}

/**
* @brief
*   Get the SerDes CSU offset mask.
* 
* @param 
*   None
*
* @return
*   Lane bitmask of which lanes have a CSU ppm offset applied.
*
* @note
*/
PUBLIC BOOL ech_serdes_loopback_csu_offset_mask_get(VOID)
{
    return (ech_boot_cfg_ptr->serdes_csu_offset_mask);
}

/**
* @brief
*   Return the host setting flag for PH_OFS_T_PRELOAD field -
*   timing phase preload offset - of OBJECT_PRELOAD_VAL_5
*   register
* 
* @param
*   None
*
* @return
*   0 - use default setting
*   1 - use host setting
* 
* @note
*/
PUBLIC UINT8 ech_ph_ofs_t_preload_use_host_get(VOID)
{
    return (ech_use_host_ph_ofs_t_preload);
}

/**
* @brief
*   Record the host setting flag for PH_OFS_T_PRELOAD field -
*   timing phase preload offset - of OBJECT_PRELOAD_VAL_5
*   register
* 
* @param [in]  use_host_value - flag indicating to use host
*                               values or default value
*
* @return
*   Nothing
* 
* @note
*/
PUBLIC VOID ech_ph_ofs_t_preload_use_host_set(UINT8 use_host_flag)
{
    ech_use_host_ph_ofs_t_preload = use_host_flag;
    bc_printf("ech_ph_ofs_t_preload_use_host_set(): %s using host settings\n",
              ((use_host_flag != 0) ? "" : "NOT")); 
}

/**
* @brief
*   Set the lane specific PH_OFS_T_PRELOAD field - timing phase
*   offset preload - of OBJECT_PRELOAD_VAL_5 register
* 
* @param [in] lane - lane to which setting will be applied
* @param [in] reg_val - host specified setting
*
* @return
*   Nothing
* 
* @note
*/
PUBLIC VOID ech_ph_ofs_t_preload_set(UINT32 lane,
                                     UINT8 reg_val)
{
    bc_printf("ech_ph_ofs_t_preload_set() lane = %u  ph_ofs_t_preload = 0x%02X\n", lane, reg_val);
    ech_ph_ofs_t_preload_val[lane] = reg_val;
}

/**
* @brief
*   Get the lane specific PH_OFS_T_PRELOAD field - timing phase
*   offset preload - of OBJECT_PRELOAD_VAL_5 register
* 
* @param [in] lane - lane to which setting will be applied
*
* @return
*   lane register setting
*
* @note
*/
PUBLIC UINT8 ech_ph_ofs_t_preload_get(UINT32 lane)
{
    bc_printf("ech_ph_ofs_t_preload_get() lane = %u  ph_ofs_t_preload = ", lane);
    if (ech_ph_ofs_t_preload_use_host_get() == 0)
    {
        /* host settings are not enabled, return default value */
        bc_printf("0x%02X\n", SERDES_PH_OFS_T_PRELOAD_SETTING);
        return (SERDES_PH_OFS_T_PRELOAD_SETTING);
    }
    bc_printf("0x%02X\n", ech_ph_ofs_t_preload_val[lane]);
    return (ech_ph_ofs_t_preload_val[lane]);
}

/**
* @brief
*   Return the host setting flag for D_IQ_OFFSET field - data
*   phase offset - of TR_CONFIG_5 register
* 
* @param
*   None
*
* @return
*   0 - use hardware calibration result
*   1 - use host setting
* 
* @note
*/
PUBLIC UINT8 ech_d_iq_offset_use_host_get(VOID)
{
    return (ech_use_host_d_iq_offset);
}

/**
* @brief
*   Record the host setting flag for D_IQ_OFFSET field - data
*   phase offset - of TR_CONFIG_5 register
* 
* @param [in]  use_host_value - flag indicating to use host
*                               value or hardware calibration
*                               result
*
* @return
*   Nothing
* 
* @note
*/
PUBLIC VOID ech_d_iq_offset_use_host_set(UINT8 use_host_flag)
{
    ech_use_host_d_iq_offset = use_host_flag;
    bc_printf("ech_d_iq_offset_use_host_set(): %s using host settings\n",
              ((use_host_flag != 0) ? "" : "NOT")) ;
}

/**
* @brief
*   Get the lane specific D_IQ_OFFSET field - data phase offset
*   - of TR_CONFIG_5 register
* 
* @param [in] lane - lane to which setting will be applied
*
* @return
*   lane register setting
*
* @note
*/
PUBLIC UINT8 ech_d_iq_offset_get(UINT32 lane)
{
    return (ech_d_iq_offset_val[lane]);
}

/**
* @brief
*   Set the lane specific D_IQ_OFFSET field - data phase offset
*   - of TR_CONFIG_5 register
* 
* @param [in] lane - lane to which setting will be applied
* @param [in]  reg_val - host specified setting
*
* @return
*   Nothing
* 
* @note
*/
PUBLIC VOID ech_d_iq_offset_set(UINT32 lane, UINT8 reg_val)
{
    ech_d_iq_offset_val[lane] = reg_val;
    bc_printf("ech_d_iq_offset_set(): lane %u d_iq_offset = 0x%02X\n", 
              lane, ech_d_iq_offset_val[lane]);
}

/** @} end addtogroup */
