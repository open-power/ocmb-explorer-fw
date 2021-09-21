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
*     Platform-specific definitions and declarations for FLM.
*
*******************************************************************************/


#ifndef _FLM_PLAT_H
#define _FLM_PLAT_H

/*
** Local Constants
*/

/* 
** Set to 1 to enable SPI flash support in FLM, or 0 to disable. Enabling SPI 
** support disables support for parallel flash devices (including CFI querying), 
** while disabling SPI support enables support for parallel flash devices.
*/
#define FLM_SPI 1

/* SPI flash port and chip select. These must be defined when FLM_SPI is 1. */
#define FLM_SPI_PORT 0
#define FLM_SPI_CS   0

/* Timeouts */
#define FLM_SECTOR_ERASE_POLL_PERIOD_US      (1000) /* 1 ms */
#define FLM_SECTOR_ERASE_POLL_MAX_RETRIES    (FLM_SECTOR_ERASE_POLL_PERIOD_US * 20) /* period * retries = timeout: 3 seconds */
#define FLM_PROGRAM_TIMEOUT_US               (FLM_SECTOR_ERASE_POLL_PERIOD_US * 10)

/* Array sizes */
#define FLM_NUM_SUPPORTED_CMD_INTERFACES     (2)

/* FLASH width */
#if !defined(FLM_FLASH_WIDTH_8_BITS)
#define FLM_FLASH_WIDTH_8_BITS
#undef FLM_FLASH_WIDTH_16_BITS
#endif

#endif /* _FLM_PLAT_H */



