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
*                                                                               
* Synopsys DDR Firmware
*                                                                               
* Licensee also agrees not to modify the Synopsys DDR Firmware. The binary 
* Synopsys DDR Firmware may be used only in connection with Microchip integrated 
* circuits that implement the Synopsys DDR PHY IP.  
*                                                                               
* Licensee will maintain the copyright, trademark, and other notices that appear 
* on the Synopsys DDR Firmware, and reproduce such notices on all copies of the 
* Synopsys DDR Firmware. 
*                                                                               
* THE SYNOPSYS FIRMWARE IS PROVIDED ?AS IS? AND WITHOUT ANY WARRANTY.  SYNOPSYS 
* AND MICROCHIP EXPRESSLY DISCLAIM ANY AND ALL WARRANTIES, EXPRESS, IMPLIED OR 
* OTHERWISE, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE, AND NON-INFRINGEMENT, AND ANY WARRANTIES ARISING OUT OF A 
* COURSE OF DEALING OR USAGE OF TRADE.
* 
* IN NO EVENT SHALL SYNOPSYS OR MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, 
* SPECIAL, INCIDENTAL, SPECULATIVE OR CONSEQUENTIAL DAMAGES OF ANY KIND RELATING 
* TO THE SYNOPSYS FIRMWARE INCLUDING BUT NOT LIMITED TO LOSS OF PROFITS, LOSS OF 
* USE, LOSS OF GOODWILL, OR INTERRUPTION OF BUSINESS, EVEN IF MICROCHIP OR 
* RECIPIENTS ARE ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
********************************************************************************/

#ifndef _DDR_PHY_TOOLBOX_H_
#define _DDR_PHY_TOOLBOX_H_

#include <stdint.h>
#include <stddef.h>
#include "ddrphy_trace_strings.h"

typedef struct _PMU_SMB_LPDDR4_1D_t {char notEmpty;} PMU_SMB_LPDDR4_1D_t; 
typedef struct _PMU_SMB_LPDDR4_2D_t {char notEmpty;} PMU_SMB_LPDDR4_2D_t; 
typedef struct _PMU_SMB_LPDDR3_1D_t {char notEmpty;} PMU_SMB_LPDDR3_1D_t; 

#if defined PMC_DDR_SIM_DPI
#define TRACE(...)  MSDG_TRACE(__VA_ARGS__)
#define MSDG_TRACE(trace_enum, fw_format, msdg_format, ...) sprintf(ppp, msdg_format ,##__VA_ARGS__); ddr_phy_print(ppp)
#define TRACETMP(...) sprintf(ppp, ##__VA_ARGS__); ddr_phy_print(ppp)
#else
#define FW_DDR_TRACE(args...) sprintf(ppp, args); ddr_phy_print(ppp)
#define TRACE(...)  FW_TRACE(__VA_ARGS__)
#define FW_TRACE(trace_enum, fw_format, msdg_format, ...) trace_print(trace_enum, fw_format, ##__VA_ARGS__)

#define TRACETMP(...) sprintf(ppp, ##__VA_ARGS__); ddr_phy_print(ppp)
#define microsemi_memset(args...) memset(args)
#endif

//MSDG_MAX_PSTATE defines how many power state this PHY supports. Explorer supports only 1 pstate.
#ifndef MSDG_MAX_PSTATE
    #define MSDG_MAX_PSTATE 1
#endif

#define VERSION_NUM_INPUT_MSDG 2
#define VERSION_NUM_RESP_MSDG 1

#define JEDEC_T_WRITE_RECOVERY 15000
#define JEDEC_T_CCD_L 5000

#define DWC_DDRPHY_PHYINIT_FOR_CUST

/* Error codes */

#ifndef DDR_ERR_BASE
#ifdef PMC_DDR_SIM_DPI
#define DDR_ERR_BASE                            (0xD000)
#else
#define DDR_ERR_BASE                            PMCFW_ERR_BASE_DDR_TOOLBOX
#endif
#endif
#define DDR_ERR_CODE_CREATE(err_suffix)         ((DDR_ERR_BASE) | (err_suffix))

#ifdef PMC_DDR_SIM_DPI
#ifndef DDR_SUCCESS
#define DDR_SUCCESS                             (0x000)
#endif
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

#define DDR_ERR_PHY_WRITE_LEVELING              DDR_ERR_CODE_CREATE(0x050)
#define DDR_ERR_PHY_GATE_TRAINING               DDR_ERR_CODE_CREATE(0x051)
#define DDR_ERR_PHY_WL_ADJ                      DDR_ERR_CODE_CREATE(0x052)
#define DDR_ERR_PHY_TIMING_TAG_ERROR            DDR_ERR_CODE_CREATE(0x053)
#define DDR_ERR_PHY_REG_RANGE_ERROR             DDR_ERR_CODE_CREATE(0x054)
#define DDR_ERR_PHY_INLD_TWR                    DDR_ERR_CODE_CREATE(0x055)
#define DDR_ERR_PHY_MRLTRAINING                 DDR_ERR_CODE_CREATE(0x056)

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

#define DDR_ERR_TRAIN_BIST_PATTERN_LIST_OUT_OF_RANGE  DDR_ERR_CODE_CREATE(0x200)
#define DDR_ERR_TRAIN_READ_PATTERN_OUT_OF_RANGE       DDR_ERR_CODE_CREATE(0x210)

#define DDR_ERR_VERSION_CHCK                    DDR_ERR_CODE_CREATE(0x0FF)

/* Error code for user_input_msdg check */
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

#define DDR_VREF_LOCAL                    (0x0)
#define DDR_VREF_GLOBAL                   (0x1)
//#define DDR_VREF_EXTERNAL                 (0x2)


//#############################################################################
//
// Structure for MSCC inputs
//
//#############################################################################
#ifdef PMC_DDR_SIM_DPI
typedef struct user_input_msdg {
#else
typedef __packed struct user_input_msdg {
#endif
    uint32_t version_number;
    uint16_t DimmType;          // Choose the Dimm type from one of below:
                                // 0 = UDIMM
                                // 1 = RDIMM 
                                // 2 = LRDIMM (Invalid for Explorer)
                                // 3 = MDS-LRDIMM
                                // 4 = MDS

    uint16_t CsPresent;         // Indicates presence of DRAM at each chip select for PHY. Each bit corresponds to a logical CS. 
                                // If the bit is set to 1, the CS is connected to DRAM. 
                                // If the bit is set to 0, the CS is not connected to DRAM.
                                // 
                                // CsPresent BIT [0] = CS0 is populated with DRAM
                                // CsPresent BIT [1] = CS1 is populated with DRAM
                                // CsPresent BIT [2] = CS2 is populated with DRAM (only available when Rank4Mode is 1, or using Encoded QuadCs Mode for RDIMM/LRDIMM)
                                // CsPresent BIT [3] = CS3 is populated with DRAM (only available when Rank4Mode is 1, or using Encoded QuadCs Mode for RDIMM/LRDIMM)

    uint16_t DramDataWidth;     // Enter 4,8 depending on protocol and dram type.
                                // See below for legal types for each protocol.
                                // DDR4   4:X4, 8:X8     -- default = X8
                                
    uint16_t Height3DS;         // Enter 0,2,4, depending on 3Ds Stack
                                // See below for legal types for each protocol.
                                // 0 = Planar
                                // 2 = H2
                                // 4 = H4
                                // 8 = H8, (only valid when either DDPCompatible or TSV8HSupport is valid)

    uint16_t ActiveDBYTE;       // [9:0] each bit to enable one DBYTE macro
                // 1 = Enable DBYTE macro
                // 0 = Disable DBYTE macro (clock gating and IO tri-state)

    uint32_t ActiveNibble;      // [19:0] each bit to enable one X4 nibble. This field is only valid for X4 DRAMs
                // 1 = Account training/dfi_bist result on the selected nibble.
                // 0 = Ignore training/dfi_bist result on the selected nibble.

    uint16_t AddrMirror;        // Each bit corresponds to CSN[3:0], it determines which rank implements address mirroring routing based on JEDEC spec. 
                                // 1 = Address Mirror.
                                // 0 = No Address Mirror.

    uint16_t ColumnAddrWidth;   // DRAM Column Addr Width (Valid value: 10)
    uint16_t RowAddrWidth;      // DRAM Row Addr Width (Valid value: 14,15,16,17,18)
    uint32_t SpdCLSupported;    // Cas Latency Supported by DRAM (from DDR4 SPD Byte 20~23)
                                // SpdCLSupported BIT [7:0]   = CL14~CL7
                                // SpdCLSupported BIT [15:8]  = CL22~CL15
                                // SpdCLSupported BIT [23:16] = CL30~CL23
                                // SpdCLSupported BIT [29:24] = CL36~CL31
                                // SpdCLSupported BIT [31:30] Reserved

    uint16_t SpdtAAmin;         // Minimum Cas Latency Time (tAAmin) in Picosecond (Byte 24)
                                // examples: DDR4-2400P = 12500 ps
                                //           DDR4-2400R = 13320 ps
                                //           DDR4-2400U = 15000 ps

    uint16_t Rank4Mode;         // Operate PHY in 4-rank mode. 
                // when enabled, A-side CA bus drives rank 0/1, B-side CA bus drives rank 2/3, DQ/DQS bus are shared across all the ranks
                // 1 = 4-rank mode
                // 0 = Normal mode (2-rank);

    uint16_t EncodedQuadCs;     // Operate PHY in Encoded QuadCs Mode (only valid for RDIMM/RLDIMM) (NOT Supported in Explorer)
                // when enabled, each CA bus drives one RCD in Encoded QuadCs mode.
                                // {cid_a/b[0],csn_a/b[1:0]} are connected to {DC0,DCS1_n,DCS0_n} to select master ranks. cid_a/b[1] is connected to DC2 to select up to 2 logic ranks (2H 3DStack)
                // 1 = Encoded QuadCs Mode
                // 0 = Direct DualCs Mode

    uint16_t DDPCompatible;     // Support 1rank 3DS Device in DDP board routing. 
                // 1 = 1 rank 3DS in DDP board routing. (cke[1],csn[1],odt[1] of PHY are connected to c[0],c[1],c[2] of DRAM);
                // 0 = Normal Mode (cid[0],cid[1] of PHY are connected to c[0],c[1] of DRAM, c[2] of DRAM ties to ground);

    uint16_t TSV8HSupport;      // Support 8H 3DS routing in board routing when pairty check is disabled.
                // 1 = Support DDR4 3DS 8H DRAM (caparity is connected to c[2] of DRAM);
                // 0 = Normal Mode (caparity is connected to PAR of DRAM or DPAR of RCD);

    uint16_t MRAMSupport;       // Support timing parameters of Everspin MRAM.
                // 1 = Support Everspin DDR4 MRAM;
                // 0 = Normal DDR4 DRAM;

    uint16_t MDSSupport;        // Support board routing for MDS DRAM support
                // 1 = Support MDS 8H DRAM (odt[1] is connected to c[2] of MDS DRAM);
                // 0 = Normal Mode;

    uint16_t NumPStates;          // Number of p-states used 
                // Always set NumPStates to 1 for Explorer. For the fields with Pstate array, only need to fill [0] entry.

    uint16_t Frequency[MSDG_MAX_PSTATE];    // Memclk frequency in MHz -- round up to next highest integer.  Enter 334 for 333.333, etc.
                                // examples: DDR4-3200 = 1600
                                //           DDR4-2933 = 1467
                                //           DDR4-2666 = 1333
                // Offset_0[15:0] - P0 pstate Memclk frequency in MHz
                                // Offset_1[15:0] - P1 pstate Memclk frequency in MHz (invalid for Explorer)
                                // Offset_2[15:0] - P2 pstate Memclk frequency in MHz (invalid for Explorer)
                                // Offset_3[15:0] - P3 pstate Memclk frequency in MHz (invalid for Explorer)
    
    uint16_t PhyOdtImpedance[MSDG_MAX_PSTATE];  // Enter desired ODT impedance for DQ/DQS in Ohm  for each pstates
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)

    uint16_t PhyDrvImpedancePU[MSDG_MAX_PSTATE];// Tx Pull-up Drive Impedance for DQ/DQS in ohm for each pstates
                                // Valid values = 480,240,160,120,96,80,68,60,53,48,43,40,36,34,32,30,28
                                // Offset_0[15:0] - Impedance in Ohm for P0
                                // Offset_1[15:0] - Impedance in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Impedance in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Impedance in Ohm for P3 (invalid for Explorer)

    uint16_t PhyDrvImpedancePD[MSDG_MAX_PSTATE];// Tx Pull-down Drive Impedance for DQ/DQS in ohm for each pstates
                                // Valid values = 480,240,160,120,96,80,68,60,53,48,43,40,36,34,32,30,28
                                // Offset_0[15:0] - Impedance in Ohm for P0
                                // Offset_1[15:0] - Impedance in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Impedance in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Impedance in Ohm for P3 (invalid for Explorer)

    uint16_t PhySlewRate[MSDG_MAX_PSTATE];      // Enter desired slew rate setting for DQ/DQS for each pstates
                                // Valid values = 0~15 (0 has a slower slew rate than 15)
                                // [0] - Slew rate in Ohm for P0
                                // [1] - Slew rate in Ohm for P1 (invalid for Explorer)
                                // [2] - Slew rate in Ohm for P2 (invalid for Explorer)
                                // [3] - Slew rate in Ohm for P3 (invalid for Explorer)

    uint16_t ATxImpedance;        // Tx Drive Impedance for address/control bus in ohm
                                // Valid values  = 120, 60, 40, 30, 24, 20

    uint16_t ATxSlewRate;       // Enter desired slew rate setting for address/control bus
                // ATxSlewRate BIT [1:0] = 0: CA SR Slow mode; = 1: CA SR Fast mode; =2: CK SR mode
                // ATxSlewRate BIT [3:2] = 0: 6pf signal caps; = 1: 5pf signal caps; =2: 4pf signal caps; =3: 3pf signal caps;

    uint16_t CKTxImpedance;     // Tx Drive Impedance for CK bus in ohm
                                // Valid values  = 120, 60, 40, 30, 24, 20

    uint16_t CKTxSlewRate;      // Enter desired slew rate setting for CK bus
                // CKTxSlewRate BIT [1:0] = 0: CA SR Slow mode; = 1: CA SR Fast mode; =2: CK SR mode
                // CKTxSlewRate BIT [3:2] = 0: 6pf signal caps; = 1: 5pf signal caps; =2: 4pf signal caps; =3: 3pf signal caps;

    uint16_t AlertOdtImpedance;   // Enter desired ODT Impedance for alert_n in ohm
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 0(Disabled)

    uint16_t DramRttNomR0[MSDG_MAX_PSTATE];     // Enter desired RttNom of Rank0 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttNomR1[MSDG_MAX_PSTATE];     // Enter desired RttNom of Rank1 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttNomR2[MSDG_MAX_PSTATE];     // Enter desired RttNom of Rank2 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttNomR3[MSDG_MAX_PSTATE];     // Enter desired RttNom of Rank3 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)

    uint16_t DramRttWrR0[MSDG_MAX_PSTATE];      // Enter desired RttWr of Rank0 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttWrR1[MSDG_MAX_PSTATE];      // Enter desired RttWr of Rank1 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttWrR2[MSDG_MAX_PSTATE];      // Enter desired RttWr of Rank2 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttWrR3[MSDG_MAX_PSTATE];      // Enter desired RttWr of Rank3 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)

    uint16_t DramRttParkR0[MSDG_MAX_PSTATE];    // Enter desired RttPark of Rank0 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttParkR1[MSDG_MAX_PSTATE];    // Enter desired RttPark of Rank1 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttParkR2[MSDG_MAX_PSTATE];    // Enter desired RttPark of Rank2 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)
    uint16_t DramRttParkR3[MSDG_MAX_PSTATE];    // Enter desired RttPark of Rank3 in Ohm 
                                // Enter 0 for high-impedance
                                // Valid values for DDR4   = 240, 120, 80, 60, 48, 40, 34, 0(Disabled)
                                // Offset_0[15:0] - ODT in Ohm for P0
                                // Offset_1[15:0] - ODT in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT in Ohm for P3 (invalid for Explorer)

    uint16_t DramDic[MSDG_MAX_PSTATE];          // Tx Drive Impedance for DQ/DQS of all DRAM ranks in ohm
                                // Valid values for all DramType = 48, 34
                                // Offset_0[15:0] - Impedance in Ohm for P0
                                // Offset_1[15:0] - Impedance in Ohm for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Impedance in Ohm for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Impedance in Ohm for P3 (invalid for Explorer)

    uint16_t DramWritePreamble[MSDG_MAX_PSTATE];// Write Preamble setting for DRAM (MR4)
                                // 0 = 1 nCK; 1 = 2 nCK;(only available at DDR4 2400~3200)
                                // Offset_0[15:0] - Write Preamble setting for P0
                                // Offset_1[15:0] - Write Preamble setting for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Write Preamble setting for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Write Preamble setting for P3 (invalid for Explorer)
    uint16_t DramReadPreamble[MSDG_MAX_PSTATE]; // Read Preamble setting for DRAM (MR4)
                                // 0 = 1 nCK; 1 = 2 nCK;
                                // Offset_0[15:0] - Read Preamble setting for P0
                                // Offset_1[15:0] - Read Preamble setting for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Read Preamble setting for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Read Preamble setting for P3 (invalid for Explorer)

    uint16_t PhyEqualization[MSDG_MAX_PSTATE];  // Control DFE of DQ/DQS receiver and FFE of DQ/DQS driver
                // PhyEqualization BIT [0] = 1: Enable Receiver DFE; = 0: Disable Receiver DFE
                // PhyEqualization BIT [1] = 1: Enable Driver FFE; = 0: Disable Driver FFE
                                // Offset_0[15:0] - DFE/FFE setting for P0
                                // Offset_1[15:0] - DFE/FFE setting for P1 (invalid for Explorer)
                                // Offset_2[15:0] - DFE/FFE setting for P2 (invalid for Explorer)
                                // Offset_3[15:0] - DFE/FFE setting for P3 (invalid for Explorer)

    uint16_t InitVrefDQ[MSDG_MAX_PSTATE];   // Initial Global VrefDQ (MR6) setting before training
                                // For example, 0x17 -> 74.9%, 0x0f -> 69.75%, 0x9-> 65.85%
                // InitVrefDQ BIT [6] = VrefDQ training range (same as MR6[6])
                // InitVrefDQ BIT [5:0] = VrefDQ training value (same as MR6[5:0])
                                // Offset_0[15:0] - Initial VrefDQ setting for P0
                                // Offset_1[15:0] - Initial VrefDQ setting for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Initial VrefDQ setting for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Initial VrefDQ setting for P3 (invalid for Explorer)

    uint16_t InitPhyVref[MSDG_MAX_PSTATE];  // Initial Global DQ Vref setting of PHY before training
                                // Receiver Vref = VDDQ*PhyVref[6:0]/128
                // For example, 0x60 = 75% * VDDQ
                                // Offset_0[15:0] - Initial PhyVref setting for P0
                                // Offset_1[15:0] - Initial PhyVref setting for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Initial PhyVref setting for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Initial PhyVref setting for P3 (invalid for Explorer)

    uint16_t OdtWrMapCs[MSDG_MAX_PSTATE];       // Enter desired ODT[3:0] value when writing to ranks
                                // in normal mode (2 rank)
                                // OdtWrMapCs BIT [1:0] ODT_A/B[1:0] value when writing to rank 0
                                // OdtWrMapCs BIT [5:4] ODT_A/B[1:0] value when writing to rank 1
                                // If EncodedQuadCs = 1
                                // OdtWrMapCs BIT [1:0] ODT_A/B[1:0] value when writing to rank 0
                                // OdtWrMapCs BIT [5:4] ODT_A/B[1:0] value when writing to rank 1
                                // OdtWrMapCs BIT [9:8] ODT_A/B[1:0] value when writing to rank 2
                                // OdtWrMapCs BIT [13:12] ODT_A/B[1:0] value when writing to rank 3
                                // If Rank4Mode = 1 
                                // OdtWrMapCs BIT [1:0] ODT_A[1:0] value when writing to rank 0
                                // OdtWrMapCs BIT [3:2] ODT_B[1:0] value when writing to rank 0
                                // OdtWrMapCs BIT [5:4] ODT_A[1:0] value when writing to rank 1
                                // OdtWrMapCs BIT [7:6] ODT_B[1:0] value when writing to rank 1
                                // OdtWrMapCs BIT [9:8] ODT_A[1:0] value when writing to rank 2
                                // OdtWrMapCs BIT [11:10] ODT_B[1:0] value when writing to rank 2
                                // OdtWrMapCs BIT [13:12] ODT_A[1:0] value when writing to rank 3
                                // OdtWrMapCs BIT [15:14] ODT_B[1:0] value when writing to rank 3
                                // Offset_0[15:0] - ODT value for P0
                                // Offset_1[15:0] - ODT value for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT value for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT value for P3 (invalid for Explorer)

    uint16_t OdtRdMapCs[MSDG_MAX_PSTATE];       // Enter desired ODT[3:0] value when reading from ranks
                                // in normal mode (2 rank)
                                // OdtRdMapCs BIT [1:0] ODT_A/B[1:0] value when reading to rank 0
                                // OdtRdMapCs BIT [5:4] ODT_A/B[1:0] value when reading to rank 1
                                // If EncodedQuadCs = 1
                                // OdtRdMapCs BIT [1:0] ODT_A/B[1:0] value when reading to rank 0
                                // OdtRdMapCs BIT [5:4] ODT_A/B[1:0] value when reading to rank 1
                                // OdtRdMapCs BIT [9:8] ODT_A/B[1:0] value when reading to rank 2
                                // OdtRdMapCs BIT [13:12] ODT_A/B[1:0] value when reading to rank 3
                                // If Rank4Mode = 1 
                                // OdtRdMapCs BIT [1:0] ODT_A[1:0] value when reading to rank 0
                                // OdtRdMapCs BIT [3:2] ODT_B[1:0] value when reading to rank 0
                                // OdtRdMapCs BIT [5:4] ODT_A[1:0] value when reading to rank 1
                                // OdtRdMapCs BIT [7:6] ODT_B[1:0] value when reading to rank 1
                                // OdtRdMapCs BIT [9:8] ODT_A[1:0] value when reading to rank 2
                                // OdtRdMapCs BIT [11:10] ODT_B[1:0] value when reading to rank 2
                                // OdtRdMapCs BIT [13:12] ODT_A[1:0] value when reading to rank 3
                                // OdtRdMapCs BIT [15:14] ODT_B[1:0] value when reading to rank 3
                                // Offset_0[15:0] - ODT value for P0
                                // Offset_1[15:0] - ODT value for P1 (invalid for Explorer)
                                // Offset_2[15:0] - ODT value for P2 (invalid for Explorer)
                                // Offset_3[15:0] - ODT value for P3 (invalid for Explorer)

    uint16_t Geardown[MSDG_MAX_PSTATE];         // Enable geardown mode during training/dfi_bist. 
                                // 0 = 1/2 Rate; 1 = 1/4 Rate;
                                // Offset_0[15:0] - Geardown value for P0
                                // Offset_1[15:0] - Geardown value for P1 (invalid for Explorer)
                                // Offset_2[15:0] - Geardown value for P2 (invalid for Explorer)
                                // Offset_3[15:0] - Geardown value for P3 (invalid for Explorer)

    uint16_t CALatencyAdder[MSDG_MAX_PSTATE];   // Value of RCD parity checking & Command Latency Adder (F0RC0E, FORC0F)
                                // 0 = 0nCK latency adder, parity disabled; 
                // 1 = 1nCK latency adder;
                // 2 = 2nCK latency adder;
                // 3 = 3nCK latency adder;
                // 4 = 4nCK latency adder;
                                // Offset_0[15:0] - CALatencyAdder value for P0
                                // Offset_1[15:0] - CALatencyAdder value for P1 (invalid for Explorer)
                                // Offset_2[15:0] - CALatencyAdder value for P2 (invalid for Explorer)
                                // Offset_3[15:0] - CALatencyAdder value for P3 (invalid for Explorer)

    uint16_t BistCALMode[MSDG_MAX_PSTATE];      // Value of CS to CMD/ADDR Latency mode (MR4.CAL) for dfi_bist
                // (training runs with CALMode = 0)
                                // Valid value: 0,3,4,5,6,8
                                // Offset_0[15:0] - BistCALMode value for P0
                                // Offset_1[15:0] - BistCALMode value for P1 (invalid for Explorer)
                                // Offset_2[15:0] - BistCALMode value for P2 (invalid for Explorer)
                                // Offset_3[15:0] - BistCALMode value for P3 (invalid for Explorer)

    uint16_t BistCAParityLatency[MSDG_MAX_PSTATE]; // Value of CA Parity Latency mode (MR5.PL) for dfi_bist
                // (training runs with CAParityLatency = 0)
                                // Valid value: 0,4,5,6,8
                                // Offset_0[15:0] - BistCAParityLatency for P0
                                // Offset_1[15:0] - BistCAParityLatency for P1 (invalid for Explorer)
                                // Offset_2[15:0] - BistCAParityLatency for P2 (invalid for Explorer)
                                // Offset_3[15:0] - BistCAParityLatency for P3 (invalid for Explorer)

    uint16_t RcdDic[MSDG_MAX_PSTATE]; // F0RC03[3:0], F0RC04[3:0], F0RC05[3:0] CA and CS signal Driver Characteristics
                                // RcdDic BIT [1:0] QxA,QxBA,QxBG...: =00 light; =01 moderate; =10 strong; =11 very strong
                                // RcdDic BIT [3:2] QxCSx_n: =00 light; =01 moderate; =10 strong; =11 very strong
                                // RcdDic BIT [5:4] QxODT: =00 light; =01 moderate; =10 strong; =11 very strong
                                // RcdDic BIT [7:6] QxCKE: =00 light; =01 moderate; =10 strong; =11 very strong
                                // RcdDic BIT [9:8] Y1/Y3(A side): =00 light; =01 moderate; =10 strong; =11 very strong
                                // RcdDic BIT [11:10] Y0/Y2(B side): =00 light; =01 moderate; =10 strong; =11 very strong
                                // Offset_0[15:0] - RcdDic for P0
                                // Offset_1[15:0] - RcdDic for P1 (invalid for Explorer)
                                // Offset_2[15:0] - RcdDic for P2 (invalid for Explorer)
                                // Offset_3[15:0] - RcdDic for P3 (invalid for Explorer)

    uint16_t RcdVoltageCtrl[MSDG_MAX_PSTATE]; // RCD operating voltage VDD and VrefCA control
                // RcdVoltageCtrl BIT [3:0] F0RC0B;
                // RcdVoltageCtrl BIT [11:4] F0RC1x;
                                // Offset_0[15:0] - RcdVoltageCtrl for P0
                                // Offset_1[15:0] - RcdVoltageCtrl for P1 (invalid for Explorer)
                                // Offset_2[15:0] - RcdVoltageCtrl for P2 (invalid for Explorer)
                                // Offset_3[15:0] - RcdVoltageCtrl for P3 (invalid for Explorer)

    uint16_t RcdIBTCtrl[MSDG_MAX_PSTATE];        // RCD IBT Control Word (F0RC7x)
                // RcdIBTCtrl BIT [1:0] CA Input Bus Termination
                // RcdIBTCtrl BIT [3:2] DCS[3:0]_n Input Bus Termination
                // RcdIBTCtrl BIT [5:4] DCKE Input Bus Termination
                // RcdIBTCtrl BIT [7:6] DODT Input Bus Termination
                                // Offset_0[15:0] - RcdIBTCtrl for P0
                                // Offset_1[15:0] - RcdIBTCtrl for P1 (invalid for Explorer)
                                // Offset_2[15:0] - RcdIBTCtrl for P2 (invalid for Explorer)
                                // Offset_3[15:0] - RcdIBTCtrl for P3 (invalid for Explorer)

    uint16_t RcdDBDic[MSDG_MAX_PSTATE];     // RCD Data Buffer Interface Driver Characteristics (F1RC00)
                // RcdDBDic BIT [0] BCOM[3:0],BODT,BCKE, driver strength
                // RcdDBDic BIT [1] Reserved
                // RcdDBDic BIT [2] BCK_t/BCK_c driver strength
                // RcdDBDic BIT [3] Reserved
                                // Offset_0[15:0] - RcdDBDic for P0
                                // Offset_1[15:0] - RcdDBDic for P1 (invalid for Explorer)
                                // Offset_2[15:0] - RcdDBDic for P2 (invalid for Explorer)
                                // Offset_3[15:0] - RcdDBDic for P3 (invalid for Explorer)

    uint16_t RcdSlewRate[MSDG_MAX_PSTATE];  // RCD slew rate control (F1RC02,F1RC03,F1RC04,F1RC05)
                // RcdSlewRate BIT [1:0]   slew rate control of address/command
                // RcdSlewRate BIT [3:2]   slew rate control of QxCs*_n
                // RcdSlewRate BIT [5:4]   slew rate control of QxODT*
                // RcdSlewRate BIT [7:6]   slew rate control of QxCKE*
                // RcdSlewRate BIT [9:8]   slew rate control of Y1_t/c, Y3_t/c
                // RcdSlewRate BIT [11:10] slew rate control of Y0_t/c, Y2_t/c
                // RcdSlewRate BIT [13:12] slew rate control of BCOM[3:0], BODT, BCKE
                // RcdSlewRate BIT [15:14] slew rate control of BCK_t/c
                                // Offset_0[15:0] - RcdSlewRate for P0
                                // Offset_1[15:0] - RcdSlewRate for P1 (invalid for Explorer)
                                // Offset_2[15:0] - RcdSlewRate for P2 (invalid for Explorer)
                                // Offset_3[15:0] - RcdSlewRate for P3 (invalid for Explorer)

    uint16_t DFIMRL_DDRCLK;     // DFIMRL_DDRCLK: Max Read Latency counted by DDR Clock. Valid value: 0 ~ 31
                                // dfi_rddata is returned (14 + DFIMRL_DDRCLK) cycles after dfi_rddata_en is asserted. 
    uint8_t  ATxDly_A[MSDG_MAX_PSTATE][8];      //7bit A-side AC Delay 
                                                //ATxDly_A[pstate][NumAnib]
    uint8_t  ATxDly_B[MSDG_MAX_PSTATE][8];      //7bit B-side AC Delay 
                                                //ATxDly_B[pstate][NumAnib]
                                                //ATxDly[6] is the coarse delay, ie one unit of delay is 1 UI.  
                                                //ATxDly[5] is reserved.
                                                //ATxDly[4:0] is the fine delay, ie one unit of delay is one-thirtysecond of a UI = UI/32.
                                                //ATxDly_A/B[0]: ODT[1],ODT[0],CS_N[0],CS_N[1]
                                                //ATxDly_A/B[1]: ADDR[13],ADDR[5],BG[0],CKE[1]
                                                //ATxDly_A/B[2]: ADDR[17],ADDR[7],BA[0],ADDR[16]
                                                //ATxDly_A/B[3]: ADDR[8],BG[1],CID[1],CID[0]
                                                //ATxDly_A/B[4]: ADDR[1],ADDR[9],ADDR[2],CAPARITY
                                                //ATxDly_A/B[5]: ADDR[12],ADDR[3],ADDR[4],ADDR[0]
                                                //ATxDly_A/B[6]: CKE[0],ADDR[15],ACT_N,ADDR[10]
                                                //ATxDly_A/B[7]: ADDR[11],ADDR[6],BA[1],ADDR[14]
} user_input_msdg_t;

typedef enum {
    NO_EYE_SAVE = 0,
    SAVE_READ_EYE = 1,
    SAVE_WRITE_EYE = 2
} save_eye_data;

typedef struct user_input_dbg_msdg {
    uint8_t  HdtCtrl;          // To control the total number of debug messages, a verbosity subfield (HdtCtrl, Hardware Debug Trace Control) exists in the message block. Every message has a verbosity level associated with it, and as the HdtCtrl value is increased, less important s messages stop being sent through the mailboxes. The meanings of several major HdtCtrl thresholds are explained below:
                              // 
                              //    0x05 = Detailed debug messages (e.g. Eye delays)
                              //    0x0A = Coarse debug messages (e.g. rank information)
                              //    0xC8 = Stage completion
                              //    0xC9 = Assertion messages
                              //    0xFF = Firmware completion messages only
                              // 
    uint8_t  Delay_Weight2D;   // During 2D training, the ideal eye center changes depending on how valuable delay margin is compared to voltage margin. delay_weight2D sets the value, or weight, of one step of delay margin. The ratio of voltage_weight2D to delay_weight2D will be used by 2D training to choose your preferred center point. There are 32 delay steps in a perfect eye.
    uint8_t  Voltage_Weight2D; // During 2D training, the ideal eye center changes depending on how valuable voltage margin is compared to delay margin. voltage_weight2D sets the value, or weight, of one step of voltage margin. The ratio of voltage_weight2D to delay_weight2D will be used by 2D training to choose your preferred center point. There are 128 voltage steps in a perfect eye. 
    uint8_t  DisableChipKill;  // To disable ChipKill feature of firmware training. Chipkill feature is only valid for X4 DRAM. When it's disabled, firmware training will error out when any training failure is detected, and it won't return failure status.
    uint8_t DelayRankIndex;      // To represent the training result of which rank is stored in which rank.
                    //BIT [1:0] = 0,1,2,3 the training result of which rank is stored in rank 0 (TG0) registers
                    //BIT [3:2] = 0,1,2,3 the training result of which rank is stored in rank 1 (TG1) registers
                    //BIT [5:4] = 0,1,2,3 the training result of which rank is stored in rank 2 (TG2) registers
                    //BIT [7:6] = 0,1,2,3 the training result of which rank is stored in rank 3 (TG3) registers
                    //By default, DelayRankIndex=0xE4. For Encoded QuadCs Dram, it would change to work-around bugs.
    uint8_t ARdPtrInitVal;      // PHY register control that affects PHY output latency. For Explorer default is 1. 
                                // ARdPtrInitVal = 0...7
    uint16_t SequenceCtrl_1D;   // Controls the 1D training steps to be run. Each bit corresponds to a training step. 
                                // 
                                // If the bit is set to 1, the training step will run. 
                                // If the bit is set to 0, the training step will be skipped.
                                // 
                                // Training step to bit mapping:
                                // SequenceCtrl_1D BIT [0] = Run DevInit - Device/phy initialization. Should always be set.
                                // SequenceCtrl_1D BIT [1] = Run WrLvl - Write leveling
                                // SequenceCtrl_1D BIT [2] = Run RxEn - Read gate training
                                // SequenceCtrl_1D BIT [3] = Run RdDQS1D - 1d read dqs training
                                // SequenceCtrl_1D BIT [4] = Run WrDQ1D - 1d write dq training
                                // SequenceCtrl_1D BIT [5] = RFU, must be zero
                                // SequenceCtrl_1D BIT [6] = RFU, must be zero
                                // SequenceCtrl_1D BIT [7] =  RFU, must be zero
                                // SequenceCtrl_1D BIT [8] = Run RdDeskew - Per lane read dq deskew training
                                // SequenceCtrl_1D BIT [9] = Run MxRdLat - Max read latency training
                                // SequenceCtrl_1D BIT [10] = Run Reserved
                                // SequenceCtrl_1D BIT [11] = Run Reserved
                                // SequenceCtrl_1D BIT [12] = Run Reserved
                                // SequenceCtrl_1D BIT [13] = Run Reserved
                                // SequenceCtrl_1D BIT [15-14] =  RFU, must be zero
    uint16_t SequenceCtrl_2D;   // Controls the 2D training steps to be run. Each bit corresponds to a training step. 
                                // 
                                // If the bit is set to 1, the training step will run. 
                                // If the bit is set to 0, the training step will be skipped.
                                // 
                                // Training step to bit mapping:
                                // SequenceCtrl_2D BIT[0] = Run DevInit - Device/phy initialization. Should always be set.
                                // SequenceCtrl_2D BIT[1] = RFU, must be zero
                                // SequenceCtrl_2D BIT[2] = RFU, must be zero
                                // SequenceCtrl_2D BIT[3] = RFU, must be zero
                                // SequenceCtrl_2D BIT[4] = RFU, must be zero
                                // SequenceCtrl_2D BIT[5] = Run rd2D - 2d read dqs training
                                // SequenceCtrl_2D BIT[6] = Run wr2D - 2d write dq training
                                // SequenceCtrl_2D BIT[7] =  RFU, must be zero
                                // SequenceCtrl_2D BIT[8] = RFU, must be zero
                                // SequenceCtrl_2D BIT[9] = RFU, must be zero
                                // SequenceCtrl_2D BIT[10] = RFU, must be zero
                                // SequenceCtrl_2D BIT[11] = RFU, must be zero
                                // SequenceCtrl_2D BIT[12] = RFU, must be zero
                                // SequenceCtrl_2D BIT[13] = RFU, must be zero
                                // SequenceCtrl_2D BIT[15-14] =  RFU, must be zero
    uint16_t EmulationSupport;  // (Reserved) Enable Special mode for Emulation and GateSim Support
                // EmulationSupport BIT [0] = 0 Normal firmware mode
                // EmulationSupport BIT [0] = 1 Emulation firmware mode
                // EmulationSupport BIT [1] = 0 Normal firmware mode
                // EmulationSupport BIT [1] = 1 Gate sim firmware mode
                // EmulationSupport BIT [2] = 0 Normal firmware mode
                // EmulationSupport BIT [2] = 1 Zero delay Gate sim firmware mode
    uint32_t BSideNibble;       // [19:0] each bit to represent one nibble is on B side of an RDIMM/LRDIMM.

} user_input_dbg_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct user_response_timing_msdg {
#else
typedef __packed struct user_response_timing_msdg {
#endif
    uint16_t DFIMRL_DDRCLK_trained; // Training result of DFIMRL_DDRCLK parameter (by mrlTraining step).
                                    // DFIMRL_DDRCLK: Max Read Latency counted by DDR Clock. dfi_rddata is returned (14 + DFIMRL_DDRCLK) cycles after dfi_rddata_en is asserted. 
    //DFI rank-to rank space timing must be determined by the actual board delay (DQ/DQS bus turnarounds). 
    //PHY firmware training result provide CDD (critical delay difference) information to help to calculate the minimum required timing spacing for memory controllers.
    //Minimal possible gap between rank-to-rank read-read transaction is defined by tCCDmin(R_rank[i], R_rank[j]) = 4 + max(abs(CDD_RR_[i]_[j]));
    int8_t   CDD_RR[4][4];      // CDD_RR[n][m]: This is a signed integer value. Read to read critical delay difference from cs n to cs m
                                // CDD_RR[0][0], CDD_RR[1][1], CDD_RR[2][2], CDD_RR[3][3] are always 0; 
    //Minimal possible gap between rank-to-rank write-write transaction is defined by tCCDmin(W_rank[i], W_rank[j]) = 4 + max(abs(CDD_WW_[i]_[j]));
    int8_t   CDD_WW[4][4];      // CDD_WW[n][m]: This is a signed integer value. Write to write critical delay difference from cs n to cs m
                                // CDD_WW[0][0], CDD_WW[1][1], CDD_WW[2][2], CDD_WW[3][3] are always 0; 
    //Minimal possible gap between rank-to-rank read-write transaction is defined by tCCDmin(R_rank[i], W_rank[j]) = (RL + BL/2 + 1 + WR_PREAMBLE - WL) + max(abs(CDD_RW_[i]_[j]));
    //RL: Read Latency; WL: Write Latency; BL: Burst Length; WR_PREAMBLE: Write Preamble cycles
    int8_t   CDD_RW[4][4];      // CDD_RW[n][m]This is a signed integer value. Read to write critical delay difference from cs n to cs m
    //Minimal possible gap between rank-to-rank write-read transaction is defined by tCCDmin(W_rank[i], R_rank[j]) = (WL + PL + BL/2 + tWTR_L) + max(abs(CDD_RW_[i]_[j]));
    //WL: Write Latency; BL: Burst Length; PL: CA Parity Latency; tWTR_L: delay from internal write to internal read for same bank group 
    int8_t   CDD_WR[4][4];      // CDD_WR[n][m]This is a signed integer value. Write to read critical delay difference from cs n to cs m
} user_response_timing_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct user_response_error_msdg {
#else
typedef __packed struct user_response_error_msdg {
#endif
    uint16_t Failure_Lane[80]; // error code of DQ[n] on Rank 3,2,1 & 0. Rank 0 is in LS Nibble.
    //Failure status of training. Each uint16_t field contains the training error code of all 4 ranks on 1 DQ lane. 
    //4-bit error code reports the training errors:
    //0x0: No Error
    //0x1: DevInit Error
    //0x2: RxEnable Error
    //0x3: Find Write Leveling Error
    //0x4: Read Deskew Error
    //0x5: Read 1D SI Friendly Training Error
    //0x6: Coarse Write Leveling Error
    //0x7: Write 1D Training Error
    //0x8: Read 1D Training Error
    //0x9: DFIMRL_DDRCLK Training Error
    //0xa~0xF: Reserved
} user_response_error_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct user_response_mrs_msdg {
#else
typedef __packed struct user_response_mrs_msdg {
#endif
    uint16_t MR0;               // Value of DDR mode register MR0 for all ranks, all devices
    uint16_t MR1[4];            // Value of DDR mode register MR1 for each rank (up to 4 ranks)
    uint16_t MR2[4];            // Value of DDR mode register MR2 for each rank (up to 4 ranks)
    uint16_t MR3;               // Value of DDR mode register MR3 for all ranks, all devices
    uint16_t MR4;               // Value of DDR mode register MR4 for all ranks, all devices
    uint16_t MR5[4];            // Value of DDR mode register MR5 for each rank (up to 4 ranks)
    uint16_t MR6[4][20];        // Value of DDR mode register MR6 for each nibble on each rank 
                                // for X8,X16 DRAMs MR6[i][2n+1] = MR6[i][2n] (n = 0~9)
} user_response_mrs_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct user_response_rc_msdg {
#else
typedef __packed struct user_response_rc_msdg {
#endif
    uint8_t   F0RC_D0[27];      // RCD control words for DIMM0; Invalid for UDIMM
                                // F0RC_D0[15:0] BIT [3:0]: 4-bit value of F0RC00~F0RC0F
                                // F0RC_D0[26:16] BIT [7:0]: 8-bit value of F0RC1x~F0RCBx
    uint8_t   F1RC_D0[27];      // RCD control words for DIMM0; Invalid for UDIMM
                                // F1RC_D0[15:0] BIT [3:0]: 4-bit value of F1RC00~F1RC0F
                                // F1RC_D0[26:16] BIT [7:0]: 8-bit value of F1RC1x~F1RCBx
    uint8_t   F0RC_D1[27];      // RCD control words for DIMM1; Invalid for UDIMM
                                // F0RC_D0[15:0] BIT [3:0]: 4-bit value of F0RC00~F0RC0F
                                // F0RC_D0[26:16] BIT [7:0]: 8-bit value of F0RC1x~F0RCBx
    uint8_t   F1RC_D1[27];      // RCD control words for DIMM1; Invalid for UDIMM
                                // F1RC_D0[15:0] BIT [3:0]: 4-bit value of F1RC00~F1RC0F
                                // F1RC_D0[26:16] BIT [7:0]: 8-bit value of F1RC1x~F1RCBx
} user_response_rc_msdg_t;

//#############################################################################
//
// Structure for MSCC Response
//
//#############################################################################
#ifdef PMC_DDR_SIM_DPI
typedef struct user_response_msdg {
#else
typedef __packed struct user_response_msdg {
#endif
    uint32_t            version_number;
    user_response_timing_msdg_t tm_resp;
    user_response_error_msdg_t  err_resp;
    user_response_mrs_msdg_t    mrs_resp;
    user_response_rc_msdg_t rc_resp;

}user_response_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct train_2d_eye_min_max_msdg {
#else
typedef __packed struct train_2d_eye_min_max_msdg {
#endif
    //2D training has to run with 1D training results in the delay registers. Horizontally it takes 1D 
    //centered value as starting position and only sweep half of a UI to left and right (1 UI in total).
    //Vertically it takes preset vref value as starting position and sweep until bit error is detected.
    //The assumption is that data eye will be enclosed in that range.
    //eye_max/min contain the max/min VrefDAC0/VrefDQ value which passes test with the Nth step shift
    //within the 1UI range. The 1D centered RxClkDly/TxDqDly is always normalized to index 15 of the
    //array. 
    //In train_2d_read_eye_msdg, eye_max/min value represents VrefDAC0 (PHY DQ receiver Vref setting)
    //Vref = (0.510 + VrefDAC0[6:0] * 0.00385) * VDDQ
    //In train_2d_write_eye_msdg, eye_max/min value represents VrefDQ (Dram DQ receiver Vref setting)
    //Vref = (0.450 + VrefDQ[6:0] * 0.0065) * VDDQ

    uint16_t eye_min[31];
    uint16_t eye_max[31];
} train_2d_eye_min_max_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct train_2d_read_eye_msdg {
#else
typedef __packed struct train_2d_read_eye_msdg {
#endif
    //train_2d_read_eye_msdg_t returns the read eye diagram from point of view of 2D training firmware.
    //1D training center RxClkDly (per nibble) horizontally (left and right search), 2D training does 
    //sweep on horizontal (RxClkDly) and vertical (VrefDAC0) directions, then decide the center.
    //2D training has to run with 1D training results in the delay registers. Horizontally it takes 1D 
    //centered value as starting position and only sweep half of a UI to left and right (1 UI in total).
    //Vertically it takes preset vref value as starting position and sweep until bit error is detected.
    //The assumption is that data eye will be enclosed in that range.
    //VrefDAC0[*][*][*] contains the max/min VrefDAC0 value which passes test with the N-15 delay offset from nominal
    //within the 1UI range. The 1D centered RxClkDly is always normalized to index 15 of the 
    //array.
    //With both arrays, 2D read eye diagram can be plotted on debug host.
    train_2d_eye_min_max_msdg_t VrefDAC0[4][10][8];  // VrefDAC0[RANKi][DBYTEn][BITn] Maximum
                                                     // and minimum passing VrefDAC0 in 2D read training
    uint16_t VrefDAC0_Center[10][8];                 // VrefDAC0_Center[DBYTEn][BITn] Centered
                                                     // VrefDAC0 value after 2D read training
    uint16_t RxClkDly_Center[4][20];                 // RxClkDly_Center[RANKi][NIBBLEn] Centered
                                                     // RxClkDly location (w.r.t. eye diagram)  after 2D
                                                     // read training
} train_2d_read_eye_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct train_2d_write_eye_msdg {
#else
typedef __packed struct train_2d_write_eye_msdg {
#endif
    //train_2d_write_eye_msdg_t returns the write eye diagram from point of view of 2D training 
    //firmware.
    //1D training center TxDqDly (per DQ) horizontally (left and right search), 2D training does sweep 
    //on horizontal (TxDqDly) and vertical (VrefDQ) directions, then decide the center.
    //2D training has to run with 1D training results in the delay registers. Horizontally it takes 1D 
    //centered value as starting position and only sweep half of a UI to left and right (1 UI in total).
    //Vertically it takes preset vref value as starting position and sweep until bit error is detected. 
    //The assumption is that data eye will be enclosed in that range.
    //VrefDQ[*][*][*] contains the max/min VrefDQ value which passes test with the N-15 delay offset from nominal
    //the 1UI range. The 1D centered TxDqDly is always normalized to index 15 of the array.
    //With both arrays, 2D read eye diagram can be plotted on debug host.
    train_2d_eye_min_max_msdg_t VrefDQ[4][10][8];  // VrefDQ[RANKi][DBYTEn][BITn] Maximum and
                                                   // minimum passing VrefDQ in 2D write training
    uint16_t VrefDQ_Center[4][20];                 // VrefDQ_Center[RANKi][NIBBLEn] Centered VrefDQ
                                                   // value after 2D write training
    uint16_t TxDqDly_Center[4][10][8];             // TxDqDly_Center[RANKi][DBYTEn][BITn] Centered
                                                   // TxDqDly location (w.r.t. eye diagram)  after 2D
                                                   // write training
} train_2d_write_eye_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct user_2d_eye_response_1_msdg {
#else
typedef __packed struct user_2d_eye_response_1_msdg {
#endif
    uint32_t                 version_number;
    train_2d_read_eye_msdg_t read_2d_eye_resp;
    user_response_timing_msdg_t tm_resp;
    user_response_error_msdg_t  err_resp;
    user_response_mrs_msdg_t    mrs_resp;
    user_response_rc_msdg_t     rc_resp;
}user_2d_eye_response_1_msdg_t;

#ifdef PMC_DDR_SIM_DPI
typedef struct user_2d_eye_response_2_msdg {
#else
typedef __packed struct user_2d_eye_response_2_msdg {
#endif
    uint32_t                    version_number;
    train_2d_write_eye_msdg_t   write_2d_eye_resp;
    user_response_timing_msdg_t tm_resp;
    user_response_error_msdg_t  err_resp;
    user_response_mrs_msdg_t    mrs_resp;
    user_response_rc_msdg_t     rc_resp;
}user_2d_eye_response_2_msdg_t;


typedef struct train_delay_msdg {
    uint16_t TxDqDly[MSDG_MAX_PSTATE][4][80];       //9bit Write Data Delay
                                    //TxDqDly[pstate][rank][DQ index]
    uint16_t TxDmDly[MSDG_MAX_PSTATE][4][10];       //9bit Write DM Delay (invalid in X4 device)
                                    //TxDmDly[pstate][rank][byte index]
    uint16_t TxDqsDly_u0[MSDG_MAX_PSTATE][4][10];   //10bit Write Dqs Delay (lower nibble)
                                    //TxDqsDly_u0[pstate][rank][byte index]
    uint16_t TxDqsDly_u1[MSDG_MAX_PSTATE][4][10];   //10bit Write Dqs Delay (upper nibble)
                                    //TxDqsDly_u1[pstate][rank][byte index]
    uint8_t  RxClkDly_u0[MSDG_MAX_PSTATE][4][10];   //6bit Read DQS Delay (lower nibble)
                                    //RxClkDly_u0[pstate][rank][byte index]
    uint8_t  RxClkDly_u1[MSDG_MAX_PSTATE][4][10];   //6bit read DQS Delay (upper nibble)
                                    //RxClkDly_u1[pstate][rank][byte index]
    uint16_t RxEnDly_u0[MSDG_MAX_PSTATE][4][10];    //11bit Read Gate Delay (lower nibble)
                                    //RxEnDly_u0[pstate][rank][byte index]
    uint16_t RxEnDly_u1[MSDG_MAX_PSTATE][4][10];    //11bit Read Gate Delay (upper nibble)
                                    //RxEnDly_u1[pstate][rank][byte index]
    uint8_t  ATxDly_A[MSDG_MAX_PSTATE][10];             //7bit A-side AC Delay (ACX4[8]:CK0/1;ACX4[9]:N/A)
                                                        //ATxDly_A[pstate][NumAnib]
    uint8_t  ATxDly_B[MSDG_MAX_PSTATE][10];             //7bit B-side AC Delay (ACX4[8]:CK0/1;ACX4[9]:N/A)
                                                        //ATxDly_B[pstate][NumAnib]
    uint8_t  DFIMRL[MSDG_MAX_PSTATE][10];               //5 bit Max Read Latency (per DFICLK)
                                                        //DFIMRL[pstate][byte index]
    uint8_t  RxPBDly[4][80];                        //7bit Read Data Delay
                                    //RxPBDly[rank][DQ index]
    uint16_t StepSize[10];                              //LCDL Step size from lcdl_txln0
                                                        //It is used to convert RxPBDly 
                                                        //StepSize[byte index]
    uint8_t  DFIMRL_ddrclk[MSDG_MAX_PSTATE];            //5 bit Max Read Latency (per DFICTLCLK)
                                                        //DFIMRL_ddrclk[pstate]
} train_delay_msdg_t;

typedef struct board_tof_msdg {
    uint16_t tofCk[4][20];      //Ck topology delay in picosecond
                    //tofCk[rank][nibble]
    uint16_t tofRdDqs[4][20];       //DQS topology delay from DRAM to PHY in picosecond
                    //tofRdDqs[rank][nibble] (odd index is invalid for X8/16 device)
    uint16_t tofWrDqs[4][20];       //DQS topology delay from PHY to DRAM in picosecond
                    //tofWrDqs[rank][nibble] (odd index is invalid for X8/16 device)
    uint16_t tofDm[4][10];      //DM topology delay from PHY to DRAM in picosecond
                    //tofDm[rank][byte] (invalid for X4 device)
    uint16_t tofRdDq[4][80];        //DQ topology delay from DRAM to PHY in picosecond
                    //tofRdDq[rank][DQ index]
    uint16_t tofWrDq[4][80];        //DQ topology delay from PHY to DRAM in picosecond
                    //tofWrDq[rank][DQ index]
    uint16_t tofCk2Ca_A[8];            //Additional topology delay on A-side CK groups in picosecond
                                        //Topo delay of CK signals compares with CA signals from each ACX4 group. 
    uint16_t tofCk2Ca_B[8];            //Additional topology delay on B-side CK groups in picosecond
                                        //Topo delay of CK signals compares with CA signals from each ACX4 group. 
                                        //tofCk2Ca_A/B[0]: ODT[1],ODT[0],CS_N[0],CS_N[1]
                                        //tofCk2Ca_A/B[1]: ADDR[13],ADDR[5],BG[0],CKE[1]
                                        //tofCk2Ca_A/B[2]: ADDR[17],ADDR[7],BA[0],ADDR[16]
                                        //tofCk2Ca_A/B[3]: ADDR[8],BG[1],CID[1],CID[0]
                                        //tofCk2Ca_A/B[4]: ADDR[1],ADDR[9],ADDR[2],CAPARITY
                                        //tofCk2Ca_A/B[5]: ADDR[12],ADDR[3],ADDR[4],ADDR[0]
                                        //tofCk2Ca_A/B[6]: CKE[0],ADDR[15],ACT_N,ADDR[10]
                                        //tofCk2Ca_A/B[7]: ADDR[11],ADDR[6],BA[1],ADDR[14]
} board_tof_msdg_t;

typedef struct train_dram_vref_msdg {
   uint8_t  VrefDQ[MSDG_MAX_PSTATE][4][20];// VrefDQ value for each nibble MR6[6:0]. 
                    // VrefDQ[pstate][rank][nibble]
                    // X4 DRAM:  VrefDQ[pstate][rank][19:0] represents VrefDQ for each nibble
                    // X8 DRAM:  VrefDQ[pstate][rank][2*i+1] == VrefDQ[pstate][rank][2*i]
                    // X16 DRAM: VrefDQ[pstate][rank][4*i+3] == VrefDQ[pstate][rank][4*i+2] == VrefDQ[pstate][rank][4*i+1] == VrefDQ[pstate][rank][4*i]
} train_dram_vref_msdg_t;


typedef struct train_host_vref_msdg {
   uint8_t  VrefDAC0[80];               // VrefDAC0 value for each bit VrefDAC0[6:0]
                                        // PHY RX VREF DAC control for rxdq cell internal VREF, (used only when 2D training is enabled in DDR4)
                                        // DAC control for rxdq cell internal VREF, trained by Firmware
                                        // The VREF generators have different ranges, depending on the Mission Mode settings for
                                        // {DqDqsRcvCntrl::MajorMode,DqDqsRcvCntrl::ExtVrefRange}
                                        // {011,0} :: VREF = VDDQ*(0.510 + VrefDAC0[6:0]*0.00345)
   uint8_t  VrefDAC1[80];               // VrefDAC1 value for each bit VrefDAC1[6:0]
                                        // VrefDAC1 controls the alternate VREF setting for DFE (used only when DFE is enabled in DDR4)
                                        // DAC control for rxdq cell internal VREF, trained by Firmware
                                        // The VREF generators have different ranges, depending on the Mission Mode settings for
                                        // {DqDqsRcvCntrl::MajorMode,DqDqsRcvCntrl::ExtVrefRange}
                                        // {011,0} :: VREF = VDDQ*(0.510 + VrefDAC1[6:0]*0.00345)
                                        // {non-enumerated encodings are reserved}
   uint32_t  VrefInGlobal[MSDG_MAX_PSTATE]; // VrefInGlobal[pstate].
                    // Vref setting has two registers assocoated, it is better to put those critical registers in this same struct. Not clear why SNPS has per power state setting, will checking.
                                    // VrefInGlobal = {GlobalVrefInDac[6:0], GlobalVreInSel[2:0]}
                                    // GlobalVrefInDac[6:0]: DAC code for internal Vref generation,
                                    // For DDR4, GlobalVrefInSel[2] = 0, the coding :
                                    // DAC Output Voltage = GlobalVrefInDAC == 6'h00 ? Hi-Z : 0.345*VDDQ +
                    // (0.005*GlobalVrefInDAC)*VDDQ
                    // GlobalVrefInSel[1:0] :
                    // 2'b00 - PHY Vref DAC Range0 -- BP_VREF = Hi-Z   (internal Vref mode)
                    // 2'b01 - Reserved Encoding
                    // 2'b10 - PHY Vref DAC Range0 -- BP_VREF connected to PLL Analog Bus (used and anlog test pin)
                    // 2'b11 - PHY Vref DAC Range0 -- BP_VREF connected to PHY Vref DAC   (External VREF mode)
   uint32_t  DqDqsRcvCntrl[MSDG_MAX_PSTATE][10][2];    // DqDqsRcvCntrl[pstate][byte][nibble]
                    // = {GainCurrAdj[4:0], MajorModebyte[2:0], DfeCtrl[1:0], ExtVrefRange, SelAnalogVref}
                    // GainCurrAdj[4:0] :   Adjust gain current of RX amplifier stage, It is recommended to use default values for this CSR.
                    // MajorModebyte[2:0] :
                    // Selects the major mode of operation for the receiver.
                    // These settings are determined by PHY Configuration based on DRAM protocol
                    // 000 - Input signal is terminated/referenced to VDDQ/2 (or unterminated)
                    // 001 - reserved
                    // 010 - Input signal is terminated/referenced to VSSQ
                    // 011 - Input signal is terminated/referenced to VDDQ, this is the recommended mode to use for DDR4.
                    // others - reserved.
                    // DFECTRL
                    // 00 - DFE off
                    // 01 - DFE on
                    // 10 - Train DFE0 Amplifier
                    // 11 - Train DFE1 Amplifier
                    // These settings are determined by PHY Training FW and should not
                    // be overridden.
                    // ExtVrefRange: Extends the range available in the local per-bit VREF gener
                    // SelAnalogVref  :  Setting this signal high will force the local per-bit VREF generator to pass the global VREFA to the samplers.

   uint32_t  PllTestMode;               // PLL Test Mode
   uint32_t  PllCtrl2   ;               // PLL Control Register 2
   uint32_t  PllCtrl3   ;               // PLL Control Register 3  
   uint32_t  DllLockParam[MSDG_MAX_PSTATE]; // DLL locking parameter for each power state
   uint32_t  DllGainCtl[MSDG_MAX_PSTATE];   // DLL gain control 
} train_host_vref_msdg_t;

uint32_t get_mail16(uint16_t *mail);
uint32_t get_mail32(uint32_t *mail);
uint32_t decode_major_message(uint16_t mail);
int lookup_string_index_DDR4U_1D(int string_index, char* debug_string);
int lookup_string_index_DDR4U_2D(int string_index, char* debug_string);
int lookup_string_index_DDR4R_1D(int string_index, char* debug_string);
int lookup_string_index_DDR4R_2D(int string_index, char* debug_string);
int lookup_string_index_DDR4LR_1D(int string_index, char* debug_string);
int lookup_string_index_DDR4LR_2D(int string_index, char* debug_string);
uint32_t lookup_string_index(uint32_t string_index, char* debug_string);
     
void reset_eye_data_fsm(void);
uint32_t decode_streaming_message(save_eye_data collect_eye, uint8_t * data_buffer_ptr);
uint32_t ddrphy_config_userInput (uint8_t Train2D);
uint32_t ddrphy_config_SMB (void);
uint32_t ddrphy_config_SMB_DDR4U_1D(void);
uint32_t ddrphy_config_SMB_DDR4U_2D(void);
uint32_t ddrphy_config_SMB_DDR4R_1D(void);
uint32_t ddrphy_config_SMB_DDR4R_2D(void);
uint32_t ddrphy_config_SMB_DDR4LR_1D(void);
uint32_t ddrphy_config_SMB_DDR4LR_2D(void);
uint32_t ddrTCAL (uint8_t ps, uint32_t *tCALPtr);
uint32_t ddrCAParityLatency (uint8_t ps, uint32_t *plPtr);
uint32_t ddrCALatencyAdder (uint8_t ps, uint32_t *nladdPtr);
uint32_t ddrCasLatency (uint8_t ps, uint32_t *clPtr);
uint32_t ddrCasWrLatency (uint8_t ps, uint32_t *cwlPtr);
uint32_t ddrALT_WCAS_L (uint8_t ps, uint8_t *valuePtr);
uint32_t ddrDramMr0Val (uint8_t ps, uint16_t *mr0Ptr);
uint32_t ddrDramMr1Val (uint8_t ps, uint16_t *mr1Ptr);
uint32_t ddrDramMr2Val (uint8_t ps, uint16_t *mr2Ptr);
uint32_t ddrDramMr3Val (uint8_t ps, uint16_t *mr3Ptr);
uint32_t ddrDramMr4Val (uint8_t ps, uint16_t *mr4Ptr);
uint32_t ddrDramMr5Val (uint8_t ps, uint16_t *mr5Ptr);
uint32_t ddrDramMr6Val (uint8_t ps, uint16_t *mr6Ptr);
uint32_t ddrF0RC01 (uint8_t ps, uint8_t *rcPtr);
uint32_t ddrF0RC0A(uint8_t ps, uint8_t *rcPtr);
uint32_t ddrF0RC0D(uint8_t ps, uint8_t *rcPtr);
uint32_t ddrF0RC0F(uint8_t ps, uint8_t *rcPtr);
uint32_t ddrF0RC3x(uint8_t ps, uint8_t *rcPtr);
void ddrPwrOkIn(void);
void ddrEnterPubMode(void);
void ddrExitPubMode(void);
uint32_t ddrphy_dfiInitSeq(void);
uint32_t ddrphy_MALERTPuStren(uint32_t ohm);
uint32_t ddrphy_DramRttNom(uint32_t ohm);
uint32_t ddrphy_DramRttWr(uint32_t ohm);
uint32_t ddrphy_DramRttPark(uint32_t ohm);
uint32_t ddrphy_ATxmapDrvStren(uint32_t DrvStren_ohm);
uint32_t ddrphy_initPhyConfigMsdg(void);
void ddrphy_enterMissionModeMsdg(void);
uint32_t ddrphy_wait_mailbox(save_eye_data collect_eye, uint8_t * data_buffer_ptr);
uint32_t ddrphy_init_DFIMRL_DDRCLK (uint32_t ps);
uint32_t ddrphy_mrlTraining (uint32_t ps, user_response_error_msdg_t *respErrPtr);

void ddrphy_phy_vref_write (train_host_vref_msdg_t  * host_vref_val ); 
void ddrphy_phy_vref_read (train_host_vref_msdg_t  * host_vref_val ); 
void ddrphy_set_phy_vref_mode (uint8_t  mode );

uint32_t ddrphy_dram_vref_devinit_write (uint8_t pda, train_dram_vref_msdg_t  * dramVrefPtr );  

uint32_t ddrphy_dram_vref_bist_write_pda (uint8_t  cur_pstate,  train_dram_vref_msdg_t  * dramVrefPtr ) ;
uint32_t ddrphy_dram_vref_bist_read (uint8_t  cur_pstate,    train_dram_vref_msdg_t  * dramVrefPtr );    

uint32_t ddrphy_mr6  (uint8_t pstate, uint8_t rank,  uint8_t vrefDqEnable, uint8_t vrefDqRange, uint8_t vrefDqValue ) ;

uint32_t CalculateRegOffset_RxEn(  uint32_t                   pstateNr,
                                   uint32_t                   sweepNibbleNr,
                                   uint32_t                   rankNr);
 
//  
uint32_t CalculateRegOffset_RxClk( uint32_t                     pstateNr,
                                   uint32_t                     sweepNibbleNr,
                                   uint32_t                     rankNr);
uint32_t CalculateRegOffset_RxPb(  uint32_t                     byteNr,
                                   uint32_t                     rankNr,
                                   uint32_t                     bitNr);
 
uint32_t CalculateRegOffset_AniBnPj( uint32_t                     pstateNr,
                                     uint32_t                     nibbleNr);
uint32_t CalculateRegOffset_TxDq( uint32_t                     pstateNr,
                                  uint32_t                     byteNr,
                                  uint32_t                     rankNr,
                                  uint32_t                     bitNr );
uint32_t CalculateRegOffset_TxDqs( uint32_t                     pstateNr,
                                   uint32_t                     sweepNibbleNr,
                                   uint32_t                     rankNr);
                                               
//uint32_t ddrphy_force_dl_update  (void) ;
void ddrphy_mrs_read (uint8_t pstate, uint32_t  rankIndex, uint32_t nibbleIndex, uint32_t mprPage, uint32_t mprLoc);
uint32_t ddrphy_dram_vref_get(train_dram_vref_msdg_t *dramVrefPtr);
uint32_t ddrphy_collect_train (train_delay_msdg_t *dlyPtr, train_host_vref_msdg_t *hostVrefPtr, train_dram_vref_msdg_t *dramVrefPtr);
uint32_t ddrphy_print_train (train_delay_msdg_t *dlyPtr, train_host_vref_msdg_t *hostVrefPtr, train_dram_vref_msdg_t *dramVrefPtr);
uint32_t ddrphy_collect_response (user_response_timing_msdg_t *respTmPtr, user_response_error_msdg_t *respErrPtr, user_response_mrs_msdg_t *respMrsPtr, user_response_rc_msdg_t *respRcPtr);

uint32_t ddrphy_print_response (user_response_timing_msdg_t *respTmPtr, user_response_error_msdg_t *respErrPtr, user_response_mrs_msdg_t *respMrsPtr, user_response_rc_msdg_t *respRcPtr);

user_response_msdg_t* ddrphy_training_results_get(void);
uint32_t ddrphy_userInput_check(void);


#define  DDR_PHY_INITIAL_VREF  0x60         // Byte offset 0x0c, CSR Addr 0x54006, Direction=In
                                         // Must be programmed with the Vref level to be used by the PHY during reads
                                         //   
                                         // The units of this field are a percentage of VDDQ according to the following equation:
                                         // 
                                         // Receiver Vref = VDDQ*PhyVref[6:0]/128
                                         // 
                                         // For example to set Vref at 0.75*VDDQ, set this field to 0x60. if set 66%,  this field is 0x55

/* Compile time check that MB offsets are aligned in MB definitions for supported DRAM types. */
/* This allows for code simplification for processing MB information in a common manner across MB definitions for different DRAM Types. */
/* Member mismatches will resolve to a negative arrary size if the values do not match and will generate a compile time error. */
/* As this is just typedef, this is just a compile time check. No memory is allocated. */
//<sc> TODO: Need to revisit for LRDIMM
#define DDR_PHY_COMPARE_STRUCT_OFFSET(struct_type1, struct_type2, member)   (2*(offsetof(struct_type1, member) == offsetof(struct_type2, member)) - 1)
#define DDR_PHY_COMPARE_MEMBER_SIZE(struct_type1, struct_type2, member)     (2*(sizeof(((struct_type1 *)(0))->member) == sizeof(((struct_type2 *)(0))->member))- 1)
#define DDR_PHY_COMPARE_STRUCT_SIZE(struct_type1, struct_type2)             (2*(sizeof(struct_type1) == sizeof(struct_type2)) - 1) 



#endif /*_DDR_PHY_TOOLBOX_H_*/




