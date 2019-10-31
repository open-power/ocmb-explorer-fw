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
* @addtogroup CPUHAL
* @{ 
* @file 
* @brief
*     This file implements the CPUHAL register read and write macros by
*     simply remapping the macros to their BUSIO equivalent.
*
*     The register address and data alignment in the M1004KHAL are predefined.
*     They are as follows:
*           register width              - 32 bits wide
*           register read/write size    - 32 bits at a time
*           register address alignment  - 32-bit (4-byte) boundary
* @note 
*/ 

#ifndef _CPUHAL_HW_H
#define _CPUHAL_HW_H

/*
* Include Files
*/
#if defined(__ASSEMBLER__) || defined(__LANGUAGE_ASM__)

#else
#include "busio.h"
#endif

#include "pmc_hw.h"

/* Co processor defines for the MIP 1004K */
//#define C0_MVPControl $0,1
//#define C0_VPEControl $1,1
//#define C0_YQMask     $1,4
//#define C0_TCStatus   $2,1
//#define C0_TCBind     $2,2
//#define C0_TCHalt     $2,4
//#define C0_UserLocal  $4,2
//#define C0_SRSConf0   $6,1
//#define C0_IntCtl     $12,1
//#define C0_SRSCtl     $12,2
//#define C0_SRSMap     $12,3
//#define C0_EBase      $15,1



#define CPUHAL_GIC_REG_BASE_ADDR_UPPER_BITS (CPUHAL_GIC_REG_BASE_ADDR >> 16)
#define CPUHAL_GIC_SRCS_PER_INT_SUM_REG     32


/* GIC shared section offset */
#define CPUHAL_GIC_SHARED_REG_BASE_ADDR     (CPUHAL_GIC_REG_BASE_ADDR | 0x00000)
#define CPUHAL_GIC_SH_CONFIG                 0x0000
#define CPUHAL_GIC_SH_CounterLo              0x0010
#define CPUHAL_GIC_SH_CounterHi              0x0014
#define CPUHAL_GIC_SH_POL31_0                0x0100
#define CPUHAL_GIC_SH_TRIG31_0               0x0180
#define CPUHAL_GIC_SH_DUAL31_0               0x0200
#define CPUHAL_GIC_SHARED_WEDGE              0x0280
#define CPUHAL_GIC_SH_RMASK31_0              0x0300
#define CPUHAL_GIC_SH_RMASK223_192           0x0318
#define CPUHAL_GIC_SH_SMASK31_0              0x0380
#define CPUHAL_GIC_SH_SMASK223_192           0x0398
#define CPUHAL_GIC_SH_RMASK255_224           0x031C
#define CPUHAL_GIC_SH_MASK31_00_REG          0x0400
#define CPUHAL_GIC_SH_MASK63_32_REG          0x0404
#define CPUHAL_GIC_SH_MASK95_64_REG          0x0408
#define CPUHAL_GIC_SH_MASK127_96_REG         0x040C
#define CPUHAL_GIC_SH_MASK159_128_REG        0x0410
#define CPUHAL_GIC_SH_MASK191_160_REG        0x0414
#define CPUHAL_GIC_SH_MASK223_192_REG        0x0418
#define CPUHAL_GIC_SH_MASK255_224_REG        0x041C
#define CPUHAL_GIC_SH_PEND31_00_REG          0x0480
#define CPUHAL_GIC_SH_PEND63_32_REG          0x0484
#define CPUHAL_GIC_SH_PEND95_64_REG          0x0488
#define CPUHAL_GIC_SH_PEND127_96_REG         0x048C
#define CPUHAL_GIC_SH_PEND159_128_REG        0x0490
#define CPUHAL_GIC_SH_PEND191_160_REG        0x0494
#define CPUHAL_GIC_SH_PEND223_192_REG        0x0498
#define CPUHAL_GIC_SH_PEND255_224_REG        0x049C
#define CPUHAL_GIC_SH_MAP0_PIN               0x0500
#define CPUHAL_GIC_SH_MAP0_VPE31_0           0x2000


/* GIC shared section constants */
#define CPUHAL_GIC_SH_CONFIG_BITMSK_NUMINTS      0x00ff0000
#define CPUHAL_GIC_SH_CONFIG_BITOFF_NUMINTS      16
#define CPUHAL_GIC_SH_CONFIG_ARRAYOFF_NUMINTS    8
#define CPUHAL_GIC_SH_CONFIG_BITMSK_PVPES        0x000001ff
#define CPUHAL_GIC_SH_CONFIG_BITOFF_PVPES        0
#define CPUHAL_GIC_ROUTE_LOCAL_INT_MAP_TO_PIN    0x80000000
#define CPUHAL_GIC_ROUTE_SHARED_INT_MAP_TO_PIN   0x80000000
#define CPUHAL_GIC_ROUTE_SHARED_INT_MAP_TO_NMI   0x40000000
#define CPUHAL_GIC_ROUTE_SHARED_INT_MAP_TO_YQ    0x20000000
#define CPUHAL_GIC_GLOBAL_EDGE_SET_BIT_MASK      0x80000000
#define CPUHAL_GIC_SH_MAPx_VPE31_0_SEPARATION    0x20

/* GIC VPE-local  section offset */
#define CPUHAL_GIC_LOCAL_REG_BASE_OFFSET    (0x8000)
#define CPUHAL_GIC_LOCAL_REG_BASE_ADDR      (CPUHAL_GIC_REG_BASE_ADDR | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_REG_BASE_ADDR_UPPER_BITS (CPUHAL_GIC_LOCAL_REG_BASE_ADDR >> 16)
#define CPUHAL_GIC_LOCAL_CTL                  (0x00 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_PEND                 (0x04 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_MASK                 (0x08 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_RMASK                (0x0c | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_SMASK                (0x10 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_WD_MAP               (0x40 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_COMPARE_MAP          (0x44 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_TIMER_MAP            (0x48 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_FDC_MAP              (0x4c | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_PERFCTR_MAP          (0x50 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_SWInt0_MAP           (0x54 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_SWInt1_MAP           (0x58 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_OTHER_ADDR           (0x80 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_IDENT                (0x88 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_WD_CONFIG0           (0x90 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_WD_COUNT0            (0x94 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_WD_INITIAL0          (0x98 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_CompareLo            (0xA0 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GIC_LOCAL_CompareHi            (0xA4 | CPUHAL_GIC_LOCAL_REG_BASE_OFFSET)

/* GIC VPE-other  section offset */
#define CPUHAL_GIC_OTHER_REG_BASE_OFFSET    (0xC000)
#define CPUHAL_GIC_OTHER_REG_BASE_ADDR      (CPUHAL_GIC_REG_BASE_ADDR | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_REG_BASE_ADDR_UPPER_BITS (CPUHAL_GIC_OTHER_REG_BASE_ADDR >> 16)
#define CPUHAL_GIC_OTHER_CTL                  (0x00 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_PEND                 (0x04 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_MASK                 (0x08 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_RMASK                (0x0c | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_SMASK                (0x10 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_WD_MAP               (0x40 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_COMPARE_MAP          (0x44 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_TIMER_MAP            (0x48 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_FDC_MAP              (0x4c | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_PERFCTR_MAP          (0x50 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_SWInt0_MAP           (0x54 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_SWInt1_MAP           (0x58 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_OTHER_ADDR           (0x80 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_IDENT                (0x88 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_WD_CONFIG0           (0x90 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_WD_COUNT0            (0x94 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_WD_INITIAL0          (0x98 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_CompareLo            (0xA0 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GIC_OTHER_CompareHi            (0xA4 | CPUHAL_GIC_OTHER_REG_BASE_OFFSET)

/* GIC VPE-local/other section constants */
#define CPUHAL_GIC_VPE_INTMSK_FDC            0x00000040
#define CPUHAL_GIC_VPE_INTMSK_SWINT1         0x00000020
#define CPUHAL_GIC_VPE_INTMSK_SWINT0         0x00000010
#define CPUHAL_GIC_VPE_INTMSK_PERFCNT        0x00000008
#define CPUHAL_GIC_VPE_INTMSK_TIMER          0x00000004
#define CPUHAL_GIC_VPE_INTMSK_CMP            0x00000002
#define CPUHAL_GIC_VPE_INTMSK_WD             0x00000001
#define CPUHAL_GIC_VPE_INTMSK_ALL            0x0000007f

#define CPUHAL_GIC_VPE_BITMSK_EIC            0x00000001
#define CPUHAL_GIC_VPE_BITOFF_EIC            0x00000000

/* GCR shared section offset */
#define CPUHAL_GCR_SH_REG_BASE_OFFSET           0x0000
#define CPUHAL_GCR_SH_CONFIG                    (0x0000 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_BASE                      (0x0008 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_CONTROL                   (0x0010 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_CONTROL2                  (0x0018 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_ACCESS                    (0x0020 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REV                       (0x0030 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_ERROR_MASK                (0x0040 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_ERROR_MASK_CAUSE          (0x0048 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_ERROR_ADDR                (0x0050 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_ERROR_MULTI               (0x0058 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_CUSTOM_BASE               (0x0060 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_CUSTOM_STATUS             (0x0068 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_L2_ONLY_SYNC_BASE         (0x0070 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_GIC_REG_BASE              (0x0080 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_CPC_BASE                  (0x0088 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG0_BASE                 (0x0090 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG0_MASK                 (0x0098 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG1_BASE                 (0x00A0 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG1_MASK                 (0x00A8 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG2_BASE                 (0x00B0 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG2_MASK                 (0x00B8 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG3_BASE                 (0x00C0 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG3_MASK                 (0x00C8 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_GIC_STATUS                (0x00D0 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_CACHE_REV                 (0x00E0 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_CPC_STATUS                (0x00F0 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG4_BASE                 (0x0190 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG4_MASK                 (0x0198 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG5_BASE                 (0x01A0 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG5_MASK                 (0x01A8 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_IOCU_REV                  (0x0200 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG6_BASE                 (0x0210 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG6_MASK                 (0x0218 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG7_BASE                 (0x0220 | CPUHAL_GCR_SH_REG_BASE_OFFSET)
#define CPUHAL_GCR_SH_REG7_MASK                 (0x0228 | CPUHAL_GCR_SH_REG_BASE_OFFSET)


/* GCR Core LOCAL section */
#define CPUHAL_GCR_LOCAL_REG_BASE_OFFSET     0x2000
#define CPUHAL_GCR_LOCAL_COHERENCE           (0x0008 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_CONFIG              (0x0010 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_OTHER               (0x0018 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_RESET_BASE          (0x0020 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_ID                  (0x0028 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_RESET_EXT_BASE      (0x0030 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_0_PRIORITY     (0x0040 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_1_PRIORITY     (0x0048 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_2_PRIORITY     (0x0050 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_3_PRIORITY     (0x0058 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_4_PRIORITY     (0x0060 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_5_PRIORITY     (0x0068 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_6_PRIORITY     (0x0070 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_7_PRIORITY     (0x0078 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_GCR_LOCAL_TCID_8_PRIORITY     (0x0080 | CPUHAL_GCR_LOCAL_REG_BASE_OFFSET)

/* GCR Core OTHER section */
#define CPUHAL_GCR_OTHER_REG_BASE_OFFSET     0x4000
#define CPUHAL_GCR_OTHER_RESET_RELEASE       (0x0000 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_COHERENCE           (0x0008 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_CONFIG              (0x0010 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_OTHER               (0x0018 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_RESET_BASE          (0x0020 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_ID                  (0x0028 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_RESET_EXT_BASE      (0x0030 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_0_PRIORITY     (0x0040 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_1_PRIORITY     (0x0048 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_2_PRIORITY     (0x0050 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_3_PRIORITY     (0x0058 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_4_PRIORITY     (0x0060 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_5_PRIORITY     (0x0068 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_6_PRIORITY     (0x0070 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_7_PRIORITY     (0x0078 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)
#define CPUHAL_GCR_OTHER_TCID_8_PRIORITY     (0x0080 | CPUHAL_GCR_OTHER_REG_BASE_OFFSET)

/* GCR Debug Control Block */
#define CPUHAL_GCR_DEBUG_REG_BASE_OFFSET     0x6000
#define CPUHAL_GCR_DEBUG_TCBCONTROLB         (0x0008 | CPUHAL_GCR_DEBUG_REG_BASE_OFFSET)
#define CPUHAL_GCR_DEBUG_TCBTW_LO            (0x0200 | CPUHAL_GCR_DEBUG_REG_BASE_OFFSET)
#define CPUHAL_GCR_DEBUG_TCBTW_HI            (0x0208 | CPUHAL_GCR_DEBUG_REG_BASE_OFFSET)

#define CPUHAL_GCR_DEBUG_TCBCONTROLB_BITMSK_EN     0x00000001
#define CPUHAL_GCR_DEBUG_TCBCONTROLB_BITMSK_OFC    0x00000002
#define CPUHAL_GCR_DEBUG_TCBCONTROLB_BITMSK_RM     0x00010000
#define CPUHAL_GCR_DEBUG_TCBCONTROLB_BITMSK_WE     0x80000000

/* CPC Shared section */
#define CPUHAL_CPC_SH_REG_BASE_OFFSET      0x0000
#define CPUHAL_CPC_SH_ACCESS              (0x0000 | CPUHAL_CPC_SH_REG_BASE_OFFSET)
#define CPUHAL_CPC_SH_SEQDEL              (0x0008 | CPUHAL_CPC_SH_REG_BASE_OFFSET)
#define CPUHAL_CPC_SH_RAIL                (0x0010 | CPUHAL_CPC_SH_REG_BASE_OFFSET)
#define CPUHAL_CPC_SH_RESETLEN            (0x0018 | CPUHAL_CPC_SH_REG_BASE_OFFSET)
#define CPUHAL_CPC_SH_REVISION            (0x0020 | CPUHAL_CPC_SH_REG_BASE_OFFSET)

/* CPC Core LOCAL section */
#define CPUHAL_CPC_LOCAL_REG_BASE_OFFSET   0x2000
#define CPUHAL_CPC_LOCAL_CMD              (0x0000 | CPUHAL_CPC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_CPC_LOCAL_STAT_CONF        (0x0008 | CPUHAL_CPC_LOCAL_REG_BASE_OFFSET)
#define CPUHAL_CPC_LOCAL_OTHER            (0x0010 | CPUHAL_CPC_LOCAL_REG_BASE_OFFSET)

/* CPC Core OTHER section */
#define CPUHAL_CPC_OTHER_REG_BASE_OFFSET  0x4000
#define CPUHAL_CPC_OTHER_CMD             (0x0000 | CPUHAL_CPC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_CPC_OTHER_STAT_CONF       (0x0008 | CPUHAL_CPC_OTHER_REG_BASE_OFFSET)
#define CPUHAL_CPC_OTHER_OTHER           (0x0010 | CPUHAL_CPC_OTHER_REG_BASE_OFFSET)

#define CPUHAL_GCR_REGION_OFFSET             0x10

/* CM region mapping */
#define CPUHAL_GCR_CM_REGION_MEM            0x1
#define CPUHAL_GCR_CM_REGION_IOCU           0x2

/* CM CCA Override_Value */
#define CPUHAL_GCR_CM_CCA_VALUE_OFFSET      0x5
#define CPUHAL_GCR_CM_CCA_VALUE_WT          0x0
#define CPUHAL_GCR_CM_CCA_VALUE_WTNA        0x1
#define CPUHAL_GCR_CM_CCA_VALUE_UC          0x2
#define CPUHAL_GCR_CM_CCA_VALUE_WB_C_NCO    0x3
#define CPUHAL_GCR_CM_CCA_VALUE_UCA         0x7


/* CM CCA enable */
#define CPUHAL_GCR_CM_CCA_ENABLE_OFFSET     0x4
#define CPUHAL_GCR_CM_CCA_ENABLE            0x1
#define CPUHAL_GCR_CM_CCA_DISABLE           0x0

/* GCR shared section constants */
#define CPUHAL_GCR_GIC_BASE_ENABLE           0x1

/* CPC */
#define CPUHAL_CPC_CMD_CLOCK_OFF          (0x1)
#define CPUHAL_CPC_CMD_POWER_DOWN         (0x2)
#define CPUHAL_CPC_CMD_POWER_UP           (0x3)
#define CPUHAL_CPC_CMD_RESET              (0x4)

#define CPUHAL_CPC_PWRUP_EVT_BITMASK       0x800000
#define CPUHAL_CPC_PWRUP_EVT_BITOFF        23
#define CPUHAL_CPC_SEQ_STATE_BITMASK       0x780000
#define CPUHAL_CPC_SEQ_STATE_BITOFF        19
#define CPUHAL_CPC_CLKGATE_BITMASK         0x20000
#define CPUHAL_CPC_CLKGATE_BITOFF          17
#define CPUHAL_CPC_PWRDN_IMPL_BITMASK      0x10000
#define CPUHAL_CPC_PWRDN_IMPL_BITOFF       16
#define CPUHAL_CPC_EJTAG_BITMASK           0x8000
#define CPUHAL_CPC_EJTAG_BITOFF            15

#define CPUHAL_CPC_SEQ_STATE_D0_PWRDWN          0x0
#define CPUHAL_CPC_SEQ_STATE_U0_VDDOK           0x1
#define CPUHAL_CPC_SEQ_STATE_U1_UPDELAY         0x2
#define CPUHAL_CPC_SEQ_STATE_U2_UCLKOFF         0x3
#define CPUHAL_CPC_SEQ_STATE_U3_RESET           0x4
#define CPUHAL_CPC_SEQ_STATE_U4_RESETDLY        0x5
#define CPUHAL_CPC_SEQ_STATE_U5_NONCOH_EXEC     0x6
#define CPUHAL_CPC_SEQ_STATE_U6_COH_EXEC        0x7
#define CPUHAL_CPC_SEQ_STATE_D1_ISOLATE         0x8
#define CPUHAL_CPC_SEQ_STATE_D3_CLRBUS          0x9
#define CPUHAL_CPC_SEQ_STATE_D2_DCLKOFF         0xa


/* SPRAM Access - Index Ld/St Tag instructios will access SPRAM tag values */
#define CPUHAL_SPRAM_WST_SPR_ITC_VALUE          0x4
#define CPUHAL_SPRAM_WST_SPR_ITC_BIT_OFFSET     26
#define CPUHAL_SPRAM_BASEPA_BIT_MASK            0xFFFFF000
#define CPUHAL_SPRAM_ENABLE_BIT_MASK            0x80
#define CPUHAL_SPRAM_SIZE                       PCS_DSPRAM_SIZE
#define CPUHAL_SPRAM_DSPRAM_PRESENT_BIT_MASK    0x800000

/* 
** Debug register offsets from DRSEG region**
** See section 14.10 of interAptive CPS Software User Manual
*/ 
#define CPUHAL_DRSEG_TCBCONTROLA_OFFSET                 0x3000
#define CPUHAL_DRSEG_TCBCONTROLB_OFFSET                 0x3008
#define CPUHAL_DRSEG_TCBCONTROLC_OFFSET                 0x3010
#define CPUHAL_DRSEG_TCBCONTROLD_OFFSET                 0x3018
#define CPUHAL_DRSEG_TCBCONTROLE_OFFSET                 0x3020

#define CPUHAL_DRSEG_TCBCONTROLB_BITMSK_EN              0x00000001
#define CPUHAL_DRSEG_TCBCONTROLB_BITMSK_WE              0x80000000

#define CPUHAL_DRSEG_TCBCONTROLC_BITMSK_MTTRACETC       0x00000001
#define CPUHAL_DRSEG_TCBCONTROLC_BITMSK_MODE_PC         0x00800000

/*
* Enumerated Types
*/


/*
* Constants
*/
#if defined(__ASSEMBLER__) || defined(__LANGUAGE_ASM__)

#else
/*
* Macro Definitions
*/
/**
* @brief
*   This macro reads a register from the CPUHAL_GIC when given a valid register
*   offset.
*
* @param[in] reg_offset - The register offset (not including its base address)
*
* @return 
*   The content of the given register.
*
* @note
*   For example, the the following statement
*     reg_val = CPUHAL_GIC_REG_READ(0x04);
*   reads a 32-bit value from register offset 0x04 within the CPUHAL_GIC and
*   assigns the value read to reg_val, which is of the UINT32 type.
*
*/
#define CPUHAL_GIC_REG_READ(reg_offset)   \
            BUSIO_READ32_32_8(CPUHAL_GIC_REG_BASE_ADDR, reg_offset)


/**
* @brief
*   This macro writes a 32-bit value to a register in the CPUHAL_GIC when given
*   a valid register offset.
*
* @param[in] reg_offset  - The register offset (not including its base address)
* @param[in] reg_val     - the 32-bit value to write
*
* @return 
*    None.
*
* @note
*     For example, the following statement
*
* @note
*     MBIC_REG_WRITE(0x04, 0x7ABCD);
*
* @note
*     writes a 32-bit value of 0x7ABCD to register offset 0x04 within the CPUHAL_GIC.
*
*/
#define CPUHAL_GIC_REG_WRITE(reg_offset, reg_val) \
            BUSIO_WRITE32_32_8(CPUHAL_GIC_REG_BASE_ADDR, reg_offset, reg_val)


/**
* @brief
*   This macro reads a register from the CPUHAL_GCR when given a valid register
*   offset.
*
* @param[in] reg_offset - The register offset (not including its base address)
*
* @return 
*   The content of the given register.
*
* @note
*     For example, the the following statement
*
* @note
*     reg_val = CPUHAL_GCR_REG_READ(0x04);
*
* @note
*     reads a 32-bit value from register offset 0x04 within the CPUHAL_GCR and
*   assigns the value read to reg_val, which is of the UINT32 type.
*
*/
#define CPUHAL_GCR_REG_READ(reg_offset)   \
            BUSIO_READ32_32_8(CPUHAL_GCR_REG_BASE_ADDR, reg_offset)


/**
* @brief
*   This macro writes a 32-bit value to a register in the CPUHAL_GCR when given
*   a valid register offset.
*
* @param[in] reg_offset  - The register offset (not including its base address)
* @param[in] reg_val     - the 32-bit value to write
*
* @return 
*    None.
*
* @note
*     For example, the following statement
*
* @note
*     CPUHAL_GCR_REG_WRITE(0x04, 0x7ABCD);
*
* @note
*     writes a 32-bit value of 0x7ABCD to register offset 0x04 within the CPUHAL_GCR.
*
*/
#define CPUHAL_GCR_REG_WRITE(reg_offset, reg_val) \
            BUSIO_WRITE32_32_8(CPUHAL_GCR_REG_BASE_ADDR, reg_offset, reg_val)


/**
* @brief
*   This macro reads a register from the DRSEG when given a
*   valid register offset.
*
* @param[in] reg_offset - The register offset (not including its base address)
*
* @return 
*   The content of the given register.
*
* @note
*   DRSEG region is only accessible when CPU is in DEBUG mode.
*/
#define CPUHAL_DRSEG_REG_READ(reg_offset)   \
            BUSIO_READ32_32_8(CPUHAL_DRSEG_BASE_ADDR, reg_offset)


/**
* @brief
*   This macro writes a 32-bit value to a register in the DRSEG
*   when given a valid register offset.
*
* @param[in] reg_offset  - The register offset (not including its base address)
* @param[in] reg_val     - the 32-bit value to write
*
* @return 
*    None.
*
* @note
*   DRSEG region is only accessible when CPU is in DEBUG mode.
*/
#define CPUHAL_DRSEG_REG_WRITE(reg_offset, reg_val) \
            BUSIO_WRITE32_32_8(CPUHAL_DRSEG_BASE_ADDR, reg_offset, reg_val)

#if !defined(CPU_34K)
/**
* @brief
*   This macro writes a 32-bit value to a register in the CPUHAL_CPC when given
*   a valid register offset.
*
* @param[in] reg_offset  - The register offset (not including its base address)
* @param[in] reg_val     - the 32-bit value to write
*
* @return 
*    None.
*
* @note
*     For example, the following statement
*
* @note
*     MBIC_REG_WRITE(0x04, 0x7ABCD);
*
* @note
*     writes a 32-bit value of 0x7ABCD to register offset 0x04 within the CPUHAL_GCR.
*
*/
#define CPUHAL_CPC_REG_WRITE(reg_offset, reg_val) \
            BUSIO_WRITE32_32_8(CPUHAL_CPC_REG_BASE_ADDR, reg_offset, reg_val)


/**
* @brief
*   This macro reads a register from the CPUHAL_CPC when given
*   a valid register offset.
*
* @param[in] reg_offset - The register offset (not including its
*       base address)
*
* @return 
*   The content of the given register.
*
* @note
*     For example, the the following statement
*
* @note
*     reg_val = CPUHAL_CPC_REG_READ(0x04);
*
* @note
*     reads a 32-bit value from register offset 0x04 within the
*   CPUHAL_CPC and assigns the value read to reg_val, which is
*   of the UINT32 type.
*
*/
#define CPUHAL_CPC_REG_READ(reg_offset)   \
            BUSIO_READ32_32_8(CPUHAL_CPC_REG_BASE_ADDR, reg_offset)

#endif

/**
* @brief
*   This macro reads a register from the CPUHAL_UD_GCR when
*   given a valid register offset.
*
* @param[in] reg_offset - The register offset (not including its base address)
*
* @return 
*   The content of the given register.
*
* @note
*     For example, the the following statement
*
* @note
*     reg_val = CPUHAL_UD_GCR_REG_READ(0x04);
*
* @note
*     reads a 32-bit value from register offset 0x04 within the CPUHAL_GCR and
*   assigns the value read to reg_val, which is of the UINT32 type.
*
*/
#define CPUHAL_UD_GCR_REG_READ(reg_offset)   \
            BUSIO_READ32_32_8(CPUHAL_UD_GCR_REG_BASE_ADDR, reg_offset)

/**
* @brief
*   This macro writes a 32-bit value to a register in the
*   CPUHAL_UD_GCR when given a valid register offset.
*
* @param[in] reg_offset  - The register offset (not including its base address)
* @param[in] reg_val     - the 32-bit value to write
*
* @return 
*    None.
*
* @note
*     For example, the following statement
*
* @note
*     CPUHAL_UD_GCR_REG_WRITE(0x04, 0x7ABCD);
*
* @note
*     writes a 32-bit value of 0x7ABCD to register offset 0x04 within the CPUHAL_GCR.
*
*/
#define CPUHAL_UD_GCR_REG_WRITE(reg_offset, reg_val) \
            BUSIO_WRITE32_32_8(CPUHAL_UD_GCR_REG_BASE_ADDR, reg_offset, reg_val)

/**
* @brief
*   This macro performs a 32-bit read-modify-write to a register in the
*   CPUHAL_UD_GCR when given a valid register offset.
*
* @param[in] reg_offset  - The register offset (not including its base address)
* @param[in] mask        - 32-bit mask of writable bits
* @param[in] value       - 32-bit value of bits to write
*
* @return
*    None.
*
*/
#define CPUHAL_UD_GCR_REG_READ_WRITE(reg_offset, mask, value) \
    busio_field_write32_32_8(((void *)CPUHAL_UD_GCR_REG_BASE_ADDR), (reg_offset), (mask), (value))

/*
* Structures and Unions
*/

/*
* Global variables
*/

/*
* Function Prototypes
*/
#endif

#endif /* _CPUHAL_HW_H */
/** @} end addtogroup */


