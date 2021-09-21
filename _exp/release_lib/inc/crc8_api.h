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
* @defgroup CRC8 CRC8: CRC-8 Generator
* @brief 
*  This module contains function(s) to generate CRC-8 checksums.
*
* @{  
* @file 
* @brief 
*   This file contains CRC-8 generator function prototype(s) and declaration(s).
* 
* @note  
*/

#ifndef _CRC8_API_H
#define _CRC8_API_H

#include "pmcfw_types.h"

/*
** CRC-8 generator function prototype
*/
EXTERN UINT8 crc8_0107(UINT8 *msg_ptr, UINT32 byte_cnt, UINT8 oldchksum, BOOL init);


#endif /* _CRC8_API_H */

/** @} end addtogroup */


