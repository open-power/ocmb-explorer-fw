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
#define SERDES_PH_OFS_T_PRELOAD_SETTING     0x1C

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

EXTERN UINT32 serdes_plat_low_level_init(UINT8 lane_bitmask,
                                         UINT32 frequency,
                                         BOOL dfe_state);
EXTERN UINT32 serdes_plat_low_level_single_lane_init(UINT32 lane,
                                                     UINT32 frequency,
                                                     BOOL dfe_state);
EXTERN VOID serdes_fatal_error_handler(void);
EXTERN UINT32 serdes_plat_adapt_step1(BOOL dfe_state, BOOL adpt_state, UINT8 lane_bitmask);
EXTERN UINT32 serdes_plat_adapt_step2(BOOL dfe_state,  BOOL adpt_state, UINT8 lane_bitmask);
EXTERN UINT32 serdes_plat_low_level_standalone_init(UINT8 lane_bitmask, BOOL dfe_state);
EXTERN UINT8 serdes_plat_loopback_test(UINT8 lane_bitmask);
EXTERN VOID serdes_plat_crash_dump_register(VOID);
EXTERN VOID serdes_plat_initialized_set(BOOL is_initialized);
EXTERN BOOL serdes_plat_initialized_get(VOID);
EXTERN UINT32  serdes_plat_lane_inversion_config(UINT8 lane_bitmask, UINT8 lane_pattern_bitmask);
EXTERN UINT32 serdes_plat_iq_offset_calibration(UINT8 lane_bitmask);

#endif /* _SERDES_PLAT_H */



