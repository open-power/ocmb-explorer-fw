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
*     Platform-specific definitions and declarations for Serial EEPROM.
*
*******************************************************************************/


#ifndef _SEEPROM_PLAT_H
#define _SEEPROM_PLAT_H

/*
** Local Constants
*/

/* initstring information for bootstrap SEEPROM */
#define PLATFORM_SEEPROM_PORT                         0
#define PLATFORM_SEEPROM_TIME_OUT_MS                  100
#define PLATFORM_SEEPROM_PMC_RESERVE_SIZE             0x1400  /* First 5.6K is reserved for PMC. MPI is at 5.6K */
#define PLATFORM_SEEPROM_IMAGE_OFFSET                 PLATFORM_SEEPROM_PMC_RESERVE_SIZE
#define PLATFORM_SEEPROM_IMAGE_SIZE                   0x600
#define PLATFORM_SEEPROM_TABLE_START                  PLATFORM_SSID_AND_ID_SIZE
#define PLATFORM_BOARD_CONFIG_OFFSET                  (0x1A00 - PLATFORM_SEEPROM_PMC_RESERVE_SIZE)

/* Information for 64bit SEEPROM scratchpad*/
#define PLATFORM_SEEPROM_SCRATCHPAD_OFFSET            0x3240
#define PLATFORM_SEEPROM_SCRATCHPAD_SIZE              64
#define PLATFORM_SEEPROM_DEV_ID_OVERRIDE_FLAG_OFFSET  (PLATFORM_SEEPROM_SCRATCHPAD_OFFSET + 0x4)
#define PLATFORM_SEEPROM_DEV_ID_OVERRIDE_OFFSET       (PLATFORM_SEEPROM_SCRATCHPAD_OFFSET + 0x8)

/*
** Time to wait between successive SEEPROM writes (us). This should be the
** maximum of all supported SEEPROM devices, and is typically labelled as tWR or
** tW on datasheets.
*/
#define SEEPROM_PLAT_TWR_US    10000

#endif /* _SEEPROM_PLAT_H */



