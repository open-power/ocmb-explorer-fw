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
*     Public stack trace platform specific definitions and prototypes.
*
*   NOTES:
*     None.
*
*******************************************************************************/


#ifndef _STACK_TRACE_PLAT_H
#define _STACK_TRACE_PLAT_H

/*
** Include Files
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
** Global Variables
*/

/*
** Function prototypes
*/
EXTERN CHAR* stack_trace_get_function_name(UINT32 pc);
EXTERN UINT32 stack_trace_tx_pc_get(UINT32* tx_stack_ptr);
EXTERN UINT32 stack_trace_tx_sp_get(UINT32* tx_stack_ptr);
EXTERN BOOL stack_trace_tc_bare_metal_check(UINT32 tc_id);

#endif /* _STACK_TRACE_PLAT_H */


