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



/**
* @addtogroup Flashloader
* @{
* @file
* @brief
*   Explorer Flashloader Module.
*
* @note
*/

#ifndef _FLASHLOADER_H
#define _FLASHLOADER_H


/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
** Constants
*/


/*
* Structures and Enumerated Types
*/
/**
* @brief
*   Flashloader Command handlers.
*/
typedef struct _flashloader_handler_struct
{
    VOID (*flashloader_fw_image_upgrade_handler)(VOID);    /**< flashloader Binary upgrade handler*/
    VOID (*flashloader_version_info_handler)(VOID);         /**< flashloader version info handler */

} flashloader_handler_struct;


/** 
*  @brief 
*   Flashloader command
*/
typedef enum
{
    FLASHLOADER_CMD_NULL = 0,           /**< Null command */
    FLASHLOADER_CMD_WRITE,              /**< Flashloader write command */
    FLASHLOADER_CMD_COMMIT,             /**< Flashloader commit command */
    FLASHLOADER_CMD_WRITE_ABORT,        /**< Flashloader write abort command */
    FLASHLOADER_CMD_PARTITION_INFO_GET, /**< Flashloader partition info get command */
    FLASHLOADER_CMD_FLASH_READ,         /**< Flashloader flash read command */
    FLASHLOADER_CMD_PARTITION_ERASE,    /**< Flashloader partition erase command */
    FLASHLOADER_CMD_MAX
} flashloader_cmd_enum;

/** 
*  @brief 
*   Flashloader Error Code
*/
typedef enum
{
    FLASHLOADER_ERR_READ_PARAM_ERROR =1,
    FLASHLOADER_ERR_INVALID_READ_LENGTH,
    FLASHLOADER_ERR_ADDRESS_OUT_OF_RANGE,
    FLASHLOADER_ERR_INVALID_PARTITION_ID, 
    FLASHLOADER_ERR_DEVINFO_GET,
    FLASHLOADER_ERR_PARAMS_GET,
    FLASHLOADER_ERR_SUBSECTOR_ERASE,
    FLASHLOADER_ERR_SUBSECTOR_ERASE_TIMEOUT,
    FLASHLOADER_ERR_PARTITION_VALUE_WRITE,
    FLASHLOADER_ERR_PARTITION_VALUE_WRITE_TIMEOUT,
    FLASHLOADER_ERR_FLASH_WRITE_TIMEOUT,
    FLASHLOADER_ERR_FLASH_WRITE_FAIL,
    FLASHLOADER_ERR_FLASH_IMAGE_COMMIT_FW_LENGTH_OUT_OF_RANGE
}flashloader_error_code_enum;


/*
* Function Prototypes
*/

EXTERN VOID flashloader_init(VOID);



#endif /* _FLASHLOADER_H */
/** @} end addtogroup */



