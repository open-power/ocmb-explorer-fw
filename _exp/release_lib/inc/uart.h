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
* @addtogroup UART
*   @remarks This file contains all the definitions & declarations of the UART
*   driver that are required by external applications. 
*
* @note
*  :
*   The UART driver starts off in the buffer mode, which means that
*   a circular buffer is utilized for output (if possible).  In other 
*   words, data is immediately sent to hardware during the initialization
*   context, but buffered by the circular queue in the interrupt
*   or thread context.  User can change to unbuffer mode (if necessary),
*   but all output functions become blocking call.
*
*/ 
#ifndef _UART_H
#define _UART_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "uart_common.h"
#include "uart_api.h"

/*
* Enumerated types
*/

/** UART tx mode */
typedef enum
{
    UART_TX_MODE_UNBUFFER,       /**< Unbuffered (direct) */
    UART_TX_MODE_BUFFER,         /**< Buffered (circular buffer) */
    
} uart_tx_mode_enum;

/* 
* Constants
*/

/*
* Structures
*/

/** application-level reset notification callback function */
typedef void (*uart_rx_cback_ptr)(void *rx_cback_arg);

/*
* Function Prototypes
*/
EXTERN void uart_clear_locks(UINT32 id);
EXTERN void uart_boot_init(void);

PUBLIC void uart_early_init(UINT32             id,
                            UINT32             base,
                            UINT32             data_bits,
                            UINT32             stop_bits,
                            uart_parity_enum   parity,
                            uart_baud_enum     baud,
                            BOOL               flow_ctrl);

EXTERN void uart_very_early_init(UINT32 id, UINT32 base);

EXTERN void uart_init(UINT32             id,
                      UINT32             base,
                      UINT32             int_num,
                      UINT32             data_bits,
                      UINT32             stop_bits,
                      uart_parity_enum   parity,
                      uart_baud_enum     baud,
                      BOOL               flow_ctrl,
                      uart_rx_cback_ptr  rx_cback_fcn,
                      void              *rx_cback_arg);

EXTERN void uart_reconfig(UINT32             id,
                          UINT32             data_bits,
                          UINT32             stop_bits,
                          uart_parity_enum   parity,
                          uart_baud_enum     baud,
                          uart_rx_cback_ptr  rx_cback_fcn,
                          void              *rx_cback_arg);
                      
EXTERN void uart_tx_mode(UINT32             id,
                         uart_tx_mode_enum  mode);

EXTERN PMCFW_ERROR uart_rx_timeout(UINT32 id, 
                                   UINT32 wait_option,
                                   CHAR   *c_ptr);

EXTERN void uart_crlf_convert_set(UINT32 id, BOOL flag);

EXTERN BOOL uart_crlf_convert_get(UINT32 id);

#endif /* _UART_H */
/** @} end addtogroup */


