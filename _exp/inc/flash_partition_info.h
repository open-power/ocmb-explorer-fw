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
* @addtogroup FLASH_PARTITION_INFO
* @{
* @file
* @brief
*   Flash partition info.
*
* @note
*/
#ifndef _FLASH_PARTITION_INFO
#define _FLASH_PARTITION_INFO

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "spi_flash_plat.h"
#include "fam.h"

/*
** Enumerated Types
*/


/*
** Constants
*/

#define FLASH_ACTIVE_IMAGE_INDEX        0
#define FLASH_REDUNDANT_IMAGE_INDEX     1


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

EXTERN VOID flash_partition_image_list_get(fam_image_desc_struct *image_list);
EXTERN UINT32 flash_partition_boot_partition_id_get(void);
EXTERN VOID flash_partition_fw_version_get(UINT32 image_id, fw_version_info_struct *ver);


#endif /* _FLASH_PARTITION_INFO */
/** @} end addtogroup */



