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
* @addtogroup TWI
* @{  
* @file 
* @brief 
*   This file contains all the TWI header 
* 
* @note 
*/ 

#ifndef _TWI_H
#define _TWI_H

/*
* Include Files
*/
#include "pmcfw_err.h"
#include "pmcfw_types.h"
#include "twi_api.h"

/*
* Constants
*/

/* Slave activities that should be processed by the user */
#define TWI_SLAVE_ACTIVITY_NONE              0x00000000 /**< no slave activity */
#define TWI_SLAVE_ACTIVITY_RX_REQ            0x00000001 /**< data receive request */
#define TWI_SLAVE_ACTIVITY_TX_REQ            0x00000002 /**< data transmit request */
#define TWI_SLAVE_ACTIVITY_START_DET         0x00000004 /**< start / restart condition */
#define TWI_SLAVE_ACTIVITY_STOP_DET          0x00000008 /**< stop condition */
#define TWI_SLAVE_ACTIVITY_RX_ERROR          0x00000010 /**< receive error */
#define TWI_SLAVE_ACTIVITY_TX_ERROR          0x00000020 /**< transmit error */
#define TWI_SLAVE_ACTIVITY_RX_REQ_SMBUS      0x00000040 /**< data receive request via SMBus address */
#define TWI_SLAVE_ACTIVITY_TX_REQ_SMBUS      0x00000080 /**< data transmit request via SMBus address */
#define TWI_SLAVE_ACTIVITY_SMBUS_SCL_TIMEOUT 0x00000100 /**< SCL timeout during SMBus transfer */
#define TWI_SLAVE_ACTIVITY_SRST              0x00000400 /**< reset */

#define TWI_MASTER_ACTIVITY_RX_REQ      0x00000001 /**< data receive request */
#define TWI_MASTER_ACTIVITY_TX_REQ      0x00000002 /**< data transmit request */
#define TWI_MASTER_ACTIVITY_START_DET   0x00000004 /**< start / restart condition */
#define TWI_MASTER_ACTIVITY_STOP_DET    0x00000008 /**< stop condition */
#define TWI_MASTER_ACTIVITY_RX_ERROR    0x00000010 /**< receive error */
#define TWI_MASTER_ACTIVITY_TX_ERROR    0x00000020 /**< transmit error */
#define TWI_MASTER_ACTIVITY_UNEXPECTED  0x00000400 /**< reset */

/*
* Enumerated Types
*/

/**
* @brief
*   TWI operating modes
*/
typedef enum
{
    TWI_OPER_MODE_POLLING, /**< polling mode */
    TWI_OPER_MODE_INTERRUPT/**< interrupt mode */
} twi_oper_mode_enum;

/*
* Structures and Unions
*/



/*
* Function Prototypes
*/

EXTERN VOID twi_init(const twi_oper_mode_enum oper_mode);
                             
EXTERN BOOL twi_mst_validate(const UINT port_id);

EXTERN BOOL twi_slv_validate(const UINT port_id);

EXTERN PMCFW_ERROR twi_port_ms_mode_set(const UINT port_id,
                                        const twi_port_ms_mode_enum mode);

EXTERN twi_oper_mode_enum twi_get_oper_mode(VOID);

EXTERN VOID twi_slv_all_disable(VOID);

EXTERN VOID twi_slv_all_enable(VOID);

EXTERN PMCFW_ERROR twi_mst_smbus_quick_cmd(const twi_slave_struct * const slave_ptr,
                                           const BOOL is_read);

EXTERN PMCFW_ERROR twi_slv_smbus_addr_get(const UINT port_id,
                                          UINT8 * const addr);

EXTERN PMCFW_ERROR twi_slv_smbus_addr_set(const UINT port_id,
                                          const UINT8 addr);
EXTERN PMCFW_ERROR twi_slv_port_disable(const UINT port_id);


#endif

/** @} end addtogroup */


