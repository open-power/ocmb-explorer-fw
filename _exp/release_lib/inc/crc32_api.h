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
* @defgroup CRC32 CRC32: CRC-32 Generator
* @brief 
*  This module contains function(s) to generate CRC-32 checksums.
*
*
* @{  
* @file 
* @brief 
*   This file contains CRC-32 generator function prototype(s) and declaration(s).
* 
* @note  
*/

#ifndef _CRC32_API_H
#define _CRC32_API_H

#include "pmcfw_types.h"

/*
** Function Prototypes and Pointers to Functions in RAM
**
** To accommodate PIC code executing in SPI flash and non-PIC code executing in
** RAM, the functions in RAM are accessed through pointers. The changes that were made:
**
**      - original function name:     $type func_name($type, $type)
**        changed with prepended '_': $type _func_name($type, $type)
**      - define a typedef for the function pointer: typedef $type (_func_name_fn_ptr)($type, $type)
**      - public function pointers initialized in source code files:
**        PUBLIC _func_name_fn_ptr (*func_name_ptr) =  _func_name;
**      - private function pointers initialized in source code files:
**        PRIVATE _func_name_fn_ptr (*func_name_ptr) = _func_name;
**      - for public functions new define for original function name in header file:
**        #define func_name (*func_name_ptr)
**      - for private functions new define for original function name in source code file:
**        #define func_name (*func_name_ptr)
*/
typedef UINT32 (*pmc_crc32_fn_ptr_type)(const UINT8 *msg_ptr, UINT32 byte_cnt, UINT32 oldchksum, BOOL init, BOOL last);
EXTERN pmc_crc32_fn_ptr_type pmc_crc32_fn_ptr;
#define pmc_crc32 (*pmc_crc32_fn_ptr)


#endif /* _CRC32_API_H */
/** @} end addtogroup */


