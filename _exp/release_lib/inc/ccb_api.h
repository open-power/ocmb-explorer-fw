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
* @defgroup CCB CCB: Character Circular buffer
* @brief
* This module implements a  circular buffer to store printf logs to DDR memory.
*
* The CCB module implements a circular buffer in DDR memory to store
* print logs that are usually output to an UART. The module stores these
* character logs in memory and wraps around on reaching the end of the
* buffer. After each write to the buffer, the module writes a specific
* pattern to memory. A parser tool can look for this pattern to locate the
* beginning and end of the log messages.
*
* The implementation of the circular buffer is different from a typical
* producer - consumer problem. There is only a producer index to keep track
* of the next write location. The reader for the buffer is typically
* the RAID stack (RS). The RS gets the address and size of the CCB
* at start up. In case of a crash or firmware lockup, the RS blindly
* retrieves the contents of the memory for post-crash analysis. Hence
* the design does not include a consumer index - instead provides a way
* for the RS to identify the beginning and end of log messages.
*
* @{
* @file
* @brief 
*    This file contains the public CCB API and definitions
*    for use by applications.
* 
* @note  
*/

#ifndef _CCB_API_H
#define _CCB_API_H

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
* Macros
*/

/*
* Enumerated Types
*/

/*
* Constants
*/

/*
* Macro Definitions
*/
/**
* @brief
*   This macro shall be used to creates error codes for the CCB module.
*
*   Each CCB error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | CCB error base (16-bit) | error code suffix (12-bit)]
*
*   where
*     CCB error base   - 16-bit PMCFW_ERR_BASE_CCB, defined in "pmcfw_err.h".
*     error code suffix - 12-bit specific error code suffix input to the macro.
*
*   Note:
*   - For success, the return code = PMC_SUCCESS.
*
* @param [in] err_suffix - 12-bit error suffix.  See above for details.
*
* @return
*   Error code in format described above.
* @hideinitializer
*/
#define CCB_ERR_CODE_CREATE(err_suffix)    ((PMCFW_ERR_BASE_CCB) | (err_suffix))
#define CCB_ERR_CFG_NULL                     CCB_ERR_CODE_CREATE(0x001)
#define CCB_ERR_CFG_NOT_FOUND                CCB_ERR_CODE_CREATE(0x002)
#define CCB_ERR_INVALID_SIZE                 CCB_ERR_CODE_CREATE(0x003)
#define CCB_ERR_DUPLICATE_ALLOCATION         CCB_ERR_CODE_CREATE(0x004)
#define CCB_ERR_INVALID_CTRL_PTR_1           CCB_ERR_CODE_CREATE(0x005)
#define CCB_ERR_INVALID_CTRL_PTR_2           CCB_ERR_CODE_CREATE(0x006)
#define CCB_ERR_INVALID_CTRL_PTR_3           CCB_ERR_CODE_CREATE(0x007)
#define CCB_ERR_INVALID_CTRL_PTR_4           CCB_ERR_CODE_CREATE(0x008)

/*
* Structures and Unions
*/

/*
* Function Prototypes
*/

EXTERN void ccb_init(UINT32 buffer_count);
EXTERN void *ccb_buffer_init(UINT32 buffer_size);    
EXTERN void ccb_put(void *ccb_ctrl_ptr, const CHAR* data_buffer, UINT32 data_size);
EXTERN UINT32 ccb_info_get(void *ccb_ctrl_ptr, void **addr_pptr);
EXTERN void ccb_copy(void *dst_ccb_ctrl_ptr, void *src_ccb_ctrl_ptr);
EXTERN UINT32 ccb_get(void *ccb_ctrl_ptr, CHAR *dst_ptr, UINT32 num_bytes_requested);
EXTERN void ccb_clear(void *ccb_ctrl_ptr);

#endif /* _CCB_API_H */

/** @} end addtogroup */


