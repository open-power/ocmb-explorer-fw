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
*     Header file containing all the OCMB API function definitions.
*
*   NOTES:
*     None.
*
*******************************************************************************/
#ifndef _OCMB_TEMP_H
#define _OCMB_TEMP_H

/*
** Include Files
*/

/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Macro Definitions
*/

/***************************************************************************
* MACRO: OCMB_API_ERR_CODE_CREATE
* __________________________________________________________________________
*
* DESCRIPTION:
*   This macro creates an error code for the OCMB API module.
*
* INPUTS:
*   UINT16 err_suffix - The error suffix that is appended to the 16-bit
*                       OCMB module ID.
*
* RETURNS:
*   The error code in this format:
*       [OCMB API module ID (16-bit) | error suffix (16-bit)]
****************************************************************************/
#define OCMB_API_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_OCMB_API) | (err_suffix))

/*
** Constants
*/

/* Error code */
#define OCMB_API_MCBIST_NOT_COMPLETE          OCMB_API_ERR_CODE_CREATE(0x001) /* MCBIST is still running */
#define OCMB_API_MCBIST_FAILED                OCMB_API_ERR_CODE_CREATE(0x002) /* MCBIST failed */

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN VOID ocmb_api_rxd_clr(VOID);
EXTERN VOID ocmb_api_rxd_flag_set(VOID);
EXTERN BOOL ocmb_api_rxd_flag_get(VOID);
EXTERN VOID ocmb_api_temp_dimm0_update(UINT16 temp, BOOL temp_valid, BOOL present, BOOL error);
EXTERN VOID ocmb_api_temp_dimm1_update(UINT16 temp, BOOL temp_valid, BOOL present, BOOL error);
EXTERN VOID ocmb_api_temp_onchip_update(UINT16 temp, BOOL temp_valid, BOOL present, BOOL error);
EXTERN PMCFW_ERROR ocmb_api_mcbist_result_get(VOID);

#endif /* _OCMB_TEMP_H */
