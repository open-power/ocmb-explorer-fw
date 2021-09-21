/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2021 Microchip Technology Inc. All rights reserved. 
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
*     Platform-specific definitions and declarations for SERDES.
*
*******************************************************************************/


#ifndef _SERDES_PLAT_H
#define _SERDES_PLAT_H

/*
** Include Files
*/
#include "pmc_hw_base.h"


/*
** Enumerated Types 
*/


/*
** Constants 
*/
#define SERDES_LANE_0               0
#define SERDES_LANE_1               1
#define SERDES_LANE_2               2
#define SERDES_LANE_3               3
#define SERDES_LANE_4               4
#define SERDES_LANE_5               5
#define SERDES_LANE_6               6
#define SERDES_LANE_7               7
#define SERDES_LANES                8

#define SERDES_LANE_REG_OFFSET      0x1000

/* current default setting of PH_OFS_T_PRELOAD field of SERDES_OBJECT_PRELOAD_VAL_5 register */
#define SERDES_PH_OFS_T_PRELOAD_SETTING     0x1A

/* SerDes FFE settings */
#define SERDES_FFE_DEFAULT_PRECURSOR    0
#define SERDES_FFE_MIN_PRECURSOR        0
#define SERDES_FFE_MAX_PRECURSOR        32

#define SERDES_FFE_DEFAULT_POSTCURSOR   0
#define SERDES_FFE_MIN_POSTCURSOR       0
#define SERDES_FFE_MAX_POSTCURSOR       32

#define SERDES_FFE_MIN_CALIBRATION      64
#define SERDES_FFE_MAX_CALIBRATION      80

/* SERDES rtrim_34_15 validity masks */
#define SERDES_RTRIM_34_15_BIT6_VALID   0x40
#define SERDES_RTRIM_34_15_MASK         0x00FF

/* CDR Offset Calibration parameters */
#define SERDES_FORCE_ADAPT_VALUE_EN     0x400
#define SERDES_OBJECTS_EN_PASS11        0xE40
#define SERDES_ADAPT_LOOPS_PASS11       0x3

/* CDR Offset Mission Mode Cycle parameters */
#define SERDES_MISSION_MODE_FORCE_ADAPT_VALUE_EN     0x0000
#define SERDES_MISSION_MODE_OBJECTS_EN_PASS11        0x1E68
#define SERDES_MISSION_MODE_ADAPT_LOOPS_PASS11       0xA

/* 
** BER expressed in errors per ms 
*/
#define SERDES_BER_21G33_PER_MS_1e_4    2133
#define SERDES_BER_21G33_PER_MS_1e_6    21
#define SERDES_BER_23G46_PER_MS_1e_4    2346
#define SERDES_BER_23G46_PER_MS_1e_6    23
#define SERDES_BER_25G6_PER_MS_1e_4     2560
#define SERDES_BER_25G6_PER_MS_1e_6     26

/* 
** phase related values 
*/
#define SERDES_IQ_ACCUM_CYCLES          0x4000
#define SERDES_IQ_MAX_NEGATIVE          -16
#define SERDES_IQ_MAX_POSITIVE          15


/*
** Fast and Slow Calibration Periods
*/
#define SERDES_CDR_CAL_DWELL_10_MS      10
#define SERDES_CDR_SLO_DWELL_100_MS     100

/*
** Macro Definitions
*/

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN VOID serdes_plat_periodic_cal_init(BOOL serdes_cal_prbs);
EXTERN VOID serdes_plat_cal_update(VOID);
EXTERN VOID serdes_plat_cal_disable(BOOL cal_disable);
EXTERN UINT32 serdes_plat_low_level_init(UINT8 lane_bitmask,
                                         UINT32 frequency,
                                         BOOL dfe_state);
EXTERN UINT32 serdes_plat_low_level_single_lane_init(UINT32 lane,
                                                     UINT32 frequency,
                                                     BOOL dfe_state);
EXTERN VOID serdes_fatal_error_handler(UINT8 lane_bitmask);
EXTERN UINT32 serdes_plat_adapt_step1(BOOL dfe_state, BOOL adpt_state, UINT8 lane_bitmask, BOOL force_start);
EXTERN UINT32 serdes_plat_adapt_step2(BOOL dfe_state,  BOOL adpt_state, UINT8 lane_bitmask);
EXTERN UINT32 serdes_plat_pattmon_config(UINT8 lane_bitmask, UINT32 ber_dwell_ms, UINT32 ber_per_ms_threshold, UINT8 * pattmon_detected_bitmask);
EXTERN UINT32 serdes_plat_low_level_standalone_init(UINT8 lane_bitmask, BOOL dfe_state);
EXTERN UINT8 serdes_plat_loopback_test(UINT8 lane_bitmask);
EXTERN VOID serdes_plat_crash_dump_register(VOID);
EXTERN VOID serdes_plat_initialized_set(BOOL is_initialized);
EXTERN BOOL serdes_plat_initialized_get(VOID);
EXTERN UINT32  serdes_plat_lane_inversion_config(UINT8 lane_bitmask, UINT8 * lane_pattern_bitmask);
EXTERN VOID serdes_plat_ffe_precursor_set(UINT32 precursor);
EXTERN VOID serdes_plat_ffe_postcursor_set(UINT32 postcursor);
EXTERN VOID serdes_plat_ffe_calibration_set(UINT32 calibration);
EXTERN UINT32 serdes_plat_ffe_precursor_get(VOID);
EXTERN UINT32 serdes_plat_ffe_postcursor_get(VOID);
EXTERN UINT32 serdes_plat_ffe_calibration_get(VOID);
EXTERN void serdes_fatal_init(UINT8 lane_bitmask);
EXTERN BOOL serdes_fatal_get(UINT8 lane_bitmask, UINT32 *err_offset);


#endif /* _SERDES_PLAT_H */



