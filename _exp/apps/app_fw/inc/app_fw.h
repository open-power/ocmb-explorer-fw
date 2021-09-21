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
* @addtogroup APP_FW
* @{
* @file
* @brief
*   This file contains the platform defines for application FW.
*
*/ 


#ifndef _APP_FW_H
#define _APP_FW_H

/*
** Include Files
*/
#include "exp_api.h"
#include "ddr_api.h"
#include "log_plat_cfg.h"
#include "pmcfw_common.h"


/*
** Enumerated Types 
*/


/*
** Constants 
*/

/*
** Flag to disable restoring DDR parameters from SPI flash. 
** The code currently stores DDR training results in SPI flash for debug support.
*/
#define APP_FW_DISABLE_DDR_SPI_RELOAD 1

/* UART channel for output */
#define APP_FW_UART_ID              0
#define APP_FW_UART_BASE_ADDRESS    UART0_BASEADDR

/* Tiny Shell configuration */
#define APP_FW_TSH_SHELL_IDX        0        

/*
** The following defines the layout for the CFA_DATA_FW_LOG_A and
** CFA_DATA_FW_LOG_B sections in the SPI flash partition map defined
** in spi_flash_plat.h.
**
** NOTE: Sections must be 4K aligned.
*/
#define APP_FW_LOG_OFFSET              0

/*
** To use string logging, modify free_mem_reserve in
** app_fw.ld and add (4 * 1024) = 0x1000.
*/
#define APP_FW_DDR_TRAINING_DATA_SIZE   (4 * 1024)
#define APP_FW_LOG_SIZE                 EXP_FW_LOG_SIZE_4KB

#define APP_FW_DDR_TRAINING_DATA_OFFSET 0
#define APP_FW_LOG_DATA_OFFSET          APP_FW_DDR_TRAINING_DATA_SIZE

/* RAM Size on the board*/
#define APP_FW_PLAT_RAM_SIZE   (256*1024)

/* Current PBOOT Version */
#define PBOOT_MAJOR_VERSION_INFO		1
#define PBOOT_MINOR_VERSION_INFO		6

/* Error codes */
#define APP_FW_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_APPFW) | (err_suffix))
#define APP_FW_ERR_SPI_INIT_FAIL            APP_FW_ERR_CODE_CREATE(0x001)  /* SPI not initialized */
#define APP_FW_ERR_TLS_MEMMOVE_CALL         APP_FW_ERR_CODE_CREATE(0x002)  /* MEMMOVE call from TLS */
#define APP_FW_ERR_TLS_GHS_SYSCALL          APP_FW_ERR_CODE_CREATE(0x003)  /* GHS SYSCALL call from TLS */
#define APP_FW_ERR_FLASH_ID_GET             APP_FW_ERR_CODE_CREATE(0x004)  /* Error to get the flash ID */
#define APP_FW_ERR_DEV_INFO_GET             APP_FW_ERR_CODE_CREATE(0x005)  /* Error to get flash device info*/
#define APP_FW_ERR_FW_VERSION_LENGTH        APP_FW_ERR_CODE_CREATE(0x006)  /* Error on length*/
#define APP_FW_ERR_FLASH_BOUNDARY           APP_FW_ERR_CODE_CREATE(0x007)  /* Error: Address out of range*/
#define APP_FW_ERR_PBOOT_VERSION            APP_FW_ERR_CODE_CREATE(0x008)  /* Error: Incorrect PBOOT version */

/*
** Macro Definitions
*/

/*
** Structures and Unions
*/

/*
** Global variables
*/
EXTERN volatile BOOL app_fw_oc_ready;
/*
** Function Prototypes
*/
EXTERN UINT32 exp_plat_get_pic_offset(void);
EXTERN UINT32 app_fw_plat_ram_size_get(void);

EXTERN void app_fw_plat_flash_info_get(UINT32 *flash_id, UINT32 *flash_sector_size, 
                                          UINT32 *flash_dev_size);
EXTERN void app_fw_reset_init(void);
EXTERN void pmcfw_assert_function(PMCFW_ERROR error_id,
                                  CHAR        *file,
                                  UINT32      line);
EXTERN void app_fw_plat_di_enable(void);
EXTERN VOID app_fw_plat_hw_init_set(BOOL hw_init);
EXTERN BOOL app_fw_plat_hw_init_get(VOID);
EXTERN VOID app_fw_plat_di_enable_set(BOOL di_enable);
EXTERN BOOL app_fw_plat_di_enable_get(VOID);

#endif /* _APP_FW_H */
/** @} end group */


