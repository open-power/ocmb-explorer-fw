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
*     Platform-specific definitions and declarations for SPI.
*
*******************************************************************************/


#ifndef _SPI_FLASH_PLAT_H
#define _SPI_FLASH_PLAT_H

/*
** Include Files
*/
#include "pmc_hw_base.h"


/*
** Local Constants
*/

/* SPI flash config */
#define SPI_FLASH_PORT                       0
#define SPI_FLASH_CS                         0
#define SPI_FLASH_DEV                        SPI_FLASH_DEV_AUTO

/* 
** SPI Flash memory addresses
** based on flash sector partition information in Explorer FW architecture doc
** http://bby1dms01.pmc-sierra.internal/DocMgmt/getfile.cfm?file_id=436785
*/
#define SPI_FLASH_SIZE                             (8 * 1024 * 1024)           /* Minimum supported SPI flash size is 8MB */
#define SPI_FLASH_SIZE_ECC_MAX                     (SPI_FLASH_SIZE * 24) / 32  /* To account for ECC the real usable max size is less than 8MB */
#define SPI_FLASH_BASE_ADDRESS                     GPBC_SPI_FLASH_UNCACHE_BASE_ADD
#define SPI_FLASH_FW_PBOOT_ADDR                    SPI_FLASH_BASE_ADDRESS
#define SPI_FLASH_FW_PBOOT_SIZE                    (256 * 1024)
#define SPI_FLASH_FW_ACT_IMG_FLAG_ADDR             (SPI_FLASH_FW_PBOOT_ADDR + SPI_FLASH_FW_PBOOT_SIZE)
#define SPI_FLASH_FW_ACT_IMG_FLAG_SIZE             (2 * 1024)
#define SPI_FLASH_FW_IMG_A_HDR_ADDR                (SPI_FLASH_FW_ACT_IMG_FLAG_ADDR + SPI_FLASH_FW_ACT_IMG_FLAG_SIZE)
#define SPI_FLASH_FW_IMG_A_HDR_SIZE                (2 * 1024)
#define SPI_FLASH_FW_IMG_A_ADDR                    (SPI_FLASH_FW_IMG_A_HDR_ADDR + SPI_FLASH_FW_IMG_A_HDR_SIZE)
#define SPI_FLASH_FW_IMG_A_SIZE                    (1024 * 1024)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_ADDR            (SPI_FLASH_FW_IMG_A_ADDR + SPI_FLASH_FW_IMG_A_SIZE)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_SIZE            (128 * 1024)

/* The following are sub sections in SPI_FLASH_FW_IMG_A_CFG_LOG */
#define SPI_FLASH_FW_IMG_A_CFG_LOG_APP_FW_ADDR     SPI_FLASH_FW_IMG_A_CFG_LOG_ADDR
#define SPI_FLASH_FW_IMG_A_CFG_LOG_APP_FW_SIZE     (16 * 1024)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_RESERVED0_ADDR  (SPI_FLASH_FW_IMG_A_CFG_LOG_APP_FW_ADDR + SPI_FLASH_FW_IMG_A_CFG_LOG_APP_FW_SIZE)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_RESERVED0_SIZE  (16 * 1024)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_TRAINING_ADDR   (SPI_FLASH_FW_IMG_A_CFG_LOG_RESERVED0_ADDR + SPI_FLASH_FW_IMG_A_CFG_LOG_RESERVED0_SIZE)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_TRAINING_SIZE   (4 * 1024)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_RESERVED1_ADDR  (SPI_FLASH_FW_IMG_A_CFG_LOG_TRAINING_ADDR + SPI_FLASH_FW_IMG_A_CFG_LOG_TRAINING_SIZE)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_RESERVED1_SIZE  (92 * 1024)

#define SPI_FLASH_FW_RESERVED_ADDR                 (SPI_FLASH_FW_IMG_A_CFG_LOG_ADDR + SPI_FLASH_FW_IMG_A_CFG_LOG_SIZE)
#define SPI_FLASH_FW_RESERVED_SIZE                 (2 * 1024)
#define SPI_FLASH_FW_IMG_B_HDR_ADDR                (SPI_FLASH_FW_RESERVED_ADDR + SPI_FLASH_FW_RESERVED_SIZE)
#define SPI_FLASH_FW_IMG_B_HDR_SIZE                (2 * 1024)
#define SPI_FLASH_FW_IMG_B_ADDR                    (SPI_FLASH_FW_IMG_B_HDR_ADDR + SPI_FLASH_FW_IMG_B_HDR_SIZE)
#define SPI_FLASH_FW_IMG_B_SIZE                    (1024 * 1024)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_ADDR            (SPI_FLASH_FW_IMG_B_ADDR + SPI_FLASH_FW_IMG_B_SIZE)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_SIZE            (128 * 1024)

/* The following are sub sections in SPI_FLASH_FW_IMG_B_CFG_LOG */
#define SPI_FLASH_FW_IMG_B_CFG_LOG_APP_FW_ADDR     SPI_FLASH_FW_IMG_B_CFG_LOG_ADDR
#define SPI_FLASH_FW_IMG_B_CFG_LOG_APP_FW_SIZE     (16 * 1024)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_RESERVED0_ADDR  (SPI_FLASH_FW_IMG_B_CFG_LOG_APP_FW_ADDR + SPI_FLASH_FW_IMG_B_CFG_LOG_APP_FW_SIZE)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_RESERVED0_SIZE  (16 * 1024)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_TRAINING_ADDR   (SPI_FLASH_FW_IMG_B_CFG_LOG_RESERVED0_ADDR + SPI_FLASH_FW_IMG_B_CFG_LOG_RESERVED0_SIZE)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_TRAINING_SIZE   (4 * 1024)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_RESERVED1_ADDR  (SPI_FLASH_FW_IMG_B_CFG_LOG_TRAINING_ADDR + SPI_FLASH_FW_IMG_B_CFG_LOG_TRAINING_SIZE)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_RESERVED1_SIZE  (92 * 1024)

#define SPI_FLASH_FW_FW_UPGRADE_ADDR               (SPI_FLASH_FW_IMG_B_CFG_LOG_ADDR + SPI_FLASH_FW_IMG_B_CFG_LOG_SIZE)
#define SPI_FLASH_FW_FW_UPGRADE_SIZE               (1024 * 1024)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_ADDR (SPI_FLASH_FW_FW_UPGRADE_ADDR + SPI_FLASH_FW_FW_UPGRADE_SIZE)
#define SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_SIZE (1024 * 1024)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_ADDR (SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_ADDR + SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_SIZE)
#define SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_SIZE (1024 * 1024)
#define SPI_FLASH_UNUSED_ADDR                      (SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_ADDR + SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_SIZE)
#define SPI_FLASH_UNUSED_SIZE                      (504 * 1024)
#define SPI_FLASH_FW_END_ADDR                      (SPI_FLASH_UNUSED_ADDR + SPI_FLASH_UNUSED_SIZE)

/*
** Compile assert if too much SPI was allocated
*/
#if (SPI_FLASH_FW_END_ADDR > (SPI_FLASH_BASE_ADDRESS + SPI_FLASH_SIZE_ECC_MAX))
#error "Insufficient SPI flash space"
#endif

/* active firmware image mask and flags */
#define SPI_FLASH_ACTIVE_IMG_MASK           0x00000001 
#define SPI_FLASH_ACTIVE_IMG_A              0
#define SPI_FLASH_ACTIVE_IMG_B              1

/* Number of Flash Partition */
#define SPI_FLASH_PARTITION_NUMBER          2


/*
** Function Prototypes
*/
EXTERN VOID spi_flash_plat_red_fw_image_update(VOID);

#endif /* _SPI_FLASH_PLAT_H */



