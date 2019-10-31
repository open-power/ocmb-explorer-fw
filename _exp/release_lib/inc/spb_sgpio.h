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
*    This file contains definitions and declarations for the SPB SGPIO for
*    use by external applications.
*
*   NOTES:
*
*******************************************************************************/



#ifndef _SPB_SGPIO_H
#define _SPB_SGPIO_H

/*
** Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_mid.h"

/*
** Constants
*/

/* Error codes */
#define SPB_SGPIO_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_SPB_SGPIO) | (err_suffix))

#define SPB_SGPIO_ERR_PORT_INVAL_01     SPB_SGPIO_ERR_CODE_CREATE(0x001)  /* invalid port */
#define SPB_SGPIO_ERR_PORT_INVAL_02     SPB_SGPIO_ERR_CODE_CREATE(0x002)  /* invalid port */
#define SPB_SGPIO_ERR_PORT_INVAL_03     SPB_SGPIO_ERR_CODE_CREATE(0x003)  /* invalid port */
#define SPB_SGPIO_ERR_PORT_INVAL_04     SPB_SGPIO_ERR_CODE_CREATE(0x004)  /* invalid port */
#define SPB_SGPIO_ERR_PORT_INVAL_05     SPB_SGPIO_ERR_CODE_CREATE(0x005)  /* invalid port */
#define SPB_SGPIO_ERR_PORT_INVAL_06     SPB_SGPIO_ERR_CODE_CREATE(0x006)  /* invalid port */
#define SPB_SGPIO_ERR_PORT_INVAL_07     SPB_SGPIO_ERR_CODE_CREATE(0x007)  /* invalid port */
#define SPB_SGPIO_ERR_PORT_INVAL_08     SPB_SGPIO_ERR_CODE_CREATE(0x008)  /* invalid port */

/* maximum number of inputs or outputs in general purpose mode */
#define SPB_SGPIO_GP_INOUT_MAX                        256

/* base SDataIn register for GPI mode, corresponding to port 0 */
#define SPB_SGPIO_GPI_SDATAIN_BASE_REG                SGPIO_GPI_STAT_0

/* base PISO output register, corresponding to port 0 */
#define SPB_SGPIO_PISO_OUTPUT_BASE_REG                SGPIO_PISO_OUTPUT_0

/* base SIPO status register, corresponding to port 0 */
#define SPB_SGPIO_SIPO_STATUS_BASE_REG                SGPIO_SIPO_STAT_0

/* base GP mode GPO output register, corresponding to port 0 */
#define SPB_SGPIO_GPO_OUTPUT_BASE_REG                 SGPIO_GPO_OUTPUT_0

/* miscellaneous interrupt enable register bitmask for flash timer interrupt */
#define SPB_SGPIO_INT_MISC_EN_BITMSK_FLASH_TIMER_INT  SGPIO_INT_EN_0_BITMSK_FLASH_TIMER_E0

/* miscellaneous interrupt enable register bitmask for FPIn rise interrupt */
#define SPB_SGPIO_INT_MISC_EN_BITMSK_FPIN_RISE_INT    SGPIO_INT_EN_0_BITMSK_FPIN_RISE_E0

/* miscellaneous interrupt register bitmask for flash timer interrupt */
#define SPB_SGPIO_INT_MISC_BITMSK_FLASH_TIMER_INT     SGPIO_INT_0_BITMSK_FLASH_TIMER_I0

/* miscellaneous interrupt register bitmask for FPIn rise interrupt */
#define SPB_SGPIO_INT_MISC_BITMSK_FPIN_RISE_INT       SGPIO_INT_0_BITMSK_FPIN_RISE_I0

/* interrupt summary register bitmask for miscellaneous interrupt activity */
#define SPB_SGPIO_INT_SUMMARY_BITMSK_INT_MISC         0x10000

/*
** Function Prototypes
*/

EXTERN UINT32 spb_sgpio_hw_reg_offset_get(VOID);

EXTERN VOID spb_sgpio_muxing_cfg_gpio_to_sgpio(const UINT port_id);

EXTERN VOID spb_sgpio_reset(const UINT32 sgpio_base_addr,
                            const UINT port_id);

EXTERN VOID spb_sgpio_int_enable_set(const UINT port_id, const BOOL ena);

EXTERN UINT32 spb_sgpio_hw_int_summary_get(const UINT port_id);

EXTERN UINT32 spb_sgpio_gpi_int_en_reg_get(const UINT port_id);

EXTERN UINT32 spb_sgpio_gpi_int_reg_get(const UINT port_id);

EXTERN UINT32 spb_sgpio_int_misc_en_reg_get(const UINT port_id);

EXTERN UINT32 spb_sgpio_int_misc_reg_get(const UINT port_id);

#endif /* define _SPB_SGPIO_H */



