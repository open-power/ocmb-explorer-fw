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



#ifndef _SPI_FLASH_PLAT_CFG_H
#define _SPI_FLASH_PLAT_CFG_H

/*
** Set to 1 to enable ensuring the S25FS256S FLASH device is configured for
** uniform 64Kb sectors throughout the device. The factory default of this
** device has eight 4Kb parameter sectors at the start of FLASH. If this
** define is set to 1, firmware will read the device non-volatile, OTP register
** and if the bit to configure all uniform 64Kb sectors is not set, it will
** set it.
** 
** Set to 0 to disable this check. Presumably this bit would be set once in
** production, and may not need to be checked in regular applications.
*/
#define SPI_FLASH_S25FS256S_UNIFORM_64KB_SECTOR_WRITE     0

/*
** Set to 1 to ignore the byte in the FLASH device ID returned from a READID
** opcode that describes the memory capacity of the FLASH device when
** auto-detecting the type of populated FLASH device.
** 
** Set to 0 to use the full device ID returned from a READID opcode when
** auto-detecting the type of populated FLASH device.
*/
#define SPI_FLASH_DEVICE_MEMORY_CAPACITY_BYTE_IGNORE      0

#endif /* _SPI_FLASH_PLAT_CFG_H */



