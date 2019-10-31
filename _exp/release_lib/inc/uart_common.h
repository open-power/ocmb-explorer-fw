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
* @{ 
* @file 
* @brief
*   This file contains common definitions and function prototypes used by
*   the different variants of the UART driver.
*
*   Refer to uart_api.h for detailed of this module.
*
* @note 
*/

#ifndef _UART_COMMON_H
#define _UART_COMMON_H

/*
* Include Files
*/
#include "pmcfw_types.h"

/*
* Enumerated types
*/

/** UART baud rate selection */
typedef enum
{
    UART_BAUD_2400 = 2400,          /* 2400 baud */
    UART_BAUD_4800 = 4800,          /* 4800 baud */
    UART_BAUD_9600 = 9600,          /* 9600 baud */
    UART_BAUD_19200 = 19200,        /* 19.2 kbaud */
    UART_BAUD_28800 = 28800,        /* 28.8 kbaud */
    UART_BAUD_38400 = 38400,        /* 38.4 kbaud */
    UART_BAUD_57600 = 57600,        /* 57.6 kbaud */
    UART_BAUD_115200 = 115200,      /* 115 kbaud */
    UART_BAUD_230400 = 230400,      /* 230 kbaud */
    UART_BAUD_460800 = 460800,      /* 460 kbaud */
    UART_BAUD_9216000 = 9216000     /* 9216 kbaud */
} uart_baud_enum;

/** UART parity type selection */
typedef enum
{
    UART_PARITY_NONE,            /**< no parity bit */
    UART_PARITY_EVEN,            /**< one parity bit - even */
    UART_PARITY_ODD              /**< one parity bit - odd */

} uart_parity_enum;

/* 
* Constants
*/

/* UART port ids */
#define UART_0                    (0)
#define UART_1                    (1)

/* UART's internal clock prescaler */
#define UART_PRESCALER            (16)

/* UART TX hardware FIFO length (drop the length by half due to CR-LF replacement) */
#define UART_TX_HWFIFO_LEN        (256 / 2)

/*
* Macro Definitions
*/

/**
* @brief
*   This macro sanity checks the ranges of the data bits and stop bits, as well
*   as their combination. It will assert if the check fails.
* 
*   The data bits must be 5, 6, 7, or 8.
* 
*   Only the following data bits and stop bits combinations are allowed:
*       data_bits       stop_bits
*       5,6,7,8         1
*       6,7,8           2
*
* @param[in] data_bits - the number of data bits
* @param[in] stop_bits - the number of stop bits to generate when transmitting
* 
* @return 
*   None.
*
* @hideinitializer
*/
/* Check data bits & stop bits combination */
#define UART_ASSERT_DATA_STOP_BITS_GOOD(data_bits, stop_bits) \
{ \
    PMCFW_ASSERT((data_bits > 4) && (data_bits < 9), UART_ERR_BAD_PARAM); \
    PMCFW_ASSERT((stop_bits == 1) || (stop_bits == 2), UART_ERR_BAD_PARAM); \
    PMCFW_ASSERT((stop_bits != 2) || (data_bits != 5), UART_ERR_BAD_PARAM); \
}

/*
* Structures and Unions
*/

/*
* Global variables 
*/
 
/* 
* Function Prototypes
*/

EXTERN UINT32 uart_calc_dl(uart_baud_enum baud);

EXTERN UINT32 uart_calc_lcr(UINT32 data_bits, UINT32 stop_bits, uart_parity_enum parity);

EXTERN VOID uart_line_ctrl_reg_write(VOID *base, UINT32 offset, UINT32 val, UINT32 fcr);

EXTERN VOID uart_update_line_ctrl_regs(VOID *base, UINT32 dl, UINT32 lcr, UINT32 fcr);

EXTERN BOOL uart_poll_tx_fifo_empty(VOID *base);

#endif /* _UART_COMMON_H */
/** @} end addtogroup */


