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
*  DESCRIPTION :
*    This file contains definitions and declarations for the SPI flash module
*    for use by external applications.
*
*  NOTES :
*    The caller is responsible for all locking. Locks should be per-port.
*    Accesses to devices on different SPI ports may occur concurrently, but
*    accesses to devices on different chip selects of the same port must be
*    protected.
*
*    Some functions (e.g. flash erase and flash write) have a function to
*    trigger the operation and a function to check for completion that should be
*    polled. No accesses to other devices on the same port should be made
*    between these two function calls.
*
*******************************************************************************/


#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

/*
** Include Files
*/

#include "spi_flash_api.h"

/*
** Constants
*/

/* SPI FLASH device manufacturer and device IDs */
#define SPI_FLASH_MT25QU128ABA_VENDOR_ID          (0x20)
#define SPI_FLASH_MT25QU128ABA_DEVICE_ID          (0xBB18)
#define SPI_FLASH_MT25QU256ABA_VENDOR_ID          (0x20)
#define SPI_FLASH_MT25QU256ABA_DEVICE_ID          (0xBB19)
#define SPI_FLASH_MT25QU01GBBB_VENDOR_ID          (0x20)
#define SPI_FLASH_MT25QU01GBBB_DEVICE_ID          (0xBB21)
#define SPI_FLASH_MX25U25645G_VENDOR_ID           (0xC2)
#define SPI_FLASH_MX25U25645G_DEVICE_ID           (0x2539)
#define SPI_FLASH_MX25U12835F_VENDOR_ID           (0xC2)
#define SPI_FLASH_MX25U12835F_DEVICE_ID           (0x2538)
#define SPI_FLASH_W25Q256JW_VENDOR_ID             (0xEF)
#define SPI_FLASH_W25Q256JW_DEVICE_ID             (0x8019)
#define SPI_FLASH_W25Q256JW_DEVICE_ID_NON_INDUSTRIAL (0x6019)
#define SPI_FLASH_S25FS256S_VENDOR_ID             (0x01)
#define SPI_FLASH_S25FS256S_DEVICE_ID             (0x0219)

#endif /* _SPI_FLASH_H */



