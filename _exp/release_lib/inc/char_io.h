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
* @addtogroup CHAR_IO
* @{
* @file
* @brief
*   char_io module header file.
*
* @note
*/

#ifndef _CHAR_IO_H
#define _CHAR_IO_H

/*
** Include Files
*/
#include "pmcfw_types.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/* char_io channels */
#define CHAR_IO_CHANNEL_ID_RUNTIME          0
#define CHAR_IO_CHANNEL_ID_CRASH            1
#define CHAR_IO_CHANNEL_ID_UNINITIALIZED    0xFF
#define CHAR_IO_NUM_CHANNELS                2

/*
** Macro Definitions
*/

/**
* @brief
*   This macro shall be used to creates error codes for the CHAR_IO module.
*
*   Each GAP error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | EGSM error base (16-bit) | error code suffix (12-bit)]
*
*   where
*     GAP error base   - 16-bit PMCFW_ERR_BASE_CHAR_IO, defined in
*     "pmcfw_mid.h". error code suffix - 12-bit specific error code suffix
*     input to the macro.
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
#define CHAR_IO_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_PRINTF) | (err_suffix))
#define CHAR_IO_ERR_BAD_CHANNEL_ID           CHAR_IO_ERR_CODE_CREATE(0x001)

/*
** Structures and Unions
*/

/* Call back function prototype for registering char_io_put functions*/
typedef void (*char_io_put_fn_ptr_type)(const UINT32 buffer_size, 
                                        const UCHAR *buffer_ptr);

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN void char_io_init(UINT32 runtime_ccb_size,
                         UINT32 crash_ccb_size);
EXTERN void char_io_output_buffer_put_fn_register(char_io_put_fn_ptr_type runtime_put_fn_ptr,
                                                  char_io_put_fn_ptr_type crash_put_fn_ptr,
                                                  BOOL copy);
EXTERN void char_io_put(UINT8 channel_id,
                        const char* buffer_ptr,
                        UINT32 buffer_size);
EXTERN void* char_io_ccb_ctrl_get(UINT8 channel_id);
EXTERN UINT32 char_io_loc_buffer_info_get(UINT8 channel_id, 
                                          void **addr_pptr);

#endif /* _CHAR_IO_H */



