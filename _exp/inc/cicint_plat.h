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
*     Platform specific defines
*******************************************************************************/


#ifndef _CICINT_PLAT_H
#define _CICINT_PLAT_H
#include "pmc_profile.h"
/*
** Constants
*/

/* 
** Based on "Wildfire PCS Interrupt Map" spreadsheet
** http://bby1dms01/DocMgmt/getfile.cfm?file_id=426174
** 
** Note: PCS and SPCS interrupt maps are currently identical therefore the same
**       mapping will be used for both processors.
**       This may change in the future in which case some or all of the following
**       definitions may need to become processor specific.
**       SPCS interrupt map spreadsheet is here:
**       http://bby1dms01.pmc-sierra.internal/DocMgmt/getfile.cfm?file_id=427759
** 
** GIC[127:0]   ->  DQ0_EMPTYB[127:0] or DQ1_EMPTYB[127:0]
** GIC[63:0]    ->  DC_FIFO_EMPTYB[63:0]
** GIC[127:64]  ->  DC_FIFO_EMPTYB[63:0]
** GIC[203:80]  ->  FASTPATH[203:80]
** GIC[235:204] ->  BLOCK_FP_INT[255:0] (8 block_fp_int per GIC)
** GIC[243:236] ->  PCS
** GIC[251:244] ->  WSM[511:0] (64 wsm_int per GIC)
** GIC[252]     ->  PWR_MGMT_EVENT[7:0]
** GIC[255:253] ->  BLOCK INTS (32 block_ints per GIC)
*/ 

/* GIC[79:0] are reserved for eGSM/DC DQ EmptyB use ONLY */

#define CICINT_DEDICATED_EMPTYB_INT_NUM             (80)

/* GIC[127:80] may be used for eGSM/DC DQ EmptyB or by FASTPATH interrupts */

/* FASTPATH GPBC GSX interrupts */
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_7_0           (80)
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_15_8          (81)
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_23_16         (82)
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_31_24         (83)
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_39_32         (84)
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_47_40         (85)
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_55_48         (86)
#define CICINT_FP_INT_GPBC_GSX_0_GPIO_63_56         (87)
/* GIC[95:88] are configured by rocmain basecode for eGSM/DC DQ EmptyB use */
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_7_0           (88)
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_15_8          (89)
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_23_16         (90)
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_31_24         (91)
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_39_32         (92)
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_47_40         (93)
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_55_48         (94)
#define CICINT_FP_INT_GPBC_GSX_1_GPIO_63_56         (95)

/* FASTPATH GPBC TWI Master interrupts */
#define CICINT_FP_INT_GPBC_TWI_0_M                  (96)
#define CICINT_FP_INT_GPBC_TWI_1_M                  (97)
#define CICINT_FP_INT_GPBC_TWI_2_M                  (98)
#define CICINT_FP_INT_GPBC_TWI_3_M                  (99)
#define CICINT_FP_INT_GPBC_TWI_4_M                  (100)
#define CICINT_FP_INT_GPBC_TWI_5_M                  (101)
#define CICINT_FP_INT_GPBC_TWI_6_M                  (102)
#define CICINT_FP_INT_GPBC_TWI_7_M                  (103)
#define CICINT_FP_INT_GPBC_TWI_8_M                  (104)
#define CICINT_FP_INT_GPBC_TWI_9_M                  (105)
#define CICINT_FP_INT_GPBC_TWI_10_M                 (106)
#define CICINT_FP_INT_GPBC_TWI_11_M                 (107)
#define CICINT_FP_INT_GPBC_TWI_12_M                 (108)
#define CICINT_FP_INT_GPBC_TWI_13_M                 (109)
#define CICINT_FP_INT_GPBC_TWI_14_M                 (110)
#define CICINT_FP_INT_GPBC_TWI_15_M                 (111)

/* FASTPATH GPBC TWI Slave interrupts */
#define CICINT_FP_INT_GPBC_TWI_0_S                  (112)
#define CICINT_FP_INT_GPBC_TWI_1_S                  (113)
#define CICINT_FP_INT_GPBC_TWI_2_S                  (114)
#define CICINT_FP_INT_GPBC_TWI_3_S                  (115)
#define CICINT_FP_INT_GPBC_TWI_4_S                  (116)
#define CICINT_FP_INT_GPBC_TWI_5_S                  (117)
#define CICINT_FP_INT_GPBC_TWI_6_S                  (118)
#define CICINT_FP_INT_GPBC_TWI_7_S                  (119)
#define CICINT_FP_INT_GPBC_TWI_8_S                  (120)
#define CICINT_FP_INT_GPBC_TWI_9_S                  (121)
#define CICINT_FP_INT_GPBC_TWI_10_S                 (122)
#define CICINT_FP_INT_GPBC_TWI_11_S                 (123)
#define CICINT_FP_INT_GPBC_TWI_12_S                 (124)
#define CICINT_FP_INT_GPBC_TWI_13_S                 (125)
#define CICINT_FP_INT_GPBC_TWI_14_S                 (126)
#define CICINT_FP_INT_GPBC_TWI_15_S                 (127)

/* FASTPATH GPBC GPIO interrupts */
#define CICINT_FP_INT_GPBC_GPIO_0                   (128)
#define CICINT_FP_INT_GPBC_GPIO_1                   (129)
#define CICINT_FP_INT_GPBC_GPIO_2                   (130)
#define CICINT_FP_INT_GPBC_GPIO_3                   (131)
#define CICINT_FP_INT_GPBC_GPIO_4                   (132)
#define CICINT_FP_INT_GPBC_GPIO_5                   (133)
#define CICINT_FP_INT_GPBC_GPIO_6                   (134)
#define CICINT_FP_INT_GPBC_GPIO_7                   (135)
#define CICINT_FP_INT_GPBC_GPIO_8                   (136)
#define CICINT_FP_INT_GPBC_GPIO_9                   (137)
#define CICINT_FP_INT_GPBC_GPIO_10                  (138)
#define CICINT_FP_INT_GPBC_GPIO_11                  (139)
#define CICINT_FP_INT_GPBC_GPIO_12                  (140)
#define CICINT_FP_INT_GPBC_GPIO_13                  (141)
#define CICINT_FP_INT_GPBC_GPIO_14                  (142)
#define CICINT_FP_INT_GPBC_GPIO_15                  (143)
#define CICINT_FP_INT_GPBC_GPIO_16                  (144)
#define CICINT_FP_INT_GPBC_GPIO_17                  (145)
#define CICINT_FP_INT_GPBC_GPIO_18                  (146)
#define CICINT_FP_INT_GPBC_GPIO_19                  (147)
#define CICINT_FP_INT_GPBC_GPIO_20                  (148)
#define CICINT_FP_INT_GPBC_GPIO_21                  (149)
#define CICINT_FP_INT_RESERVED_150                  (150)
#define CICINT_FP_INT_RESERVED_151                  (151)
#define CICINT_FP_INT_RESERVED_152                  (152)
#define CICINT_FP_INT_RESERVED_153                  (153)
#define CICINT_FP_INT_RESERVED_154                  (154)
#define CICINT_FP_INT_RESERVED_155                  (155)
#define CICINT_FP_INT_RESERVED_156                  (156)
#define CICINT_FP_INT_RESERVED_157                  (157)
#define CICINT_FP_INT_RESERVED_158                  (158)
#define CICINT_FP_INT_RESERVED_159                  (159)

/* TODO-ROCw: find an appropriate set of interrupt numbers for YQ on Wildfire */
/* Ints 144-159 are per-TC yield wakeup pins (1 per TC) */
#define CICINT_INT_YQ_BASE                          (144)
#define CICINT_INT_YQ(tc)                           (CICINT_INT_YQ_BASE + tc)

/* FASTPATH GPBC SGPIO interrupts */
#define CICINT_FP_INT_GPBC_SGPIO_0                  (160)
#define CICINT_FP_INT_GPBC_SGPIO_1                  (161)
#define CICINT_FP_INT_GPBC_SGPIO_2                  (162)
#define CICINT_FP_INT_GPBC_SGPIO_3                  (163)
#define CICINT_FP_INT_GPBC_SGPIO_4                  (164)
#define CICINT_FP_INT_GPBC_SGPIO_5                  (165)
#define CICINT_FP_INT_GPBC_SGPIO_6                  (166)
#define CICINT_FP_INT_GPBC_SGPIO_7                  (167)

/* FASTPATH GPBC UART interrupts */
#define CICINT_FP_INT_GPBC_UART_0                   (168)
#define CICINT_FP_INT_GPBC_UART_1                   (169)
#define CICINT_FP_INT_GPBC_UART_2                   (170)
#define CICINT_FP_INT_GPBC_UART_3                   (171)

/* FASTPATH GPBC SPI interrupts */
#define CICINT_FP_INT_GPBC_SPI_0                    (172)
#define CICINT_FP_INT_GPBC_SPI_1                    (173)

/* FASTPATH GPBC Fan Tacho interrupts */
#define CICINT_FP_INT_GPBC_FAN_TACHO_0              (174)
#define CICINT_FP_INT_GPBC_FAN_TACHO_1              (175)
#define CICINT_FP_INT_RESERVED_176                  (176)
#define CICINT_FP_INT_RESERVED_177                  (177)
#define CICINT_FP_INT_RESERVED_178                  (178)
#define CICINT_FP_INT_RESERVED_179                  (179)
#define CICINT_FP_INT_RESERVED_180                  (180)
#define CICINT_FP_INT_RESERVED_181                  (181)

/* TODO-ROCw: find an appropriate set of interrupt numbers for IPI on Wildfire */
/* Ints 174-181 are for OS preemption use (one per VPE) */
#define CICINT_INT_IPI_BASE                         (174)
#define CICINT_INT_IPI(vpe)                         (CICINT_INT_IPI_BASE + vpe)

/* FASTPATH GPBC miscellaneous interrupts */
#define CICINT_FP_INT_GPBC_ETH_0                    (182)
#define CICINT_FP_INT_GPBC_WOL_0                    (183)
#define CICINT_FP_INT_GPBC_ETH_1                    (184)
#define CICINT_FP_INT_GPBC_WOL_1                    (185)
#define CICINT_FP_INT_GPBC_WDT                      (186)
#define CICINT_FP_INT_GPBC_NFC                      (187)
#define CICINT_FP_INT_RESERVED_188                  (188)
#define CICINT_FP_INT_RESERVED_189                  (189)
#define CICINT_INT_RAIDGEN                          (189)
#define CICINT_FP_INT_RESERVED_190                  (190)
#define CICINT_FP_INT_RESERVED_191                  (191)
#define CICINT_INT_SAS_ASYNC_REQ                    (191)

/* FASTPATH MSGU interrupts */
#define CICINT_FP_INT_MSGU_LEGACY                   (192)
#define CICINT_FP_INT_MSGU_FLR                      (193)
#define CICINT_FP_INT_MSGU_WDG                      (194)
#define CICINT_FP_INT_MSGU_RESERVED_195             (195)

/* FASTPATH Power and Reset interrupts */
#define CICINT_FP_INT_PERST                         (196)
#define CICINT_FP_INT_POWER_LOSS                    (197)
#define CICINT_FP_INT_POWER_MISS                    (198)
#define CICINT_FP_INT_RESERVED_199                  (199)
#define CICINT_FP_INT_RESERVED_200                  (200)
#define CICINT_FP_INT_RESERVED_201                  (201)
#define CICINT_FP_INT_RESERVED_202                  (202)
#define CICINT_FP_INT_RESERVED_203                  (203)

/* TODO-ROCw: find an appropriate set of interrupt numbers for VPE on Wildfire */
/* Ints 195-202 are for software-initiated NMI (one per VPE) */
#define CICINT_INT_RESET_VPE0                       (195)
#define CICINT_INT_RESET_VPE1                       (196)
#define CICINT_INT_RESET_VPE2                       (197)
#define CICINT_INT_RESET_VPE3                       (198)
#define CICINT_INT_RESET_VPE4                       (199)
#define CICINT_INT_RESET_VPE5                       (200)
#define CICINT_INT_RESET_VPE6                       (201)
#define CICINT_INT_RESET_VPE7                       (202)

/* GPBC block interrupts */
#define CICINT_BLK_INT_GPBC_TWI_M_7_0               (204)   /* BLOCK_FP_INT[7:0]    */
#define CICINT_BLK_INT_GPBC_TWI_M_15_8              (205)   /* BLOCK_FP_INT[15:8]   */ 
#define CICINT_BLK_INT_GPBC_TWI_S_7_0               (206)   /* BLOCK_FP_INT[23:16]  */
#define CICINT_BLK_INT_GPBC_TWI_S_15_8              (207)   /* BLOCK_FP_INT[31:24]  */
#define CICINT_BLK_INT_GPBC_GSX_0_7_0               (208)   /* BLOCK_FP_INT[39:32]  */
#define CICINT_BLK_INT_GPBC_GSX_1_7_0               (209)   /* BLOCK_FP_INT[47:40]  */

/* DCSU block interrupts */
#define CICINT_BLK_INT_DCSU                         (210)   /* BLOCK_FP_INT[55:48]  */
#define CICINT_BLK_INT_TRIMODE_CSU                  (211)   /* BLOCK_FP_INT[63:56]  */

/* MSGU block interrupts */
#define CICINT_BLK_INT_MSGU_TIMERS_Q_EMPTY          (212)   /* BLOCK_FP_INT[71:64]  */
#define CICINT_BLK_INT_MSGU_EMPTYB                  (212)   /* BLOCK_FP_INT[71:64]  */
#define CICINT_BLK_INT_MSGU_DMA_ERRORS_STATUS       (213)   /* BLOCK_FP_INT[79:72]  */
#define CICINT_BLK_INT_MSGU_WSM                     (214)   /* BLOCK_FP_INT[87:80]  */
#define CICINT_BLK_INT_MSGU_HQA_HWA_ERRORS          (215)   /* BLOCK_FP_INT[95:88]  */

/* OSSP Block interrupts */
#define CICINT_BLK_INT_OSSP0_SSTL                   (216)   /* BLOCK_FP_INT[103:96]  */
#define CICINT_BLK_INT_OSSP1_SSTL                   (217)   /* BLOCK_FP_INT[111:104] */
#define CICINT_BLK_INT_OSSP2_SSTL                   (218)   /* BLOCK_FP_INT[119:112] */
#define CICINT_BLK_INT_OSSP3_SSTL                   (219)   /* BLOCK_FP_INT[127:120] */
#define CICINT_BLK_INT_OSSP0_SSPL                   (220)   /* BLOCK_FP_INT[135:128] */
#define CICINT_BLK_INT_OSSP1_SSPL                   (221)   /* BLOCK_FP_INT[143:136] */
#define CICINT_BLK_INT_OSSP2_SSPL                   (222)   /* BLOCK_FP_INT[151:144] */
#define CICINT_BLK_INT_OSSP3_SSPL                   (223)   /* BLOCK_FP_INT[159:152] */

/* PCIe block interrupts */
#define CICINT_INT_BLOCK_PCIE_IEP                   (224)   /* BLOCK_FP_INT[167:160] */
#define CICINT_INT_BLOCK_PCIE_SP0                   (225)   /* BLOCK_FP_INT[175:168] */
#define CICINT_INT_BLOCK_PCIE_SP1                   (226)   /* BLOCK_FP_INT[183:176] */
#define CICINT_INT_BLOCK_PCIE_SP2                   (227)   /* BLOCK_FP_INT[191:184] */
#define CICINT_INT_BLOCK_PCIE_SP3                   (228)   /* BLOCK_FP_INT[199:192] */
#define CICINT_INT_BLOCK_PCIE_SP4                   (229)   /* BLOCK_FP_INT[207:200] */
#define CICINT_INT_BLOCK_PCIE_SP5                   (230)   /* BLOCK_FP_INT[215:208] */
#define CICINT_INT_BLOCK_PCIE_SC                    (231)   /* BLOCK_FP_INT[223:216] */
#define CICINT_INT_BLOCK_PPU_OB                     (232)   /* BLOCK_FP_INT[231:224] */
#define CICINT_INT_BLOCK_PPU_IB                     (233)   /* BLOCK_FP_INT[239:232] */
#define CICINT_BLK_INT_PCIE_PPU_OB_DMA              (234)   /* BLOCK_FP_INT[247:240] */
#define CICINT_BLK_INT_PCIE_PPCU_IB_DMA             (235)   /* BLOCK_FP_INT[255:248] */

/* PCS interrupts */
#define CICINT_INT_PCS_EVENT_INT                    (236)
#define CICINT_INT_PCS_FATAL_ERROR_INT              (237)
#define CICINT_INT_PCS_NON_FATAL_ERROR_INT          (238)
#define CICINT_INT_COP2_3_0                         (239)
#define CICINT_INT_PCS_TIMER0                       (240)
#define CICINT_INT_TIMER0                           (240)
#define CICINT_INT_PCS_TIMER1                       (241)
#define CICINT_INT_PCS_TIMER2                       (242)
#define CICINT_INT_PCS_TIMER3                       (243)

/* Write Sensitive Memory interrupts */
#define CICINT_INT_WSM_63_0                         (244)
#define CICINT_INT_WSM_127_64                       (245)
#define CICINT_INT_WSM_191_128                      (246)
#define CICINT_INT_WSM_255_192                      (247)
#define CICINT_INT_WSM_319_256                      (248)
#define CICINT_INT_WSM_383_320                      (249)
#define CICINT_INT_WSM_447_384                      (250)
#define CICINT_INT_WSM_511_448                      (251)

/* Power Management Events (7:0) */
#define CICINT_INT_PWR_MGMT_EVENTS                  (252)

/* Block interrupts (31:0) each */
#define CICINT_INT_BLOCK_EVENT_INT                  (253)
#define CICINT_INT_FATAL_ERROR_INT                  (254)
#define CICINT_INT_NON_FATAL_ERROR_INT              (255)

#define CICINT_NUM_INT                              (256)

/* L2B timer interrupt - set to 0 for no dedicated timer interrupt pin */
#define CICINT_INT_L2B_PERFCNT_TIMER                (0)

/* 
** Yield pin definitions for fast_io queues  
** NOTE: YQ Pins 12 is currently not used
*/ 
#define CICINT_YQ_EGSM_OSSP0_IMQ_0_POSTQ            (4)
#define CICINT_YQ_EGSM_HBA_REQ_POSTQ                (5)
#define CICINT_YQ_EGSM_RAID_IO_REQ_POSTQ            (6)
#define CICINT_YQ_EGSM_SAT_HBA_REQUEUE_REQ_POSTQ    (7)
#define CICINT_YQ_EGSM_ALL_RMWRA_EGSM_QUEUES        (8) /* EGSM Queues handled by rmwra_event_process() */
/* Pin 9 unused */
#define CICINT_YQ_EGSM_NONSEQ_RAID_IO_REQ_POSTQ     (10)
#define CICINT_YQ_NVME_CQ_PEND                      (11)
/* Pin 12 Unused */
#define CICINT_YQ_DATA_THROTTLE                     (13)
#define CICINT_YQ_EGSM_SAT_RAID_REQUEUE_REQ_POSTQ   (14)
#define CICINT_YQ_EGSM_FPF_ADMIN_POSTQ              (15)

/* 
*******************************************************************************
** Everything beyond this point is for LUXOR
** Keeping for now until all references are removed
*******************************************************************************
*/
#define CICINT_INT_MSGU_EMPTYB_36                (232)
#define CICINT_INT_MSGU_EMPTYB_37                (233)

#endif /* _CICINT_PLAT_H */



