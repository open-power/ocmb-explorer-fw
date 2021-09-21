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
* @defgroup FATAL FATAL: Handles Fatal/non-fatal HW interrupts
* @brief 
*     Utilities functions used by applications to handle fatal and
*     non-fatal hardware interrupts.
* @{  
* @file 
* @brief 
*   Public API for FATAL 
*
* @note  
*   Fatal hardware interrupts are non recoverable by HW. Many non-fatal HW 
*   interrupts are actually fatal to the firmware. 
*/ 
#ifndef _FATAL_API_H
#define _FATAL_API_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
* Enumerated Types
*/

/*
* Constants
*/

/*
* Macro Definitions
*/

/** application-level fatal gpio callback function */
typedef void (*fatal_gpio_app_callback_fcn_ptr)(UINT32 num);

/*
* Structures and Unions
*/

/*
* Global variables
*/

/*
* Function Prototypes
*/

EXTERN void fatal_app_gpio_handler_set(fatal_gpio_app_callback_fcn_ptr callback_ptr);

#endif /* _FATAL_API_H */


/** @} end addtogroup */


