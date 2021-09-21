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
* @defgroup UART UART: Universal Asynchronous Receiver/Transmitter
* @ingroup PlatformDriversModulePlatform
* @brief
*    This module implements the UART driver. The UART driver configures the 
*    serial port hardware and performs read and write transactions.
* @{  
* @file 
* @brief
*    This module provides the interface of the UART driver that are 
*    required by applications to send/receive char from UART.
*
* @internal
*    On the tx side, a circular
*    buffer is utilized in the buffer mode.  When the producer index (PI)
*    is equal to consumer index (CI), the circular buffer is empty.  When
*    the PI is one less than the CI, the circular buffer is full.
*
*    The UART hardware block must contain a RX FIFO with reasonable size,
*    preferably, more than 16 characters.
* @endinternal
*/ 
#ifndef _UART_API_H
#define _UART_API_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_mid.h"

/**
* @brief
*   This macro shall be used to create error codes for the UART module.
*   Each UART error code has 32 bits and is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | UART error base (16-bit) | error code suffix (12-bit)]
*
* @brief
*   where
*     UART error base   - 16-bit PMCFW_ERR_BASE_UART, defined in "pmcfw_mid.h".
*     error code suffix - 12-bit specific error code suffix input to the macro.
*
* @brief
*   Note:
*   - For success, the return code = PMC_SUCCESS.
*
* @param [in] err_suffix - 12-bit error suffix.  See above for details.
*
* @return 
*   Error code in format described above.
*
* @hideinitializer
*/
#define UART_ERR_CODE_CREATE(err_suffix)  ((PMCFW_ERR_BASE_UART) | (err_suffix))
#define UART_ERR_CONTEXT            UART_ERR_CODE_CREATE(0x00) /**< wrong context                  */
#define UART_ERR_INITIALIZED        UART_ERR_CODE_CREATE(0x01) /**< uart initialized               */
#define UART_ERR_UNINITIALIZED      UART_ERR_CODE_CREATE(0x02) /**< uart not initialized           */
#define UART_ERR_BAD_INTR           UART_ERR_CODE_CREATE(0x03) /**< bad interrupt source           */
#define UART_ERR_BAD_PARAM          UART_ERR_CODE_CREATE(0x04) /**< bad call parameter(s)          */
#define UART_ERR_EMPTY              UART_ERR_CODE_CREATE(0x05) /**< no data available              */
#define UART_ERR_NOT_EMPTY          UART_ERR_CODE_CREATE(0x06) /**< more data available            */
#define UART_ERR_FULL               UART_ERR_CODE_CREATE(0x07) /**< full or no more space          */
#define UART_ERR_CTRL_PTR_NOT_FOUND UART_ERR_CODE_CREATE(0x08) /**< UART control pointer not found */
#define UART_ERR_NULL_CTRL_PTR_1    UART_ERR_CODE_CREATE(0x09) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_2    UART_ERR_CODE_CREATE(0x0A) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_3    UART_ERR_CODE_CREATE(0x0B) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_4    UART_ERR_CODE_CREATE(0x0C) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_5    UART_ERR_CODE_CREATE(0x0D) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_6    UART_ERR_CODE_CREATE(0x0E) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_7    UART_ERR_CODE_CREATE(0x0F) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_8    UART_ERR_CODE_CREATE(0x10) /**< UART control pointer NULL      */
#define UART_ERR_NULL_CTRL_PTR_9    UART_ERR_CODE_CREATE(0x11) /**< UART control pointer NULL      */
#define UART_ERR_RX_NOT_SUPPORTED   UART_ERR_CODE_CREATE(0x12) /**< uart rx not supported          */
#define UART_ERR_RX_TIMEOUT         UART_ERR_CODE_CREATE(0x13) /**< uart rx timeout                */
#define UART_ERR_GENERAL            UART_ERR_CODE_CREATE(0xFF) /**< general unclassified error     */

/*
* Enumerated types
*/

/* 
* Constants
*/

/*
* Structures
*/

/* 
* Function Prototypes
*/
EXTERN BOOL uart_rx_data_rdy(UINT32 id);

EXTERN PMCFW_ERROR uart_rx(UINT32  id,
                           CHAR   *c_ptr);

EXTERN void uart_tx(UINT32  id, 
                    CHAR   *d_ptr, 
                    UINT32  len);

EXTERN void uart_tx_flush(UINT32 id);

EXTERN void uart_tx_output_set(UINT32  id,
                               BOOL    enable);

#endif /* _UART_API_H */
/** @} end addtogroup */


