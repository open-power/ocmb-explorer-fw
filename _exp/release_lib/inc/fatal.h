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
* @addtogroup FATAL
* @{ 
* @file 
* @brief
*   Utilities functions used during fatal and non-fatal hardware interrupts.
*   
*   Refer to fatal.h for detailed of this module.
* @note 
*
*/ 
#ifndef _FATAL_H
#define _FATAL_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "fatal_api.h"

/*
* Enumerated Types
*/

/*
* Constants
*/

/*
* Macro Definitions
*/

/*
* Structures and Unions
*/

/*
* Global variables
*/

/*
* Function Prototypes
*/

EXTERN void fatal_dump_list(const unsigned list_entries, 
                            const UINT32 *dump_list_ptr, 
                            const UINT32 reg_base_address);
EXTERN void fatal_dump_reg(const UINT32 from_addr, const UINT32 to_addr);
EXTERN void fatal_dump_title(const char *title_ptr);

#endif /* _FATAL_H */


/** @} end addtogroup */


