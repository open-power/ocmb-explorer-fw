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
*     Platform-specific definitions and declarations for temperature sensor
*
*******************************************************************************/


#ifndef _TEMP_SENSOR_PLAT_H
#define _TEMP_SENSOR_PLAT_H

/*
** Include Files
*/
#include "pmc_hw_base.h"


/*
** Enumerated Types 
*/


/*
** Constants 
*/

/**
* @brief
*   This macro shall be used to creates error codes for the TEMP module.
*
*   Each TEMP error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | CCB error base (16-bit) | error code suffix (12-bit)]
*
*   where
*     TEMP error base   - 16-bit PMCFW_ERR_BASE_CCB, defined in "pmcfw_err.h".
*     error code suffix - 12-bit specific error code suffix input to the macro.
*
*   Note:
*   - For success, the return code = PMC_SUCCESS.
*
* @param [in] err_suffix - 12-bit error suffix.  See above for details.
*
* @return
*   Error code in format described above.
* @hideinitializer
*/
#define TEMP_ERR_CODE_CREATE(err_suffix)            ((PMCFW_ERR_BASE_TEMP) | (err_suffix))
#define TEMP_ERR_SWITCH_CHANNEL_ID_INVALID          TEMP_ERR_CODE_CREATE(0x001)
#define TEMP_ERR_FW_MANAGED_UNSUPPORTED             TEMP_ERR_CODE_CREATE(0x010)
#define TEMP_ERR_ONCHIP_REG_READ_LEN_UNSUPPORTED    TEMP_ERR_CODE_CREATE(0x011)
#define TEMP_ERR_ONCHIP_NUM_REG_READ_OP_UNSUPPORTED TEMP_ERR_CODE_CREATE(0x012)

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


EXTERN VOID temp_sensor_plat_init(VOID);
EXTERN VOID temp_sensor_plat_update(VOID);


#endif /* _TEMP_SENSOR_PLAT_H */



