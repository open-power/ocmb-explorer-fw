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
*   DESCRIPTION: This file implements adapter_info handler,
*	variables and functions
*
*   NOTES:       None.
*
******************************************************************************/




/**
* @addtogroup ADAPTER_INFO
* @{
* @file
* @brief
*   This file contains the adapter info handler.
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
#include "spi_flash_plat.h"
#include <string.h>


/*
** Global Variables
*/

/*
** Local Structures and Unions
*/

/** 
*  @brief 
*   FW Adapter Properties structure
*/

typedef struct
{
    UINT32 fw_number_of_images;                 /**<  Number of FW images                   */
    UINT32 boot_partion_id;                     /**<  ID of current boot patition           */
    fw_version_info_struct fw_ver_str[SPI_FLASH_PARTITION_NUMBER];
                                                /**<  FW Version string                     */
    UINT32 ram_size_in_bytes;                   /**<  On-CHIP RAM size in Bytes             */
    UINT32 chip_version;                        /**<  Explorer chip revision                */
    UINT32 spi_flash_id;                        /**<  SPI flash ID                          */    
    UINT32 spi_flash_sector_size;               /**<  Flash sector size in Bytes            */
    UINT32 spi_flash_size;                      /**<  SPI flash size in Bytes               */
    UINT32 error_buffer_size;                   /**<  FW error buffer size in Bytes         */
    spi_flash_plat_auth_info_struct spi_flash_plat_auth_info;
                                                /**<  SPI flash image authentication info   */
}fw_adapter_properties_struct;


/*
** Forward Reference
*/
PRIVATE VOID adapter_info_properties_handler_get(VOID);


/*
** Private Data
*/

/*
** Private Functions
*/

/**
* @brief
*   Adapter properties get command handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID adapter_info_properties_handler_get(VOID)
{
    int i;
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    fw_adapter_properties_struct * adapt = (fw_adapter_properties_struct *)ech_ext_data_ptr_get();    

    /*Fill out the response*/
    adapt->fw_number_of_images = SPI_FLASH_PARTITION_NUMBER;
    
    adapt->boot_partion_id = flash_partition_boot_partition_id_get(); 

    for(i=0; i < SPI_FLASH_PARTITION_NUMBER; i++)
    {
        flash_partition_fw_version_get(i, &adapt->fw_ver_str[i]);
    }
                                              
    adapt->ram_size_in_bytes = app_fw_plat_ram_size_get();                              
    adapt->chip_version = top_device_rev_absolute();
    app_fw_plat_flash_info_get(&adapt->spi_flash_id, &adapt->spi_flash_sector_size, &adapt->spi_flash_size);

    adapt->error_buffer_size = EXP_FW_LOG_SIZE_4KB;

    /* Fill out SPI authentication information */
    spi_flash_plat_image_info_get(&adapt->spi_flash_plat_auth_info);

    /* set the success indication */
    rsp_ptr->parms[0] = PMC_SUCCESS;

    /* Set the extended error code */    
    rsp_ptr->parms[1] = PMC_SUCCESS;

    /* set the extended data response length */
    rsp_ptr->ext_data_len = sizeof(fw_adapter_properties_struct);

    /* set the no extended data flag */
    rsp_ptr->flags = EXP_FW_EXTENDED_DATA;

    /* Copy the result in Extended buffer*/
    
    /* send the response */
    ech_oc_rsp_proc();

} /* adapter_info_properties_handler_get */

/*
** Public Functions
*/

/**
* @brief
*   Adapter info module initialization
*
* @param
*   None
* @return
*   None.
*
*/
PUBLIC void adapter_info_module_init(VOID)
{
    /* Register the adapter info command handler with ECH module*/
    ech_api_func_register(EXP_FW_ADAPTER_PROPERTIES_GET, adapter_info_properties_handler_get);
}

/** @} end group */



