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
* @defgroup TWI TWI: Two Wire Interface
* @ingroup PlatformDriversModulePlatform
* @brief
*   Two Wire Interface. This includes the definition of busses in master and slave
*   mode and support for both 10-bit and 7-bit slave addresses.
* @{  
* @file 
* @brief 
*   This file contains all the TWI API and definitions, which are used to access 
*   TWI based temperature sensor and Seeprom, etc.
* 
* @note 
*/ 

#ifndef _TWI_API_H
#define _TWI_API_H

/*
* Include Files
*/
#include "pmcfw_err.h"
#include "pmcfw_types.h"

/*
* Constants
*/
 

#define TWI_ERR_CODE_MASK                   (0x00000FFF)  /**< Mask for error code suffix */
/**
* @brief
*   This macro shall be used to create error codes for the TWI module.
*
*   Each TWI error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (31:28) | TWI error base (27:12) | error code suffix (11:0)]
*
* @param [in] err_suffix - 12-bit error suffix.  See above for details.
*
* @return
*   Error code in format described above.
* @hideinitializer
*/
#define TWI_ERR_CODE_CREATE(err_code)       ((PMCFW_ERR_BASE_TWI) | ((err_code) & TWI_ERR_CODE_MASK)) 
#define TWI_ERR_UNEXPECTED_INT_SRC          TWI_ERR_CODE_CREATE(0x001)  /**< Unexpected interrupt source */
#define TWI_ERR_UNHANDLED_INT_SRC           TWI_ERR_CODE_CREATE(0x002)  /**< Unhandled interrupt source */
#define TWI_ERR_MTX_TIMEOUT                 TWI_ERR_CODE_CREATE(0x003)  /**< Mutex waiting timeout */
#define TWI_ERR_MST_TIMEOUT_RECOVERED       TWI_ERR_CODE_CREATE(0x004)  /**< TWI master transaction timeout and bus recovery success */
#define TWI_ERR_MST_TIMEOUT_UNRECOVERED     TWI_ERR_CODE_CREATE(0x005)  /**< TWI master transaction timeout and bus recovery fail */
#define TWI_ERR_INVALID_PARAMETERS          TWI_ERR_CODE_CREATE(0x006)  /**< Invalid parameter */
#define TWI_ERR_INVALID_PTR                 TWI_ERR_CODE_CREATE(0x007)  /**< Invalid pointer */
#define TWI_ERR_UNINITIALIZED_STRUCT        TWI_ERR_CODE_CREATE(0x008)  /**< Uninitialized structure */
#define TWI_ERR_RX_UNDER                    TWI_ERR_CODE_CREATE(0x009)  /**< Attempts to read the empty receive buffer */
#define TWI_ERR_RX_OVER                     TWI_ERR_CODE_CREATE(0x00a)  /**< Receive additional byte when receive buffer is full  */    
#define TWI_ERR_TX_OVER                     TWI_ERR_CODE_CREATE(0x00b)  /**< Attempts to write to the full transmit buffer */
#define TWI_ERR_MST_RECOVERY_FAIL           TWI_ERR_CODE_CREATE(0x00c)  /**< Master recovery process fail */
#define TWI_ERR_TXABRT_7ADDR_NOACK          TWI_ERR_CODE_CREATE(0x00d)  /**< Master is in 7-bit addressing mode and the address sent was not acknowledged by any slave */
#define TWI_ERR_TXABRT_10ADDR1_NOACK        TWI_ERR_CODE_CREATE(0x00e)  /**< Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave. */
#define TWI_ERR_TXABRT_10ADDR2_NOACK        TWI_ERR_CODE_CREATE(0x00f)  /**< Master is in 10-bit address mode and the second address byte of the 10-bit address was not acknowledged by any slave. */
#define TWI_ERR_TXABRT_DATA_NOACK           TWI_ERR_CODE_CREATE(0x010)  /**< Master does not receive an acknowledge from the slave */
#define TWI_ERR_TXABRT_GCALL_NOACK          TWI_ERR_CODE_CREATE(0x011)  /**< No slave on the bus acknowledged the General Call */
#define TWI_ERR_TXABRT_GCALL_READ           TWI_ERR_CODE_CREATE(0x012)  /**< Master sent a General Call but the user programmed the byte following the General Call to be a read from the bus */
#define TWI_ERR_TXABRT_HS_ACKDET            TWI_ERR_CODE_CREATE(0x013)  /**< Master is in High Speed mode and the High Speed Master code was acknowledged */
#define TWI_ERR_TXABRT_SBYTE_ACKDET         TWI_ERR_CODE_CREATE(0x014)  /**< START Byte was acknowledged */
#define TWI_ERR_TXABRT_HS_NORESTART         TWI_ERR_CODE_CREATE(0x015)  /**< The restart is disabled, and the user is trying to use the master to transfer data in High Speed mode */
#define TWI_ERR_TXABRT_SBYTE_NORESTART      TWI_ERR_CODE_CREATE(0x016)  /**< The restart is disabled, and the user is trying to send a START Byte. */
#define TWI_ERR_TXABRT_10RD_NORESTART       TWI_ERR_CODE_CREATE(0x017)  /**< The restart is disabled, and the master sends a read command in 10-bit addressing mode */
#define TWI_ERR_TXABRT_MASTER_DISABLE       TWI_ERR_CODE_CREATE(0x018)  /**< Initiate a Master operation with the Master mode disabled */
#define TWI_ERR_TXABRT_ARB_LOST             TWI_ERR_CODE_CREATE(0x019)  /**< Master or the slave transmitter has lost arbitration */
#define TWI_ERR_TXABRT_SLVFLUSH_TXFIFO      TWI_ERR_CODE_CREATE(0x01a)  /**< Slave has received a read command and some data exists in the TX FIFO so the slave issues a TX_ABRT interrupt to flush old data in TX FIFO */
#define TWI_ERR_TXABRT_SLV_ARB_LOST         TWI_ERR_CODE_CREATE(0x01b)  /**< Slave lost the bus while transmitting data to a remote master */
#define TWI_ERR_TXABRT_SLVRD_INTX           TWI_ERR_CODE_CREATE(0x01c)  /**< Set the required SDA hold time in units of ic_clk period*/
#define TWI_ERR_TXABRT_USER_ABRT            TWI_ERR_CODE_CREATE(0x01d)  /**< Master has detected the transfer abort */
#define TWI_ERR_TXABRT_SLV_SCL_STRETCH_ERR  TWI_ERR_CODE_CREATE(0x01e)  /**< There was a possibility of clock stretching in the slave */
#define TWI_ERR_TXABRT_UNKNOWN              TWI_ERR_CODE_CREATE(0x01f)  /**< Master has detected the transfer abort with an unknown reason */
#define TWI_ERR_FUNC_NON_SUPPORTED          TWI_ERR_CODE_CREATE(0x100)  /**< Non supported function */
#define TWI_ERR_SLAVE_ADDR_SET_UNRECOVERED  TWI_ERR_CODE_CREATE(0x101)  /**< Failed recovery after slave address set failure */
#define TWI_ERR_SLAVE_ADDR_SET_MST_ACTIVITY TWI_ERR_CODE_CREATE(0x102)  /**< Master activity during slave address set attempt */
#define TWI_ERR_SLAVE_RX_LEN_TOO_LARGE      TWI_ERR_CODE_CREATE(0x103)  /**< Slave RX length request too large */
#define TWI_ERR_SLAVE_ADDR_SET_TIMEOUT      TWI_ERR_CODE_CREATE(0x104)  /**< Fail to set slave address */
#define TWI_ERR_SMBUS_QUICK_CMD_NSUP        TWI_ERR_CODE_CREATE(0x105)  /**< SMBus Quick Command not supported by hardware */
#define TWI_ERR_QUICK_COMMAND_TIMEOUT       TWI_ERR_CODE_CREATE(0x106)  /**< SMBus Quick Command timed out */
#define TWI_ERR_SMBUS_NOT_CONFIGURED        TWI_ERR_CODE_CREATE(0x107)  /**< TWI port isn't configured to use SMBus */
#define TWI_ERR_MST_NOT_DISABLED            TWI_ERR_CODE_CREATE(0x108)  /**< Master port didn't get disabled */
#define TWI_ERR_SLV_NOT_DISABLED            TWI_ERR_CODE_CREATE(0x109)  /**< Slave port didn't get disabled */
#define TWI_ERR_INVALID_PORT_MS_MODE_01     TWI_ERR_CODE_CREATE(0x10a)  /**< Invalid port master/slave mode setting */
#define TWI_ERR_INVALID_PORT_MS_MODE_02     TWI_ERR_CODE_CREATE(0x10b)  /**< Invalid port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_01   TWI_ERR_CODE_CREATE(0x10c)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_02   TWI_ERR_CODE_CREATE(0x10d)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_03   TWI_ERR_CODE_CREATE(0x10e)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_04   TWI_ERR_CODE_CREATE(0x10f)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_05   TWI_ERR_CODE_CREATE(0x110)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_06   TWI_ERR_CODE_CREATE(0x111)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_07   TWI_ERR_CODE_CREATE(0x112)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_08   TWI_ERR_CODE_CREATE(0x113)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_09   TWI_ERR_CODE_CREATE(0x114)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_10   TWI_ERR_CODE_CREATE(0x115)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_PORT_MS_MODE_INCORRECT_11   TWI_ERR_CODE_CREATE(0x116)  /**< Incorrect port master/slave mode setting */
#define TWI_ERR_MS_SWITCH_DISABLED          TWI_ERR_CODE_CREATE(0x117)  /**< Master/slave switching is disabled */
#define TWI_ERR_SLV_PORT_ENABLE             TWI_ERR_CODE_CREATE(0x118)  /**< Error enabling slave port */
#define TWI_ERR_SLV_PORT_DISABLE            TWI_ERR_CODE_CREATE(0x119)  /**< Error disabling slave port */
#define TWI_ERR_INVALID_CLK_RATE_MCTP       TWI_ERR_CODE_CREATE(0x11a)  /**< Invalid clock rate for MCTP over SMBus */
#define TWI_ERR_MST_NOT_ABORTED             TWI_ERR_CODE_CREATE(0x11b)  /**< Master port transfer didn't get aborted */
#define TWI_ERR_INT_MODE_NOT_SUPPORTED      TWI_ERR_CODE_CREATE(0x11c)  /**< Interrupt mode is not supported */
#define TWI_ERR_MST_ISR_NOT_INSTALLED       TWI_ERR_CODE_CREATE(0x11d)  /**< TWI Master ISR is not installed */
#define TWI_ERR_SLV_ISR_NOT_INSTALLED       TWI_ERR_CODE_CREATE(0x11e)  /**< TWI Slave ISR is not installed */
#define TWI_ERR_SRST_ISR_NOT_INSTALLED      TWI_ERR_CODE_CREATE(0x11f)  /**< TWI Slave Reset ISR is not installed */
#define TWI_ERR_CMDSVR_INVALID_ARGS         TWI_ERR_CODE_CREATE(0x120)  /**< TWI command server invalid arguements */


/*
** Bitmasks to specify upper-layer protocols. Some of these protocol features 
** are implemented in the TWI driver itself, so the driver must know about the 
** protocol in use. 
*/
#define TWI_PROTO_NONE      0x00000000  /**< No upper-layer protocol */
#define TWI_PROTO_SMBUS     0x00000001  /**< SMBus */
#define TWI_PROTO_MCTP      0x00000002  /**< MCTP */

/**
* @brief
* TWI port master/slave mode configurations
*/
typedef enum
{
    TWI_PORT_MS_MODE_MASTER,  /**< master */
    TWI_PORT_MS_MODE_SLAVE,   /**< slave */
} twi_port_ms_mode_enum;

/**
* @brief
* TWI clock rates
*/
typedef enum
{
    TWI_CLK_RATE_100KHZ = 1,   /**< 100 kHz (standard mode) */
    TWI_CLK_RATE_400KHZ = 2,   /**< 400 kHz (fast mode) */
    TWI_CLK_RATE_3_4MHZ = 3    /**< 3.4 MHz (high speed mode) */
} twi_clk_rate_enum;

/**
* @brief
*   TWI device address sizes
*
*/
typedef enum
{
    TWI_ADDR_SIZE_7BIT,    /**< 7 bit address */
    TWI_ADDR_SIZE_10BIT    /**< 10 bit address */
} twi_addr_size_enum;

/**
* @brief
*   TWI offset sizes. These are used when calling the _offset() functions to
*   determine whether to send an 8 or 16 bit offset.
*/
typedef enum
{
    TWI_OFFSET_SIZE_8BIT = 1, /**< 8 bit offset */
    TWI_OFFSET_SIZE_16BIT= 2  /**< 16 bit offset */
} twi_offset_size_enum;

/**
* @brief 
*  TWI slave device information.
*/
typedef struct
{
    UINT port_id;                     /**< TWI port the slave connects to */
    UINT16 addr;                      /**< 7 or 10-bit slave address without read/write bit. If the address 
                                      * has a read/write bit, it should be right-shifted by one.
                                      */
    twi_addr_size_enum addr_size;     /**< address size (7 or 10 bits) */
    UINT32 stretch_timeout_ms;        /**<
                                      * Per-byte stretch time timeout (ms) used for calculating
                                      * the overall timeout when waiting for a response from the
                                      * slave. The overall timeout for a transaction will be this
                                      * timeout multiplied by the byte count, added to the minimum
                                      * TWI transaction time based on the clock rate and byte count.
                                      */
    UINT32 stretch_timeout_ms_offset; /**<
                                      * Same as stretch_timeout_ms above, but used for the offset
                                      * portion of a transaction when using the _offset() functions.
                                      */
    twi_offset_size_enum offset_size; /**<
                                      * Size of offset to use (8 or 16 bits). This only applies
                                      * when using the _offset() functions.
                                      */
} twi_slave_struct;

/**
* @brief
*   Slave callback function.
*
* @param[in] port_id               - port_id
* @param[in] activity              - activity
* @param[in] slv_callback_data_ptr - (pointer to) callback data
*/
typedef VOID (*twi_slv_callback_fptr) (const UINT   port_id,
                                       const UINT32 activity,
                                       const VOID   * const slv_callback_data_ptr);
/**
* @brief
*   Master callback function
*
* @param[in] port_id               - port_id
* @param[in] activity              - activity
* @param[in] mst_callback_data_ptr - (pointer to) callback data
*/
typedef VOID (*twi_mst_callback_fptr)   (UINT                      port_id,
                                         const UINT32              activity,
                                         VOID                      *mst_callback_data_ptr);

/**
* @brief 
*   Configuration for a TWI port supporting slave mode.
*/
typedef struct
{
    twi_slv_callback_fptr cback;  /**< callback function to call when an RX/TX is pending or on error */
    VOID *cback_data_ptr;         /**< data to pass to the callback */
    UINT16 tx_buf_len;            /**< transmit buffer length */
    UINT16 addr;                  /**< 7 or 10-bit slave address without read/write bit. If the address 
                                  * has a read/write bit, it should be right-shifted by one.
                                  */
    twi_addr_size_enum addr_size; /**< address size (7 or 10 bits) */
} twi_slave_port_cfg_struct;

/**
* @brief 
*  TWI port configuration.
*/
typedef struct
{
    twi_clk_rate_enum clk_rate;              /**< TWI clock rate */
    twi_mst_callback_fptr mst_callback_fptr; /**<
                                             * callback function to call when an RX/TX is pending
                                             * or on error
                                             */
    VOID *mst_callback_data_ptr;             /**< data to pass to the callback */
    BOOL mst_init_line_reset_en;             /**<
                                             * If TRUE, assert the reset line when initializing
                                             * the port.
                                             */
    BOOL mst_init_recovery_en;               /**<
                                             * If TRUE, perform master recovery when initializing
                                             * the port. This is useful for when a stale
                                             * transaction might exist that needs to be terminated.
                                             * This should be set to FALSE if there will be
                                             * multiple masters on the port.
                                             */
    UINT32 proto;                            /**<
                                             * upper-layer protocol, as a bitmask of
                                             * TWI_PROTO_xxx values
                                             */
    twi_port_ms_mode_enum ms_mode;           /**< initial master/slave mode configuration */
    BOOL ms_switch_en;                       /**<
                                             * if TRUE, support switching between master
                                             * and slave modes
                                             */
    twi_slave_port_cfg_struct slave_cfg;     /**< slave configuration for the port */
} twi_port_cfg_struct;

/*
* Function Prototypes
*/

EXTERN VOID twi_mst_line_reset(const UINT port_id, const UINT duration_ms);

EXTERN PMCFW_ERROR twi_mst_tx(const twi_slave_struct * const slave_ptr,
                              const UINT8 * const data_ptr,
                              const UINT32 len,
                              const BOOL stop_en);

EXTERN PMCFW_ERROR twi_mst_tx_offset(const twi_slave_struct * const slave_ptr,
                                     const UINT32 offset,
                                     const UINT8 * const data_ptr,
                                     const UINT32 len);

EXTERN PMCFW_ERROR twi_mst_rx(const twi_slave_struct * const slave_ptr,
                              UINT8 * const data_ptr,
                              const UINT32 len,
                              const BOOL stop_en);

EXTERN PMCFW_ERROR twi_mst_rx_offset(const twi_slave_struct * const slave_ptr,
                                     const UINT32 offset,
                                     UINT8 * const data_ptr,
                                     const UINT32 len);

EXTERN PMCFW_ERROR twi_mst_tx_rx(const twi_slave_struct * const slave_ptr,
                                 const UINT8 * const tx_data_ptr,
                                 const UINT32 tx_len,
                                 UINT8 * const rx_data_ptr,
                                 const UINT32 rx_len);

EXTERN VOID twi_port_init(const UINT port_id,
                          const twi_port_cfg_struct * const cfg_ptr);

EXTERN PMCFW_ERROR twi_slv_poll(const UINT port_id,
                                UINT32 * const activity_ptr);
                                
EXTERN PMCFW_ERROR twi_slv_data_len_get(const UINT port_id,
                                        UINT32 * const len_ptr);
                                        
EXTERN PMCFW_ERROR twi_slv_data_get(const UINT  port_id, 
                                    UINT8 * const data_ptr, 
                                    const UINT32 length);
                                    
EXTERN PMCFW_ERROR twi_slv_data_put(const UINT port_id,
                                    const UINT8 * const data_ptr,
                                    const UINT32 len);

EXTERN BOOL twi_mst_init_recovery_en_get(const UINT port_id);
EXTERN VOID twi_mst_init_recovery_en_set(const UINT port_id, BOOL mst_line_reset_en);

#endif

/** @} end addtogroup */


