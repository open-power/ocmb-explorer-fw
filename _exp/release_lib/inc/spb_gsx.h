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

/********************************************************************************
*   DESCRIPTION:
*    This file contains definitions and declarations for the SPB GSX module for
*    use by external applications.
*
*   NOTES:
*
*******************************************************************************/



#ifndef _SPB_GSX_H
#define _SPB_GSX_H

/*
** Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_mid.h"

/*
** Constants
*/
#define SPB_GSX_NUM_PORTS           1

/* Error codes */
#define SPB_GSX_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_SPB_GSX) | (err_suffix))
#define SPB_GSX_ERR_BAD_PORT_NUM_0  SPB_GSX_ERR_CODE_CREATE(0x001)  /* bad port number */
#define SPB_GSX_ERR_BAD_PORT_NUM_1  SPB_GSX_ERR_CODE_CREATE(0x002)  /* bad port number */
#define SPB_GSX_ERR_BAD_PORT_NUM_2  SPB_GSX_ERR_CODE_CREATE(0x003)  /* bad port number */
#define SPB_GSX_ERR_BAD_PORT_NUM_3  SPB_GSX_ERR_CODE_CREATE(0x004)  /* bad port number */

/* GSX 0 hardware/software control select register and mask */
#define SPB_GSX_0_HARD_SOFT_SEL_REG        SPB_GSX_0_GPIO_HARDWARE_SW_CTRL_SELECT_CFG_REG
#define SPB_GSX_0_HARD_SOFT_SEL_REG_BITMSK_GSX_0_GPIO_FW_HW_CNTL SPB_GSX_0_GPIO_HARDWARE_SW_CTRL_SELECT_CFG_REG_BITMSK_GSX_0_GPIO_FW_HW_CNTL

/* GSX 0 error detection interrupt enable register, mask, and offset */
#define SPB_GSX_0_GPIO_ERR_DET_INT_EN_REG  SPB_GSX_0_GPIO_ERR_DETECTED_INT_EN
#define SPB_GSX_0_GPIO_ERR_DET_INT_EN_REG_BITMSK_GSX_0_ERR_DET_INT_E6 SPB_GSX_0_GPIO_ERR_DETECTED_INT_EN_BITMSK_GSX_0_ERR_DET_INT_E0
#define SPB_GSX_0_GPIO_ERR_DET_INT_EN_REG_BITOFF_GSX_0_ERR_DET_INT_E6 SPB_GSX_0_GPIO_ERR_DETECTED_INT_EN_BITOFF_GSX_0_ERR_DET_INT_E0

/* GSX 0 error detection interrupt register and mask */
#define SPB_GSX_0_GPIO_ERR_DET_INT_REG     SPB_GSX_0_GPIO_ERR_DETECTED_INT_STAT
#define SPB_GSX_0_GPIO_ERR_DET_INT_REG_BITMSK_GSX_0_ERR_DET_INT_I6 SPB_GSX_0_GPIO_ERR_DETECTED_INT_STAT_BITMSK_GSX_0_ERR_DET_INT_I0

/* GSX 0 input/output select register */
#define SPB_GSX_0_GPIO_IN_OUT_SELECT_REG   SPB_GSX_0_GPIO_INPUT_OUTPUT_SELECT_CFG_REG

/* GSX 0 set configuration register */
#define SPB_GSX_0_GPIO_SET_REG             SPB_GSX_0_GPIO_SET_CFG_REG

/* GSX 0 clear configuration register */
#define SPB_GSX_0_GPIO_CLR_REG             SPB_GSX_0_GPIO_CLEAR_CFG_REG

/*
** Function Prototypes
*/

EXTERN UINT32 spb_gsx_reg_base_addr_get(const UINT port_id);
EXTERN VOID spb_gsx_mux_to_gsx(const UINT port_id);
EXTERN VOID spb_gsx_pattern_check_int_prop_en(const UINT port_id,
                                              const BOOL enable);
EXTERN VOID spb_gsx_int_enable_set(const UINT port_id, 
                                   const UINT slice_id,
                                   const BOOL ena);

#endif /* define _SPB_GSX_H */



