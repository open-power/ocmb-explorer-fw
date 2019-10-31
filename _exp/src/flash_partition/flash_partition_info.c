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
*   DESCRIPTION: This file implements various functions related to flash partition,
*	variables and functions
*
*   NOTES:       None.
*
******************************************************************************/




/**
* @addtogroup flash_partition
* @{
* @file
* @brief
*   This file contains various functions related to flash partition .
*
*/ 


/*
** Include Files
*/
#include "pmcfw_common.h"
#include "top.h"
#include "bc_printf.h"
#include "ech.h"
#include "fam.h"
#include "fw_version_info.h"
#include "flash_partition_info.h"
#include "app_fw.h"
#include <string.h>

/*
* Local Enumerated Types
*/


/*
** Local Constants
*/

/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/

/*
** Global Variables
*/


/*
** Private Data
*/

/*
** Private Functions
*/


/**
* Public Functions
*/

/**
* @brief
*   Create ordered array of image candidates for authentication.
*   Depends on active image flag in flash memory. The active
*   image information is returned in the 0th entry of the array
*   and the redundant image information is returned in the 1st
*   entry of the array.
*
* @param [out] image_list - array into which image list is written
*
* @return
*   none
*
*/
PUBLIC VOID flash_partition_image_list_get(fam_image_desc_struct *image_list) 
{

    UINT32 act_img;
    UINT32 img_b_requested;

        
    /* retrieve active image flag from flash */
    act_img = * (UINT32 *) (SPI_FLASH_FW_ACT_IMG_FLAG_ADDR);
    
    
    img_b_requested =
        ((act_img & SPI_FLASH_ACTIVE_IMG_B) == SPI_FLASH_ACTIVE_IMG_B) ? SPI_FLASH_ACTIVE_IMG_B : SPI_FLASH_ACTIVE_IMG_A;
        
    
    if (img_b_requested)
    {
        /* -->  image B --> image A */
        image_list[0].image_addr   = (UINT8 *) (SPI_FLASH_FW_IMG_B_HDR_ADDR);
        image_list[1].image_addr   = (UINT8 *) (SPI_FLASH_FW_IMG_A_HDR_ADDR);
        image_list[0].image_id   = 'B';
        image_list[1].image_id   = 'A';
    }
    else 
    {
        /* -->  image A --> image B*/
        image_list[0].image_addr   = (UINT8 *) (SPI_FLASH_FW_IMG_A_HDR_ADDR); 
        image_list[1].image_addr   = (UINT8 *) (SPI_FLASH_FW_IMG_B_HDR_ADDR);
        image_list[0].image_id   = 'A';
        image_list[1].image_id   = 'B';
    }
    
 }
    

/**
* @brief
*   Returns Image ID by reading the partition flag
*   
*
* @param
*   none
* @return
*   Image ID
*
*/

PUBLIC UINT32 flash_partition_boot_partition_id_get(void)
{
    UINT32 act_img;
    UINT32 img_id;

        
    /* retrieve active image flag from flash */
    act_img = * (UINT32 *) (SPI_FLASH_FW_ACT_IMG_FLAG_ADDR);
    
    
    img_id =
        ((act_img & SPI_FLASH_ACTIVE_IMG_B) == SPI_FLASH_ACTIVE_IMG_B) ? 'B' : 'A';
    return img_id;

}

/**
* @brief
*   Returns flash partition version string
*
* @param [in] image_id - Image ID for which version string will be returned 
*                        image_id == 0 is request for active
*                        firmware image information
*                        image_id == 1 is request for redundant
*                        firmware image information
* @param [out] ver - pointer to a buffer where version string is written
*
* 
* @return
*   FW version string
*
*/
PUBLIC VOID flash_partition_fw_version_get(UINT32 image_id, fw_version_info_struct *ver)
{
    UINT32 image_length;
    UINT32 fw_version_address;
    fam_image_desc_struct fw_image_list[SPI_FLASH_PARTITION_NUMBER];
    
    /*Populate image list*/
    flash_partition_image_list_get(&fw_image_list[0]);
    image_length = fam_image_length_get(&fw_image_list[0], image_id);
    /* 
    ** Max size for image A or image B are the same, so using SPI_FLASH_FW_IMG_A_SIZE
    ** for both the images. Check the image length, if the length 
    ** is incorrect, return
    */
    if (image_length > SPI_FLASH_FW_IMG_A_SIZE)
    {
        /* Fill with 0xFF and return */
        memset((void *)ver, 0xFF, sizeof(fw_version_info_struct));
        bc_printf("Error!!! flash_partition_fw_version_get:APP_FW_ERR_FW_VERSION_LENGTH \n");
        return;
    }
    /* 
    ** Header Size for image A or image B are the same, so using SPI_FLASH_FW_IMG_A_HDR_SIZE
    ** for the calculation.
    */
    fw_version_address = (UINT32)fw_image_list[image_id].image_addr + SPI_FLASH_FW_IMG_A_HDR_SIZE+image_length - 
                        sizeof(fw_version_info_struct);

    /* If address of the FW version field is outside of flash partition, return */
    if (fw_version_address > SPI_FLASH_FW_FW_UPGRADE_ADDR)
    {
        /* Fill with 0xFE and return */
        memset((void *)ver, 0xFF, sizeof(fw_version_info_struct));
        bc_printf("Error!!! flash_partition_fw_version_get:APP_FW_ERR_FLASH_BOUNDARY \n");
        return;
    }
        
    memcpy((void *)ver, (void *)fw_version_address, sizeof(fw_version_info_struct));
}

/** @} end group */



