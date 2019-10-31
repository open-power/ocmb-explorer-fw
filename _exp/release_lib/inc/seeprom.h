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
*  DESCRIPTION : The header file for the Serial EEPROM Module.
*
*  NOTES:
*
*******************************************************************************/


#ifndef _SEEPROM_H
#define _SEEPROM_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "seeprom_plat.h"

/*
** Macro Definitions
*/

/***************************************************************************
* MACRO: SEEP_ERR_CODE_CREATE
* __________________________________________________________________________
*
* DESCRIPTION:
*   This macro creates an error code for the SEEP module.
*
* INPUTS:
*   UINT16 err_suffix - The error suffix that is appended to the 16-bit
*                       PEM module ID.
*
* RETURNS:
*   The error code in this format: 
*       [PEM module ID (16-bit) | error suffix (16-bit)]
****************************************************************************/
#define SEEP_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_SEEP) | (err_suffix))

/*
** Constants
*/

/* Error code */ 
#define SEEP_ERR_OOR_00             SEEP_ERR_CODE_CREATE(0x001) /* memory access out of range */ 
#define SEEP_ERR_OOR_01             SEEP_ERR_CODE_CREATE(0x002) /* memory access out of range */ 
#define SEEP_ERR_UNKNOWN_DEV_00     SEEP_ERR_CODE_CREATE(0x003) /* accessing a unknown device */ 
#define SEEP_ERR_UNKNOWN_DEV_01     SEEP_ERR_CODE_CREATE(0x004) /* accessing a unknown device */ 

/*
** Public Function Declarations
*/
EXTERN void seeprom_init(void); 

EXTERN PMCFW_ERROR seeprom_write(UINT8   port_id,
                                 UINT8   addr,
                                 UINT32  offset,
                                 UINT8   offset_size,
                                 UINT8   *data_ptr,
                                 UINT32  num_bytes,
                                 UINT16  page_size,
                                 UINT32  *len_written_ptr,
                                 UINT32  timeout_ms);

EXTERN PMCFW_ERROR seeprom_read(UINT8   port_id,
                                UINT8   addr,
                                UINT32  offset,
                                UINT8   offset_size,
                                UINT8   *data_ptr,
                                UINT32  num_bytes,
                                UINT16  page_size,
                                UINT32  *len_read_ptr,
                                UINT32  timeout_ms);

#endif /* define _SEEPROM_H */



