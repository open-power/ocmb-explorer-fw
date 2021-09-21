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
*     PRINTF Module header file shared by bc_printf
*
*******************************************************************************/


#ifndef _PRINTF_H
#define _PRINTF_H

/*
** Include Files
*/
#include "pmcfw_mid.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/* maximum buffer length */
#define PRINTF_LEN              256


/*
** Macro Definitions
*/

/***************************************************************************
* MACRO: PRINTF_ERR_CODE_CREATE
* __________________________________________________________________________
*
* DESCRIPTION:
*   This macro shall be used to creates error codes for the PRINTF module.
*   Each PRINTF error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | PRINTF error base (16-bit) | error code suffix (12-bit)]
*
*   where
*     PRINTF error base - 16-bit PMCFW_ERR_BASE_MEM, defined in "pmcfw_mid.h".
*     error code suffix - 12-bit specific error code suffix input to the macro.
*
*   Note:
*   - For success, the return code = PMC_SUCCESS.
*
* INPUTS:
*   UINT16 err_suffix - 12-bit error suffix.  See above for details.
*
* RETURNS:
*   Error code in format described above.
*
****************************************************************************/
#define PRINTF_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_PRINTF) | (err_suffix))
#define PRINTF_ERR_PLACEHOLDER       PRINTF_ERR_CODE_CREATE(0x001)

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN void printf_init(void);
EXTERN void bc_printf_channel_set(UINT8 channel_id);

#endif /* _PRINTF_H */



