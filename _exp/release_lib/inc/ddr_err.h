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
*  DESCRIPTION :
*    This file contains error code definitions and declarations for the DDR module
*    for use by external applications.
*
*  NOTES :
*
*******************************************************************************/


#ifndef _DDRW_ERR_H
#define _DDRW_ERR_H

/*
** Include Files
*/
#include <stdint.h>
#include "pmcfw_err.h"

/*
** Constants
*/

/* Error codes */

#ifndef DDR_ERR_FW_BASE
#define DDR_ERR_FW_BASE                         PMCFW_ERR_BASE_DDR_FW
#endif
#define DDR_FW_ERR_CODE_CREATE(err_suffix)      ((DDR_ERR_FW_BASE) | (err_suffix))

#ifndef DDR_SUCCESS
#define DDR_SUCCESS                             PMC_SUCCESS
#endif

/* General DDR FW error codes */
#define DDR_FW_ERR_INVALID_EYE_CAPTURE_INIT_INPUT             DDR_FW_ERR_CODE_CREATE(0x001)
#define DDR_FW_ERR_INVALID_EYE_CAPTURE_TRAIN_INPUT            DDR_FW_ERR_CODE_CREATE(0x002)
#define DDR_FW_ERR_BIST_FAIL                                  DDR_FW_ERR_CODE_CREATE(0x003)
#define DDR_FW_ERR_MRGN_BIST_FAIL                             DDR_FW_ERR_CODE_CREATE(0x004)
#define DDR_FW_ERR_MRGN_SANITY_BIST_FAIL                      DDR_FW_ERR_CODE_CREATE(0x005)
#define DDR_FW_ERR_PHY_TIMING_TAG_ERROR                       DDR_FW_ERR_CODE_CREATE(0x006)
#define DDR_FW_ERR_MRGN_INVALID_VOLTAGE_SWEEP_TYPE            DDR_FW_ERR_CODE_CREATE(0x007)
#define DDR_FW_ERR_MRGN_INVALID_VOLTAGE_MARGIN_TYPE           DDR_FW_ERR_CODE_CREATE(0x008)
#define DDR_FW_ERR_MRGN_INVALID_ALGO_TYPE                     DDR_FW_ERR_CODE_CREATE(0x009)
#define DDR_FW_ERR_PHY_ERROR_RESPONSE                         DDR_FW_ERR_CODE_CREATE(0x00A)
#define DDR_FW_ERR_CMDSVR_INVALID_ARGS                        DDR_FW_ERR_CODE_CREATE(0x00B)

/* Error codes for host interface */
#define DDR_FW_ERR_API_EYE_CAPTURE_BUFFER_SIZE                DDR_FW_ERR_CODE_CREATE(0x031)
#define DDR_FW_ERR_API_EYE_CAPTURE_SEQ_ERROR                  DDR_FW_ERR_CODE_CREATE(0x032)
#define DDR_FW_ERR_UNSUPPORTED_PHY_INIT_MODE                  DDR_FW_ERR_CODE_CREATE(0x033)
#define DDR_FW_ERR_INCORRECT_DATA_LENGTH                      DDR_FW_ERR_CODE_CREATE(0x034)
#define DDR_FW_ERR_NO_EXTENDED_DATA                           DDR_FW_ERR_CODE_CREATE(0x035)

/* Delay out of range error codes */
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_0                       DDR_FW_ERR_CODE_CREATE(0x100)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_1                       DDR_FW_ERR_CODE_CREATE(0x101)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_2                       DDR_FW_ERR_CODE_CREATE(0x102)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_3                       DDR_FW_ERR_CODE_CREATE(0x103)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_4                       DDR_FW_ERR_CODE_CREATE(0x104)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_5                       DDR_FW_ERR_CODE_CREATE(0x105)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_6                       DDR_FW_ERR_CODE_CREATE(0x106)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_7                       DDR_FW_ERR_CODE_CREATE(0x107)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_8                       DDR_FW_ERR_CODE_CREATE(0x108)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_9                       DDR_FW_ERR_CODE_CREATE(0x109)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_10                      DDR_FW_ERR_CODE_CREATE(0x110)
#define DDR_FW_ERR_DELAY_OUT_OF_RANGE_11                      DDR_FW_ERR_CODE_CREATE(0x111)

/* Register out of range error codes */
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_0                      DDR_FW_ERR_CODE_CREATE(0x200)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_1                      DDR_FW_ERR_CODE_CREATE(0x201)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_2                      DDR_FW_ERR_CODE_CREATE(0x202)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_3                      DDR_FW_ERR_CODE_CREATE(0x203)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_4                      DDR_FW_ERR_CODE_CREATE(0x204)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_5                      DDR_FW_ERR_CODE_CREATE(0x205)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_6                      DDR_FW_ERR_CODE_CREATE(0x206)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_7                      DDR_FW_ERR_CODE_CREATE(0x207)
#define DDR_FW_ERR_PHY_REG_RANGE_ERROR_8                      DDR_FW_ERR_CODE_CREATE(0x208)

/* MDS Training error response */
#define DDR_FW_ERR_TRAIN_BIST_PATTERN_LIST_OUT_OF_RANGE       DDR_FW_ERR_CODE_CREATE(0x300)
#define DDR_FW_ERR_TRAIN_READ_PATTERN_OUT_OF_RANGE            DDR_FW_ERR_CODE_CREATE(0x310)
#define DDR_FW_ERR_TRAIN_WRITE_LEVEL_INIT_BIST_FAIL           DDR_FW_ERR_CODE_CREATE(0x311)
#define DDR_FW_ERR_TRAIN_WRITE_LEVEL_FWD_SWEEP_OUT_OF_RANGE   DDR_FW_ERR_CODE_CREATE(0x312)
#define DDR_FW_ERR_TRAIN_WRITE_LEVEL_BWD_SWEEP_OUT_OF_RANGE   DDR_FW_ERR_CODE_CREATE(0x313)

#define DDR_FW_ERR_TRAIN_PATTERN_WRITE_INIT_BIST_FAIL         DDR_FW_ERR_CODE_CREATE(0x414)
#define DDR_FW_ERR_TRAIN_PATTERN_WRITE_FWD_SWEEP_OUT_OF_RANGE DDR_FW_ERR_CODE_CREATE(0x415)
#define DDR_FW_ERR_TRAIN_PATTERN_WRITE_BWD_SWEEP_OUT_OF_RANGE DDR_FW_ERR_CODE_CREATE(0x416)

#endif /* _DDRW_ERR_H */



