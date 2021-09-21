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
* THE SYNOPSYS FIRMWARE IS PROVIDED “AS IS” AND WITHOUT ANY WARRANTY.  SYNOPSYS 
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

/********************************************************************************
* Description:
*  Defines used for ddrBist.c
*
* $Revision: 1607 $
* $Date: 2019-08-01 10:45:46 -0700 (Thu, 01 Aug 2019) $
* $Author: xiealex1 $
* Release $Name$
*
******************************************************************************/



/**
 * @file ddrBist.h
 * @brief Contains the defines needed by the ddrBist.c
 */
#ifndef DDR_BIST_H
#define DDR_BIST_H

#include <ddrphy_toolbox.h>
#ifdef PMC_DDR_SIM_DPI
#include <stdio.h>
#endif

/**
 * @brief Define for the DFI BIST read mask type
 */
#define DDR_DFI_READ_MASK            0

/**
 * @brief Define for the DFI BIST write mask type
 */
#define DDR_DFI_WRITE_MASK           1

/**
 * @brief - DDR DFI BIST defines
 *
 * @defgroup DDR DFI BIST bit fields and masks
 */
/** @{ */
#define DDR_DFI_FAIL_RANK_MASK               0x7800000
#define DDR_DFI_FAIL_RANK_OFFSET                    23
#define DDR_DFI_FAIL_CID_MASK                 0x700000
#define DDR_DFI_FAIL_CID_OFFSET                     20
#define DDR_DFI_FAIL_BG_MASK                  0x0c0000
#define DDR_DFI_FAIL_BG_OFFSET                      18
#define DDR_DFI_FAIL_BA_MASK                   0x70000
#define DDR_DFI_FAIL_BA_OFFSET                      16
#define DDR_DFI_FAIL_COL_ADDRESS_MASK           0xFFFF
#define DDR_DFI_FAIL_COL_ADDRESS_OFFSET              0
#define DDR_DFI_FAIL_ROW_ADDRESS_MASK          0x3FFFF
#define DDR_DFI_FAIL_ROW_ADDRESS_OFFSET              0
#define DDR_DFI_FAIL_ROW_ADDRESS_MASK          0x3FFFF
#define DDR_PHY_WRAPPER_DFI_BIST_CONF23_BITOFF_DFI_BIST_DM_TEST              4
#define DDR_PHY_WRAPPER_DFI_BIST_CONF23_BITMSK_DFI_BIST_DM_TEST           0x10
/** @} */


/**
 * @brief Enumerated type for the different failure data points that can be
 * extracted after a DFI BIST.
 */
typedef enum
{
    DDR_BIST_PER_BIT_ERROR_31_0   = 0,
    DDR_BIST_PER_BIT_ERROR_63_32  = 1,
    DDR_BIST_PER_BIT_ERROR_79_64  = 2,
    DDR_BIST_FAIL_EXP_DATA_31_0   = 3,
    DDR_BIST_FAIL_EXP_DATA_63_32  = 4,
    DDR_BIST_FAIL_EXP_DATA_79_64  = 5,
    DDR_BIST_FAIL_ACT_DATA_31_0   = 6,
    DDR_BIST_FAIL_ACT_DATA_63_32  = 7,
    DDR_BIST_FAIL_ACT_DATA_79_64  = 8,
    DDR_BIST_FAIL_HW              = 9,
    DDR_BIST_FAIL_ROW_ADDRESS     = 10,
    DDR_BIST_FAIL_NUM             = 11
} ddr_bist_fail_info_ee;

/**
 * @brief A structure for bist error status
 */
typedef struct
{
    uint32_t                 numWordErrors;
    uint32_t                 parityError;
    uint32_t                 alertN;
    uint32_t                 bitErr_31_0;
    uint32_t                 bitErr_63_32;
    uint32_t                 bitErr_79_64;
    uint32_t                 failExpData_31_0;
    uint32_t                 failExpData_63_32;
    uint32_t                 failExpData_79_64;
    uint32_t                 failActData_31_0;
    uint32_t                 failActData_63_32;
    uint32_t                 failActData_79_64;
    uint32_t                 failRank;
    uint32_t                 failCid;
    uint32_t                 failBg;
    uint32_t                 failBa;
    uint32_t                 failRowAddr;
    uint32_t                 failColAddr;
} ddr_bist_result_t;

/**
 * @brief Enumerated type for the different dfi BIST patterns.
 */
typedef enum
{
    DDR_BIST_2D_LFSR            = 0,
    DDR_BIST_WALKING_ONE        = 1,
    DDR_BIST_WALKING_ZERO       = 2,
    DDR_BIST_1D_LFSR            = 3,
    DDR_BIST_WORST_CASE         = 4,
    DDR_BIST_USER_DEFINED_1     = 5,
    DDR_BIST_USER_DEFINED_2     = 6,
    DDR_BIST_LFSR_SHIFTING      = 7,
    DDR_BIST_NUM_PATTERN        = 8
} ddr_bist_pattern_e;

/**
 * @brief Enumerated type for the different ways to apply the mask before a BIST
 * test is run.
 *
 * DDR_MASK_APPLY_TO_READ       - This applies the mask to the read only and
 *                                enables all the write bits.  This is used when
 *                                wanting to write every lane to create cross talk
 *                                but only see the results of a subset of the
 *                                byte lanes.
 * DDR_MASK_APPLY_TO_WRITE      - This applies the mask to the write only and
 *                                enables all the read bits.
 * DDR_MASK_APPLY_TO_READ_WRITE - This applies the mask to the write and
 *                                read.  The is useful when you want to isolate
 *                                all the traffic to a specific set of byte
 *                                lanes.
 */
typedef enum
{
    DDR_MASK_APPLY_TO_READ          = 0,
    DDR_MASK_APPLY_TO_WRITE         = 1,
    DDR_MASK_APPLY_TO_READ_WRITE    = 2
} ddr_bist_mask_usage_e;


/**
 * @brief A structure that holds all the setup parameters for a single DFI BIST.
 */
typedef struct
{
    uint32_t                addressTest;     // Enable address test. 0:Disabled, 1:Enabled
    uint32_t                addrIncr;        // Address Increment steps. 
    uint32_t                rankIndex;       // rank index (3~0) 
    uint32_t                cid;             // logic rank address
    uint32_t                bank;            // bank address
    uint32_t                bg;              // bank group address
    uint32_t                rowAddr;         // row address
    uint32_t                startColAddr;    // start column address
    uint32_t                length;          // SEQ_LEN
    uint32_t                numReadWrite;    // Transaction per round
    uint32_t                patternStyle;    // 3 traffic patterns
    uint32_t                addressMode;     // 0: randomized address during DES. 1: keeps 1
    uint32_t                dqsJitter;       // DQS jitter measure mode
    uint32_t                tPRE;            // numer of cycles waiting after PRE command
    uint32_t                tWrRd;           // numer of cycles waiting from WR to RD
    uint32_t                tWrWr;           // numer of cycles waiting from WR to WR
    uint32_t                tACT;            // numer of cycles waiting after ACT command
    uint32_t                tToPRE;          // numer of cycles waiting from WR/RD to PRE
    uint32_t                tRdWr;           // numer of cycles waiting from RD to WR
    uint32_t                tRdRd;           // numer of cycles waiting from RD to RD
    uint32_t                userData0;       // user defined data pattern [31:0]
    uint32_t                userData1;       // user defined data pattern [63:32]
    uint32_t                datamaskTest;    // Enable DM test. 0: Disabled, 1: Enabled
    uint32_t                crossRkTest;     // Enable Cross rank test. 0: Disabled, 1: Enabled
    uint32_t                crossRkMax;      // Number of Cross rank test commands. (0~7)
    uint32_t                crossRkCmd;      // Hold 8 commands {command7, ?, command0}, each command has 4 bits. Command 0 is sent out first, followed by command 1, as so on. For command[3], 0 means the command is for rank (CID) defined by register  bist_cid[2:0]  and start_rank[3:0] , 1 means the command is using CID = cid_mask[2:0] and max_rank[3:0].  command bit [2:0] decoding:  0b000: ACT command, 0b001: Write command, 0b010: Read command, 0b011: precharge command. Other values are reserved.Enable Cross rank test. 0: Disabled, 1: Enabled
    uint32_t                crossRkRk;      // 2nd Rank setting of Cross rank test (1st Rank setting is rankIndex);
    uint32_t                crossRkCid;     // 2nd CID setting of Cross rank test (1st CID setting is cid);
    uint32_t                crossRkLength;  // iterration of Cross rank test
    uint32_t                cabusMode;     // 0: normal mode, 1: 4 rank mode, 2: RAIM-A mode, 3: RAIM-B mode
    uint32_t                writeMask_31_00; // bist write mask [31:0] when set to 1, mask WR DQ bit to 0
    uint32_t                writeMask_63_32; // bist write mask [63:32] when set to 1, mask WR DQ bit to 0
    uint32_t                writeMask_79_64; // bist write mask [79:64] when set to 1, mask WR DQ bit to 0
    uint32_t                readMask_31_00; // bist read mask [31:0] when set to 1, ignore read comparison on DQ bit
    uint32_t                readMask_63_32; // bist read mask [63:32] when set to 1, ignore read comparison on DQ bit
    uint32_t                readMask_79_64; // bist read mask [79:64] when set to 1, ignore read comparison on DQ bit
} ddr_bist_setup_t;


/**
 * @brief Enumerated type for the rank selection to use when executing the margin test.
 */
typedef enum
{
    DDR_RANK_0         = 0,
    DDR_RANK_1         = 1,
    DDR_RANK_2         = 2,
    DDR_RANK_3         = 3,
    DDR_RANK_ALL       = 4  /* This is also the maximum number of ranks supported */
} ddr_rank_select_e;

/**
 * @brief A structure for MRLTraining status
 */
typedef struct
{
    uint32_t                fail;         // Pass:0; Fail:1
    uint32_t                mrl;            // current mrl value
    uint32_t                status[DDR_RANK_ALL];        // fail status for each rank
 //     ddr_bist_pattern_e naming   : binary rep :   explorer_phy_dfi_bist doc naming
 //     ----------------------------:------------:------------------------------------
 // DDR_BIST_2D_LFSR            : b000000001 :   2D PRBS data pattern
 //     DDR_BIST_WALKING_ONE        : b000000010 :   walking 1 data pattern
 //     DDR_BIST_WALKING_ZERO       : b000000100 :   walking 1 data pattern
 //     DDR_BIST_1D_LFSR            : b000001000 :   1D PRBS pattern
 //     DDR_BIST_WORST_CASE         : b000010000 :   worse case data pattern
 //     DDR_BIST_USER_DEFINED_1     : b000100000 :   user defined data pattern mode 1
 //     DDR_BIST_USER_DEFINED_2     : b001000000 :   user defined data pattern mode 2
 //     DDR_BIST_LFSR_SHIFTING      : b010000000 :   PRBS11 shift pattern
 //     DDR_BIST_NUM_PATTERN        : b100000000 :   User defined data pattern mode 1 with dq_flip_bits enabled
} ddr_bist_mrl_train_t;

void ddrCleanupBist(void);
void ddrGetBistResults(ddr_bist_result_t *singleBistResultPtr);
void ddrSetBistMask(uint32_t         nibLaneMask,
                    uint32_t         bitMask,
                    uint32_t         maskType);


uint32_t ddrRunBist(ddr_bist_pattern_e       pattern,
               uint32_t                      bistTimeout);
uint32_t ddrRunBistInfinite(ddr_bist_pattern_e       pattern);
void ddrBistSetup( ddr_bist_setup_t  *bistSetupPtr);
void ddrPrintBistResult(ddr_bist_result_t *singleBistResultPtr);
uint32_t ddrBistInit(uint8_t ps, ddr_bist_setup_t *bistSetupPtr);
uint32_t ddrRunBistSequence(ddr_bist_pattern_e   bistPattern,     
                       uint32_t                  bistTimeout,     
                       ddr_bist_result_t    *singleBistResultPtr); 


#ifdef PMC_DDR_SIM_DPI  
int ddrBistSetSetup (ddr_bist_setup_t *bistSetupPtr, char *field, int value); 
#endif
void ddrPrintBistSetup(ddr_bist_setup_t *bistSetupPtr) ;  
void ddrEnterBistCmd(void);
void ddrExitBistCmd(void) ; 
uint32_t ddrRunBistCmd(int cke, int csn, int cid, int ba, int bg, int act_n, int rasn, int casn, int wen, int addr13_0, int addr17, int pda);
uint32_t ddrBistREF(int csn, int cid) ;
uint32_t ddrBistCtrlUpd(void);
uint32_t ddrBistSRE(int csn, int prv_cke);
uint32_t ddrBistSRX0(int csn, int prv_cke);
uint32_t ddrBistSRX1(int csn, int prv_cke);
uint32_t ddrBistPDE(int csn, int prv_cke);  
uint32_t ddrBistPDX(int csn, int prv_cke);
uint32_t ddrBistZQCL(int csn);
uint32_t ddrBistZQCS(int csn); 
uint32_t ddrBistACT(int csn, int cid, int bg, int ba, int row);
uint32_t ddrBistWRS8(int csn, int cid, int bg, int ba, int col, int pattern) ;
uint32_t ddrBistRDS8(int csn, int cid, int bg, int ba, int col);    
uint32_t ddrBistPRE(int csn, int cid, int bg, int ba) ;
uint32_t ddrBistPREA(int csn, int cid) ; 
uint32_t ddrBistWRAS8(int csn, int cid, int bg, int ba, int col, int pattern);
uint32_t ddrBistRDAS8(int csn, int cid, int bg, int ba, int col); 
uint32_t ddrBistMRS(int csn, int idx, int value, int invert);
uint32_t ddrBistMPRWr(int csn, int idx, int value, int invert) ; 
uint32_t ddrBistMPRRd(int csn, int idx, int invert) ;
uint32_t ddrBistMRLTrainingEntry(void);
uint32_t ddrBistMRLTrainingExit(void);
void ddrBistMrlPrintFailRuns(ddr_bist_mrl_train_t *mrlStatusPtr, uint32_t patternSel);
uint32_t ddrBistMRLTrainingRun(user_response_error_msdg_t *respErrPtr);
uint32_t ddrBistRunningMRLTraining(uint32_t mrl, uint32_t patternSel, ddr_bist_mrl_train_t *mrlStatusPtr);
uint32_t ddrBistMRSPDA(int csn, int idx, int value, int pdanibs, int invert);

                                                     
#endif /* DDR_BIST_H */


