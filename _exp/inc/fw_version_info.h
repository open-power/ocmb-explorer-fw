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
* @addtogroup FW_VERSION_INFO
* @{
* @file
* @brief
*   FW version information
*
* @note
*/
#ifndef _FW_VERSION_INFO_H
#define _FW_VERSION_INFO_H

/*
* Include Files
*/

#include "pmcfw_types.h"

/*
** Enumerated Types
*/


/*
** Constants
*/
#define FW_VERSION_MAJOR_RELEASE_NUMBER         8
#define FW_VERSION_MINOR_RELEASE_NUMBER         3
#define FW_VERSION_CL_NUMBER                    444714
#define FW_VERSION_PATCH_RELEASE_NUMBER         0
#define FW_VERSION_BUILD_DATE                   0x09282021

/*
** This can be used by the host to track changes in the FW API. It should be 
** incremented each time a new API command is added or parameters are changed. 
*/
#define FW_API_VERSION_NUMBER                   6

/*
** Macro Definitions
*/

/*
* Structures and Unions
*/

/**
* @brief 
*  FW version info structure
*
* @note
*
*/
typedef struct
{
    UINT32 fw_ver_major;    /**< FW version - Major release */
    UINT32 fw_ver_minor;    /**< FW version - Minor release */ 
    UINT32 fw_build_patch;  /**< FW build patch number      */
    UINT32 fw_build_num;    /**< FW build number            */
    UINT32 fw_build_date;   /**< FW build date mmddyyyy     */
} fw_version_info_struct;


/*
** Global variables
*/

/*
** Function Prototypes
*/


#endif /* _FW_VERSION_INFO_H */
/** @} end addtogroup */







