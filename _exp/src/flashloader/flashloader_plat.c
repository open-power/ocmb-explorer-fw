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
*   DESCRIPTION: This file implements platform specific pboot global
*	variables and functions
*
*   NOTES:       None.
*
******************************************************************************/




/**
* @addtogroup PBOOT
* @{
* @file
* @brief
*   This file contains the pboot application.
*
*/ 


/*
** Include Files
*/
#include "pmc_profile.h"
#include "pmcfw_common.h"
#include "top.h"
#include "bc_printf.h"
#include "flashloader.h"
#include "ech.h"
#include "fam.h"
#include "fw_version_info.h"
#include "flashloader_plat.h"
#include "flash_partition_info.h"
#include "spi_flash_api.h"
#include "spi_api.h"
#include "spi_plat.h"
#include "string.h"
#include "tmr_sys_plat.h"
#include "sys_timer_api.h"
#include "cpuhal.h"
#include "pmc_plat.h"
#include "wdt.h"
#include "top_plat.h"

/*
** Macro Constants
*/

/* Flash Read Command offsets */
#define FLASH_LOADER_FLASH_READ_CMD_PARTITION_ID_OFFSET     1
#define FLASH_LOADER_FLASH_READ_CMD_PACKET_SEQUENCE_OFFSET  2
#define FLASH_LOADER_FLASH_READ_CMD_ADDRESS_OFFSET          4
#define FLASH_LOADER_FLASH_READ_CMD_PACKET_LENGTH_OFFSET    8

/* Flash Write Command offset*/
#define FLASH_LOADER_FLASH_WRITE_IMAGE_SIZE_OFFSET         1
#define FLASH_LOADER_FLASH_WRITE_PACKET_SEQUENCE_OFFSET     4

#define FLASH_LOADER_SUBSECTOR_SIZE                         (4*1024)



fam_image_desc_struct img_list[SPI_FLASH_PARTITION_NUMBER];
/* This is for interim testing, eventually will be moved to FW partion*/
PRIVATE UCHAR FW_VERSION_INFO[FLASH_PLAT_VERSION_LENGTH] = "MICROCHIP Explorer FW Version 1.0 Build date: 12/01/2019";
/* SPI flash device info */
PRIVATE spi_flash_dev_info_struct flashloader_plat_spi_dev_info;

/** Global variables
*/
/* Public keys were supplied by the Smart Array team. */
PRIVATE const UINT8 pub_key_modulus[FLASH_LOADER_PLAT_NUM_PUBLIC_KEYS][FLASH_LOADER_PLAT_PUB_KEY_LENGTH_BYTES] =
{
    /* a1 */
    {0x57, 0x30, 0x6b, 0x25, 0x23, 0x7d, 0x63, 0x35, 0x03, 0xfc, 0xe5, 0x4f,
     0x8d, 0x11, 0xf6, 0xff, 0x92, 0x51, 0x70, 0x55, 0x9c, 0xcc, 0xc1, 0x71,
     0x8a, 0x46, 0xc8, 0xa5, 0x1c, 0x4f, 0xb9, 0x98, 0x23, 0xb7, 0xaf, 0x09,
     0x8b, 0xde, 0xb3, 0xb5, 0x70, 0xad, 0x21, 0x9b, 0x5b, 0x60, 0xaf, 0xa5,
     0xe9, 0xce, 0xde, 0xb2, 0x16, 0x70, 0xe6, 0x4f, 0xa2, 0x83, 0x06, 0x4b,
     0x0b, 0x76, 0xbd, 0x28, 0xea, 0x22, 0x8d, 0x5e, 0x71, 0xcb, 0xb5, 0x37,
     0x2d, 0x6c, 0xa0, 0x80, 0x51, 0x13, 0x7f, 0xf5, 0x51, 0xc9, 0xfc, 0x3f,
     0x73, 0xb1, 0xd8, 0x2f, 0xf0, 0x70, 0xe4, 0xab, 0x31, 0x1b, 0xf4, 0x78,
     0xc1, 0x82, 0xf8, 0xe5, 0xe9, 0x00, 0x8f, 0x83, 0xea, 0xd2, 0x1f, 0xba,
     0x41, 0x68, 0xec, 0x1b, 0xd9, 0x88, 0x3f, 0x6b, 0x4f, 0xc4, 0xfa, 0xbf,
     0xeb, 0x8b, 0x58, 0x49, 0x6c, 0x61, 0x9f, 0x43, 0x7b, 0xec, 0xaa, 0x46,
     0x23, 0xde, 0xf8, 0xbf, 0x37, 0xc1, 0x4d, 0xa6, 0xd1, 0x34, 0x31, 0xf8,
     0x00, 0x06, 0x9b, 0xb8, 0xc6, 0x32, 0x63, 0x6d, 0x32, 0x1c, 0x91, 0xb4,
     0xbb, 0x23, 0x5b, 0x10, 0xe9, 0xb7, 0x19, 0x3c, 0xa8, 0xec, 0x5d, 0x25,
     0x8e, 0xa5, 0x56, 0xcb, 0x36, 0x14, 0xc9, 0x3c, 0xbd, 0xe4, 0x83, 0xef,
     0x3f, 0xa9, 0x66, 0xe8, 0x21, 0xab, 0xbc, 0x5c, 0xfd, 0xdf, 0x89, 0x1c,
     0xfa, 0x11, 0x27, 0x48, 0x34, 0x58, 0xe7, 0x7a, 0xd2, 0xa4, 0xe5, 0x13,
     0x20, 0xfc, 0x06, 0xf1, 0x8d, 0xc6, 0x96, 0xcf, 0x13, 0xa8, 0x58, 0xa2,
     0x1e, 0xf7, 0x52, 0x83, 0x16, 0x31, 0x90, 0xa8, 0xa1, 0xe5, 0x2f, 0x2a,
     0x2a, 0x11, 0x43, 0x9d, 0x86, 0x64, 0xad, 0xe9, 0x70, 0xa5, 0x2a, 0xab,
     0x22, 0x17, 0x11, 0x4e, 0xdb, 0xc9, 0x42, 0xc5, 0x7e, 0x65, 0xad, 0xab,
     0x45, 0x7c, 0x21, 0x89, 0xbf, 0x76, 0xcf, 0xce, 0xf6, 0x99, 0x1d, 0xdb,
     0xc5, 0x8c, 0x30, 0xab, 0x96, 0xc4, 0x3c, 0x43, 0x38, 0xea, 0xb5, 0x6b,
     0x46, 0x63, 0xb7, 0xe9, 0xa2, 0x2e, 0x8d, 0x30, 0x59, 0x85, 0x86, 0xc5,
     0x71, 0x95, 0x9e, 0x4f, 0x9e, 0xb4, 0xfa, 0x11, 0x9b, 0xf1, 0x11, 0xf5,
     0xc6, 0x2c, 0x23, 0xeb, 0xbd, 0x48, 0xa2, 0x13, 0xac, 0x2d, 0x94, 0xaf,
     0x35, 0xe0, 0x7f, 0xbe, 0x65, 0x64, 0xf9, 0xd8, 0x1a, 0x1e, 0x13, 0x94,
     0x40, 0xfc, 0x7a, 0xb2, 0xe3, 0xa5, 0x98, 0x5f, 0x9c, 0xb5, 0x18, 0x2c,
     0x1c, 0x13, 0x71, 0xfe, 0x68, 0xe1, 0x36, 0xb8, 0x6d, 0x45, 0xd5, 0x98,
     0x24, 0xe9, 0x25, 0xed, 0xed, 0x8f, 0x1a, 0xc8, 0x4f, 0xa6, 0xe3, 0xd7,
     0x72, 0xf8, 0x29, 0x3e, 0x9c, 0x79, 0x4b, 0x3c, 0xce, 0xd1, 0xe3, 0x94,
     0xe9, 0x17, 0x25, 0x6e, 0x2d, 0x24, 0x5d, 0xed, 0xb2, 0xfe, 0x12, 0xa1,
     0xee, 0xe3, 0x73, 0x4b, 0x81, 0x1c, 0xdd, 0x7c, 0x5c, 0xf2, 0x22, 0xcf,
     0x77, 0x10, 0xdb, 0x06, 0x05, 0xe5, 0x67, 0xdf, 0xb4, 0xbc, 0x0b, 0xb6,
     0xa3, 0x51, 0xb6, 0xe6, 0xed, 0xd0, 0x4d, 0x7e, 0x3e, 0x0c, 0xd9, 0x3a,
     0xbb, 0x89, 0x63, 0x91, 0x4c, 0xe8, 0x14, 0xc5, 0xe6, 0x31, 0x86, 0xd0,
     0x80, 0x3f, 0x9a, 0x22, 0x50, 0xe1, 0x20, 0x0c, 0x4d, 0x28, 0x66, 0x46,
     0x90, 0x4b, 0x07, 0xda, 0x8b, 0xdb, 0x63, 0xc0, 0xdd, 0x6c, 0x47, 0xb9,
     0x0c, 0x5f, 0x60, 0xe2, 0x43, 0x31, 0xe6, 0x36, 0xd6, 0x41, 0x7a, 0x19,
     0x02, 0xb7, 0xa7, 0xc3, 0x8f, 0xea, 0xbd, 0x23, 0x7f, 0xea, 0x55, 0xb3,
     0xcb, 0xa0, 0xb9, 0x45, 0xd5, 0xcf, 0x90, 0x7f, 0xf4, 0x44, 0x24, 0xfa,
     0x27, 0x7b, 0x12, 0xa4, 0x0e, 0x5d, 0xf5, 0xb4, 0x2b, 0x4d, 0x0b, 0xf4,
     0xd3, 0x77, 0x0e, 0x0c, 0x73, 0xa6, 0xea, 0x91},

    /* a2 */
    {0xb1, 0x66, 0xc1, 0x81, 0x76, 0x59, 0x80, 0x5e, 0x84, 0x4b, 0xd2, 0x62,
     0xb6, 0x85, 0x1b, 0x41, 0xe8, 0x03, 0x82, 0xa1, 0xf6, 0x41, 0x82, 0x3b,
     0x30, 0x22, 0x21, 0x81, 0x7c, 0x20, 0xc0, 0x4c, 0x40, 0x95, 0x96, 0xd0,
     0x04, 0x2f, 0x23, 0x4e, 0xa8, 0x95, 0x34, 0xdc, 0x48, 0x26, 0xba, 0xf3,
     0x84, 0x20, 0xda, 0x7f, 0x56, 0xce, 0xca, 0x5c, 0x34, 0x77, 0x50, 0x7e,
     0x39, 0x7c, 0xba, 0x0b, 0xc2, 0x6a, 0xbc, 0x40, 0x90, 0xd3, 0x6c, 0x11,
     0xf2, 0x03, 0x0a, 0xb3, 0x0d, 0xe8, 0x69, 0xa2, 0xdb, 0x54, 0xf2, 0xe4,
     0x31, 0xd3, 0x57, 0xa4, 0xc9, 0xe2, 0xf2, 0x05, 0x1f, 0x75, 0x2e, 0x8a,
     0x3d, 0x19, 0x81, 0xa0, 0x24, 0xfe, 0xa2, 0x07, 0x62, 0x6d, 0x91, 0x5d,
     0xf9, 0xf6, 0x0d, 0xd7, 0xa7, 0xc5, 0x79, 0x34, 0xd5, 0x39, 0x35, 0xe2,
     0xdd, 0x38, 0xc3, 0xf0, 0x6a, 0x03, 0xc4, 0xbf, 0x57, 0x3a, 0x70, 0xbc,
     0x39, 0x39, 0x61, 0x8e, 0x30, 0x6f, 0xee, 0xa0, 0xac, 0xb0, 0xc8, 0x18,
     0x0b, 0xc0, 0x4c, 0xe2, 0x03, 0xf7, 0xa1, 0x99, 0x09, 0x57, 0x00, 0x4d,
     0xb5, 0xba, 0xe6, 0x29, 0xde, 0xf4, 0x85, 0x87, 0xa0, 0x3e, 0x7a, 0x75,
     0x4c, 0xf4, 0xd7, 0x4a, 0x0a, 0xd0, 0x58, 0xd5, 0xc3, 0xaa, 0xf6, 0x67,
     0x02, 0x4f, 0xfb, 0xb2, 0x15, 0x89, 0x36, 0x94, 0xc0, 0x29, 0x75, 0x68,
     0xd1, 0xf9, 0x67, 0x3b, 0x16, 0x11, 0x8c, 0xd1, 0x9b, 0x8c, 0x46, 0xf0,
     0xba, 0xb7, 0x0f, 0xfb, 0xe5, 0x32, 0x18, 0x81, 0xaa, 0x12, 0xce, 0x96,
     0x6d, 0x36, 0xc5, 0xfb, 0x05, 0x3b, 0xd1, 0x65, 0x08, 0x35, 0x26, 0x84,
     0x7e, 0x77, 0xed, 0x28, 0x6e, 0x7c, 0x10, 0xec, 0x8d, 0x28, 0xdc, 0x8f,
     0x6c, 0x20, 0x01, 0xab, 0x27, 0x62, 0x67, 0x90, 0x9c, 0x2f, 0x49, 0x47,
     0x76, 0x12, 0x2e, 0x38, 0xc4, 0x35, 0x83, 0x17, 0xe3, 0xb7, 0x25, 0x1a,
     0x68, 0x87, 0x53, 0x55, 0xa4, 0x5d, 0x3c, 0x9e, 0x55, 0xa4, 0x31, 0x65,
     0xc3, 0x42, 0x67, 0x0a, 0x80, 0x5b, 0x9d, 0xdd, 0x4b, 0xa1, 0xa0, 0x54,
     0xa9, 0x42, 0x27, 0x11, 0xd9, 0xec, 0x06, 0x14, 0xc5, 0x40, 0xeb, 0x64,
     0xfb, 0xdd, 0xd2, 0x12, 0xe0, 0xac, 0xd9, 0xae, 0xa6, 0xb2, 0x1a, 0xa3,
     0x72, 0xf9, 0x9d, 0x0b, 0x00, 0xd1, 0xd1, 0x99, 0x02, 0xf0, 0x84, 0xc3,
     0x76, 0xe6, 0x43, 0xcd, 0x64, 0x4a, 0x72, 0xab, 0x74, 0xa8, 0x67, 0xe2,
     0x54, 0x52, 0x8b, 0xfa, 0x83, 0x33, 0x87, 0x40, 0x9a, 0x07, 0x1a, 0xda,
     0x1c, 0x58, 0x90, 0x0c, 0x19, 0x62, 0x21, 0xf4, 0x46, 0x6f, 0xe4, 0xe8,
     0xec, 0x86, 0x7f, 0x09, 0x82, 0x24, 0xc2, 0x82, 0xa5, 0x67, 0xaf, 0x41,
     0xbe, 0x93, 0x63, 0x3a, 0x2a, 0x03, 0xe0, 0x8d, 0xdc, 0x40, 0xc2, 0x6e,
     0xe8, 0x0c, 0xb0, 0xa7, 0x8c, 0x95, 0x88, 0x86, 0x38, 0x1a, 0x00, 0x67,
     0x16, 0x6c, 0x38, 0x23, 0x32, 0x35, 0xa1, 0x34, 0xd3, 0x5a, 0x3c, 0x0c,
     0xfe, 0xd9, 0x82, 0xce, 0xd1, 0x81, 0x04, 0x0c, 0xce, 0x53, 0xff, 0x9c,
     0xfe, 0x77, 0xf3, 0x00, 0xf5, 0xea, 0xdc, 0x2c, 0xda, 0x17, 0x58, 0x79,
     0xf0, 0x53, 0x74, 0x6c, 0x12, 0xa3, 0x5a, 0xf7, 0xe3, 0x5c, 0x52, 0xff,
     0xd8, 0xb9, 0x6f, 0x02, 0x17, 0xdb, 0xb2, 0x56, 0x62, 0x96, 0x29, 0x0c,
     0x72, 0x7d, 0x8c, 0x35, 0xaf, 0xdd, 0xb3, 0xd3, 0xda, 0xbb, 0x89, 0xf7,
     0x4c, 0x5d, 0x11, 0x29, 0xec, 0x3e, 0x91, 0x06, 0xb5, 0xb2, 0x9f, 0xc4,
     0x31, 0x43, 0xad, 0x35, 0x1e, 0x66, 0xcd, 0xd4, 0x61, 0x81, 0x91, 0x3f,
     0xcf, 0xbe, 0xe9, 0xe4, 0x6b, 0xae, 0xba, 0xd8, 0x52, 0x39, 0xa5, 0x5c,
     0x95, 0x88, 0x95, 0xc2, 0xc0, 0x4f, 0x9d, 0xcd},

    /* a3 */
    {0x3f, 0xbe, 0xce, 0x29, 0xea, 0x72, 0x31, 0x99, 0x48, 0x05, 0xc6, 0x9f,
     0xf0, 0x45, 0x76, 0xed, 0x1d, 0x3c, 0x4d, 0x64, 0xa0, 0xc7, 0x15, 0x9a,
     0xdf, 0x93, 0x58, 0x00, 0x96, 0x55, 0xa6, 0x66, 0x13, 0xd7, 0xc7, 0x7b,
     0xfd, 0xa0, 0xb3, 0x93, 0xee, 0xe7, 0xd8, 0xb0, 0xf8, 0xc0, 0x79, 0xbe,
     0xef, 0x10, 0x9d, 0x1b, 0x19, 0xee, 0x4f, 0x89, 0x99, 0x4a, 0xb2, 0x60,
     0xf8, 0x4d, 0x67, 0x41, 0x46, 0x05, 0x67, 0xfc, 0x67, 0xb3, 0x16, 0x9b,
     0x56, 0x35, 0x52, 0x0b, 0x69, 0x63, 0x6e, 0xb4, 0x05, 0xe2, 0xde, 0x2f,
     0xf8, 0x2a, 0x96, 0x59, 0x3d, 0xea, 0x8d, 0xdd, 0xbb, 0xb3, 0xa5, 0xe5,
     0x8d, 0xf9, 0xd3, 0xf7, 0x54, 0x83, 0x21, 0xa7, 0x28, 0xa0, 0x00, 0x06,
     0x27, 0x03, 0x5f, 0x71, 0x47, 0x1a, 0x4a, 0xe4, 0xad, 0xac, 0xff, 0x93,
     0xa5, 0x63, 0x46, 0x04, 0xa2, 0x3b, 0x56, 0x8f, 0x74, 0x32, 0x1b, 0x05,
     0x3e, 0x72, 0x93, 0x89, 0xbc, 0xfb, 0x6f, 0xde, 0x61, 0x0b, 0x4c, 0x52,
     0x36, 0x2c, 0x55, 0x18, 0x5f, 0x6d, 0xd4, 0xd7, 0x1e, 0xac, 0x1d, 0x33,
     0xaa, 0xde, 0x94, 0x53, 0x07, 0xc8, 0xf4, 0xd9, 0x87, 0x11, 0x91, 0x3e,
     0xff, 0x14, 0xf8, 0x4e, 0xb6, 0xfa, 0xc2, 0x06, 0x1e, 0xe7, 0x7d, 0xb1,
     0xf1, 0x34, 0x11, 0x9e, 0x82, 0xa7, 0x64, 0x05, 0x0c, 0xf5, 0xd6, 0xb3,
     0x2f, 0xce, 0x78, 0x02, 0xb3, 0xb1, 0x6c, 0xd8, 0xbc, 0x6e, 0x46, 0x15,
     0x9a, 0xfa, 0x22, 0xba, 0xa4, 0xe2, 0xc0, 0x7c, 0xb2, 0xcb, 0x91, 0xda,
     0x6e, 0xf1, 0x58, 0xdb, 0x48, 0x4e, 0x57, 0x02, 0xe0, 0x09, 0x76, 0x59,
     0x78, 0xf8, 0x89, 0x21, 0xfa, 0xbe, 0x8f, 0x25, 0x26, 0xbe, 0x7a, 0x5e,
     0x4b, 0x6e, 0xe0, 0xa0, 0xed, 0x64, 0x52, 0x59, 0x9f, 0xea, 0x22, 0x61,
     0xe4, 0x74, 0xd7, 0x91, 0x5e, 0x9c, 0xdb, 0xf7, 0x3b, 0x5c, 0x81, 0x07,
     0xdb, 0x56, 0xe9, 0xfc, 0x88, 0xa5, 0x16, 0x20, 0x3d, 0xc2, 0xad, 0xe8,
     0xa5, 0xa8, 0x57, 0x66, 0x4f, 0x33, 0x2e, 0x2e, 0x93, 0x22, 0x92, 0x37,
     0x6d, 0x57, 0xe7, 0x1f, 0x83, 0x22, 0xe4, 0xa2, 0x65, 0x5c, 0xb0, 0xf2,
     0x4e, 0x8b, 0x4a, 0xb1, 0x27, 0xaf, 0x5b, 0x70, 0xf4, 0xb7, 0x6d, 0xe7,
     0xc1, 0x96, 0xf9, 0x92, 0xb9, 0x96, 0xe4, 0xb5, 0x46, 0x57, 0x14, 0x2e,
     0x09, 0xe6, 0x2a, 0xd0, 0xb0, 0x6e, 0xd2, 0xcd, 0xbc, 0x94, 0xd2, 0xd4,
     0xbe, 0x8c, 0x63, 0xad, 0x2b, 0xcd, 0x66, 0xcc, 0x4a, 0x8e, 0x5b, 0x90,
     0x78, 0x8a, 0xeb, 0x97, 0xfa, 0x17, 0x8d, 0x9e, 0x14, 0xb9, 0x32, 0xdf,
     0x50, 0x02, 0xa4, 0x3f, 0x09, 0xfc, 0x6a, 0x9f, 0xe3, 0xcb, 0x5e, 0x63,
     0xbd, 0x57, 0x1f, 0x5f, 0xa8, 0xc6, 0xb1, 0xb4, 0xf4, 0xfd, 0x22, 0x04,
     0x35, 0x20, 0x56, 0x54, 0x6d, 0x88, 0xfc, 0x26, 0x4f, 0x3a, 0x20, 0xe1,
     0x32, 0xb1, 0x7c, 0x78, 0x14, 0x7d, 0xb0, 0xbe, 0xce, 0x78, 0xf2, 0x40,
     0xf5, 0xeb, 0x26, 0xf5, 0x6c, 0x49, 0x8d, 0xd2, 0x65, 0xcc, 0x73, 0x84,
     0x88, 0x7a, 0x91, 0x57, 0x0f, 0xfa, 0xd1, 0x0c, 0x94, 0xb2, 0x88, 0x89,
     0xb1, 0x4a, 0x54, 0x4d, 0x27, 0x99, 0x4c, 0x54, 0xa4, 0xcd, 0x04, 0x36,
     0xed, 0xaa, 0xe3, 0xef, 0xd1, 0xbf, 0xaa, 0xe5, 0x8c, 0x4c, 0xc1, 0x7b,
     0x3a, 0x7f, 0x50, 0x43, 0x00, 0x55, 0x11, 0xf1, 0x81, 0x3c, 0xde, 0x37,
     0x92, 0x55, 0x6d, 0x10, 0xf7, 0x64, 0x78, 0x71, 0xb7, 0x9b, 0xf5, 0x08,
     0xd7, 0x04, 0x37, 0xbc, 0x39, 0xe2, 0xa0, 0x6f, 0xce, 0x02, 0xdc, 0xf3,
     0x5b, 0x2c, 0xb1, 0x77, 0xfb, 0x3c, 0xae, 0x56, 0x50, 0x0c, 0x39, 0xa8,
     0x03, 0xe1, 0xa7, 0xdc, 0xe4, 0x7d, 0x8a, 0xcd},

    /* a4 */
    {0x4f, 0x86, 0x55, 0x88, 0xcc, 0xb0, 0x83, 0xee, 0x23, 0x41, 0x3e, 0x5c,
     0x56, 0x72, 0x5a, 0x7a, 0x59, 0x87, 0x2f, 0x9a, 0x6c, 0x19, 0xb0, 0xe8,
     0x4a, 0x9e, 0x84, 0x84, 0xb7, 0xf8, 0xb8, 0xa3, 0xcd, 0xc2, 0xe8, 0xf0,
     0x8c, 0x7c, 0x3d, 0xdd, 0xf8, 0x5f, 0xd6, 0x4d, 0xc3, 0x15, 0x07, 0x0d,
     0x1b, 0x14, 0x07, 0x7f, 0xfb, 0x6f, 0x72, 0xc8, 0x82, 0xe5, 0x51, 0xcb,
     0xc0, 0x6b, 0xc5, 0x4f, 0x22, 0x04, 0x0a, 0x65, 0x3e, 0x2a, 0x82, 0xa9,
     0x83, 0xe2, 0xe1, 0x5c, 0x8f, 0x98, 0xf0, 0x77, 0x79, 0x8c, 0x22, 0xf8,
     0x78, 0x71, 0x10, 0xcf, 0x71, 0x56, 0x7d, 0x2a, 0xff, 0x99, 0xca, 0x70,
     0x48, 0xb4, 0x24, 0xdf, 0xd5, 0xe8, 0x90, 0x6a, 0x1c, 0x6d, 0xa9, 0x7b,
     0x5a, 0xd1, 0xe7, 0x34, 0xf2, 0xe9, 0x28, 0x13, 0xc6, 0xfc, 0x5d, 0xaf,
     0x88, 0x7a, 0x64, 0x6d, 0x64, 0x72, 0xe7, 0x51, 0x33, 0xce, 0x73, 0x79,
     0xa1, 0x09, 0x1a, 0x96, 0x2d, 0xc2, 0xc6, 0x30, 0x67, 0x2b, 0x29, 0x78,
     0x1f, 0x98, 0x27, 0x61, 0xaf, 0x07, 0x3a, 0x66, 0x00, 0x25, 0x1e, 0x7d,
     0x62, 0xb4, 0xca, 0xd4, 0xa3, 0x56, 0xf3, 0xeb, 0xd5, 0x52, 0x64, 0x24,
     0x6c, 0xa0, 0xcf, 0x68, 0x11, 0xae, 0x42, 0x03, 0xe8, 0xa0, 0xf0, 0xc1,
     0x87, 0x89, 0x9b, 0xc3, 0x0b, 0xf3, 0x81, 0xb2, 0x40, 0x05, 0x27, 0x2a,
     0x65, 0x3a, 0x07, 0x6b, 0x0d, 0x80, 0x94, 0x99, 0x91, 0xd0, 0x9b, 0x61,
     0xc6, 0x66, 0xa9, 0xdd, 0xc8, 0x7e, 0x02, 0xe0, 0xcf, 0x4b, 0x7b, 0xbe,
     0x18, 0xeb, 0x12, 0x0c, 0x14, 0x14, 0xd5, 0xe9, 0x07, 0xdc, 0x9d, 0xdf,
     0xc9, 0xb5, 0x33, 0x27, 0xdd, 0xfb, 0x86, 0x6f, 0xd2, 0xc9, 0x82, 0x04,
     0x62, 0x97, 0x9f, 0x94, 0x85, 0x8c, 0x80, 0xe9, 0x0c, 0x4f, 0xb5, 0x7a,
     0x3c, 0x97, 0xe3, 0x04, 0x68, 0xa2, 0x64, 0xbe, 0x04, 0x5a, 0x7f, 0xd7,
     0x63, 0x32, 0x80, 0x39, 0xcb, 0x7c, 0x74, 0x61, 0x4a, 0xec, 0xfe, 0x65,
     0xd6, 0x96, 0x5e, 0x44, 0xf3, 0x29, 0xf0, 0x6f, 0xf2, 0x3d, 0x62, 0x37,
     0xe3, 0xb1, 0x1d, 0x9f, 0x0c, 0x63, 0x53, 0xa0, 0xfc, 0x56, 0x56, 0x1f,
     0xf3, 0x85, 0xe2, 0xda, 0x9a, 0x14, 0x52, 0xce, 0x26, 0xe2, 0x39, 0x0e,
     0xa5, 0x40, 0x8d, 0x7a, 0x2c, 0x96, 0x45, 0xb1, 0x47, 0xdc, 0xe8, 0xe1,
     0x0c, 0x7d, 0xd9, 0x15, 0x49, 0xc4, 0xf8, 0xc7, 0x72, 0xe8, 0x61, 0x12,
     0x0a, 0xd9, 0x80, 0x21, 0xc3, 0xff, 0xab, 0xa0, 0x3d, 0x24, 0x8d, 0xe0,
     0x0c, 0x35, 0xf5, 0x8e, 0xab, 0x4e, 0xf8, 0x69, 0x11, 0x56, 0xc5, 0xf4,
     0x56, 0x15, 0x85, 0x9d, 0x2e, 0x32, 0x15, 0x4d, 0x62, 0x82, 0xda, 0x49,
     0x46, 0x5a, 0xb0, 0xfb, 0xa1, 0xbc, 0x78, 0x5c, 0xb3, 0x30, 0x2c, 0x59,
     0x07, 0xdc, 0x60, 0xf7, 0x91, 0xfe, 0xff, 0xf7, 0xc7, 0x30, 0x2c, 0xf0,
     0xe7, 0xfb, 0x49, 0x58, 0x51, 0x87, 0x96, 0x37, 0x35, 0xf5, 0x86, 0x7e,
     0x11, 0xae, 0x63, 0x79, 0xb7, 0xf4, 0xf1, 0x6f, 0x33, 0x46, 0xca, 0x09,
     0xa2, 0xda, 0x9a, 0x41, 0xfa, 0x11, 0xf9, 0x88, 0xb8, 0xd2, 0xec, 0xd6,
     0x2a, 0xa0, 0xca, 0x97, 0x28, 0x10, 0x17, 0x83, 0xeb, 0x23, 0x44, 0x26,
     0xca, 0xfc, 0x75, 0xfb, 0x8b, 0x09, 0x52, 0x57, 0xb1, 0x30, 0x68, 0x82,
     0x3f, 0xa3, 0x97, 0x8b, 0x50, 0x00, 0x95, 0x14, 0x0e, 0x55, 0x49, 0x12,
     0xa2, 0x59, 0x46, 0xef, 0xd4, 0x58, 0x2b, 0xd5, 0xf6, 0xb0, 0x21, 0x66,
     0x36, 0x7e, 0xa5, 0xa7, 0x77, 0xbf, 0xdc, 0xcd, 0x77, 0xaf, 0x97, 0x7d,
     0xce, 0x73, 0x99, 0x4f, 0x66, 0x58, 0xec, 0x8c, 0x56, 0x24, 0xc7, 0xcd,
     0xd0, 0x8c, 0xab, 0xb0, 0x28, 0x7a, 0x02, 0xa5},
};

/**
* @brief
*   Flashloader plat init.
*
* @param [in] fl - pointer to flashloader handler structure
* @return
*   None.
*
*/
PUBLIC void flashloader_plat_init(flashloader_handler_struct *fl)
{
    spi_flash_dev_enum dev;
    PMCFW_ERROR rc;

    /* Register the Flashloader handlers with ECH module*/
    ech_api_func_register(EXP_FW_BINARY_UPGRADE, fl->flashloader_fw_image_upgrade_handler);
    ech_api_func_register(EXP_FW_FLASH_LOADER_VERSION_INFO,fl->flashloader_version_info_handler);
    /*Initialize the plaform image list structure*/
    flash_partition_image_list_get(&img_list[0]);
    
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &flashloader_plat_spi_dev_info);
     PMCFW_ASSERT(rc == PMC_SUCCESS, rc);
}

/**
* @brief
*   To get the active image flag
*
* @return
*   Active image flag.
*
*/
PUBLIC UINT32 flashloader_plat_actv_image_flag_get(void)
{
    UINT32 act_img;
    /* retrieve active image flag from flash */
    act_img = * (UINT32 *) (SPI_FLASH_FW_ACT_IMG_FLAG_ADDR);
    return act_img;
    
}

/**
* @brief
*   To get FW version info
*
* @return
*   FW version information.
*
*/
PUBLIC VOID * flashloader_plat_fw_version_info_get(void)
{
    return (void *)&FW_VERSION_INFO[0];
}

/**
* @brief
*   To get FW image length
*
* @param [in] img_id - Image ID: A or B
* @return
*   FW image length in Bytes.
*
*/

PUBLIC UINT32 flashloader_plat_fw_image_length_get(UINT32 img_id)
{
    return fam_image_length_get(img_list, img_id);
}

/**
* @brief
*   To get flashloader command
*
* @return
*   Command ID.
*
*/
PUBLIC flashloader_cmd_enum flashloader_plat_flash_command_get(void)
{
    flashloader_cmd_enum cmd;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    cmd = (flashloader_cmd_enum)cmd_ptr->parms[0];
    return cmd;
}


/**
* @brief
*   Get the partition ID sent by HOST
*
* @return
*   partition ID.
*
*/
PUBLIC CHAR flashloader_plat_flash_command_partition_id_get(void)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    return cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_PARTITION_ID_OFFSET];

}

/**
* @brief
*   Get the command sequence number
*   This can be used to identify the packet
*
* @return
*   command sequence number
*
*/
PUBLIC UINT32 flashloader_plat_flash_command_packet_sequence_get(void)
{
    UINT32 sequence_no=0;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    sequence_no = cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_PACKET_SEQUENCE_OFFSET] << 8 |
                  cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_PACKET_SEQUENCE_OFFSET+1] << 0 ;
    return sequence_no;

}

/**
* @brief
*   Get the flash address offeset from the start of partition
*  
*
* @return
*   flash address offeset in Byte from the start of partition
*
*/
PUBLIC UINT32 flashloader_plat_flash_command_address_offset_get(void)
{
    UINT32 address_offset=0;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    address_offset = cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_ADDRESS_OFFSET]     << 24 |
                     cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_ADDRESS_OFFSET+1]   << 16 |
                     cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_ADDRESS_OFFSET+2]   << 8  |
                     cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_ADDRESS_OFFSET+3]   << 0 ;
    return address_offset;

}

/**
* @brief
*   Get the number of bytes to be read from the flash offset
*  
*
* @return
*   Number of bytes to be read.
*
*/
PUBLIC UINT32 flashloader_plat_flash_command_read_length_get(void)
{
    UINT32 read_length=0;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    read_length = cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_PACKET_LENGTH_OFFSET]     << 8 |
                  cmd_ptr->parms[FLASH_LOADER_FLASH_READ_CMD_PACKET_LENGTH_OFFSET+1]   << 0 ;
    return read_length;

}


/**
* @brief
*   Validate various input parameters
*  
* @param [in] flash_partition_id - Partition ID
* @param [in] flash_partition_addr_offset - Address offset within flash
* @param [in] flash_read_length - Length of bytes of to be read.
* @param [out] flash_read_base - Absolute address of the flash location to be read.
*
* @return
*   Success or Failure code
*
*/
PUBLIC UINT32 flashloader_plat_flash_read_command_validate(INT8 flash_partition_id, 
                                                           UINT32 flash_partition_addr_offset,
                                                           UINT32 flash_read_length,
                                                           UINT32 *flash_read_base)
{
    UINT32 i;
    UINT32 flash_partition_offset = 0;
    *flash_read_base = 0;

    for (i = 0; i < SPI_FLASH_PARTITION_NUMBER; i++)
    {
        if (img_list[i].image_id != flash_partition_id)
        {
            continue;
        }
        else
        {
            /* Got the flash partition, now check remaining parameter*/
            if (flash_read_length > FLASH_PLAT_REQ_RESP_BUF_LENGTH)
            {
                return FLASHLOADER_ERR_INVALID_READ_LENGTH;
            }

            /* Image A and B are of same length, so using A*/
            if ((flash_partition_addr_offset + flash_read_length) > SPI_FLASH_FW_IMG_A_SIZE)
            {
                return FLASHLOADER_ERR_ADDRESS_OUT_OF_RANGE;
            }

            flash_partition_offset = (img_list[i].image_id == 'A') ? SPI_FLASH_FW_IMG_A_HDR_ADDR : SPI_FLASH_FW_IMG_B_HDR_ADDR;
            *flash_read_base  = (UINT32)(flash_partition_offset + flash_partition_addr_offset);

            return PMC_SUCCESS;                
        }
    }

    return FLASHLOADER_ERR_INVALID_PARTITION_ID;
}

/**
* @brief
*   Perform read operation from flash address
*  
* @param [in]  src_start_addr -  Source address
* @param [in]  flash_length -    Length of data byte to be read
* @param [out] dest_start_addr - Start of destination address
* 
* @return
*   None
*
*/
PUBLIC VOID flashloader_plat_flash_read(void* dest_start_addr, void* src_start_addr, UINT32 flash_length)
{
    memcpy(dest_start_addr, src_start_addr, flash_length);
}

/**
* @brief
*   Perform flash erase operation
*  
* @param [in]  partition ID -  Partition ID: 'A' or 'B'
* @param [out] err_code     -  flash error codes
* 
* @return
*   Success or Failure code
*
*/
PUBLIC UINT32 flashloader_plat_partition_erase(INT8 partition_id, UINT32 *err_code)
{
    UINT8* subsector_base;
    UINT32 subsector_len;
    BOOL   write_partition_flag = FALSE;
    UINT32 partition_flag;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    UINT8* erase_addr = (UINT8*)(((partition_id == 'A') ? SPI_FLASH_FW_IMG_A_HDR_ADDR : SPI_FLASH_FW_IMG_B_HDR_ADDR) & (~SPI_FLASH_BASE_ADDRESS));
    PMCFW_ERROR rc;
    UINT32 i;
    UINT32 flash_subsector_number = (SPI_FLASH_FW_IMG_A_SIZE / 
                                    (flashloader_plat_spi_dev_info.page_size * flashloader_plat_spi_dev_info.pages_per_subsector)) + 1;
    top_plat_lock_struct lock_struct;

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);
    
    if (rc != PMC_SUCCESS)
    {
        bc_printf("Flashloader: flashloader_plat_partition_erase dev_info failed\n");            
        *err_code = FLASHLOADER_ERR_DEVINFO_GET;
        return (FLASHLOADER_ERR_DEVINFO_GET);
    }

    /* 
    ** If partition 'A' is being erased, save the content
    ** of partition flag
    */ 
    if (partition_id == 'A')
    {            
        write_partition_flag = TRUE;
        partition_flag = *((UINT32 *)(SPI_FLASH_FW_ACT_IMG_FLAG_ADDR));
    }

    UINT32 dot_count = 0;
    bc_printf("Begin partition erase...\n");
    for(i = 0; i < flash_subsector_number ; i++, erase_addr += subsector_len)
    {
#if (EXPLORER_WDT_DISABLE == 0)	
        wdt_hardware_tmr_kick();
        wdt_interval_tmr_kick();
#endif

        /* Show erase progress on the UART using dots. */
        if (50 == dot_count)
        {
            dot_count = 0;
            bc_printf("\n");
        }
        else
        {
            bc_printf(".");
            dot_count++;
        }

        /* get the subsector address and length */
        rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                            SPI_FLASH_CS,
                                            erase_addr,
                                            &subsector_base,
                                            &subsector_len);

        if (rc != PMC_SUCCESS)
        {
            bc_printf("Flashloader: flashloader_plat_partition_erase subsector_params_get failed\n");
            *err_code = FLASHLOADER_ERR_PARAMS_GET;
            return FLASHLOADER_ERR_PARAMS_GET;
        }

        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);

        /* erase the subsector */
        rc = spi_flash_subsector_erase_wait(SPI_FLASH_PORT,
                                            SPI_FLASH_CS,
                                            subsector_base,
                                            dev_info.max_time_subsector_erase);

        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        if (rc != PMC_SUCCESS)
        {
            bc_printf("Flashloader: flashloader_plat_partition_erase subsector erase failed\n");
            *err_code = FLASHLOADER_ERR_SUBSECTOR_ERASE;
            return FLASHLOADER_ERR_SUBSECTOR_ERASE;
        }

    }

    if (write_partition_flag == TRUE)
    {
        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);

        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   (UINT8*)&partition_flag,
                                   (UINT8*) ((SPI_FLASH_FW_ACT_IMG_FLAG_ADDR) & GPBC_FLASH_PHYS_ADDR_MASK),
                                   sizeof(UINT32),
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);

        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        if (rc != PMC_SUCCESS)
        {
            bc_printf("Flashloader: flashloader_plat_partition_erase write partition flag failed\n");
            *err_code = FLASHLOADER_ERR_FLASH_WRITE_FAIL;
            return FLASHLOADER_ERR_FLASH_WRITE_FAIL;
        }

    }

    bc_printf("\n...Done target partition erase.\n");
           
    return PMC_SUCCESS;   
}

/**
* @brief
*   Get the Image size   
* @param None
* 
* @return
*   Image size in bytes
*
*/
PUBLIC UINT32 flashloader_plat_flash_image_size_get(void)
{
    UINT32 flash_image_size;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    flash_image_size = cmd_ptr->parms[FLASH_LOADER_FLASH_WRITE_IMAGE_SIZE_OFFSET] |
                        cmd_ptr->parms[FLASH_LOADER_FLASH_WRITE_IMAGE_SIZE_OFFSET+1] << 8 |
                        cmd_ptr->parms[FLASH_LOADER_FLASH_WRITE_IMAGE_SIZE_OFFSET+2] << 16;
    return flash_image_size;
}

/**
* @brief
*   Get the packet sequence
* @param None
* 
* @return
*   packet sequence
*
*/
PUBLIC UINT32 flashloader_plat_flash_image_packet_sequence_get(void)
{
    UINT32 flash_image_sequence;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    flash_image_sequence = cmd_ptr->parms[FLASH_LOADER_FLASH_WRITE_PACKET_SEQUENCE_OFFSET] |
                        cmd_ptr->parms[FLASH_LOADER_FLASH_WRITE_PACKET_SEQUENCE_OFFSET+1] << 8;
    return flash_image_sequence;
}

/**
* @brief
*   Get the data buffer offset
* @param None
* 
* @return
*   packet sequence
*
*/
PUBLIC void * flashloader_plat_flash_image_data_buf_address_get(void)
{
    return ((void *)ech_ext_data_ptr_get());
}

/**
* @brief
*   Write the data into temp buffer within the flash
*   Data authentication will happen during FLASH COMMIT command
* 
* @param [in]  flash_write_index     -  Actual address of flash is flash_write_index * 256B   
* @param [in]  flash_image_data_buf -  Data buffer 
* @param [out] err_code             -  flash error codes
* 
* @return
*   packet sequence
*
*/
PUBLIC UINT32 flashloader_plat_flash_buffer_write(UINT32 flash_write_index, 
                                                  UINT8 *flash_image_data_buf, 
                                                  UINT32 *err_code)
{
    PMCFW_ERROR rc;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    UINT8* subsector_base;
    UINT32 subsector_len;
    UINT32 flash_write_buffer_addr = (flash_write_index * FLASH_LOADER_PAGE_BUF_SIZE) + SPI_FLASH_FW_FW_UPGRADE_ADDR;
    top_plat_lock_struct lock_struct;

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);
        
    if (rc != PMC_SUCCESS)
    {
        bc_printf("Flashloader: flashloader_plat_flash_buffer_write dev_info failed\n");            
        *err_code = FLASHLOADER_ERR_DEVINFO_GET;
        return FLASHLOADER_ERR_DEVINFO_GET;
    }

    /* get the subsector address and length */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        (UINT8 *)(flash_write_buffer_addr & (~SPI_FLASH_BASE_ADDRESS)),
                                        &subsector_base,
                                        &subsector_len);
    
    /* 
    ** ensure that the flash memory about to be programmed is erased 
    ** when performing a firmware download the SPI sub-blocks are erased
    ** on-the-fly as the download proceeds and the sub-block is erased if:
    ** - the address is the start of the reserved flash partition for downloads 
    ** - the address is the start of the next sub-block
    */
    if ((SPI_FLASH_FW_FW_UPGRADE_ADDR == flash_write_buffer_addr) ||
        (0 == (flash_write_buffer_addr & GPBC_FLASH_PHYS_ADDR_MASK) % subsector_len))
    {
        bc_printf("Erasing Firmware Download Subsector @ 0x%08X ... ", flash_write_buffer_addr);
        rc = spi_flash_plat_erase((UINT8*)(flash_write_buffer_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                                  subsector_len);

        if (PMC_SUCCESS != rc)
        {
            bc_printf("\nFlashloader: flashloader_plat_flash_buffer_write subsector erase failed, rc = 0x%x\n", rc);
            *err_code = FLASHLOADER_ERR_SUBSECTOR_ERASE;
            return FLASHLOADER_ERR_SUBSECTOR_ERASE;
        }
        bc_printf("done\n");
    }

    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    /* write up until the end of the current page */
    rc = spi_flash_write_pages(SPI_FLASH_PORT,
                               SPI_FLASH_CS,
                               (UINT8*)flash_image_data_buf,
                               (UINT8*)(flash_write_buffer_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                               FLASH_LOADER_PAGE_BUF_SIZE,
                               dev_info.page_size,
                               dev_info.max_time_page_prog);

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);

    if (rc != PMC_SUCCESS)
    {
        bc_printf("Flashloader: flashloader_plat_flash_buffer_write write error %08lx\n", rc);
        *err_code = FLASHLOADER_ERR_FLASH_WRITE_FAIL;
        return FLASHLOADER_ERR_FLASH_WRITE_FAIL;
    }

    return PMC_SUCCESS;
}

/**
* @brief
*   Validate the flash image 
*
* @param [out] err_code - Copy the extended error code that FAM returns
*
* @return
*   SUCCESS / FAIL.
*
*/
PUBLIC UINT32 flashloader_plat_flash_image_validate(UINT32 *err_code)
{
    fam_image_desc_struct *exec_image;
    fam_image_desc_struct image_list;
    UINT32 pkey_array[FLASH_LOADER_PLAT_NUM_PUBLIC_KEYS];

    /* Point the image location to temporary partition in flash*/
    image_list.image_addr = (UINT8*)SPI_FLASH_FW_FW_UPGRADE_ADDR;
    /* To indicate this is temporary partition*/
    image_list.image_id = 0xFF;  
    /* Clear the error code*/
    image_list.status = NOT_TESTED;

    for (UINT32 i = 0; i < FLASH_LOADER_PLAT_NUM_PUBLIC_KEYS; i++)
    {
        pkey_array[i] = (UINT32)&pub_key_modulus[i][0];
    }

    exec_image = fam_authenticate_image(&image_list,
                                        1,
                                        &pkey_array[0], FLASH_LOADER_PLAT_NUM_PUBLIC_KEYS,
                                        top_secure_boot_mode_get(),
                                        SPI_FLASH_FW_FW_UPGRADE_SIZE);
    if (NULL == exec_image)
    {
        bc_printf("Flashloader: flashloader_plat_flash_image_validate failed : %08lx\n", 
                  image_list.extend_err_code);
        /* Error in authentication */
        *err_code = FLASHLOADER_ERR_AUTHENTICATION_ERROR;
        return PMCFW_ERR_FAIL;
    }
    bc_printf("Flashloader: flashloader_plat_flash_image_validate passed \n");
    return PMC_SUCCESS;

}

/**
* @brief
*   Finalize the flash image by moving it to appropriate partition
*
* @param [in]  flash_partition_id   -  Destination partition id
* @param [out] err_code             -  flash error codes
* @return
*   SUCCESS / FAIL.
*
*/
PUBLIC UINT32 flashloader_plat_flash_image_finalize(INT8 flash_partition_id, UINT32 *err_code)
{
    fam_image_desc_struct image_list;
    UINT8* subsector_base;
    UINT32 subsector_len;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    /* Image length in bytes*/
    UINT32 image_length;
    /* Number of flash write operations */
    UINT32 flash_write_loop_cnt=0;
    UINT32 image_id =0;
    UINT32 rc;
    UINT32 i;
    UINT32 flash_image_src = SPI_FLASH_FW_FW_UPGRADE_ADDR;
    UINT32 flash_image_dest = (flash_partition_id == 'A') ? SPI_FLASH_FW_IMG_A_HDR_ADDR : SPI_FLASH_FW_IMG_B_HDR_ADDR;
    top_plat_lock_struct lock_struct;

    bc_printf("flashloader_plat_flash_image_finalize, moving code to partition %c\n", flash_partition_id);
    /* Point the image location to temporary partition in flash*/
    image_list.image_addr = (UINT8*)SPI_FLASH_FW_FW_UPGRADE_ADDR;
    image_length =fam_image_length_get(&image_list, image_id) + SPI_FLASH_FW_IMG_A_HDR_SIZE;

    if(image_length > SPI_FLASH_FW_IMG_A_SIZE)
    {
        bc_printf("Flashloader Image Finalize error: invalid length=0x%08x\n", image_length);
        *err_code = FLASHLOADER_ERR_FLASH_IMAGE_COMMIT_FW_LENGTH_OUT_OF_RANGE;
        return FLASHLOADER_ERR_FLASH_IMAGE_COMMIT_FW_LENGTH_OUT_OF_RANGE;
    }
    
    bc_printf("flashloader_plat_flash_image_finalize, erasing target partition %c\n", flash_partition_id);
    rc = flashloader_plat_partition_erase(flash_partition_id, err_code);
    if( rc != PMC_SUCCESS)
    {        
        bc_printf("flashloader_plat_partition_erase fail, rc=0x%x\n", rc);
        return rc;
    }

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);
        
    if (rc != PMC_SUCCESS)
    {
        bc_printf("Flashloader: flashloader_plat_flash_image_finalize dev_info failed\n");            
        *err_code = FLASHLOADER_ERR_DEVINFO_GET;
        return (FLASHLOADER_ERR_DEVINFO_GET);
    }
    

    /* Calculate number of Page buffer write sequence*/
    flash_write_loop_cnt = (image_length / FLASH_LOADER_PAGE_BUF_SIZE) + ((image_length % FLASH_LOADER_PAGE_BUF_SIZE) ? 1 : 0);
    bc_printf("flashloader_plat_flash_image_finalize, copying new image to target partition %c\n", flash_partition_id);
    for(i=0; i < flash_write_loop_cnt; i++)
    {
#if (EXPLORER_WDT_DISABLE == 0)	
        wdt_hardware_tmr_kick();
        wdt_interval_tmr_kick();
#endif

        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);

        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   (UINT8*)flash_image_src,
                                   (UINT8*)(flash_image_dest & GPBC_FLASH_PHYS_ADDR_MASK),
                                   FLASH_LOADER_PAGE_BUF_SIZE,
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);

        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        if (rc != PMC_SUCCESS)
        {
            *err_code = FLASHLOADER_ERR_FLASH_WRITE_FAIL;
            bc_printf("Flashloader: flashloader_plat_flash_image_finalize write error %08lx\n", rc);
            return FLASHLOADER_ERR_FLASH_WRITE_FAIL;
        }
        
        flash_image_src += FLASH_LOADER_PAGE_BUF_SIZE;
        flash_image_dest +=FLASH_LOADER_PAGE_BUF_SIZE;        
    }
    bc_printf("...Done \n");

    /* 
    ** Update the BOOT partition flag - Read, Modify then Write
    ** Reason: Image A header lies in 4K erase sector
    ** Step 1: Initialize 4K buffer with all 0xFF
    ** Step 1: Read 2K header of Image 'A'
    ** Step 3: Update BOOT flag
    ** Step 4: Erase the flash
    ** Step 5: Write entire buffer
    */
    /* Make sure the buffer is initialized to 0xFF*/
    bc_printf("flashloader_plat_flash_image_finalize, updating BOOT partition flag");
    memset((void *)__ghsbegin_fw_auth_mem,0xFF, FLASH_LOADER_SUBSECTOR_SIZE);
    /* Copy 2K Image A header into the buffer */
    memcpy((void *)(__ghsbegin_fw_auth_mem + SPI_FLASH_FW_ACT_IMG_FLAG_SIZE),  (void *)(SPI_FLASH_FW_IMG_A_HDR_ADDR), SPI_FLASH_FW_IMG_A_HDR_SIZE);

    __ghsbegin_fw_auth_mem[0] = flash_partition_id == 'A' ? 0x0 : 0x1;

    /* get the subsector address and length */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        (const UINT8 *)(SPI_FLASH_FW_ACT_IMG_FLAG_ADDR & (~SPI_FLASH_BASE_ADDRESS)),
                                        &subsector_base,
                                        &subsector_len);
    
    /* disable interrupts and disable multi-VPE operation */
    top_plat_critical_region_enter(&lock_struct);

    rc = spi_flash_subsector_erase_wait(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        subsector_base,
                                        dev_info.max_time_subsector_erase);

    /* restore interrupts and enable multi-VPE operation */
    top_plat_critical_region_exit(lock_struct);

    if (rc != PMC_SUCCESS)
    {
        bc_printf("Flashloader: flashloader_plat_flash_image_finalize subsector erase failed\n");
        *err_code = FLASHLOADER_ERR_SUBSECTOR_ERASE;
        return FLASHLOADER_ERR_SUBSECTOR_ERASE;
    }


    /* Calculate number of Page buffer write sequence*/
    flash_write_loop_cnt = (FLASH_LOADER_SUBSECTOR_SIZE / FLASH_LOADER_PAGE_BUF_SIZE);

    flash_image_src = (UINT32)__ghsbegin_fw_auth_mem;
    flash_image_dest = SPI_FLASH_FW_ACT_IMG_FLAG_ADDR;
    
    for (i=0; i < flash_write_loop_cnt; i++)
    {
        /* disable interrupts and disable multi-VPE operation */
        top_plat_critical_region_enter(&lock_struct);

        rc = spi_flash_write_pages(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   (UINT8*)flash_image_src,
                                   (UINT8*)(flash_image_dest & GPBC_FLASH_PHYS_ADDR_MASK),
                                   FLASH_LOADER_PAGE_BUF_SIZE,
                                   dev_info.page_size,
                                   dev_info.max_time_page_prog);

        /* restore interrupts and enable multi-VPE operation */
        top_plat_critical_region_exit(lock_struct);

        if (rc != PMC_SUCCESS)
        {
            *err_code = FLASHLOADER_ERR_FLASH_WRITE_FAIL;
            bc_printf("Flashloader: flashloader_plat_flash_image_finalize write error %08lx\n", rc);
            return FLASHLOADER_ERR_FLASH_WRITE_FAIL;
        }
        
        flash_image_src += FLASH_LOADER_PAGE_BUF_SIZE;
        flash_image_dest +=FLASH_LOADER_PAGE_BUF_SIZE;         
    }
    
    bc_printf("...Done\n");
    return PMC_SUCCESS;    

}

/**
* @brief
*   Sends the response back to the HOST
*
* @param [in] return_code - SUCCESS or Error indication
* @param [in] ext_err_code - Extended error code
* @param [in] ext_resp_length - Extended response length
* @param [in] resp_buf - Pointer to response buffer
*
* @return
*   None.
*
*/
PUBLIC VOID flashloader_plat_send_respnse(UINT8 return_code, 
                                          UINT32 ext_err_code, 
                                          UINT32 ext_resp_length, 
                                          UINT8* resp_buf)
{
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();

    /* set the success indication */
    rsp_ptr->parms[0] = return_code;

    /* Set the extended error code */    
    rsp_ptr->parms[1] = ext_err_code;

    /* set the extended data response length */
    rsp_ptr->ext_data_len = ext_resp_length;

    /* set the no extended data flag */
    rsp_ptr->flags = (ext_resp_length > 0) ? EXP_FW_EXTENDED_DATA : EXP_FW_NO_EXTENDED_DATA;

    if (ext_resp_length > 0 )
    {
        /* Copy the result in Extended buffer*/
        memcpy(ext_data_ptr,
               resp_buf,
               ext_resp_length);
    }

    /* send the response */
    ech_oc_rsp_proc();
}



/** @} end group */



