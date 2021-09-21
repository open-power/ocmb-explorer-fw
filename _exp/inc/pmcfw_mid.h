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
* DESCRIPTION:  Module IDs
*   This file contains module IDs and corresponding module error base
*   definitions.  When you add a module ID, please also add the base error
*   code for the module ID.
*
*   Each platform has its own copy of pmcfw_mid.h; however the module
*   ID list may be slightly different depending on which modules are
*   actually used by the platform.  A copy of this file will exist in
*   each project however the list of MIDs and values will likely be
*   different.
*
*   This file is used by the logging facility (_log.h/gen_log_def.sh),
*   by the module error code definitions, and to form message types:
*   1. Logging:
*      The values for PMCFW_MID_xxxx in this file are output
*      by gen_log_def.sh in the osf_log_def.data file.  These
*      values are used by the pmc_log.exe utility to translate
*      log values to human-readable text.  Note that the
*      gen_log_def.sh script keys off of the string format for
*      each PMCFW_MID definition in this file.
*
*   2. Error codes:
*      Each module has a error base definition defined in
*      this file.  Each module defines a set of module-specific
*      error codes (using the base code defined in this file).
*      The module-specific error codes are used by the assert
*      handler.  The MID in the error code identifies which module
*      had the asserted line.
*
*   3. Message types:
*      OSF message type values are created using the MID.
*
* NOTES:
*
*******************************************************************************/



/*---- Compile Options -----------------------------------------------------*/
#ifndef _PMCFW_MID_H
#define _PMCFW_MID_H

/*---- Include Files -------------------------------------------------------*/
#include "pmcfw_types.h"

/*---- Constant and Type Declarations --------------------------------------*/

/* Common error codes */
#define PMCFW_ERR_MID_OFFSET                    (12)        /* Offset for Module ID in error code               */
#define PMCFW_ERR_MID_CODE_MASK                 (0x00000FFF)/* Bit mask for each modules set of error codes     */

/*
* Firmware module IDs.  Maximum size is 16-bit.
* It is used to form the PMCFW_ERROR for each module.
* Should also use this to form message types, log codes, etc.
*/
#define PMCFW_MID_APPLICATION_SPECIFIC          (0x0001)    /* Use this module ID for top level software code   */
#define PMCFW_MID_UART                          (0x0002)    /* UART code                                        */
#define PMCFW_MID_TWI                           (0x0003)    /* TWI code                                         */
#define PMCFW_MID_BOOT                          (0x0004)    /* Boot loader code                                 */
#define PMCFW_MID_CMDSVR                        (0x0005)    /* UART/TWI Command Server                          */
#define PMCFW_MID_FLM                           (0x0006)    /* FLM code                                         */
#define PMCFW_MID_WDG                           (0x0007)    /* Watchdog                                         */
#define PMCFW_MID_SEEP                          (0x0008)    /* Serial EEPROM driver module                      */
#define PMCFW_MID_DDR_TOOLBOX                   (0x0009)    /* DDR Toolbox                                      */
#define PMCFW_MID_CPUHAL                        (0x000A)    /* CPUHAL module                                    */
#define PMCFW_MID_RESET                         (0x000B)    /* reset module                                     */
#define PMCFW_MID_HTWI                          (0x000C)    /* HTWI Module                                      */
#define PMCFW_MID_TOP                           (0x000D)    /* TOP Module                                       */
#define PMCFW_MID_FATAL                         (0x000E)    /* Fatal Error Handler                              */
#define PMCFW_MID_LOG_APP                       (0x000F)    /* LOG Module (FW's application log)                */
#define PMCFW_MID_MEM                           (0x0010)    /* MEM Module                                       */
#define PMCFW_MID_PRINTF                        (0x0011)    /* Printf module                                    */
#define PMCFW_MID_CICINT                        (0x0012)    /* CICINT module                                    */
#define PMCFW_MID_GPIO                          (0x0013)    /* GPIO code                                        */
#define PMCFW_MID_SPBC                          (0x0014)    /* SPBC                                             */
#define PMCFW_MID_DCSU                          (0x0015)    /* DCSU                                             */
#define PMCFW_MID_SPI                           (0x0016)    /* SPI                                              */
#define PMCFW_MID_SPB                           (0x0017)    /* SPB                                              */
#define PMCFW_MID_SPI_FLASH                     (0x0018)    /* SPI flash                                        */
#define PMCFW_MID_SPB_SPI                       (0x0019)    /* SPB SPI                                          */
#define PMCFW_MID_SPB_TWI                       (0x001A)    /* SPB TWI                                          */
#define PMCFW_MID_CACHE_EXC                     (0x001B)    /* Cache Exception handler                          */
#define PMCFW_MID_TSH                           (0x001C)    /* Tiny Shell                                       */
#define PMCFW_MID_ECH                           (0x001D)    /* Explorer Command Handler module                  */
#define PMCFW_MID_OCMB                          (0x001E)    /* Explorer OCMB module                             */
#define PMCFW_MID_NVFS                          (0x001F)    /* NVRAM file system                                */
#define PMCFW_MID_APPFW                         (0x0020)    /* Explorer Application FW module                   */ 
#define PMCFW_MID_EXP_DDR_CTRLR                 (0x0021)    /* Explorer DDR Controller module                   */
#define PMCFW_MID_APPFW_DDR                     (0x0022)    /* Explorer APP FW DDR module                       */
#define PMCFW_MID_CCB                           (0x0023)    /* Circular buffer modue.                           */
#define PMCFW_MID_CRASH_DUMP                    (0x0024)    /* Crash dump module                                */
#define PMCFW_MID_TEMP                          (0x0025)    /* Temperature sensor module                        */
#define PMCFW_MID_OCMB_API                      (0x0026)    /* OCMB API                                         */
#define PMCFW_MID_DDR_FW                        (0x0027)    /* FW DDR PHY module                                */
#define PMCFW_MOD_NUM                           (0x0028)    /* The number of modules PMCFW supported
                                                             * Once a new module is added, this number must be
                                                             * increased accordingly                            */




/* Error code bases, these are created based on the firmware module ID (PMCFW_MID_xxx) defined above. */
#define PMCFW_ERR_BASE_APPLICATION_SPECIFIC     (PMCFW_MID_APPLICATION_SPECIFIC << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_UART                     (PMCFW_MID_UART                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_TWI                      (PMCFW_MID_TWI                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_BOOT                     (PMCFW_MID_BOOT                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_CMDSVR                   (PMCFW_MID_CMDSVR               << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_FLM                      (PMCFW_MID_FLM                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_WDG                      (PMCFW_MID_WDG                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_SEEP                     (PMCFW_MID_SEEP                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_DDR_TOOLBOX              (PMCFW_MID_DDR_TOOLBOX          << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_RESET                    (PMCFW_MID_RESET                << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_HTWI                     (PMCFW_MID_HTWI                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_TOP                      (PMCFW_MID_TOP                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_CPUHAL                   (PMCFW_MID_CPUHAL               << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_FATAL                    (PMCFW_MID_FATAL                << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_LOG_APP                  (PMCFW_MID_LOG_APP              << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_MEM                      (PMCFW_MID_MEM                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_PRINTF                   (PMCFW_MID_PRINTF               << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_CICINT                   (PMCFW_MID_CICINT               << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_GPIO                     (PMCFW_MID_GPIO                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_SPBC                     (PMCFW_MID_SPBC                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_DCSU                     (PMCFW_MID_DCSU                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_SPI                      (PMCFW_MID_SPI                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_SPB                      (PMCFW_MID_SPB                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_SPI_FLASH                (PMCFW_MID_SPI_FLASH            << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_SPB_SPI                  (PMCFW_MID_SPB_SPI              << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_SPB_TWI                  (PMCFW_MID_SPB_TWI              << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_CACHE_EXC                (PMCFW_MID_CACHE_EXC            << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_TSH                      (PMCFW_MID_TSH                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_ECH                      (PMCFW_MID_ECH                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_OCMB                     (PMCFW_MID_OCMB                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_APPFW                    (PMCFW_MID_APPFW                << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_NVFS                     (PMCFW_MID_NVFS                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_EXP_DDR_CTRLR            (PMCFW_MID_EXP_DDR_CTRLR        << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_APPFW_DDR                (PMCFW_MID_APPFW_DDR            << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_CCB                      (PMCFW_MID_CCB                  << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_CRASH_DUMP               (PMCFW_MID_CRASH_DUMP           << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_TEMP                     (PMCFW_MID_TEMP                 << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_OCMB_API                 (PMCFW_MID_OCMB_API             << PMCFW_ERR_MID_OFFSET)
#define PMCFW_ERR_BASE_DDR_FW                   (PMCFW_MID_DDR_FW               << PMCFW_ERR_MID_OFFSET)

/*
** Macros
*/
#define PMCFW_MOD_ID_FRM_ERROR_CODE_GET(error_code)  (error_code >> PMCFW_ERR_MID_OFFSET)

/*
** Public Function Declarations
*/
EXTERN const CHAR * pmcfw_mid_module_string_get(const UINT32 module_id);

#endif /* _PMCFW_MID_H */




