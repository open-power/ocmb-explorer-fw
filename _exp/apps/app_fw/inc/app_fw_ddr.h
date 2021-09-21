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
*     This file contains the platform specific application definitions.
*
*   NOTES:
*     None.
*
*******************************************************************************/




/**
* @addtogroup APP_FW_DDR
* @{
* @file
* @brief
*   This file contains the platform defines for application FW DDR.
*
*/

#ifndef _APP_FW_DDR_H
#define _APP_FW_DDR_H

/*
** Include Files
*/

#include "pmcfw_common.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/* Error codes */
#define APP_FW_DDR_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_APPFW_DDR) | (err_suffix))
#define APP_FW_DDR_ERR_CALIBRATION_CRC          APP_FW_DDR_ERR_CODE_CREATE(0x001)  /* Error: Calibration CRC error */
#define APP_FW_DDR_ERR_DDR_TRAINING_ALIGN       APP_FW_DDR_ERR_CODE_CREATE(0x002)  /* Error: Training data is not 4K aligned */
#define APP_FW_DDR_ERR_TRAINING_ERASE           APP_FW_DDR_ERR_CODE_CREATE(0x003)  /* Error: Training data erase failed  */
#define APP_FW_DDR_ERR_TRAINING_ERASE_TIMEOUT   APP_FW_DDR_ERR_CODE_CREATE(0x004)  /* Error: Training data erase timed out */
#define APP_FW_DDR_ERR_TRAINING_WRITE           APP_FW_DDR_ERR_CODE_CREATE(0x005)  /* Error: Training data write failed */
#define APP_FW_DDR_ERR_TRAINING_WRITE_TIMEOUT   APP_FW_DDR_ERR_CODE_CREATE(0x006)  /* Error: Training data write timed out */


/*
** Macro Definitions
*/

/*
** Structures and Unions
*/

/**
* @brief Saved DDR training data
*/
typedef struct
{
    UINT32              header;      /* 32-bit header to identify the section */
    ddr_timing_data_t   timing_data; /* Structure containing the saved timing data */
    ddr_vref_data_t     vref_data;   /* Structure containing the saved vref data */
    UINT32              crc;         /* The saved CRC value calculated for the structure */
} app_fw_ddr_calibration_data_struct;

/*
** Global variables
*/

EXTERN app_fw_ddr_calibration_data_struct app_fw_ddr_saved_data;


/*
** Function Protoypes
*/

EXTERN PMCFW_ERROR app_fw_ddr_calibration_save(app_fw_ddr_calibration_data_struct *ddr_training_data);
EXTERN PMCFW_ERROR app_fw_ddr_calibration_load(app_fw_ddr_calibration_data_struct *ddr_training_data);
EXTERN PMCFW_ERROR app_fw_ddr_bringup_init(void);
EXTERN void app_fw_ddr_cmdsvr_init(void);

#endif


