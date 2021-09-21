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
#include "spi.h"
#include "sys_timer_api.h"
#include "top_plat.h"
#include "spb_spi.h"


/*
** Local Constants
*/

/* Number of PUBLIC keys available */
#define SPI_FLASH_PLAT_NUM_PUBLIC_KEYS      4


/*
** Local Variables
*/
/* SPI flash image authentication information */
spi_flash_plat_auth_info_struct auth_info_struct;

/*
** Forward References
*/


/*
** Private Functions
*/

/**
* @brief
*   Macro to check for uncorrectable ECCs, clear them, and
*   record in the auth_info_struct
*  
* @param [in] err_str - String to be printed if an uncorrectable
*       ECC was detected
*  
* @param [out] uecc_detected - BOOL to return result of 
*        uncorrectable ECC error check
* @param [out] ecc_report_var - Variable to set to 1 if an 
*        uncorrectable ECC was detected
*  
*/
#define UECC_CHECK_REPORT(uecc_detected, ecc_report_var, err_str) \
        uecc_detected = spb_spi_ecc_err_check(0); \
        if (uecc_detected) \
        { \
            ecc_report_var = 1; \
            bc_printf(err_str); \
        }

/**
* @brief
*   Read-erase-restore a SPI flash subsector that is not being
*   completely erased.
* 
* @param [in] subsector_log_base_ptr - logical subsector address
* @param [in] byte_offset            - restored data offset
* @param [in[ subsector_len          - length of the subsector
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
PRIVATE PMCFW_ERROR spi_flash_plat_subsector_read_erase_restore(UINT8* subsector_log_base_ptr, 
                                                                UINT32 bytes_to_restore,
                                                                UINT32 subsector_len,
                                                                BOOL   restore_from_start)
{
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    UINT8* ram_buffer_ptr = (UINT8*)__ghsbegin_fw_auth_mem;
    UINT8* spi_restore_data_ptr;
    PMCFW_ERROR rc;
    top_plat_lock_struct lock_struct;

    /* determine the number of bytes that will need to be restored */
    if (TRUE == restore_from_start)
    {
        /* set restore pointer to the start of the sub-secto r*/
        spi_restore_data_ptr = (UINT8*)MIPS_KSEG1(subsector_log_base_ptr + GPBC_FLASH_PHYS_BASE_ADDR);
    }
    else
    {
        /* set restore pointer to the offset in the sub-sector where restoration starts */
        spi_restore_data_ptr = (UINT8*)MIPS_KSEG1(subsector_log_base_ptr + (subsector_len - bytes_to_restore) + GPBC_FLASH_PHYS_BASE_ADDR);
    }
    
    /* 
    ** store data from SPI flash subsector that will be restored
    ** use FAM authentication memory buffer as temporary storage location 
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

    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    /* initiate subsector erase and wait for it to complete */
    rc = spi_flash_subsector_erase_wait(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        subsector_log_base_ptr,
                                        dev_info.max_time_subsector_erase);

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);

    if (rc != PMC_SUCCESS)
    {
        return (rc);
    }

    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    /* restore the portion of SPI flash that is not being erased */
    rc = spi_flash_write_pages(SPI_FLASH_PORT,
                               SPI_FLASH_CS,
                               ram_buffer_ptr,
                               (UINT8*)((UINT32)spi_restore_data_ptr & GPBC_FLASH_PHYS_ADDR_MASK),
                               bytes_to_restore,
                               dev_info.page_size,
                               dev_info.max_time_page_prog);

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);

    if (rc != PMC_SUCCESS)
    {
        return (rc);
    }

    return (PMC_SUCCESS);

} /* spi_flash_plat_subsector_read_erase_restore */

/**
* @brief
*   Erase SPI flash sub-sectors until the number of bytes have been erased. The
*   code accommodates for images that do not start or end on a 4KB physical
*   sub-block boundary, maintaining any data that is in a sub-sector that is
*   not within the erase parameters by performing a read-erase-write.
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
PUBLIC PMCFW_ERROR spi_flash_plat_erase(UINT8* spi_flash_addr_ptr, UINT32 num_bytes)
{
    UINT8* subsector_log_base_ptr;
    UINT8* erase_log_offset_ptr;
    UINT32 subsector_len;
    UINT32 byte_index = 0;
    UINT32 bytes_to_restore;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    PMCFW_ERROR rc;
    top_plat_lock_struct lock_struct;

    /* convert SPI erase address to a logical offset */
    erase_log_offset_ptr = (UINT8*)((UINT32)spi_flash_addr_ptr & GPBC_FLASH_PHYS_ADDR_MASK);

    /* get the subsector parameters */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        erase_log_offset_ptr,
                                        &subsector_log_base_ptr,
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

    if (PMC_SUCCESS != rc)
    {
        return (rc);
    }

    /* determine if the starting offset is on a sub-sector boundary */
    if (erase_log_offset_ptr > subsector_log_base_ptr)
    {
        /* 
        ** SPI flash erase offset does not align with sub-sector boundary
        ** the offset is above the sub-sector boundary and data at the 
        ** start of the sub-sector will need to be preserved
        */ 

        /* 
        ** set the number of bytes to restore as the difference between the 
        ** starting erase address and the starting address of the sub-sector 
        */ 
        bytes_to_restore = erase_log_offset_ptr - subsector_log_base_ptr;

        /* 
        ** read the first sub-sector data that should not be erased, then erase
        ** the sub-sector, and write back the data to be preserved in SPI flash 
        */
        rc = spi_flash_plat_subsector_read_erase_restore(subsector_log_base_ptr,
                                                         bytes_to_restore,
                                                         subsector_len,
                                                         TRUE);

        if (PMC_SUCCESS != rc)
        {
            return (rc);
        }

        /* increment the flash erase logical offset to align with the base address of the next sub-sector */
        subsector_log_base_ptr = (UINT8*)((UINT32)subsector_log_base_ptr + subsector_len);

        /* set the byte index to the number of bytes in the sub-sector that were erased */
        byte_index = subsector_len - bytes_to_restore;
    }

    /* erase subsectors until the SPI flash region has been erased */
    for ( ; byte_index < num_bytes; byte_index += subsector_len)
    {
        if ((byte_index + subsector_len) > num_bytes)
        {
            /* 
            ** erasing the next sub-sector will exceed the number of bytes to 
            ** erase and data at the end of the sub-sector needs to be preserved
            */ 
            
            /* 
            ** set the number of bytes to restore as the difference between the 
            ** sub-sector length and the number of remaining bytes to erase 
            */ 
            bytes_to_restore = subsector_len - (num_bytes - byte_index);

            /* 
            ** read the last sub-sector data that should not be erased, then erase
            ** the sub-sector, and write back the data to be preserved in SPI flash 
            */
            rc = spi_flash_plat_subsector_read_erase_restore(subsector_log_base_ptr, 
                                                             bytes_to_restore,
                                                             subsector_len,
                                                             FALSE);

            if (PMC_SUCCESS != rc)
            {
                return (rc);
            }
        }
        else
        {
            /* disable interrupts and disable multi-VPE operation */
            top_plat_critical_region_enter(&lock_struct);

            /* erasing the next sub-sector will not exceed the number of bytes to erase */
            rc = spi_flash_subsector_erase_wait(SPI_FLASH_PORT,
                                                SPI_FLASH_CS,
                                                subsector_log_base_ptr,
                                                dev_info.max_time_subsector_erase);
            /* restore interrupts and enable multi-VPE operation */
            top_plat_critical_region_exit(lock_struct);

            if (PMC_SUCCESS != rc)
            {
                return (rc);
            }

            /* increment the flash erase logical offset to align with the base address of the next sector */
            subsector_log_base_ptr = (UINT8*)((UINT32)subsector_log_base_ptr + subsector_len);
        }
    }

    /* return success */
    return (PMC_SUCCESS);

} /* spi_flash_plat_erase */

/**
* @brief
*   Copy data from SPI flash to another part of SPI flash.
* 
* @param [in] spi_flash_src_addr_ptr - flash source address
* @param [in] spi_flash_dst_addr_ptr - flash destination address
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
    UINT8* spi_src_log_offset_ptr;
    UINT8* subsector_log_base_ptr;
    UINT32 subsector_len;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    PMCFW_ERROR rc;
    top_plat_lock_struct lock_struct;

    /* convert SPI source address to a logical offset */
    spi_src_log_offset_ptr = (UINT8*)((UINT32)spi_flash_src_img_ptr & GPBC_FLASH_PHYS_ADDR_MASK);

    /* get the source subsector parameters */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        spi_src_log_offset_ptr,
                                        &subsector_log_base_ptr,
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

    if (spi_src_log_offset_ptr > subsector_log_base_ptr)
    {
        /* SPI flash source offset does not align with sub-sector boundary */

        /* get the number of bytes to read from the sector */
        UINT32 partial_subsector_bytes = (subsector_log_base_ptr + subsector_len) - spi_src_log_offset_ptr;

        /* store data from SPI flash sub-sector to extended data buffer */
        memcpy(ram_buffer_ptr,
               spi_flash_src_img_ptr,
               partial_subsector_bytes);

        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);

        /* write data from extended data buffer to flash one page at a time */
        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   ram_buffer_ptr,
                                   (UINT8*)((UINT32)spi_flash_dst_img_ptr & GPBC_FLASH_PHYS_ADDR_MASK),
                                   partial_subsector_bytes,
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);

        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        if (PMC_SUCCESS != rc)
        {
            return (rc);
        }

        /* increment the flash logical source address */
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

        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);

        /* write data from extended data buffer to flash one page at a time */
        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   ram_buffer_ptr,
                                   (UINT8*)((UINT32)spi_flash_dst_img_ptr & GPBC_FLASH_PHYS_ADDR_MASK),
                                   subsector_len,
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);

        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

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
*   version than the executing image. Populate auth_info_struct
*   with information about the image authentication.
*  
* @param 
*    None
*  
* @return
*   Nothing
*  
* @note 
*   This function must be wrapped in a critical section and
*   2-bit ECC interrupt propagation must be disabled to avoid
*   uncorrectable ECC errors causing FW to assert.
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
    UINT32 red_img_num_bytes;
    UINT32 act_partition_num_bytes;
    UINT32 red_partition_num_bytes;
    PMCFW_ERROR rc;
    BOOL uecc_detected;

    memset(&auth_info_struct, 0, sizeof(auth_info_struct));

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

            auth_info_struct.active_image_index = 0;
            auth_info_struct.red_image_index = 1;
        }
        else
        {
            /* booted image B, record required data */
            act_img_num_bytes = fam_image_length_get(handoff_data_ptr->image_list, FLASH_ACTIVE_IMAGE_INDEX) + SPI_FLASH_FW_IMG_B_HDR_SIZE;
            spi_flash_red_img_ptr = (UINT8*)SPI_FLASH_FW_IMG_A_HDR_ADDR;
            red_partition_num_bytes = SPI_FLASH_FW_IMG_A_HDR_SIZE + SPI_FLASH_FW_IMG_A_SIZE;
            bc_printf("Image B has been booted..\n");

            auth_info_struct.active_image_index = 1;
            auth_info_struct.red_image_index = 0;
        }

        /* assign a pointer to the redundant image descriptor */
        red_img_desc_ptr = &(handoff_data_ptr->image_list[FLASH_REDUNDANT_IMAGE_INDEX]);

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

        /* Check for uncorrectable ECC errors during image authentication */
        UECC_CHECK_REPORT(uecc_detected, 
                          auth_info_struct.uecc_detected[auth_info_struct.red_image_index], 
                          "Uncorrectable ECC detected during image authentication\n");

        if ((NULL !=red_img_desc_ptr) && (SUCCESS == red_img_desc_ptr->status) && (FALSE == uecc_detected))
        {
            /* redundant image authenticated */

            /* get the version information for active image and redundant image in flash */
            fw_version_info_struct fw_ver_str[SPI_FLASH_PARTITION_NUMBER];
            for(UINT32 i = 0; i < SPI_FLASH_PARTITION_NUMBER; i++)
            {
                flash_partition_fw_version_get(i, &fw_ver_str[i]);
            }

            if (fw_ver_str[0].fw_build_num == fw_ver_str[1].fw_build_num)
            {
                /*
                ** active image build number is equal to redundant take no action and exit
                */
                bc_printf("active image build number is equal to redundant\n");
                return;
            }
        }
        else
        {
            auth_info_struct.failed_authentication[auth_info_struct.red_image_index] = 1;
        }

        bc_printf("Updating Redundant Image %s with Active Image %s\n",
                  ((SPI_FLASH_ACTIVE_IMG_A == active_image_flag) ? "B" : "A"),
                  ((SPI_FLASH_ACTIVE_IMG_A == active_image_flag) ? "A" : "B"));

        /* erase the redundant firmware image */
        bc_printf("Erasing Redundant Image ... ");
        rc = spi_flash_plat_erase(spi_flash_red_img_ptr, red_partition_num_bytes);

        /* Check for uncorrectable ECC errors */
        UECC_CHECK_REPORT(uecc_detected, 
                          auth_info_struct.uecc_detected[auth_info_struct.red_image_index], 
                          "Uncorrectable ECC detected ... ");

        if (PMC_SUCCESS != rc)
        {
            bc_printf("failed, rc = 0x%08X\n", rc);
            return;
        }
        bc_printf("done\n");

        /* program active firmware image over redundant firmware image */
        bc_printf("Updating Redundant Image ... ");
        rc = spi_flash_plat_flash_copy(spi_flash_act_img_ptr, spi_flash_red_img_ptr, act_img_num_bytes);

        /* Check for uncorrectable ECC errors */
        UECC_CHECK_REPORT(uecc_detected, 
                          auth_info_struct.uecc_detected[auth_info_struct.red_image_index], 
                          "Uncorrectable ECC detected ... ");

        if (PMC_SUCCESS != rc)
        {
            bc_printf("failed, rc = 0x%08X\n", rc);
            return;
        }
        bc_printf("done\n");
    }
    else if (SUCCESS == handoff_data_ptr->image_list[FLASH_REDUNDANT_IMAGE_INDEX].status)
    {
        /* 
        ** the redundant firmware image was authenticated and is the executing 
        ** image
        */
        bc_printf("Redundant Image has been booted..\n");

        bc_printf("Updating Active Image %s with Redundant Image %s\n",
                  ((SPI_FLASH_ACTIVE_IMG_A == active_image_flag) ? "A" : "B"),
                  ((SPI_FLASH_ACTIVE_IMG_A == active_image_flag) ? "B" : "A"));

        /* erase the active firmware image */
        bc_printf("Erasing Active Image ... ");
        spi_flash_act_img_ptr = handoff_data_ptr->image_list[FLASH_ACTIVE_IMAGE_INDEX].image_addr;

        if ('A' == handoff_data_ptr->image_list[FLASH_ACTIVE_IMAGE_INDEX].image_id)
        {
            /* Active image is A, record required size of partition */
            act_partition_num_bytes = SPI_FLASH_FW_IMG_A_HDR_SIZE + SPI_FLASH_FW_IMG_A_SIZE;
            spi_flash_red_img_ptr = (UINT8*)SPI_FLASH_FW_IMG_B_HDR_ADDR;
            red_img_num_bytes = fam_image_length_get(handoff_data_ptr->image_list, FLASH_REDUNDANT_IMAGE_INDEX) + SPI_FLASH_FW_IMG_B_HDR_SIZE;

            auth_info_struct.active_image_index = 0;
            auth_info_struct.red_image_index = 1;
        }
        else
        {
            /* Active image is B, record required size of partition */
            act_partition_num_bytes = SPI_FLASH_FW_IMG_B_HDR_SIZE + SPI_FLASH_FW_IMG_B_SIZE;
            spi_flash_red_img_ptr = (UINT8*)SPI_FLASH_FW_IMG_A_HDR_ADDR;
            red_img_num_bytes = fam_image_length_get(handoff_data_ptr->image_list, FLASH_REDUNDANT_IMAGE_INDEX) + SPI_FLASH_FW_IMG_A_HDR_SIZE;

            auth_info_struct.active_image_index = 1;
            auth_info_struct.red_image_index = 0;
        }

        auth_info_struct.failed_authentication[auth_info_struct.active_image_index] = 1;
        
        rc = spi_flash_plat_erase(spi_flash_act_img_ptr, act_partition_num_bytes);

        /* Check for uncorrectable ECC errors */
        UECC_CHECK_REPORT(uecc_detected, 
                          auth_info_struct.uecc_detected[auth_info_struct.active_image_index], 
                          "Uncorrectable ECC detected ... ");

        if (PMC_SUCCESS != rc)
        {
            bc_printf("failed, rc = 0x%08X\n", rc);
            return;
        }
        bc_printf("done\n");

        /* program redundant firmware image over active firmware image */
        bc_printf("Updating Active Image ... ");
        rc = spi_flash_plat_flash_copy(spi_flash_red_img_ptr, spi_flash_act_img_ptr, red_img_num_bytes);

        /* Check for uncorrectable ECC errors */
        UECC_CHECK_REPORT(uecc_detected, 
                          auth_info_struct.uecc_detected[auth_info_struct.active_image_index], 
                          "Uncorrectable ECC detected ... ");

        if (PMC_SUCCESS != rc)
        {
            bc_printf("failed, rc = 0x%08X\n", rc);
            return;
        }
        act_img_num_bytes = red_img_num_bytes;
        bc_printf("done\n");
    }

    /* Compare image A and B memory to verify the copy */
    bc_printf("Verifying Updated Image ... ");
    rc = memcmp((VOID*)SPI_FLASH_FW_IMG_A_HDR_ADDR,
                (VOID*)SPI_FLASH_FW_IMG_B_HDR_ADDR,
                act_img_num_bytes);

    /* Check for uncorrectable ECC errors */
    UECC_CHECK_REPORT(uecc_detected, 
                      auth_info_struct.uecc_compare, 
                      "Uncorrectable ECC detected ... ");

    if ((PMC_SUCCESS != rc) | uecc_detected)
    {
        bc_printf("failed, rc = 0x%08X uecc = %d\n", rc, uecc_detected);
        return;
    }
    bc_printf("done\n");

    auth_info_struct.image_updated = 1;

} /* spi_flash_plat_red_fw_image_update */

/**
* @brief
*   Retrieve status of SPI flash authentication
*        
* @param [out] spi_flash_plat_auth_info - Image authentication 
*        information
*  
* @return
*   Nothing
* 
*/
PUBLIC VOID spi_flash_plat_image_info_get(spi_flash_plat_auth_info_struct * spi_flash_plat_auth_info)
{
    *spi_flash_plat_auth_info = auth_info_struct;
}


