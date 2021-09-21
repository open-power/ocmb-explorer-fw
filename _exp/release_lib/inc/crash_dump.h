/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2018, 2019, 2020 Microchip Technology Inc. All rights reserved. 
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
*     crash_dump.h
*
*   NOTES:
*
*****************************************************************************/
#ifndef _CRASH_DUMP_H
#define _CRASH_DUMP_H


/*
** Include Files
*/
#include "pmcfw_common.h"

/*
** Constants
*/

#define SECTION_NAME_LEN        32
#define NUM_CRASH_DUMP_HANDLERS 10
#define CRASH_DUMP_HEADER_KEY   0xC7544EAD

/*
** Macro definitions
*/
/**
* @brief
*   This macro shall be used to creates error codes for the Crash Dump module.
*
*   Each Crash Dump error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | CCB error base (16-bit) | error code suffix (12-bit)]
*
*   where
*     CCB error base   - 16-bit PMCFW_ERR_BASE_CRASH_DUMP, defined in "pmcfw_err.h".
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
#define CRASH_DUMP_ERR_CODE_CREATE(err_suffix)    ((PMCFW_ERR_BASE_CRASH_DUMP) | (err_suffix))
#define CRASH_DUMP_ARRAY_FULL                     CRASH_DUMP_ERR_CODE_CREATE(0x001)
#define CRASH_DUMP_ERR_SPI_ALIGN                  CRASH_DUMP_ERR_CODE_CREATE(0x002)
#define CRASH_DUMP_ERR_SPI_ERASE                  CRASH_DUMP_ERR_CODE_CREATE(0x003)
#define CRASH_DUMP_ERR_SPI_ERASE_TIMEOUT          CRASH_DUMP_ERR_CODE_CREATE(0x003)
#define CRASH_DUMP_ERR_SPI_WRITE                  CRASH_DUMP_ERR_CODE_CREATE(0x004)
#define CRASH_DUMP_ERR_SPI_WRITE_TOO_LARGE        CRASH_DUMP_ERR_CODE_CREATE(0x005)
#define CRASH_DUMP_ERR_SPI_FULL                   CRASH_DUMP_ERR_CODE_CREATE(0x006)
#define CRASH_DUMP_CHECK_SPI_FLASH_FULL           CRASH_DUMP_ERR_CODE_CREATE(0x007)
#define CRASH_DUMP_HEADER_TOO_LARGE               CRASH_DUMP_ERR_CODE_CREATE(0x008)
#define CRASH_DUMP_INVALID_HEADER_IDX             CRASH_DUMP_ERR_CODE_CREATE(0x009)
#define CRASH_DUMP_ERR_GET_OFFSET_TOO_LARGE       CRASH_DUMP_ERR_CODE_CREATE(0x00A)
#define CRASH_DUMP_ERR_GET_SPI_OVERFLOW           CRASH_DUMP_ERR_CODE_CREATE(0x00B)
#define CRASH_DUMP_ERR_CRASH_DUMP_A_ADDR_ALIGN    CRASH_DUMP_ERR_CODE_CREATE(0x00C)
#define CRASH_DUMP_ERR_CRASH_DUMP_B_ADDR_ALIGN    CRASH_DUMP_ERR_CODE_CREATE(0x00D)
#define CRASH_DUMP_ERR_SPI_DEV_INFO_GET_ERR       CRASH_DUMP_ERR_CODE_CREATE(0x00E)

/*
** Type declarations
*/

typedef void crash_dump_function_ptr(void);


/**
* @brief Enumerated crash dump sets 
*  
* @note 
*   CANNOT exceed more than 4 crash dump sets because the CD
*   flash partition is 256KB in size and each CD set can support
*   a maximium of 64KB.
*/
typedef enum
{
    CRASH_DUMP_SET_0,
    CRASH_DUMP_SET_1,
    CRASH_DUMP_SET_2,
    CRASH_DUMP_SET_3,
    CRASH_DUMP_SET_ID_MAX
} crash_dump_set_id_enum;

/**
* @brief Enumerated type of the cash dump types
*/
typedef enum
{
    CRASH_DUMP_ASCII,       /**< ASCII crash dump type */
    CRASH_DUMP_RAW,         /**< RAW binary crash dump type */
    NUM_CRASH_DUMP_TYPES,   /**< Number of crash dump types */
} crash_dump_type;

/*
** Structures and Unions
*/

/**
* @brief Structure of meta data for a registered crash dump
*/
typedef struct crash_dump_meta_data
{
    CHAR name[SECTION_NAME_LEN];             /**< Name of the crash dump section */
    crash_dump_function_ptr *dump_fcn_ptr;   /**< Function pointer for the function to call to dump data */
    crash_dump_type type;                    /**< Type of data dumped by this crash section */
    UINT32 size;                             /**< Alloted space for this crash dump section */
} crash_dump_meta_data;

/**
* @brief Structure for a crash dump header entry
*/
typedef struct crash_dump_header
{
    UINT32 header_key;            /**< Unique UINT32 key to identify start of a crash dump header */
    CHAR name[SECTION_NAME_LEN];  /**< Name of crash dump section */
    UINT32 type;                  /**< Type of data for the crash dump */
    UINT32 start_offset;          /**< Crash dump data start offset into crash dump data section */
    UINT32 size;                  /**< Size of the crash dump data for this section */
    UINT32 reserved[4];           /**< Align to exactly 2 cache lines for easier writing to ECC enabled flash. */  
} crash_dump_header;


/*
** Function Prototypes
*/

EXTERN void crash_dump_init(void);
EXTERN void crash_dump_put(UINT32 buffer_size, void *src_buffer_ptr);
EXTERN PMCFW_ERROR crash_dump_register(crash_dump_set_id_enum cd_set_id, 
                                       char *name, 
                                       crash_dump_function_ptr *dump_fcn, 
                                       crash_dump_type type, 
                                       UINT32 dump_size);
EXTERN void crash_dump_to_rom(void);
EXTERN void crash_dump_print(void);

#endif /* _CRASH_DUMP_H */
