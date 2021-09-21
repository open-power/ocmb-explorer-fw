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



/**
* @addtogroup ECH_LOC
* @{
* @file
* @brief
*   Explorer Command Handler local definitions and declarations.
*
* @note
*/
#ifndef _ECH_LOC_H
#define _ECH_LOC_H


/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"


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
*   Boot Configuration Data
*/
typedef struct
{
    UINT32  raw_cfg_flags;  /**< raw configuration flags received from host */
    BOOL    mfg_mode;       /**< manufacturing mode flag */
    BOOL    ocapi_loopback; /**< OpenCAPI loopback mode flag */    
    BOOL    host_boot_mode; /**< Host step-by-step boot mode */
    UINT32  tl_mode;        /**< Transport Layer selection */
    UINT32  dl_boot_mode;   /**< Data Link Layer boot mode */
    UINT32  lane_cfg;       /**< lane configuration */
    UINT32  serdes_freq;    /**< SerDes frequency */
    BOOL    dfe_state;      /**< TRUE = DFE enabled, FALSE = DFE disabled */
    BOOL    adapt_state;    /**< TRUE = Adaptation enabled, FALSE = Adaptation disabled */
    BOOL    serdes_prbs_cal_enable;    /**< TRUE = Use serdes PRBS calibration sequence, FALSE = Do not use */
    BOOL    serdes_loopback_state;     /**< TRUE = User serdes loopback cal mode, FALSE = Normal operation */
    UINT8   serdes_csu_offset_mask;    /**< Lane bitmask of which lanes should have a CSU ppm offset applied during PRBS calibration */
} ech_boot_cfg_struct;


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
EXTERN VOID ech_oc_init(VOID);

#endif /* _ECH_LOC_H */
/** @} end addtogroup */



