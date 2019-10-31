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
*   Platform-specific definitions and declarations for UART.
*
* @note
*/

#ifndef _UART_PLAT_H
#define _UART_PLAT_H

/*
* Include Files
*/

#include "uart_common.h"
#include "pmc_hw_base.h"
#include "target_platform.h"

/*
* Constants
*/

/* UART ID */
#define UART_ID         UART_0

/* UART base address */
#if (UART_ID == UART_0)
#define UART_BASEADDR   UART0_BASEADDR
#elif (UART_ID == UART_1)
#define UART_BASEADDR   UART1_BASEADDR
#else
#error "UART_ID is not valid!"
#endif

/* Baud rate */
#if (TARGET_PLATFORM == PALLADIUM) 
#define UART_BAUD_RATE  UART_BAUD_9216000
#else
#define UART_BAUD_RATE  UART_BAUD_460800
#endif

/*
* Enumerated Types
*/

/*
* Structures and Unions
*/

/*
* Function Prototypes
*/

#endif /* _UART_PLAT_H */
/** @} end addtogroup */



