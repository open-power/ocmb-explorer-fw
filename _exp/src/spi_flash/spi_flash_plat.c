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
*     Platform-specific definitions for SPI Flash.
*
*   NOTES:
*
*******************************************************************************/



/*
** Include Files
*/
#include "pmc_plat.h"
#include "bc_printf.h"
#include "spi_flash_api.h"
#include "fw_version_info.h"
#include "fam_plat.h"
#include "flash_partition_info.h"
#include "pboot_handoff_plat.h"
#include <string.h>


/*
** Local Constants
*/

/* Number of PUBLIC keys available */
#define SPI_FLASH_PLAT_NUM_PUBLIC_KEYS      4


/*
** Local Variables
*/


/*
** Forward References
*/


/*
** Private Functions
*/

/**
* @brief
*   Read-erase-restore a SPI flash sub-sector that is not being completely erased.
* 
* @param [in] subsector_phy_addr_ptr - sub-sector address
* @param [in] subsector_offset       - restored data offset
* @param [in[ subsector_len          - length of the sub-sector
* @param [in] restore_from_start     - flag indicating whether
*        data at start of sub-sector or end of sub-sector should
*        be restored after the erase cycle
*
*  @return
*   PMC_SUCCESS if no error
*   Error specific code otherwise
*
* @note
*/
PRIVATE PMCFW_ERROR spi_flash_plat_subsector_read_erase_restore(UINT8* subsector_phy_addr_ptr, 
                                                                UINT32 subsector_offset,
                                                                UINT32 subsector_len,
                                                                BOOL   restore_from_start)
{
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    UINT32 bytes_to_restore;
    UINT8* ram_buffer_ptr = (UINT8*)__ghsbegin_fw_auth_mem;
    UINT8* spi_restore_data_ptr;
    PMCFW_ERROR rc;

    /* determine the number of bytes that will need to be restored */
    if (TRUE == restore_from_start)
    {
        bytes_to_restore = subsector_offset;
        spi_restore_data_ptr = (UINT8*)((UINT32)subsector_phy_addr_ptr | GPBC_FLASH_UNCACHE_BASE_ADD);
    }
    else
    {
        bytes_to_restore = subsector_len - subsector_offset;
        spi_restore_data_ptr = (UINT8*)(((UINT32)subsector_phy_addr_ptr + subsector_offset) | GPBC_FLASH_UNCACHE_BASE_ADD);
    }

    /* 
    ** store data from SPI flash sub-sector that will be restored
    ** use extended data buffer as temporary storage location 
    */ 
    memcpy(ram_buffer_ptr,
           spi_restore_data_ptr,
           bytes_to_restore);

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);

    if (rc != PMC_SUCCESS)
    {
        return (rc);
    }

    rc = spi_flash_subsector_erase_wait(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        subsector_phy_addr_ptr,
                                        dev_info.max_time_subsector_erase);

    if (rc != PMC_SUCCESS)
    {
        return (rc);
    }

    /* restore the portion of SPI flash that is not being erased */
    rc = spi_flash_write_pages(SPI_FLASH_PORT,
                               SPI_FLASH_CS,
                               ram_buffer_ptr,
                               spi_restore_data_ptr,
                               bytes_to_restore,
                               dev_info.page_size,
                               dev_info.max_time_page_prog);

    if (rc != PMC_SUCCESS)
    {
        return (rc);
    }

    return (PMC_SUCCESS);

} /* spi_flash_plat_subsector_read_erase_restore */

/**
* @brief
*   Erase SPI flash sub-sectors until the redudant SPI flash
*   code image has been updated with the newer active image. The
*   code accommodates for code images that do not start or end
*   on a 4KB boundary, maintaining any data that is in a
*   sub-sector that is not within the erase parameters by
*   performing a read-erase-write.
* 
* @param [in] spi_flash_addr_ptr - flash erase start address
* @param [in] num_bytes          - number of bytes to erase
*
* @return
*   PMC_SUCCESS if no error
*   Error specific code otherwise
*
* @note
* 
*/ 
PRIVATE PMCFW_ERROR spi_flash_plat_erase(UINT8* spi_flash_addr_ptr, UINT32 num_bytes)
{
    UINT8* subsector_phy_addr_ptr;
    UINT8* erase_phy_addr_ptr;
    UINT32 subsector_len;
    UINT32 byte_index = 0;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    PMCFW_ERROR rc;

    /* convert SPI erase address to a physical address */
    erase_phy_addr_ptr = (UINT8*)((UINT32)spi_flash_addr_ptr & GPBC_FLASH_PHYS_ADDR_MASK);

    /* get the subsector parameters */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        erase_phy_addr_ptr,
                                        &subsector_phy_addr_ptr,
                                        &subsector_len);

    if (PMC_SUCCESS != rc)
    {
        return (rc);
    }

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);

    /* determine if the starting address is on a sub-sector boundary */
    if (erase_phy_addr_ptr > subsector_phy_addr_ptr)
    {
        /* 
        ** SPI flash erase address does not align with sub-sector boundary
        ** the address is above the sub-sector boundary and data at the 
        ** start of the sub-sector will need to be preserved
        */ 

        /* 
        ** read the first sub-sector data that should not be erased, then erase
        ** the sub-sector, and write back the data to be preserved in SPI flash 
        */ 
        rc = spi_flash_plat_subsector_read_erase_restore(subsector_phy_addr_ptr,
                                                         (erase_phy_addr_ptr - subsector_phy_addr_ptr),
                                                         subsector_len,
                                                         TRUE);

        if (PMC_SUCCESS != rc)
        {
            return (rc);
        }

        /* set the byte index to the number of bytes erased in first sub-sector */
        byte_index = erase_phy_addr_ptr - subsector_phy_addr_ptr;

        /* increment the flash erase physical address to align with the base address of the next sector */
        subsector_phy_addr_ptr = (UINT8*)((UINT32)subsector_phy_addr_ptr + subsector_len);
    }

    /* erase subsectors until the SPI flash region has been erased */
    for ( ; byte_index < num_bytes; byte_index += subsector_len)
    {

        if ((byte_index + subsector_len) > num_bytes)
        {
            /* erasing the next sub-sector will exceed the number of bytes to erase */

            /* 
            ** read the last sub-sector data that should not be erased, then erase
            ** the sub-sector, and write back the data to be preserved in SPI flash 
            */ 
            rc = spi_flash_plat_subsector_read_erase_restore(spi_flash_addr_ptr, 
                                                             (num_bytes - byte_index),
                                                             subsector_len,
                                                             FALSE);

            if (PMC_SUCCESS != rc)
            {
                return (rc);
            }

            /* increment the flash erase physical address by the number of bytes erased */
            subsector_phy_addr_ptr = (UINT8*)((UINT32)subsector_phy_addr_ptr + (num_bytes - byte_index));
        }
        else
        {
            /* erasing the next sub-sector will not exceed the number of bytes to erase */
            rc = spi_flash_subsector_erase_wait(SPI_FLASH_PORT,
                                                SPI_FLASH_CS,
                                                subsector_phy_addr_ptr,
                                                dev_info.max_time_subsector_erase);
            if (PMC_SUCCESS != rc)
            {
                return (rc);
            }

            /* increment the flash erase physical address to align with the base address of the next sector */
            subsector_phy_addr_ptr = (UINT8*)((UINT32)subsector_phy_addr_ptr + subsector_len);
        }
    }

    /* return success */
    return (PMC_SUCCESS);

} /* spi_flash_plat_erase */

/**
* @brief
*   Copy data from SPI flash to another part of SPI flash.
* 
* @param [in] spi_flash_dst_addr_ptr - flash destination address
* @param [in] spi_flash_src_addr_ptr - flash source address
* @param [in] num_bytes              - number of bytes to copy
*
* @return
*   PMC_SUCCESS if no error
*   Error specific code otherwise
*
* @note 
*   The destination flash memory must be erased before calling
*   this function.
*/
PRIVATE PMCFW_ERROR spi_flash_plat_flash_copy(UINT8* spi_flash_src_img_ptr, 
                                              UINT8* spi_flash_dst_img_ptr, 
                                              UINT32 num_bytes)
{
    UINT32 byte_index = 0;
    UINT8* ram_buffer_ptr = (UINT8*)__ghsbegin_fw_auth_mem;
    UINT8* subsector_phy_addr_ptr;
    UINT8* src_phy_addr_ptr;
    UINT32 subsector_len;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    PMCFW_ERROR rc;

    /* convert SPI source address to a physical address */
    src_phy_addr_ptr = (UINT8*)((UINT32)spi_flash_src_img_ptr & GPBC_FLASH_PHYS_ADDR_MASK);

    /* get the subsector parameters */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        src_phy_addr_ptr,
                                        &subsector_phy_addr_ptr,
                                        &subsector_len);


    if (PMC_SUCCESS != rc)
    {
        return (rc);
    }

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);


    if (rc != PMC_SUCCESS)
    {
        return (rc);
    }

    if (src_phy_addr_ptr > subsector_phy_addr_ptr)
    {
        /* SPI flash source address does not align with sub-sector boundary */

        UINT32 partial_subsector_bytes = (subsector_phy_addr_ptr + subsector_len) - src_phy_addr_ptr;

        /* store data from SPI flash sub-sector to extended data buffer */
        memcpy(ram_buffer_ptr,
               spi_flash_src_img_ptr,
               partial_subsector_bytes);

        /* write data from extended data buffer to flash one page at a time */
        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   ram_buffer_ptr,
                                   spi_flash_dst_img_ptr,
                                   partial_subsector_bytes,
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);

        /* set the byte index to the number of bytes copied in first sub-sector */
        byte_index = partial_subsector_bytes;

        /* increment the flash logical source address to align with the base address of the next sector */
        spi_flash_src_img_ptr = (UINT8*)((UINT32)spi_flash_src_img_ptr + partial_subsector_bytes);

        /* increment the flash logical destination address */
        spi_flash_dst_img_ptr = (UINT8*)((UINT32)spi_flash_dst_img_ptr + partial_subsector_bytes);
    }

    /* perform remaining flash-to-flash copy one sub-sector at a time */
    for ( ; byte_index < num_bytes; byte_index += subsector_len)
    {
        /* store data from SPI flash sub-sector to extended data buffer */
        memcpy(ram_buffer_ptr,
               spi_flash_src_img_ptr,
               subsector_len);

        /* write data from extended data buffer to flash one page at a time */
        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   ram_buffer_ptr,
                                   spi_flash_dst_img_ptr,
                                   subsector_len,
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);

        if (PMC_SUCCESS != rc)
        {
            return (rc);
        }

        /* increment the flash logical source address */
        spi_flash_src_img_ptr = (UINT8*)((UINT32)spi_flash_src_img_ptr + subsector_len);

        /* increment the flash logical destination address */
        spi_flash_dst_img_ptr = (UINT8*)((UINT32)spi_flash_dst_img_ptr + subsector_len);
    }

    /* return success */
    return (PMC_SUCCESS);

} /* spi_flash_plat_flash_copy */


/*
** Public Functions
*/

/**
* @brief
*   Determine if the redundant firmware image is an older
*   version than the executing image.
*  
* @param 
*    None
*  
* @return
*   Nothing
* 
*/
PUBLIC VOID spi_flash_plat_red_fw_image_update(VOID)
{
    pboot_handoff_data_struct* handoff_data_ptr = (pboot_handoff_data_struct*)__ghsbegin_handoff_data;
    fam_image_desc_struct* red_img_desc_ptr; 
    CHAR active_image_flag = (*((CHAR*)SPI_FLASH_FW_ACT_IMG_FLAG_ADDR)) & SPI_FLASH_ACTIVE_IMG_MASK;
    UINT8* spi_flash_act_img_ptr;
    UINT8* spi_flash_red_img_ptr;
    UINT32 act_img_num_bytes;
    UINT32 red_partition_num_bytes;
    PMCFW_ERROR rc;

    if (SUCCESS == handoff_data_ptr->image_list[FLASH_ACTIVE_IMAGE_INDEX].status)
    {
        /* 
        ** the active firmware image as specified by the active image flag was
        ** authenticated and is the executing image
        */ 

        /* assign pointer to the active image start address */
        spi_flash_act_img_ptr = handoff_data_ptr->image_list[FLASH_ACTIVE_IMAGE_INDEX].image_addr;

        if ('A' == handoff_data_ptr->image_list[FLASH_ACTIVE_IMAGE_INDEX].image_id)
        {
            /* booted image A, record required data */
            act_img_num_bytes = fam_image_length_get(handoff_data_ptr->image_list, FLASH_ACTIVE_IMAGE_INDEX) + SPI_FLASH_FW_IMG_A_HDR_SIZE;
            spi_flash_red_img_ptr = (UINT8*)SPI_FLASH_FW_IMG_B_HDR_ADDR;
            red_partition_num_bytes = SPI_FLASH_FW_IMG_B_HDR_SIZE + SPI_FLASH_FW_IMG_B_SIZE;
            bc_printf("Image A has been booted..\n");
        }
        else
        {
            /* booted image B, record required data */
            act_img_num_bytes = fam_image_length_get(handoff_data_ptr->image_list, FLASH_ACTIVE_IMAGE_INDEX) + SPI_FLASH_FW_IMG_B_HDR_SIZE;
            spi_flash_red_img_ptr = (UINT8*)SPI_FLASH_FW_IMG_A_HDR_ADDR;
            red_partition_num_bytes = SPI_FLASH_FW_IMG_A_HDR_SIZE + SPI_FLASH_FW_IMG_A_SIZE;
            bc_printf("Image B has been booted..\n");
        }

        /* assign a pointer to the redundant image descriptor */
        red_img_desc_ptr = &(handoff_data_ptr->image_list[FLASH_REDUNDANT_IMAGE_INDEX]);
    }
    else if (SUCCESS == handoff_data_ptr->image_list[FLASH_REDUNDANT_IMAGE_INDEX].status)
    {
        /* 
        ** the redundant firmware image was authenticated and is the executing 
        ** image, take no further action and exit  
        */
        bc_printf("Redundant Image has been booted..so returning\n");
        return;
    } 

    /* 
    ** if the firmware image specified by the active image flag is the image
    ** that is executing then it will be the only image that has been 
    ** authenticated
    ** before testing the redundant image for upgrade, it also must be 
    ** authenticated, if it cannot be authenticated it will be updated 
    */ 
    UINT32 pkey_array[SPI_FLASH_PLAT_NUM_PUBLIC_KEYS];
    for (UINT32 i = 0; i < SPI_FLASH_PLAT_NUM_PUBLIC_KEYS; i++)
    {
        pkey_array[i] = (UINT32)fam_plat_pka_pubkey_get(i);
    }

    /* authenticate the redundant image */
    red_img_desc_ptr = fam_authenticate_image(red_img_desc_ptr,
                                              1,
                                              &pkey_array[0],
                                              SPI_FLASH_PLAT_NUM_PUBLIC_KEYS,
                                              top_secure_boot_mode_get(),
                                              SPI_FLASH_FW_IMG_A_SIZE);

    if ((NULL !=red_img_desc_ptr) && (SUCCESS == red_img_desc_ptr->status))
    {
        /* redundant image authenticated */

        /* get the version information for active image and redundant image in flash */
        fw_version_info_struct fw_ver_str[SPI_FLASH_PARTITION_NUMBER];
        for(UINT32 i = 0; i < SPI_FLASH_PARTITION_NUMBER; i++)
        {
            flash_partition_fw_version_get(i, &fw_ver_str[i]);
        }

        if (fw_ver_str[0].fw_build_num <= fw_ver_str[1].fw_build_num)
        {
            /* 
            ** active image build number is less than or equal to redundant
            ** take no action and exit
            */
            return;
        }
    }
   
    bc_printf("Updating Redundant Image %s with Active Image %s\n",
              ((SPI_FLASH_ACTIVE_IMG_A == active_image_flag) ? "B" : "A"),
              ((SPI_FLASH_ACTIVE_IMG_A == active_image_flag) ? "A" : "B"));

    /* erase the redundant firmware image */
    bc_printf("Erasing Redundant Image ... ");
    rc = spi_flash_plat_erase(spi_flash_red_img_ptr, red_partition_num_bytes);
    if (PMC_SUCCESS != rc)
    {
        bc_printf("failed, rc = 0x%08X\n", rc);
        return;
    }
    bc_printf("done\n");

    /* program active firmware image over redundant firmware image */
    bc_printf("Updating Redundant Image ... ");
    rc = spi_flash_plat_flash_copy(spi_flash_act_img_ptr, spi_flash_red_img_ptr, act_img_num_bytes);
    if (PMC_SUCCESS != rc)
    {
        bc_printf("failed, rc = 0x%08X\n", rc);
        return;
    }
    bc_printf("done\n");

} /* spi_flash_plat_red_fw_image_update */

