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
* @addtogroup SPB
* @{
* @file
* @brief
*   This file contains GPBCw-specific TWI definitions and
*   declarations.
*
* @note
*/

#ifndef _SPB_TWI_H
#define _SPB_TWI_H

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_mid.h"
#include "pmcfw_err.h"
#include "twi_plat_cfg.h"

/*
* Enumerated Types
*/

/**
* @brief
*  TWI pin types
*
* @note
*/
typedef enum
{
    SPB_TWI_PAD_SCL = 0,    /**< SCL pad */
    SPB_TWI_PAD_SDA,        /**< SDA pad */
    SPB_TWI_PAD_RST,        /**< reset pad */
    SPB_TWI_NUM_PADS
} spb_twi_pad_enum;

/*
* Local Constants
*/

/*
* Number of master TWI ports. This should only be used for array sizing; 
* dynamic usage should be based on spb_twi_num_ports_master_get() which gets 
* the actual number for the device.
*/
#if (TWI_PORT0_ONLY == 1)
#define SPB_TWI_NUM_PORTS_MASTER    1
#else
#define SPB_TWI_NUM_PORTS_MASTER    2
#endif

/*
* Number of slave TWI ports. This should only be used for array sizing; 
* dynamic usage should be based on spb_twi_num_ports_slave_get() which gets 
* the actual number for the device.
*/
#if (TWI_PORT0_ONLY == 1)
#define SPB_TWI_NUM_PORTS_SLAVE     1
#else
#define SPB_TWI_NUM_PORTS_SLAVE     2
#endif

/* 
* Total number of TWI ports (master and/or slave).
*/
#if (TWI_PORT0_ONLY == 1)
#define SPB_TWI_NUM_PORTS_TOT       1
#else
#define SPB_TWI_NUM_PORTS_TOT       2
#endif

/*
* Reserved value used to initialize all TWI ports.
*/
#define SPB_TWI_INIT_PORT_ALL       0xFF

/* Error codes */
#define SPB_TWI_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_SPB_TWI) | (err_suffix))
#define SPB_TWI_ERR_BAD_PORT_NUM_0  SPB_TWI_ERR_CODE_CREATE(0x001)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_1  SPB_TWI_ERR_CODE_CREATE(0x002)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_2  SPB_TWI_ERR_CODE_CREATE(0x003)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_3  SPB_TWI_ERR_CODE_CREATE(0x004)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_4  SPB_TWI_ERR_CODE_CREATE(0x005)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_5  SPB_TWI_ERR_CODE_CREATE(0x006)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_6  SPB_TWI_ERR_CODE_CREATE(0x007)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_7  SPB_TWI_ERR_CODE_CREATE(0x008)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_8  SPB_TWI_ERR_CODE_CREATE(0x009)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_9  SPB_TWI_ERR_CODE_CREATE(0x00A)  /**< bad port number */
#define SPB_TWI_ERR_BAD_PORT_NUM_10 SPB_TWI_ERR_CODE_CREATE(0x00B)  /**< bad port number */

/*
** SMBUS_SCL_IN/OUT_TIMEOUT_THLD register value. This corresponds to the SMBus 
** T_TIMEOUT parameter for detecting that SCL is being held low for too long 
** (> 25-35 ms). It should be set to the number of TWI clock cycles in 25-35 ms. 
** This is used only for SMBus-configured ports and will be ignored otherwise.
*/
#define SPB_TWI_SMBUS_SCL_TIMEOUT_THLD  9000000 /**< 30 ms @ 300 MHz GPBCw TWI clock */

/*
** MCTP_FAIR_IDLE register's MCTP_FAIR_IDLE_THLD values for 100 kHz and 400 kHz
** operation. These correspond to a combination of the SMBus over MCTP T_BUF
** and T_IDLE_WINDOW timing parameters. These are used for detecting a FAIR_IDLE
** condition to allow another device to use the bus after finishing a
** transaction. These should be set to at least 34.7 us for 100 kHz and 6.3 us
** for 400 kHz as per the spec. These are used for MCTP over SMBus configured
** ports only.
*/
#define SPB_TWI_MCTP_FAIR_IDLE_THLD_100KHZ  10500  /**< 35 us @ 300 MHz GPBCw TWI clock */
#define SPB_TWI_MCTP_FAIR_IDLE_THLD_400KHZ  2100   /**< 7 us @ 300 MHz GPBCw TWI clock */

/*
** MCTP_FAIR_IDLE register's MCTP_T_IDLE_DELAY_THLD values for 100 kHz and 400
** kHz operation. These correspond to the SMBus over MCTP T_IDLE_DELAY timing
** parameter. This is used for delaying before starting a new transaction after
** detecting a FAIR_IDLE condition to ensure other devices have detected the
** FAIR_IDLE condition. These should be set to at least 31 us for 100 kHz and 16
** us for 400 kHz as per the spec. These are used for MCTP over SMBus configured
** ports only.
*/
#define SPB_TWI_MCTP_T_IDLE_DELAY_THLD_100KHZ  9300  /**< 31 us @ 300 MHz GPBCw TWI clock */
#define SPB_TWI_MCTP_T_IDLE_DELAY_THLD_400KHZ  4800  /**< 16 us @ 300 MHz GPBCw TWI clock */

/*
** MCTP_PT2B register's MCTP_PT2B_THLD value. This corresponds to the SMBus over
** MCTP PT2b timing parameter for determining a bus idle condition without
** seeing a STOP condition. This is useful for power-up, when a transaction may
** already be active on the TWI bus. It is also useful in case a STOP condition
** of another device is lost because that device dropped off the bus due to an
** error. It should be set to 50 us as per the spec. This is used for MCTP over
** SMBus configured ports only.
**
** NOTE: The TWI hardware does NOT adhere to the PT2b parameter when TWI is
** brought out of reset, which is against the spec. PT2b only kicks in after the
** controller sees the first START condition. If the behaviour of the spec is
** strictly required, firmware can instead implement PT2a by starting a timer
** and holding off on sending the initial transaction until either 100ms has
** elapsed without seeing a START/STOP, or until the first START has been seen
** (at which point the PT2b hardware will kick in).
*/
#define SPB_TWI_MCTP_PT2B_THLD  15000   /**< 50 us @ 300 MHz GPBCw TWI clock */

/*
* Function Prototypes
*/

EXTERN VOID spb_twi_init(const UINT port_id);
EXTERN UINT spb_twi_num_ports_master_get(VOID);
EXTERN UINT spb_twi_num_ports_slave_get(VOID);
EXTERN VOID spb_twi_mst_int_enable(const UINT port_id, const BOOL enable);
EXTERN VOID spb_twi_port_init(const UINT port_id);
EXTERN VOID spb_twi_mst_reset(const UINT port_id);
EXTERN VOID spb_twi_mst_line_reset(const UINT port_id, const UINT duration_ms);
EXTERN VOID spb_twi_slv_int_enable(const UINT port_id, const BOOL enable);
EXTERN VOID spb_twi_slv_reset(const UINT port_id);
EXTERN VOID spb_twi_slv_reset_all(VOID);
EXTERN VOID spb_twi_reset_all(VOID);
EXTERN VOID spb_twi_slave_reset_intr_enable(const UINT port_id, const BOOL enable);
EXTERN VOID spb_twi_slave_reset_intr_clear(const UINT port_id);
EXTERN VOID spb_twi_muxing_cfg_twi_to_gpio(const UINT port_id);
EXTERN VOID spb_twi_muxing_cfg_gpio_to_twi(const UINT port_id);
EXTERN PMCFW_ERROR spb_twi_pin_read(const UINT port_id,
                                    const spb_twi_pad_enum pad,
                                    BOOL * const value);
EXTERN PMCFW_ERROR spb_twi_pin_write(const UINT port_id,
                                     const spb_twi_pad_enum pad,
                                     const BOOL value);

#endif /* _SPB_TWI_H */

/** @} end addtogroup */



