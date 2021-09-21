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
* @defgroup UTILITY UTILITY: Common Utilities
* @{ 
* @file
* @brief 
*   PMC Firmware Standard Error Codes and PMCFW_ASSERT function.
*
* @note
*     None.
*
*/ 
#ifndef PMCFW_ERR_H
#define PMCFW_ERR_H

#include "pmcfw_types.h"
#include "pmcfw_mid.h"

/*
* Constant Declarations
*/

/* Success code */

#ifndef PMC_SUCCESS
#define PMC_SUCCESS                             (0x00000000)
#endif

/** Create type declarations for error codes - useful when creating function signatures or for exception handling */
typedef INT32       PMCFW_ERROR;

/* Common error codes */
#define PMCFW_ERR_FAIL                          (0x00000001)   /* Use this error code as an error code placeholder.
                                                                  Any occurrances of PMC_ERR_FAIL should be removed
                                                                  for production code. */
#define PMCFW_ERR_INVALID_PARAMETERS            (0x00000002)
#define PMCFW_ERR_FILE_OPEN_FAILED              (0x00000003)
#define PMCFW_ERR_FILE_READ_FAILED              (0x00000004)
#define PMCFW_ERR_FILE_WRITE_FAILED             (0x00000005)
#define PMCFW_ERR_FILE_CLOSE_FAILED             (0x00000006)
#define PMCFW_ERR_MALLOC_FAILED                 (0x00000007)
#define PMCFW_ERR_MEMORY_FAILED                 (0x00000008)
#define PMCFW_ERR_COMLINK_FAILED                (0x00000009)
#define PMCFW_ERR_TIMER_FAILED                  (0x0000000a)
#define PMCFW_ERR_INVALID_PTR                   (0x0000000b)
#define PMCFW_ERR_NUMBER_OUT_OF_RANGE           (0x0000000c)
#define PMCFW_ERR_FILE_SEEK_FAILED              (0x0000000d)
#define PMCFW_ERR_FILE_NAMED_VALUE_MATCH_FAILED (0x0000000e)
#define PMCFW_ERR_MEMORY_ALIGNMENT              (0x0000000f)

#define PMCFW_ERR_UNKNOWN_ERROR                 (0x00000fff)

/*
* The following performs the assert operation for various environments.  The error code is used as a comparison
* and if set to PMC_SUCCESS the assert will not occur.  Allowing the assert to happen as part of the #define
* results in the proper line and file info output if it is not output in the pmcfw_assert_function().
*/
#ifdef __ghs__
/* __BASE__ is a GHS compiler macro which gives the filename without the path */
#define FILENAME __BASE__
#else
#define FILENAME __FILE__
#endif
#define PMCFW_ASSERT(condition, error_code) \
                    {if (!(condition)) {pmcfw_assert_function(error_code, FILENAME, __LINE__);}}

/* For functions executing in RAM to access pmcfw_assert_function() in flash via a function pointer */
typedef VOID (*pmcfw_assert_function_fn_ptr)(PMCFW_ERROR error_code, CHAR *file_ptr, UINT32 line);
EXTERN pmcfw_assert_function_fn_ptr pmcfw_assert_function_ptr;
#define PMCFW_ASSERT_RAM(condition, error_code) \
                        {if (!(condition)) {(*pmcfw_assert_function_ptr)(error_code, FILENAME, __LINE__);}}

/**
* @brief
*   This function handles the firmware assert.
*
*   @param[in] error_id    - error id.
*   @param[in] file        - file name of the FW assert error
*   @param[in] line        - line number of the FW assert error.
*
* @returns None
*
*/
EXTERN void pmcfw_assert_function(PMCFW_ERROR error_id, CHAR *file, UINT32 line);

#endif /* PMCFW_ERR_H */
/** @} end addtogroup */



