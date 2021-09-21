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
* @defgroup DCSU DCSU: Digital Clock Synthesis Unit Control
* @brief
*     This module controls the system, DDR, MIPS, ONFI, serial, and PCIe
*     digital clock synthesis units (DCSUs).
*
*     There are 6 physically separate DCSU_16N block instances in ROCw devices:
*     - SYS_DCSU (drives sysclk, refclk_d for PCIe CSU); clocks all hardware
*       not driven by other the other clocks mentioned below.
*     - DDR_DCSU (drives ddr_clk); clocks the DDR subsystem.
*     - MIPS_DCSU (drives PCS clock, SPCS clock); clocks the CPUs.
*     - ONFI_DCSU (drives serial clock, ethernet clocks)
*     - SERIAL_DCSU (drives NFC clocks, SPI clocks, mgt_clk)
*     - PCIE_DCSU (drives PCIe clock)
*
*     Controlling the DSCUs with a functional interface allows the frequency
*     of the DSCUs to be changed during early bootup. For example, the
*     frequency must be changed when the chip is overclocked. Currently APIs
*     are only provided to change the frequencies for PCS, DDR, and NFC clocks.
*     There are no use case to change clock frequencies in the other DCSUs.
*     APIs are provided to obtain the current frequencies of all DCSUs.
*
*     In general, the various DCSU clocks and dividers are as follows:
*
*     Reference clock (input): This is either a SAS reference clock (75MHz)
*                              or a PCIe reference clock (100MHz). Which
*                              reference clock is used is selected by a
*                              device-level bootstrap setting.
*     Pre-divider (register):  Divides the reference clock down into a
*                              a pre-divided reference clock. Its range is
*                              0 (no divide) to 31 (actual divider is +1).
*     Feedback-divider (reg):  This is really a multiplier rather than a
*                              divider, with respect to its affect on the
*                              frequency; it multiplies the pre-divided
*                              reference clock frequency. There are three
*                              parts to this: an integer multiplier, and then
*                              a numerator and denominator that make up a
*                              fractional multiplier. The integer multiplier
*                              range is 32 to 255, and the fractional range
*                              is 0 (no fractional component) to 4.29E+09.
*     DCO (intermediate clk):  The DCO is the Digitally Controlled Ring
*                              Oscillator. The output DCO clock is a common
*                              frequency from which all output clocks to
*                              other subsystems are divided down. Its range
*                              is 1500MHz to 3000MHz.
*     Post-divider (register): There are eight output clocks (clk_out) from
*                              each DCSU instance that feed other subsystems.
*                              Each of these has its own post-divider, which
*                              divides down from the common DCSU DCO output
*                              clock. Each post-divider has a range of
*                              0 (no divide) to 63 (actual divider is +1).
*     clk_out[7:0] (output):   Output clocks from the DCSU that feed other
*                              subsystems. The valid range of each output
*                              clock depends on the limits of the subsystem
*                              it is feeding.
*
* @{
* @file
* @brief
*   Public API for DCSU
*
* @note
*/
#ifndef _DCSU_API_H
#define _DCSU_API_H

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
/**
* @brief
*  DCSU compatible frequency groups
*  Groups can be used to identify frequencies that are allowed
*  to be switched between
*/
typedef enum
{
    DCSU_GRP_0 = 0,
    DCSU_DDR_FREQ_MAX_3200_MHZ = DCSU_GRP_0,  /**< 3200 MHz */
    DCSU_NFC_FREQ_SPI_SAFE     = DCSU_GRP_0,  /**< NFC rates always allowed; no impact on SPI clock */
    DCSU_GRP_1 = 1,
    DCSU_DDR_FREQ_MAX_2933_MHZ = DCSU_GRP_1,  /**< 2933 MHz */
    DCSU_NFC_FREQ_SPI_UNSAFE   = DCSU_GRP_1,  /**< NFC rates only allowed by unlocking in testing; impacts SPI clock */
    DCSU_GRP_2 = 2,
    DCSU_DDR_FREQ_MAX_2666_MHZ = DCSU_GRP_2,  /**< 2666 MHz */
    DCSU_GRP_3 = 3,
    DCSU_DDR_FREQ_MAX_2400_MHZ = DCSU_GRP_3,  /**< 2400 MHz */
    DCSU_GRP_4 = 4,
    DCSU_DDR_FREQ_MAX_2133_MHZ = DCSU_GRP_4,  /**< 2133 MHz */
    DCSU_GRP_5 = 5,
    DCSU_DDR_FREQ_MAX_1866_MHZ = DCSU_GRP_5   /**< 1866 MHz */
} dcsu_grp_enum;

/*
* Constants
*/

/**
* @brief
*   This macro shall be used to creates error codes for the DCSU module.
*   Each DCSU error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | DCSU error base (16-bit) | error code suffix (12-bit)]
*
* @brief
*   where
*     DCSU error base   - 16-bit PMCFW_ERR_BASE_DCSU, defined in "pmcfw_mid.h".
*     error code suffix - 12-bit specific error code suffix input to the macro.
*
* @brief
*   Note:
*   - For success, the return code = PMC_SUCCESS.
*
* @param [in] err_suffix - 12-bit error suffix.  See above for details.
*
* @return
*   Error code in format described above.
*
* @hideinitializer
*/
#define DCSU_ERR_CODE_CREATE(err_suffix)     ((PMCFW_ERR_BASE_DCSU) | (err_suffix))
#define DCSU_ERR_INVALID_CLK_FREQ             DCSU_ERR_CODE_CREATE(0x001)
#define DCSU_ERR_INVALID_BIST_MEAS_PERIOD     DCSU_ERR_CODE_CREATE(0x002)
#define DCSU_ERR_INVALID_DCSU_PARAM_PTR       DCSU_ERR_CODE_CREATE(0x003)
#define DCSU_ERR_INVALID_FREQ_UPDATE_ORDER    DCSU_ERR_CODE_CREATE(0x004)
#define DCSU_ERR_INVALID_PRE_DIV_BY           DCSU_ERR_CODE_CREATE(0x005)
#define DCSU_ERR_INVALID_FB_INT_DIV           DCSU_ERR_CODE_CREATE(0x006)
#define DCSU_ERR_INVALID_POST_DIV_BY          DCSU_ERR_CODE_CREATE(0x007)
#define DCSU_ERR_ERROR_LOCKING_CPU_CLK        DCSU_ERR_CODE_CREATE(0x008)
/**
 * DCSU unexpected frequency get errors
 * Maintain order to match DCSU_16N block sequence in registers:
 * SYS, DDR, MIPS, ONFI, SERIAL, PCIE
 */
#define DCSU_ERR_UNEXPECTED_FREQ              DCSU_ERR_CODE_CREATE(0x010)
#define DCSU_ERR_UNEXPECTED_FREQ_SYS          DCSU_ERR_UNEXPECTED_FREQ
#define DCSU_ERR_UNEXPECTED_FREQ_DDR          DCSU_ERR_CODE_CREATE(0x011)
#define DCSU_ERR_UNEXPECTED_FREQ_MIPS         DCSU_ERR_CODE_CREATE(0x012)
#define DCSU_ERR_UNEXPECTED_FREQ_ONFI         DCSU_ERR_CODE_CREATE(0x013)
#define DCSU_ERR_UNEXPECTED_FREQ_SERIAL       DCSU_ERR_CODE_CREATE(0x014)
#define DCSU_ERR_UNEXPECTED_FREQ_PCIE         DCSU_ERR_CODE_CREATE(0x015)
/**
 * DCSU BIST errors
 * Maintain order to match DCSU_16N block sequence in registers:
 * SYS, DDR, MIPS, ONFI, SERIAL, PCIE
 */
#define DCSU_ERR_BIST_DID_NOT_COMPLETE        DCSU_ERR_CODE_CREATE(0x020)
#define DCSU_ERR_BIST_DID_NOT_COMPLETE_SYS    DCSU_ERR_BIST_DID_NOT_COMPLETE
#define DCSU_ERR_BIST_DID_NOT_COMPLETE_DDR    DCSU_ERR_CODE_CREATE(0x021)
#define DCSU_ERR_BIST_DID_NOT_COMPLETE_MIPS   DCSU_ERR_CODE_CREATE(0x022)
#define DCSU_ERR_BIST_DID_NOT_COMPLETE_ONFI   DCSU_ERR_CODE_CREATE(0x023)
#define DCSU_ERR_BIST_DID_NOT_COMPLETE_SERIAL DCSU_ERR_CODE_CREATE(0x024)
#define DCSU_ERR_BIST_DID_NOT_COMPLETE_PCIE   DCSU_ERR_CODE_CREATE(0x025)
#define DCSU_ERR_BIST_DID_NOT_PASS            DCSU_ERR_CODE_CREATE(0x030)
#define DCSU_ERR_BIST_DID_NOT_PASS_SYS        DCSU_ERR_BIST_DID_NOT_PASS
#define DCSU_ERR_BIST_DID_NOT_PASS_DDR        DCSU_ERR_CODE_CREATE(0x031)
#define DCSU_ERR_BIST_DID_NOT_PASS_MIPS       DCSU_ERR_CODE_CREATE(0x032)
#define DCSU_ERR_BIST_DID_NOT_PASS_ONFI       DCSU_ERR_CODE_CREATE(0x033)
#define DCSU_ERR_BIST_DID_NOT_PASS_SERIAL     DCSU_ERR_CODE_CREATE(0x034)
#define DCSU_ERR_BIST_DID_NOT_PASS_PCIE       DCSU_ERR_CODE_CREATE(0x035)
#define DCSU_ERR_BIST_HAD_ERRORS              DCSU_ERR_CODE_CREATE(0x030)
#define DCSU_ERR_BIST_HAD_ERRORS_SYS          DCSU_ERR_BIST_HAD_ERRORS
#define DCSU_ERR_BIST_HAD_ERRORS_DDR          DCSU_ERR_CODE_CREATE(0x041)
#define DCSU_ERR_BIST_HAD_ERRORS_MIPS         DCSU_ERR_CODE_CREATE(0x042)
#define DCSU_ERR_BIST_HAD_ERRORS_ONFI         DCSU_ERR_CODE_CREATE(0x043)
#define DCSU_ERR_BIST_HAD_ERRORS_SERIAL       DCSU_ERR_CODE_CREATE(0x044)
#define DCSU_ERR_BIST_HAD_ERRORS_PCIE         DCSU_ERR_CODE_CREATE(0x045)
#define DCSU_ERR_BIST_WRONG_CNT_RESULT        DCSU_ERR_CODE_CREATE(0x050)
#define DCSU_ERR_BIST_WRONG_CNT_RESULT_SYS    DCSU_ERR_BIST_WRONG_CNT_RESULT
#define DCSU_ERR_BIST_WRONG_CNT_RESULT_DDR    DCSU_ERR_CODE_CREATE(0x051)
#define DCSU_ERR_BIST_WRONG_CNT_RESULT_MIPS   DCSU_ERR_CODE_CREATE(0x052)
#define DCSU_ERR_BIST_WRONG_CNT_RESULT_ONFI   DCSU_ERR_CODE_CREATE(0x053)
#define DCSU_ERR_BIST_WRONG_CNT_RESULT_SERIAL DCSU_ERR_CODE_CREATE(0x054)
#define DCSU_ERR_BIST_WRONG_CNT_RESULT_PCIE   DCSU_ERR_CODE_CREATE(0x055)

/* Supported DCSU clk_out rates */
#define DCSU_40_MHZ                             40000000UL
#define DCSU_66_6_MHZ                           66666666UL
#define DCSU_80_MHZ                             80000000UL
#define DCSU_100_MHZ                           100000000UL
#define DCSU_110_MHZ                           110000000UL
#define DCSU_133_3_MHZ                         133333333UL
#define DCSU_166_6_MHZ                         166666666UL
#define DCSU_200_MHZ                           200000000UL
#define DCSU_266_6_MHZ                         266666666UL
#define DCSU_275_MHZ                           275000000UL
#define DCSU_285_7_MHZ                         285714285UL
#define DCSU_311_1_MHZ                         311111111UL
#define DCSU_314_2_MHZ                         314285714UL
#define DCSU_300_MHZ                           300000000UL
#define DCSU_320_MHZ                           320000000UL
#define DCSU_333_3_MHZ                         333333333UL
#define DCSU_342_8_MHZ                         342857142UL
#define DCSU_350_MHZ                           350000000UL
#define DCSU_360_MHZ                           360000000UL
#define DCSU_366_6_MHZ                         366666666UL
#define DCSU_380_9_MHZ                         380952380UL
#define DCSU_400_MHZ                           400000000UL
#define DCSU_440_MHZ                           440000000UL
#define DCSU_450_MHZ                           450000000UL
#define DCSU_466_6_MHZ                         466666666UL
#define DCSU_480_MHZ                           480000000UL
#define DCSU_500_MHZ                           500000000UL
#define DCSU_533_3_MHZ                         533333333UL
#define DCSU_550_MHZ                           550000000UL
#define DCSU_600_MHZ                           600000000UL
#define DCSU_666_6_MHZ                         666666666UL
#define DCSU_733_3_MHZ                         733333333UL
#define DCSU_800_MHZ                           800000000UL
#define DCSU_850_MHZ                           850000000UL
#define DCSU_1002_MHZ                         1002000000UL
#define DCSU_1700_MHZ                         1700000000UL

/*
* Macro Definitions
*/

/*
* Structures and Unions
*/

/*
** Function Prototypes and Pointers to Functions in RAM
**
** To accommodate PIC code executing in SPI flash and non-PIC code executing in
** RAM, the functions in RAM are accessed through pointers. The changes that were made:
**
**      - original function name:     $type func_name($type, $type)
**        changed with prepended '_': $type _func_name($type, $type)
**      - define a typedef for the function pointer: typedef $type (_func_name_fn_ptr)($type, $type)
**      - public function pointers initialized in source code files:
**        PUBLIC _func_name_fn_ptr (*func_name_ptr) =  _func_name;
**      - private function pointers intitialized in source code files:
**        PRIVATE _func_name_fn_ptr (*func_name_ptr) = _func_name;
**      - for public functions new define for original function name in header file:
**        #define func_name (*func_name_ptr)
**      - for private functions new define for original function name in source code file:
**        #define func_name (*func_name_ptr)
*/
typedef UINT32 (*dcsu_cpu_clk_freq_get_fn_ptr_type)(void);
EXTERN dcsu_cpu_clk_freq_get_fn_ptr_type dcsu_cpu_clk_freq_get_fn_ptr;
#define dcsu_cpu_clk_freq_get (*dcsu_cpu_clk_freq_get_fn_ptr)


/*
* Function Prototypes
*/
EXTERN PMCFW_ERROR dcsu_cpu_clk_freq_set(UINT32 clk_freq);
EXTERN UINT32 dcsu_spcs_cpu_clk_freq_get(void);
EXTERN UINT32 dcsu_cpu_clk_cnt_per_usec_get(void);
EXTERN UINT32 dcsu_sys_clk_freq_get(void);
EXTERN UINT32 dcsu_serial_clk_freq_get(void);
EXTERN VOID dcsu_ddr_clk_freq_max_set(dcsu_grp_enum clk_freq_max);
EXTERN PMCFW_ERROR dcsu_ddr_clk_freq_set(UINT32 clk_freq);
EXTERN UINT32 dcsu_ddr_clk_freq_get(void);
EXTERN VOID dcsu_nfc_core_clk_all_freq_config(BOOL allow);
EXTERN UINT32 dcsu_nfc_core_clk_freq_get(void);
EXTERN PMCFW_ERROR dcsu_nfc_core_clk_freq_set(UINT32 clk_freq);
EXTERN UINT32 dcsu_pcie_clk_freq_get(void);
EXTERN VOID dcsu_ram_code_ptr_adjust(UINT32 offset);

#endif /* _DCSU_API_H */

/** @} end addtogroup */


