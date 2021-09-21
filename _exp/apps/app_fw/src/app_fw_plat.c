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
*   DESCRIPTION : This file contains platform specific APIs needed by
*                 various other modules. For example, exp_plat_ram_size_get
*                 will return on-chip RAM size in bytes.
*
*
*   NOTES:        None.
*
*
*******************************************************************************/



/*
** Include Files
*/

#include "cpuhal.h"
#include "spi_api.h"
#include "spi_flash.h"
#include "spi_flash_plat.h"
#include "app_fw.h"
#include "reset.h"
#include "top_exp_config_guide.h"
#include "serdes_plat.h"
#include "crash_dump.h"
#include "top.h"
#include "ccb_plat.h"

/*
* Local Constants
*/

#define APP_FW_RESET_INFO_CRASH_SIZE 0x10000
#define APP_FW_RUNTIME_CCB_CRASH_SIZE 0x10000

/*
** Private Variables
*/
PRIVATE BOOL app_fw_hw_initialized = FALSE;
PRIVATE BOOL app_fw_di_enabled = FALSE;

/*
* Public Variables
*/

/*
* Private Functions
*/

/**
* @brief
*  Dump runtime CCB to crash dump
*
* @return
*   None.
*
*/
/*
PRIVATE void app_fw_runtime_ccb_dump(void)
{
    UINT32 size;
    void *ccb_ptr;

    size = char_io_loc_buffer_info_get(CHAR_IO_CHANNEL_ID_RUNTIME,
                                       &ccb_ptr);

    crash_dump_put(size, ccb_ptr);
}
*/
/**
* @brief
*  Dump crash CCB to crash dump
*
* @return
*   None.
*
*/
/*
PRIVATE void app_fw_crash_ccb_dump(void)
{
    UINT32 size;
    void *ccb_ptr;

    size = char_io_loc_buffer_info_get(CHAR_IO_CHANNEL_ID_CRASH,
                                       &ccb_ptr);

    crash_dump_put(size, ccb_ptr);
}
*/
/*
* Public Functions
*/

/**
* @brief
*  Returns PIC offset for the platform
*  
*
* @return
*   TRUE PIC offset
*
* @note
*   None.
*
*/
PUBLIC UINT32 exp_plat_get_pic_offset(void)
{
    /* 
    ** PIC base is calculated in cpu_hal_asm.mip
    ** under exc_reset. The PIC base is stored in 
    ** TC Context register and is retrived from this 
    ** function.
    */
    return hal_coprocessor_read(2,5);
}

/**
* @brief
*  Returns RAM size
*  
*
* @return
*   RAM size in bytes
*
* @note
*   None.
*
*/
PUBLIC UINT32 app_fw_plat_ram_size_get(void)
{
    return APP_FW_PLAT_RAM_SIZE;
}

/**
* @brief
*  Dummy memmove code to satisfy TLS library
*  This is never expected to be executed.
*
* @return
*   None
*
* @note
*   None.
*
*/
PUBLIC void * memmove ( void * destination, const void * source, size_t num )
{
    PMCFW_ASSERT(0, APP_FW_ERR_TLS_MEMMOVE_CALL);
    /*Dummy return code*/
    return NULL;
}

/**
* @brief
*  Dummy __ghs_syscall code to satisfy GHS library
*  This is never expected to be executed.
*
* @return
*   None
*
* @note
*   None.
*/

PUBLIC void __ghs_syscall(void)
{
    PMCFW_ASSERT(0, APP_FW_ERR_TLS_GHS_SYSCALL);
}

/**
* @brief
*   Perform read operation from flash address
*  
* @param [out] flash_id - Flash ID of the chip 
* @param [out] flash_sector_size - flash sector size in Bytes
* @param [out] flash_dev_size -    flash device size in Bytes
* 
* @return
*   None
*
*/
PUBLIC void app_fw_plat_flash_info_get(UINT32 *flash_id, UINT32 *flash_sector_size, UINT32 *flash_dev_size)
{
    UINT8 manuf_id;
    UINT16 dev_id;
    PMCFW_ERROR rc;    
    spi_flash_dev_enum dev;
    
    /* SPI flash device info */
    spi_flash_dev_info_struct spi_dev_info;
    
    rc = spi_flash_vendor_ids_get(SPI_FLASH_PORT, SPI_FLASH_CS, &manuf_id, &dev_id);    
    PMCFW_ASSERT(rc == PMC_SUCCESS, APP_FW_ERR_FLASH_ID_GET);
    *flash_id = manuf_id << sizeof(dev_id) | dev_id;

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &spi_dev_info);
    PMCFW_ASSERT(rc == PMC_SUCCESS, APP_FW_ERR_DEV_INFO_GET);


    *flash_sector_size = spi_dev_info.subsectors_per_sector * spi_dev_info.pages_per_subsector *
                         spi_dev_info.page_size;
    *flash_dev_size = *flash_sector_size * spi_dev_info.sectors;  

}

/**
* @brief
*   This function initializes the reset module.
*
* @return
*   None
*
* @note
*   None.
*
*/
PUBLIC void app_fw_reset_init()
{
    reset_parms_struct reset_parms;


    reset_parms.num_of_vpe = PMC_MAX_VPE_PER_CPU;
    reset_parms.num_of_tc =  PMC_MAX_TC_PER_CPU;

    reset_parms.reset_on_fatal_error = FALSE;

    /* No CLI after crash */
    reset_parms.fatal_cli_cback_ptr = NULL;

    reset_init(&reset_parms);

    crash_dump_register("RESET_INFO", &reset_info_print, CRASH_DUMP_ASCII, APP_FW_RESET_INFO_CRASH_SIZE);
    crash_dump_register("CCB", &ccb_plat_runtime_crash_dump, CRASH_DUMP_ASCII, APP_FW_RUNTIME_CCB_CRASH_SIZE);
}



/**
* @brief
*   This function performs top-level DI enablement.
*
* @return
*   None
*
* @note
*   None.
*
*/
PUBLIC void app_fw_plat_di_enable(void)
{
    /*Call top level function to enable DI */
    top_di_enable();
}

/**
* @brief
*   This function sets the hardware initialization flag.
*
* @param [in] hw_init - hardware intitialization setting
*  
* @return
*   None
*
* @note
*   None.
*
*/
PUBLIC VOID app_fw_plat_hw_init_set(BOOL hw_init)
{
    app_fw_hw_initialized = hw_init;
}

/**
* @brief
*   This function returns the hardware initialization flag.
*
* @return
*   hardware initialization complete flag TRUE/FALSE
*
* @note
*   None.
*
*/
PUBLIC BOOL app_fw_plat_hw_init_get(VOID)
{
    return (app_fw_hw_initialized);
}

/** 
* @brief
*   This function sets the DI initialization flag.
*
* @param [in] di_enable - DI enable setting
*  
* @return
*   None
*
* @note
*   None.
*
*/
PUBLIC VOID app_fw_plat_di_enable_set(BOOL di_enable)
{
    app_fw_di_enabled = di_enable;
}

/**
* @brief
*   This function returns the DI initialization flag.
*
* @return
*   DI enable status TRUE/FALSE
*
* @note
*   None.
*
*/
PUBLIC BOOL app_fw_plat_di_enable_get(VOID)
{
    return (app_fw_di_enabled);
}


