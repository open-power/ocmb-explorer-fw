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
* @addtogroup ddr_phy
* @{
* @file
* @brief
*   DDR PHY toolbox library external interface
*
* @note
*/

#ifndef _DDR_PHY_H
#define _DDR_PHY_H

/*
** Include Files
*/
#include <stdio.h>

#include "pmc_profile.h"
#include "bc_printf.h"
#include "pmcfw_err.h"
#include "sys_timer_api.h"
#include "ddrphy_dfibist.h"
#include "ddrphy_toolbox.h"
#include "ddr_err.h"

/*
** Constants
*/

#define DDR_PHY_EMULATION 0

/*
** Macros
*/
#define ddr_printf(...) bc_printf(__VA_ARGS__)

#ifndef LOG_MESSAGE
#define LOG_MESSAGE(...) bc_printf(__VA_ARGS__)
#endif
#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE(...) bc_printf(__VA_ARGS__)
#endif

#ifndef ASSERT_E
#define ASSERT_E(condition, ...) if (!condition) {bc_printf(__VA_ARGS__); PMCFW_ASSERT(condition, 0xff);}
#endif

/*
** Structure Definitions
*/
typedef struct
{
    int8_t rr;
    int8_t ww;
    int8_t rw;
    int8_t wr;
} phy_cdd_maxabs_t;

typedef struct
{
    phy_cdd_maxabs_t    cdd; /* Per power state */
} phy_cdd_t;

/*
** Global variables
*/
/**
* @brief
*   This enumeration specifies the supported user input MSDG configurations
*/
typedef enum
{
#if (DDR_PHY_EMULATION == 1)
    DDR_PHY_EMULATION_USER_INPUT_MSDG,          /**< User input MSDG for emulation */
#else
    DDR_PHY_SILICON_X8_UDIMM_USER_INPUT_MSDG,   /**< User input MSDG for X8 UDIMM device */
#if (EXPLORER_BRINGUP == 1)
    /* PE specific user input MSDG structures */
    DDR_PHY_SILICON_X4_RDIMM_USER_INPUT_MSDG,   /**< User Input MSDG for X4 RDIMM device */
    DDR_PHY_SILICON_RANK1_X16_UDIMM_USER_INPUT_MSDG,
    DDR_PHY_SILICON_RANK2_X4_RDIMM_USER_INPUT_MSDG,
#endif
#endif
    DDR_PHY_NUM_USER_INPUT_MSDG_CONFIG,         /**< Number of user input MSDG configurations */
    
    /* Default user input MSDG */
#if (DDR_PHY_EMULATION == 1)
    DDR_PHY_DEFAULT_USER_INPUT_MSDG = DDR_PHY_EMULATION_USER_INPUT_MSDG
#else
    DDR_PHY_DEFAULT_USER_INPUT_MSDG = DDR_PHY_SILICON_X8_UDIMM_USER_INPUT_MSDG
#endif
} ddr_phy_user_input_msdg_index_enum;

extern user_input_msdg_t user_input_msdg_array[DDR_PHY_NUM_USER_INPUT_MSDG_CONFIG];

/*
** External References
*/
extern uint32_t ddrphy_userinputdbgmsdg_configure(user_input_dbg_msdg_t *ui_dbg_msdg_p);
extern uint32_t ddr_phy_init(uint32_t runDevInit,
                             uint32_t runTraining,
                             uint32_t train_2d,
                             uint32_t restoreVref,
                             uint32_t restoreTiming);
extern uint32_t ddr_phy_eye_capture_init(save_eye_data eye_capture_step,
                                         uint8_t *eye_data_buffer_ptr);
extern void ddr_phy_reset(void);
extern uint32_t ddr_phy_bist(uint8_t power_state,
                             uint32_t rank_index,
                             uint32_t length,
                             ddr_bist_pattern_e pattern,
                             uint32_t timeout_us);
extern void ddr_phy_error(char *errorString);
extern void ddr_phy_print(char *msg);
extern void io_write16(uint32_t adr, uint16_t dat);
extern uint16_t io_read16(uint32_t adr);
extern uint32_t ddrphy_userinputmsdg_set(user_input_msdg_t *ui_msdg_p);
extern void  ddrphy_force_board_delay (board_tof_msdg_t *tofPtr);
extern int ddrphy_Tof2DelayReg (board_tof_msdg_t *tofPtr, 
                                train_delay_msdg_t *dlyPtr);
extern void ddrphy_print_userInputMsdg(void);
extern void ddrphy_print_message_block (int pstate, int Train2D, int ate);
extern uint32_t ddrphy_force_dl_update(void);
extern void trace_print(uint32_t trace_enum, char * fw_str_format, ...);

/*
** Inline Functions
*/
/**
* @brief get current time in cpu ticks
*
* @param[in] - N/A
*
* @retval Current time in cpu ticks
*
*/
static inline uint32_t getCCount(void)
{
    return sys_timer_read();
}

/**
* @brief Wait with no operation for certain us.
*
* @param[in] - delayUs. integer number of how many us is waited.
*
* @retval N/A
*
*/
static inline void timeDelayUs(uint32_t delayUs)
{
    sys_timer_busy_wait_us(delayUs);
}

/**
* @brief Wait with no operation for certain ns.
*
* @param[in] - delayNs. integer number of how many ns is waited.
*
* @retval N/A
*
*/
static inline void timeDelayNs(uint32_t delayNs)
{
    sys_timer_busy_wait_ns(delayNs);
}

/**
*
* @brief timeElapseInUs report the time difference between startTime and current sim time
*
* @param[in] - startTime
* @param[in] - sys_clk_speed. not used on this platform
*
* @retval time difference between startTime and current sim time
*/
static inline uint32_t timeElapseInUs(uint32_t startTime, uint32_t sys_clk_speed)
{
    return sys_timer_count_to_us(sys_timer_diff(startTime, sys_timer_read()));
}

#endif /* _DDR_PHY_H */




