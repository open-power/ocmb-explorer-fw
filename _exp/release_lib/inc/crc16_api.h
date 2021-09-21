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


/**
* @defgroup CRC16 CRC16: CRC-16 Generator
* @brief 
*  This module contains function(s) to generate CRC-16 checksums.
*
*   The CRC-16 is of the following characteristics:
*                      Width:              16-bit
*                      Poly:               0x8005
*                      Init value:         0x0000
*                      Reflected Input:    TRUE
*                      Reflected Output:   TRUE
*                      Final XOR Out:      0x0000
*                      Check:              0xBB3D  using the test ASCII string 
*                                                  "123456789", not include the
*                                                  NULL character.
*
* @{  
* @file 
* @brief 
*   This file contains CRC-16 generator function prototype(s) and declaration(s).
* 
* @note  
*/

#ifndef _CRC16_API_H
#define _CRC16_API_H

#include "pmcfw_types.h"

/*
** CRC-16 generator function prototype
*/
EXTERN UINT16 crc16(UINT8 *msg_ptr, UINT32 byte_cnt, UINT16 oldchksum, BOOL init);


#endif /* _CRC16_API_H */
/** @} end addtogroup */


