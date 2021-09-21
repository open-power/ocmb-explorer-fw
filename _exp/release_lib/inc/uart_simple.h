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
* @brief
*   The Simple UART driver is a light-weight module that sets up the
*   bare minimum registers to allow characters to be outputted to the
*   UART device. 
*
*   The intended consumers are applications that have
*   strict memory constraints and that would also like to write to the
*   UART device, ie. Bootloader.
*
*   When the driver receives a request to transmit a buffer of characters,
*   it will first wait until all characters currently in the TX FIFO have
*   a chance to trasmit. It does so by polling the "THR Empty" bit of
*   the Line Status Register and waiting for it to become 1.
*
*   The compile flag -DUART_TX_FIFO_POLL_TIMEOUT_EN determines the behaviour
*   of the poll:
*   1) If flag exists, the driver will poll for a maximum of 1 second before
*      concluding that a hardware failure occurs and aborting the transmit.
*   2) If flag does not exists, the driver will wait indefinitely.
*
* @{  
* @file 
* @brief 
*   This file contains all the definitions & declarations of the Simple
*   UART driver that are required by some external applications. 
* 
* @note
*/
#ifndef _UART_H
#define _UART_H

/*
* Include Files
*/
#include "pmcfw_err.h"
#include "pmcfw_types.h"
#include "uart_common.h"
#include "uart_api.h"

/*
* Enumerated types
*/

/*
* Constants
*/

/* Default UART configurations for API */
#define UART_DEFAULT_NB_DATA_BITS   (8)
#define UART_DEFAULT_NB_STOP_BITS   (1)
#define UART_DEFAULT_FLOW_CTRL      (FALSE)

/** Initializes both the UART device and the driver */
EXTERN VOID uart_init(UINT32              id,
                      UINT32              base,
                      UINT32              data_bits,
                      UINT32              stop_bits,
                      uart_parity_enum    parity,
                      uart_baud_enum      baud,
                      BOOL                flow_ctrl_en);

/*
* Initializes the UART driver only, assuming the UART device has already been
* initialized by other application. An application should either call 
* uart_init() or uart_set_base_addr(), but never both.
*/ 
EXTERN VOID uart_set_base_addr(UINT32 id, UINT32 base); 

EXTERN BOOL uart_flush(UINT32 id);

EXTERN void uart_fifo_rst(UINT32 id);

EXTERN void uart_crlf_convert_set(UINT32 id, BOOL flag);

EXTERN BOOL uart_crlf_convert_get(UINT32 id);

EXTERN void uart_tx_force(UINT32 id,
                          BOOL enable);
#endif /* _UART_H */

/** @} end addtogroup */


