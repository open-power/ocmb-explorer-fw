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
*  DESCRIPTION :
*   This file implements the platform specific stack trace module for Explorer.
*
*  NOTES:
*   None.
*
*******************************************************************************/



/*
** Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include "cpuhal.h"

/*
** Constants
*/

/*
** Structures and Unions
*/

/*
** Local variables
*/

/*
** Global variables
*/

/*
** Private functions
*/

/*
** Public functions
*/

/*******************************************************************************
* FUNCTION: stack_trace_get_function_name()
* ______________________________________________________________________________
*
* DESCRIPTION:
*   Return the function name for provided PC address.
*
* INPUTS:
*   pc - Program counter
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   Empty string because this information is not stored in Explorer.
*
*******************************************************************************/
PUBLIC const CHAR* stack_trace_get_function_name(UINT32 pc)
{
    return "";
} /* stack_trace_get_function_name() */

/*******************************************************************************
* FUNCTION: stack_trace_tx_pc_get()
* ______________________________________________________________________________
*
* DESCRIPTION:
*   Get program counter from threadX thread pointer.
*
* INPUTS:
*   tx_stack_ptr - ThreadX stack pointer
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   0, threadX is not used in Explorer.
*
*******************************************************************************/
PUBLIC UINT32 stack_trace_tx_pc_get(UINT32* tx_stack_ptr)
{
    return 0;
} /* stack_trace_tx_pc_get() */

/*******************************************************************************
* FUNCTION: stack_trace_tx_sp_get()
* ______________________________________________________________________________
*
* DESCRIPTION:
*   Get stack pointer from threadX thread pointer.
*
* INPUTS:
*   tx_stack_ptr - ThreadX stack pointer
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   0, threadX is not used in Explorer.
*
*******************************************************************************/
PUBLIC UINT32 stack_trace_tx_sp_get(UINT32* tx_stack_ptr)
{
    return 0;
} /* stack_trace_tx_sp_get() */

/*******************************************************************************
* FUNCTION: stack_trace_tc_bare_metal_check()
* ______________________________________________________________________________
*
* DESCRIPTION:
*   This function checks if the TC is running ThreadX or not by checking
*   tc_id of the TC
*   Please see NOTES for usage.
*
* INPUTS:
*   tc_id  - ID of tc for which to check if bare mental or not
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   TRUE, threadX is not used in Explorer.
*
*
*******************************************************************************/
PUBLIC BOOL stack_trace_tc_bare_metal_check(UINT32 tc_id)
{
    return TRUE;
} /* reset_tc_bare_metal_check() */



