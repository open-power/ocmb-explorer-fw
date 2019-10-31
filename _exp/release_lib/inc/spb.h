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

/********************************************************************************
*   DESCRIPTION : This file is the public module header file for the GPBCw.
*
*   NOTES:        None.
*
*
*******************************************************************************/



#ifndef _SPB_H
#define _SPB_H

/*
** Include Files
*/

#include "pmcfw_types.h"

/*
** Enumerated Types
*/

/*******************************************************************************
* ENUM: spi_int_to_cpu_enum
*_______________________________________________________________________________
*
* DESCRIPTION:
*    This enumeration specifies processor that GPBC can route the interrupts to.
*
* ELEMENTS: 
*    SPB_INT_TO_PCS  - Route interrupts to PCS (bit definition may refer to PCSX)
*    SPB_INT_TO_SPCS - Route interrupts to SPCS (bit definition may refer to OPSX)
*
*******************************************************************************/
typedef enum
{
    SPB_INT_TO_PCS  = 0, 
    SPB_INT_TO_SPCS = 1
} spi_int_to_cpu_enum;

/*
** Function Prototypes
*/

EXTERN void spb_init(void);
EXTERN void spb_di_init(void);
EXTERN void spb_int_enable(spi_int_to_cpu_enum cpu);
EXTERN void spb_dump_debug_info(const BOOL force);
EXTERN void spb_fatal_error_handler(void);
EXTERN void spb_non_fatal_error_handler(void);

#endif /* define _SPB_H */



