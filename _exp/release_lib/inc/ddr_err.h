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

#ifndef DDR_ERR_BASE
#define DDR_ERR_BASE                            PMCFW_ERR_BASE_DDR
#endif
#define DDR_ERR_CODE_CREATE(err_suffix)         ((DDR_ERR_BASE) | (err_suffix))

#ifndef DDR_SUCCESS
#define DDR_SUCCESS                             PMC_SUCCESS
#endif

#define DDR_ERR_GEN_BAD_PTR                     DDR_ERR_CODE_CREATE(0x000)
#define DDR_ERR_GEN_DIV_ZERO                    DDR_ERR_CODE_CREATE(0x001)
#define DDR_ERR_GEN_TIMEOUT                     DDR_ERR_CODE_CREATE(0x002)
#define DDR_ERR_GEN_FREQ_SUPP                   DDR_ERR_CODE_CREATE(0x003)
#define DDR_ERR_UNSUPP_CAS_LAT                  DDR_ERR_CODE_CREATE(0x004)
#define DDR_ERR_UNSUPP_CAS_WR_LAT               DDR_ERR_CODE_CREATE(0x005)
#define DDR_ERR_UNSUPP_NUM_RANK                 DDR_ERR_CODE_CREATE(0x006)
#define DDR_ERR_UNSUPP_DIMM                     DDR_ERR_CODE_CREATE(0x007)
#define DDR_ERR_UNSUPP_DATA_BITS                DDR_ERR_CODE_CREATE(0x008)
#define DDR_ERR_UNSUPP_BANK_SIZE                DDR_ERR_CODE_CREATE(0x009)
#define DDR_ERR_UNSUPP_SDRAM_WIDTH              DDR_ERR_CODE_CREATE(0x00A)
#define DDR_ERR_UNSUPP_DIVISOR                  DDR_ERR_CODE_CREATE(0x00B)
#define DDR_ERR_UNSUPP_MTB                      DDR_ERR_CODE_CREATE(0x00C)
#define DDR_ERR_UNSUPP_FTB                      DDR_ERR_CODE_CREATE(0x00D)
#define DDR_ERR_UNSUPP_HIGHTEMP                 DDR_ERR_CODE_CREATE(0x00E)
#define DDR_ERR_CALIB                           DDR_ERR_CODE_CREATE(0x00F)

#define DDR_ERR_WRONG_MAIL                      DDR_ERR_CODE_CREATE(0x010)
#define DDR_ERR_DELAY_OUT_OF_RANGE              DDR_ERR_CODE_CREATE(0x011)
#define DDR_ERR_INVALID_MS_BLOCK                DDR_ERR_CODE_CREATE(0x012)
#define DDR_ERR_RANGE_ERROR                     DDR_ERR_CODE_CREATE(0x013)
#define DDR_ERR_INVALID_INPUT                   DDR_ERR_CODE_CREATE(0x014)

#define DDR_ERR_API_RESET_PTR_NULL              DDR_ERR_CODE_CREATE(0x030)
#define DDR_ERR_API_EYE_CAPTURE_BUFFER_SIZE     DDR_ERR_CODE_CREATE(0x031)
#define DDR_ERR_API_EYE_CAPTURE_SEQ_ERROR       DDR_ERR_CODE_CREATE(0x032)
#define DDR_ERR_UNSUPPORTED_PHY_INIT_MODE       DDR_ERR_CODE_CREATE(0x033)
#define DDR_ERR_INCORRECT_DATA_LENGTH           DDR_ERR_CODE_CREATE(0x034)

#define DDR_ERR_PHY_WRITE_LEVELING              DDR_ERR_CODE_CREATE(0x050)
#define DDR_ERR_PHY_GATE_TRAINING               DDR_ERR_CODE_CREATE(0x051)
#define DDR_ERR_PHY_WL_ADJ                      DDR_ERR_CODE_CREATE(0x052)
#define DDR_ERR_PHY_TIMING_TAG_ERROR            DDR_ERR_CODE_CREATE(0x053)
#define DDR_ERR_PHY_REG_RANGE_ERROR             DDR_ERR_CODE_CREATE(0x054)
#define DDR_ERR_PHY_INLD_TWR                    DDR_ERR_CODE_CREATE(0x055)
#define DDR_ERR_PHY_MRLTRAINING                 DDR_ERR_CODE_CREATE(0x056)
#define DDR_ERR_PHY_ERROR_RESPONSE              DDR_ERR_CODE_CREATE(0x057)

#define DDR_ERR_SEQ_INVLD_PVT_TRIGGER           DDR_ERR_CODE_CREATE(0x061)

#define DDR_ERR_MRGN_SANITY_BIST_FAIL           DDR_ERR_CODE_CREATE(0x090)
#define DDR_ERR_MRGN_RANGE_ERROR                DDR_ERR_CODE_CREATE(0x091)
#define DDR_ERR_MRGN_INVALID_TYPE               DDR_ERR_CODE_CREATE(0x092)

#define DDR_ERR_CTRL_SELF_REFRESH               DDR_ERR_CODE_CREATE(0x0A0)
#define DDR_ERR_CTRL_RANK_ERR                   DDR_ERR_CODE_CREATE(0x0A1)
#define DDR_ERR_CTRL_MRS_TIMEOUT                DDR_ERR_CODE_CREATE(0x0A2)
#define DDR_ERR_CTRL_LANE_SELECT_ERROR          DDR_ERR_CODE_CREATE(0x0A3)

#define DDR_ERR_CMDSVR_INVALID_ARGS             DDR_ERR_CODE_CREATE(0x0B0)
#define DDR_ERR_CMDSVR_MEM_TEST_ERROR           DDR_ERR_CODE_CREATE(0x0B1)
#define DDR_ERR_CMDSVR_DATA_COMPLETE_ERROR      DDR_ERR_CODE_CREATE(0x0B2)

#define DDR_ERR_PMU_MAJORMSG_TRAINING_FAILED        DDR_ERR_CODE_CREATE(0x100)
#define DDR_ERR_PMU_MAJORMSG_UNKNOWN                DDR_ERR_CODE_CREATE(0x101)
#define DDR_ERR_PMU_MAJORMSG_UNKNOWN_STRINGINDEX    DDR_ERR_CODE_CREATE(0x102)
#define DDR_ERR_PMU_MAJORMSG_UNSUP_STREAM_MSG_SIZE  DDR_ERR_CODE_CREATE(0x103)
#define DDR_ERR_PMU_MAJORMSG_INITIALIZATION_END     DDR_ERR_CODE_CREATE(0x104)

#define DDR_ERR_INVALID_DIMMTYPE                DDR_ERR_CODE_CREATE(0x110)
#define DDR_ERR_INVALID_CSPRESENT               DDR_ERR_CODE_CREATE(0x111)
#define DDR_ERR_INVALID_DRAMDATAWIDTH           DDR_ERR_CODE_CREATE(0x112)
#define DDR_ERR_INVALID_HEIGHT3DS               DDR_ERR_CODE_CREATE(0x113)
#define DDR_ERR_INVALID_ACTIVEDBYTE             DDR_ERR_CODE_CREATE(0x114)
#define DDR_ERR_INVALID_ACTIVENIBBLE            DDR_ERR_CODE_CREATE(0x115)
#define DDR_ERR_INVALID_ADDRMIRROR              DDR_ERR_CODE_CREATE(0x116)
#define DDR_ERR_INVALID_COLUMNADDRWIDTH         DDR_ERR_CODE_CREATE(0x117)
#define DDR_ERR_INVALID_ROWADDRWIDTH            DDR_ERR_CODE_CREATE(0x118)
#define DDR_ERR_INVALID_SPDCLSUPPORTED          DDR_ERR_CODE_CREATE(0x119)
#define DDR_ERR_INVALID_SPDTAAMIN               DDR_ERR_CODE_CREATE(0x11a)
#define DDR_ERR_INVALID_RANK4MODE               DDR_ERR_CODE_CREATE(0x11b)
#define DDR_ERR_INVALID_ENCODEDQUADCS           DDR_ERR_CODE_CREATE(0x11c)
#define DDR_ERR_INVALID_DDPCOMPATIBLE           DDR_ERR_CODE_CREATE(0x11d)
#define DDR_ERR_INVALID_TSV8HSUPPORT            DDR_ERR_CODE_CREATE(0x11e)
#define DDR_ERR_INVALID_MRAMSUPPORT             DDR_ERR_CODE_CREATE(0x11f)
#define DDR_ERR_INVALID_MDSSUPPORT              DDR_ERR_CODE_CREATE(0x120)
#define DDR_ERR_INVALID_NUMPSTATES              DDR_ERR_CODE_CREATE(0x121)
#define DDR_ERR_INVALID_FREQUENCY               DDR_ERR_CODE_CREATE(0x122)
#define DDR_ERR_INVALID_PHYODTIMPEDANCE         DDR_ERR_CODE_CREATE(0x123)
#define DDR_ERR_INVALID_PHYDRVIMPEDANCE         DDR_ERR_CODE_CREATE(0x124)
#define DDR_ERR_INVALID_PHYSLEWRATE             DDR_ERR_CODE_CREATE(0x125)
#define DDR_ERR_INVALID_ATXIMPEDANCE            DDR_ERR_CODE_CREATE(0x126)
#define DDR_ERR_INVALID_ATXSLEWRATE             DDR_ERR_CODE_CREATE(0x127)
#define DDR_ERR_INVALID_CKTXIMPEDANCE           DDR_ERR_CODE_CREATE(0x128)
#define DDR_ERR_INVALID_CKTXSLEWRATE            DDR_ERR_CODE_CREATE(0x129)
#define DDR_ERR_INVALID_ALERTODTIMPEDANCE       DDR_ERR_CODE_CREATE(0x12a)
#define DDR_ERR_INVALID_DRAMRTTNOM              DDR_ERR_CODE_CREATE(0x12b)
#define DDR_ERR_INVALID_DRAMRTTWR               DDR_ERR_CODE_CREATE(0x12c)
#define DDR_ERR_INVALID_DRAMRTTPARK             DDR_ERR_CODE_CREATE(0x12d)
#define DDR_ERR_INVALID_DRAMDIC                 DDR_ERR_CODE_CREATE(0x12e)
#define DDR_ERR_INVALID_DRAMWRITEPREAMBLE       DDR_ERR_CODE_CREATE(0x12f)
#define DDR_ERR_INVALID_DRAMREADPREAMBLE        DDR_ERR_CODE_CREATE(0x130)
#define DDR_ERR_INVALID_PHYEQUALIZATION         DDR_ERR_CODE_CREATE(0x131)
#define DDR_ERR_INVALID_INITVREFDQ              DDR_ERR_CODE_CREATE(0x132)
#define DDR_ERR_INVALID_INITPHYVREF             DDR_ERR_CODE_CREATE(0x133)
#define DDR_ERR_INVALID_ODTWRMAPCS              DDR_ERR_CODE_CREATE(0x134)
#define DDR_ERR_INVALID_ODTRDMAPCS              DDR_ERR_CODE_CREATE(0x135)
#define DDR_ERR_INVALID_GEARDOWN                DDR_ERR_CODE_CREATE(0x136)
#define DDR_ERR_INVALID_CALATENCYADDER          DDR_ERR_CODE_CREATE(0x137)
#define DDR_ERR_INVALID_BISTCALMODE             DDR_ERR_CODE_CREATE(0x138)
#define DDR_ERR_INVALID_BISTCAPARITYLATENCY     DDR_ERR_CODE_CREATE(0x139)
#define DDR_ERR_INVALID_RCDDIC                  DDR_ERR_CODE_CREATE(0x13a)
#define DDR_ERR_INVALID_RCDVOLTAGECTRL          DDR_ERR_CODE_CREATE(0x13b)
#define DDR_ERR_INVALID_RCDIBTCTRL              DDR_ERR_CODE_CREATE(0x13c)
#define DDR_ERR_INVALID_RCDDBDIC                DDR_ERR_CODE_CREATE(0x13d)
#define DDR_ERR_INVALID_RCDSLEWRATE             DDR_ERR_CODE_CREATE(0x13e)
#define DDR_ERR_INVALID_DFIMRL_DDRCLK           DDR_ERR_CODE_CREATE(0x13f)
#define DDR_ERR_INVALID_ATXDLY                  DDR_ERR_CODE_CREATE(0x140)

#define DDR_ERR_TRAIN_BIST_PATTERN_LIST_OUT_OF_RANGE  DDR_ERR_CODE_CREATE(0x200)
#define DDR_ERR_TRAIN_READ_PATTERN_OUT_OF_RANGE       DDR_ERR_CODE_CREATE(0x210)
#define DDR_ERR_TRAIN_WRITE_LEVEL_INIT_BIST_FAIL      DDR_ERR_CODE_CREATE(0x211)
#define DDR_ERR_TRAIN_WRITE_LEVEL_FWD_SWEEP_OUT_OF_RANGE DDR_ERR_CODE_CREATE(0x212)
#define DDR_ERR_TRAIN_WRITE_LEVEL_BWD_SWEEP_OUT_OF_RANGE DDR_ERR_CODE_CREATE(0x213)

#define DDR_ERR_TRAIN_PATTERN_WRITE_INIT_BIST_FAIL      DDR_ERR_CODE_CREATE(0x214)
#define DDR_ERR_TRAIN_PATTERN_WRITE_FWD_SWEEP_OUT_OF_RANGE DDR_ERR_CODE_CREATE(0x215)
#define DDR_ERR_TRAIN_PATTERN_WRITE_BWD_SWEEP_OUT_OF_RANGE DDR_ERR_CODE_CREATE(0x216)

#define DDR_ERR_VERSION_CHCK                    DDR_ERR_CODE_CREATE(0x0FF)

#define DDR_VREF_LOCAL                    (0x0)
#define DDR_VREF_GLOBAL                   (0x1)
//#define DDR_VREF_EXTERNAL                 (0x2)



#endif /* _DDRW_ERR_H */



