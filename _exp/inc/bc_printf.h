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
*   DESCRIPTION :
*     This file facilitates code reuse. bc_printf is a special version
*     of printf that outputs to Circular Character Buffers (CCB) and UART
*     port(s). bc_printf must be used as the primary standard output function.
*
*******************************************************************************/


#ifndef _BC_PRINTF_H
#define _BC_PRINTF_H

/*
** Include Files
*/
#include "pmcfw_types.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Macro Definitions
*/

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN UINT32 bc_printf(const CHAR *format, ...);
EXTERN UINT32 bc_sprintf(const CHAR *format, ...);
EXTERN void bc_printf_init(UINT32 uart_id);
EXTERN void bc_printf_channel_set(UINT8 channel_id);
EXTERN void bc_hw_print(CHAR *buffer_ptr, const UINT32 length);
EXTERN UINT32 bc_critical_printf(const CHAR *format, ...);
#endif /* _BC_PRINTF_H */



