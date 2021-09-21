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
*     Header file derived from:
*       Foxhound_RevB_Config_Guide_pdox_r0_14.xlsx (0.14)
*
*   NOTES:
*     None.
*
*******************************************************************************/
#ifndef _SERDES_CG_SUPPLEMENT_H
#define _SERDES_CG_SUPPLEMENT_H

/*
** Include Files
*/

/*
** Enumerated Types
*/

/** 
*  @brief 
*   PJ impairment configurations in serdes_pj_impairment struct
*/
typedef enum
{
    SERDES_IMPAIRMENT_PJ_SIN0 = 3   /**< Sinusoidal PJ, currently FW only supports this */
} serdes_pj_impairment_enum;

/** 
*  @brief 
*   PJ impairments
*/
typedef enum
{
    SERDES_PJ_PATTERN_NONE,                     /**< No PJ */
    SERDES_PJ_PATTERN_TRIANGLE,                 /**< Triangular PJ */
    SERDES_PJ_PATTERN_CONTINUOUS_SINUSOID_MOD   /**< Continuous sinusoid PJ */
} serdes_pj_pattern_enum;

/*
** Constants
*/
/* For calibration debug, set to 1 to print the adapted objects and bit error counts */
#define SERDES_CAL_VERBOSE 0

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
EXTERN VOID SERDES_FH_Print_Interp_Idx(UINT8 lanes_bitmask,
                                       UINT32 iterations);
EXTERN VOID SERDES_FH_Print_Adapted_Obj(UINT8 lane_bitmask);
EXTERN VOID SERDES_FH_Print_CDR_Lock_Stats(UINT8 lane_bitmask, UINT32 iterations);
EXTERN UINT32 SERDES_FH_IQ_Offset_Calibration(UINT8 lane_bitmask);
EXTERN UINT32 SERDES_FH_CDR_Offset_Calibration_Centered(UINT8 enabled_lanes_bitmask,
                                                       serdes_pj_impairment_enum pj_impairment,
                                                       UINT32 fast_dwell_ms,
                                                       UINT32 slow_dwell_ms,
                                                       UINT32 fast_err_cnt_per_ms_threshold,
                                                       UINT32 slow_err_cnt_per_ms_threshold,
                                                       UINT32 force_adapt_value_en,
                                                       UINT32 objects_en_pass11,
                                                       UINT32 adapt_loops_pass11,
                                                       UINT8 * converged_lane_bitmask);
EXTERN PUBLIC UINT32 SERDES_FH_CDR_Offset_Force_Offset(UINT8 lane_bitmask,
                                                INT8 cdr_index_offset,
                                                UINT8 * applied_bitmask);
EXTERN VOID SERDES_FH_Meas_BER_Linking(UINT8 lanes_bitmask,
                                        UINT32 period_ms,
                                        UINT32* lane_err_cnt,
                                        UINT32* lane_err_cnt_per_ms);
#endif /* _SERDES_CG_SUPPLEMENT_H */
