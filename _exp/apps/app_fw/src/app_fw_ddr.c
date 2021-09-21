/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2021  Microchip Technology Inc. All rights reserved. 
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



/*
** Include Files
*/
#include <string.h>
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "target_platform.h"
#include "bc_printf.h"
#include "crc32.h"
#include "spi_flash_api.h"
#include "spi_flash_plat.h"
#include "exp_ddr_ctrlr_plat.h"
#include "ocmb_plat.h"
#include "ddr_api.h"
#include "app_fw.h"
#include "app_fw_ddr.h"
#include "ddr_exp_cmdsvr.h"
#include "exp_ddr_ctrlr_cmdsvr.h"
#include "fam.h"
#include "fw_version_info.h"
#include "flash_partition_info.h"
#include "spi_api.h"
#include "top.h"
#include "spi_plat.h"
#include "pmc_plat.h"
#include "top_plat.h"

/*
** Local Constants
*/

#define APP_FW_DDR_SAVED_DATA_HEADER 0xDD20DD20

/*
* Structures
*/

/*
** External References
*/

/*
** Global Variables
*/

PUBLIC app_fw_ddr_calibration_data_struct app_fw_ddr_saved_data;


/*
** Private Functions
*/

/**
* @brief
*   Get the address in SPI flash where the DDR PHY training
*   data should be stored
*
* @return
*   Address in SPI flash where the training is saved.
*
*/
PRIVATE UINT32 app_fw_ddr_training_data_addr_get(void)
{
    /* determine the active image to erase the correct stored log */
    if (flash_partition_boot_partition_id_get() == 'A')
    {
        /* firmware image A is the active image */

        /* Make sure the address for the training data is 4K aligned */
        PMCFW_ASSERT(((SPI_FLASH_FW_IMG_A_CFG_LOG_TRAINING_ADDR)& 0xFFF) == 0, APP_FW_DDR_ERR_DDR_TRAINING_ALIGN);

        /* get reference to image A SPI flash log*/
        return (UINT32)(SPI_FLASH_FW_IMG_A_CFG_LOG_TRAINING_ADDR);
    }
    else
    {
        /* firmware image B is the active image */

        /* Make sure the address for the training data is 4K aligned */
        PMCFW_ASSERT(((SPI_FLASH_FW_IMG_B_CFG_LOG_TRAINING_ADDR)& 0xFFF) == 0, APP_FW_DDR_ERR_DDR_TRAINING_ALIGN);

        /* get reference to image B SPI flash log*/
        return (UINT32)(SPI_FLASH_FW_IMG_B_CFG_LOG_TRAINING_ADDR);
    }
}

/**
* @brief
*   Initialize the DDR PHY for bringup.
*
* @return
*   PMC_SUCCESS if successful.
*
*/
PRIVATE PMCFW_ERROR app_fw_ddr_phy_bringup_init(void)
{
    PMCFW_ERROR rc;
    
    ddr_api_fw_phy_reset();

    ddr_api_init(&user_input_msdg_array[DDR_PHY_DEFAULT_USER_INPUT_MSDG]);

#if (APP_FW_DISABLE_DDR_SPI_RELOAD == 0)
    /* Try loading saved calibration data */
    if (PMC_SUCCESS == app_fw_ddr_calibration_load(&app_fw_ddr_saved_data))
    {
        bc_printf("Restoring saved DDR PHY training results\n");
        /* Restore calibration settings */
        rc = ddr_api_saved_margin_results_load(&app_fw_ddr_saved_data.timing_data,
                                               &app_fw_ddr_saved_data.vref_data);
    }
    else
#endif
    {
        bc_printf("Performing full DDR PHY training\n");

        /* Since restoring calibration settings failed perform full training */
        rc = ddr_api_fw_train();

        if (rc != PMC_SUCCESS)
        {
            bc_printf("app_fw_ddr_phy_bringup_init(): ddr_api_fw_train() failed rc = 0x%x\n", rc);
        }

        /* whether training passed or failed, save calibration results */
        ddr_api_cal_results_get(&app_fw_ddr_saved_data.timing_data,
                                &app_fw_ddr_saved_data.vref_data);

        /* save calibration results */
        if (PMC_SUCCESS != app_fw_ddr_calibration_save(&app_fw_ddr_saved_data))
        {
            bc_printf("app_fw_ddr_phy_bringup_init(): app_fw_ddr_calibration_save() failed\n");
        }
    }

    return rc;
}

/*
** Public Functions
*/

/**
* @brief
*   Save DDR PHY calibration results to SPI Flash.
*
* @param[in] ddr_training_data - Pointer to the calibration data with
*                                timing and vref data filled
*
* @return
*   PMC_SUCCESS if successful.
*
*/
PUBLIC PMCFW_ERROR app_fw_ddr_calibration_save(app_fw_ddr_calibration_data_struct *ddr_training_data)
{
    PMCFW_ERROR rc             = PMC_SUCCESS;
#if (EXPLORER_DDR_TRAIN_PARMS_SAVE_DISABLE == 0)
    UINT32      spi_flash_addr = app_fw_ddr_training_data_addr_get();
    spi_flash_dev_info_struct dev_info;
    spi_flash_dev_enum        dev;
    UINT8*                    subsector_base;
    UINT32                    subsector_len;
    top_plat_lock_struct lock_struct;

    /* Add header and CRC to training data structure */
    ddr_training_data->header = APP_FW_DDR_SAVED_DATA_HEADER;
    ddr_training_data->crc = pmc_crc32((UINT8*)&ddr_training_data,
                                       sizeof(app_fw_ddr_calibration_data_struct) - sizeof(UINT32),
                                       0, TRUE, TRUE);
    
    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);
    if (PMC_SUCCESS != rc)
    {
        return APP_FW_DDR_ERR_TRAINING_ERASE;
    }
    
    /* get the subsector address */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        (UINT8*)(spi_flash_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                                        &subsector_base,
                                        &subsector_len);
    if (PMC_SUCCESS != rc)
    {
        return APP_FW_DDR_ERR_TRAINING_ERASE;
    }
    
    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    /* erase subsector */
    rc = spi_flash_subsector_erase_wait(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        subsector_base,
                                        dev_info.max_time_subsector_erase);
    
    if (PMC_SUCCESS != rc)
    {
        rc = APP_FW_DDR_ERR_TRAINING_ERASE;
    }
    else
    {
        /* save DDR data in SPI flash */
        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   (UINT8*)ddr_training_data,
                                   (UINT8*)(spi_flash_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                                   sizeof(app_fw_ddr_calibration_data_struct),
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);
    
        if (PMC_SUCCESS != rc)
        {
            rc = APP_FW_DDR_ERR_TRAINING_WRITE;
        }
    }
    
    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);
#endif
    
    return rc;
}

/**
* @brief
*   Load DDR PHY calibration results from SPI Flash.
*
* @param[out] ddr_training_data - Pointer to the calibration data structure
*
* @return
*   PMC_SUCCESS if successful.
*
*/
PUBLIC PMCFW_ERROR app_fw_ddr_calibration_load(app_fw_ddr_calibration_data_struct *ddr_training_data)
{
    PMCFW_ERROR rc             = PMC_SUCCESS;
    UINT32      spi_flash_addr = app_fw_ddr_training_data_addr_get();

    memset(ddr_training_data, 0, sizeof(app_fw_ddr_calibration_data_struct));

#if (APP_FW_DISABLE_DDR_SPI_RELOAD == 0)

    /* disable interrupts and disable multi-VPE operation */
    top_plat_lock_struct lock_struct;
    top_plat_critical_region_enter(&lock_struct);

    /* Check for saved calibration results */
    rc = spi_flash_read(SPI_FLASH_PORT,
                        SPI_FLASH_CS,
                        (UINT8*)((UINT32)spi_flash_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                        (UINT8*)ddr_training_data,
                        sizeof(app_fw_ddr_calibration_data_struct));

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);    

    if (rc != PMC_SUCCESS)
    {
        bc_printf("Failed reading calibration from SPI flash rc = 0x%08X\n", rc);
        return rc;
    }

    UINT32 crc = pmc_crc32((UINT8*)ddr_training_data,
                           sizeof(app_fw_ddr_calibration_data_struct) - sizeof(UINT32),
                           0, TRUE, TRUE);

    if ((ddr_training_data->crc == crc) &&
        (ddr_training_data->header == APP_FW_DDR_SAVED_DATA_HEADER))
    {
        bc_printf("Calibration data CRC check failed\n");
        return APP_FW_DDR_ERR_CALIBRATION_CRC;
    }
#else
    rc = PMCFW_ERR_FAIL;
#endif
    return rc;
}

/**
* @brief
*   Read a section of DDR PHY calibration results from SPI
*   Flash.
* @param[in] offset - The offset in the ddr calibration data
*       structure to start reading from
* @param[in] size - The number of bytes to read
* @param[out] rx_data_ptr - Pointer to the calibration data
*       structure
* @param[out] size_read - The number of bytes that was read.
*       This will be less than size when the end of the
*       structure was reached
*
* @return
*   PMC_SUCCESS if successful.
*
*/
PUBLIC PMCFW_ERROR app_fw_ddr_calibration_read(UINT32 offset, UINT32 size, VOID * rx_data_ptr, UINT32 * size_read)
{

    UINT32 rc;

    UINT32 struct_size = sizeof(app_fw_ddr_calibration_data_struct);
    
    UINT32 spi_flash_addr = app_fw_ddr_training_data_addr_get();    
        
    /* Calculate the size of data that can be read */
    if (offset + size > struct_size) 
    {
        if (offset < struct_size) 
        {
            *size_read = struct_size - offset;
        }
        else
        {
            *size_read = 0;
        }
    }
    else
    {
        *size_read = size;
    }

    /* disable interrupts and disable multi-VPE operation */
    top_plat_lock_struct lock_struct;
    top_plat_critical_region_enter(&lock_struct);

    /* Copy the data from the calibration structure */
    rc = spi_flash_read(SPI_FLASH_PORT,
                        SPI_FLASH_CS,
                        (UINT8*)((UINT32)(spi_flash_addr + offset) & GPBC_FLASH_PHYS_ADDR_MASK),
                        (UINT8*)rx_data_ptr,
                        *size_read);

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);  

    return rc;
}

/**
* @brief
*   Register all the DDR related command server commands
*
* @return
*   None.
*
*/
PUBLIC VOID app_fw_ddr_cmdsvr_init(void)
{
    /* Register DDR_EXP_CMDSVR */
    ddr_exp_cmdsvr_register();

    /* Register the OCMB command server. */
    exp_ddr_ctrlr_cmdsvr_register();
}


/**
* @brief
*   Initialize DDR for bring up
*
* @return
*   PMC_SUCCESS if successful.
*
* @note
*   This initialization function is only used for testing during bringup
*   without a host
*/
PUBLIC PMCFW_ERROR app_fw_ddr_bringup_init(void)
{
    bc_printf("Initializing DDR for bringup ...\n");

    /* Initialize DDR Controller */
    exp_ddr_ctrlr_init();

    /* Initialize DDR PHY */
    app_fw_ddr_phy_bringup_init();

    bc_printf("Done DDR initialization!\n");

    return PMC_SUCCESS;
}




