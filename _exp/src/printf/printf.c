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
*       printf Module.
*
*       See bc_printf.h for details.
*
*   NOTES:
*       None.
*
*******************************************************************************/


/*
** Include Files
*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "pmcfw_types.h"
#include "pmc_plat.h"
#include "printf.h"
#include "bc_printf.h"
#include "uart.h"
#include "char_io.h"

/*
** Local Enumerated Types
*/

/*
** Local Constants
*/

/* maximum buffer length */
#define BC_PRINTF_LEN              256

/*
** Local Macro Definitions
*/

/*
** Local Structures and Unions
*/

/***************************************************************************
* STRUCTURE: bc_printf_ctrl_struct
* __________________________________________________________________________
*
* DESCRIPTION:
*   Control structure for bc_printf module.
*
* ELEMENTS:
*   uart_id          - UART Id
****************************************************************************/
typedef struct
{    
    UINT32              uart_id;
} bc_printf_ctrl_struct;

/*
** Local Variables
*/

/* 
** Pointer to bc_printf module control structure 
** All accesses to this pointer are critical section and must be 
** protected with spinlock. 
*/ 
PRIVATE bc_printf_ctrl_struct bc_printf_ctrl;

/* Current print channel ID. */
PRIVATE UINT8 printf_current_channel_id = CHAR_IO_CHANNEL_ID_UNINITIALIZED;

/*
** Forward References
*/

/*
** Private Functions
*/

/*
** Public Functions
*/

/****************************************************************************
* FUNCTION: bc_printf_init
* __________________________________________________________________________
*
* DESCRIPTION:
*   Initialize the bc_printf module.
*
* INPUTS:
*   uart_id  - UART Id
*   ccb_size - don't care.
* 
* OUTPUTS:
*   None.
*
* RETURNS:
*   None.
*
* NOTES:
*   None.
* 
*****************************************************************************/
PUBLIC void bc_printf_init(UINT32 uart_id)
{
    memset(&bc_printf_ctrl, 0, sizeof(bc_printf_ctrl_struct));

    /* Save UART Id */
    bc_printf_ctrl.uart_id      = uart_id;

    /* Set current channel ID to run-time */
    printf_current_channel_id = CHAR_IO_CHANNEL_ID_RUNTIME;

} /* End: bc_printf_init() */

/**
* @brief
*   Sets printf module to use the crash channel.
*
* @param[in] channel_id - Channel ID to change to
*
* @return
*   none
*
* @note
*   none
*
*/
PUBLIC void bc_printf_channel_set(UINT8 channel_id)
{
    /* Set current channel ID to crash */
    printf_current_channel_id = channel_id;

} /* End: printf_crash_channel_set() */

/**
* @brief
*   Prints contents of a formatted message buffer to HW stdout. Does not do any
*   formatting.
*
* @param[in] buffer_ptr - pointer to message buffer
* @param[in] length - length of message
*
* @return
*   none
*
* @note
*   none
*
*/
PUBLIC void bc_hw_print(CHAR *buffer_ptr, const UINT32 length)
{
    /* print to UART */
    uart_tx(bc_printf_ctrl.uart_id, buffer_ptr, length);
    
} /* End: bc_hw_print */


/**
* @brief
*   Basecode version of printf.  bc_printf outputs to the basecode defined HW
*   stdout and to a Character IO module. This is the printf function to be
*   used throughout the base code. The use of any other variation of printf
*   functions, such as printf from STDIO library, is prohibited. This is to
*   ensure the reusability of the common base code.
*
* @param[in] variable - Takes same input as a STDIO printf.
*
* @return
*   Number of characters printed; 0 on buffer overflow.
*
* @note
*   IMPORTANT: Only use bc_printf in common reuse base code! All
*   '\n' characters are replaced with "\r\n".
*
*/
PUBLIC UINT32 bc_printf(const CHAR *format, ...)
{
    CHAR     buffer[BC_PRINTF_LEN];
    va_list  marker;
    UINT32   length;
    
#pragma ghs startnomisra
    /* create the variable argument list */
    va_start(marker, format);
#pragma ghs endnomisra
    
    /* process the format specifications */
    length = vsnprintf(buffer, BC_PRINTF_LEN, format, marker);
    
    /* destroy the variable argument list */
    va_end(marker);

    /* check for data and space */
    if (BC_PRINTF_LEN <= length)
    {
        return 0;
    }

    /* Only print to UART in runtime */
    if (printf_current_channel_id == CHAR_IO_CHANNEL_ID_RUNTIME)
    {
        /* print to the UART */
        bc_hw_print(buffer, length);
    }

    /* print to current log buffer */
    char_io_put(printf_current_channel_id, buffer, length);

    return length;

} /* End: bc_printf */

/******************************************************************************
*
*  FUNCTION: bc_critical_printf
*  ____________________________________________________________________________
*
*  DESCRIPTION:
*   This is a stub function included for compile reasons and never used.
*
*  INPUTS:
*   Takes same input as a STDIO printf.
*
*  OUTPUTS:
*    None.
*
*  RETURNS:
*   0
*
*  NOTE:
*   None
*
******************************************************************************/
PUBLIC UINT32 bc_critical_printf(const CHAR *format, ...)
{
   return bc_printf(format);

} /* End: bc_critical_printf */


/**
* @brief
*   Basecode version of sprintf.  bc_sprintf outputs only to an output buffer
*   managed by the CHAR_IO module. 
*
* @param[in] variable - Takes same input as a STDIO printf.
*
* @return
*   Number of characters printed; 0 on buffer overflow.
*
* @note
*   IMPORTANT: Only use bc_sprintf in common reuse base code! All
*   '\n' characters are replaced with "\r\n".
*
*/
PUBLIC UINT32 bc_sprintf(const CHAR *format, ...)
{
    va_list  marker;
    CHAR     buffer[PRINTF_LEN];
    UINT32   length;

#pragma ghs startnomisra
    /* create the variable argument list */
    va_start(marker, format);
#pragma ghs endnomisra

    /* process the format specifications */
    length = vsnprintf(buffer, PRINTF_LEN, format, marker);

    va_end(marker);

    /* check for data and space */
    if (PRINTF_LEN <= length)
    {
        return 0;
    }

    /* print to current log buffer */
    char_io_put(printf_current_channel_id, buffer, length);

    return length;
} /* End: bc_sprintf */

/* End of File */



