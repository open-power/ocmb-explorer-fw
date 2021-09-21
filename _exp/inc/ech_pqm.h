/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2021 Microchip Technology Inc. All rights reserved. 
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
* @addtogroup ECH_PQM
* @{
* @file
* @brief
*   Explorer Product Qualification Mode (PQM) Command Handler
*   definitions and declarations.
*
* @note
*/
#ifndef _ECH_PQM_H
#define _ECH_PQM_H


/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_api.h"


/*
** Constants
*/


/*
* Enumerated Types
*/


/*
* Structures and Unions
*/


/*
* Global Variables
*/


/*
* Function Prototypes
*/
EXTERN BOOL ech_twi_pqm_get(VOID);
EXTERN VOID ech_twi_pqm_set(BOOL setting);
EXTERN UINT32 ech_pqm_force_delay_line_update (UINT8* rx_buf_pt, UINT32 rx_index);
EXTERN VOID ech_pqm_cmd_proc(UINT8* rx_buf_ptr, UINT32 rx_index);
EXTERN VOID ech_pqm_cmd_rx_index_increment(UINT8* rx_buf_ptr, UINT32 rx_index);

#endif /* _ECH_PQM_H */
/** @} end addtogroup */



