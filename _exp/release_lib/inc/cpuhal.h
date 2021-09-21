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
* @defgroup CPUHAL CPUHAL: CPU Hardware Abstraction Layer
* @brief
* CPU Hardware Abstraction Layer
* @{
* @file
* @brief
*     This file contains the public hardware abstraction layer (hal)
*     definitions for the MIPS32 family of CPUs
*
* @note
*     This section describes:
*
* @note
*   PROBLEM: 1004K coherency manager lock-up and the required workaround
*   ~~~~~~~
*   The MIPS 1004K Coherency Manager locks up when the cores are in the
*   coherency domain and when the 1004K is configured for non-coherent
*   writebacks (Config7.NCWB=1).  The lockup occurs when the non-coherent
*   writeback is a miss due to cache replacement and incorrect
*   handling of the SYNC(2) that was supposed to be issued in this case.
*
* @note
*   GENERAL BACKGROUND INFO:
*   ~~~~~~~~~~~~~~~~~~~~~~~~
*   Non-coherent writebacks (Config7.NCWB=1) is a performance feature to remove
*   the need for a SYNC(2) or other completion/ordering barrier.  Non-coherent
*   writebacks follow the same path as UNCACHED writes through the 1004k
*   Coherency Manager and a single queue maintains ordering of the
*   writes.  Coherent writebacks (Config7.NCWB=0) follow a different path than
*   UNCACHED writes through the Coherency Manager; the CM services a queue
*   for coherent writebacks and a separate queue for UNCACHED writes so
*   ordering is not preserved from firmware's perspective.  With coherent
*   writebacks (Config7.NCWB=0), firmware must use a SYNC(2) following the
*   coherent writebacks in order to preserve ordering.
*
* @note
*   SYNC(2) is a completion barrier that ensures that all 1004k
*   coherency manager interventions are complete.  Completion barriers
*   degrade performance (stall).
*
*
* @note
*   WORKAROUND:
*   ~~~~~~~~~~
*
* @note
*   Rev B.
*   ------
*   There are 2 workarounds for Rev B:
*
* @note
*   i)  Configure the 1004K for coherent writebacks (Config7.NCWB=0).  This
*       will require a SYNC(2) before issuing an UNCACHED write to ensure
*       that all coherency interventions complete prior to issuing UNCACHED
*       write.  This means all coherent writebacks that occur before the
*       SYNC(2) must be completed before the UNCACHED write after the SYNC(2)
*       are allowed to start.
*
* @note
*
*       IMPORTANT:
*         If the data is L2 cachable then the requirement is to use
*         a SYNC(3) to ensure ordering through the L2, this is
*         unchanged by this bug.  However, there is no requirement
*         to do both a SYNC(2) and SYNC(3) a SYNC(3) will suffice.
*
* @note
*   OR
*
* @note
*   ii) Configure the 1004K for non-coherent writebacks (Config7.NCWB=1).
*       Configure the MCPS to drop L3 cacheops in the CM (GCR_ERROR_MASK[31]=1).
*       This will require a L3 CACHEOP (HitWB) before issuing an UNCACHED
*       write to ensure that all interventions complete prior to issuing
*       UNCACHED write.
*
* @note
*
*       IMPORTANT:
*         If the data is L2 cachable then the requirement is to use
*         a SYNC(3) to ensure ordering through the L2, this is
*         unchanged by this bug.  However, there is no requirement
*         to do both a L3 Cacheop and SYNC(3) a SYNC(3) will suffice.
*
* @note
*   Option i) is the appropriate choice for current firmware, because the same
*   cache writeback functions are in use in firmware for coherent and
*   non-coherent addresses. Option ii) only applies for non-coherent memory
*   regions.
*
*
*   ~~~~~~~~
*   PROBLEM: 1004K SYNC 4, SYNC 16, SYNC 18 and SYNC 19 instruction are
*            not heavy enough to guarantee ordering across cores.
*   ~~~~~~~~
*   Even though the 1004k is an in-order core and sees all of the loads
*   and stores in program order, it is not sufficient enough to preserve
*   ordering across cores due to the possibility of re-ordering in the
*   coherence manager.  Please note that the ordering is still guarantee
*   within the same core.
*
* @note
*   EXAMPLE:
*   ~~~~~~~~
*   Consumer CPU sees the lock being released, but reads stale data.
*     1. Producer CPU acquires lock (transitions lock cacheline to
*        modified)
*     2. Consumer CPU issues read of lock (it's on its way, but has
*        not been seen by the producer CPU yet).
*     3. Producer CPU writes data (issues cohUpgrade to invalidate
*        other core.  Assume this gets stuck in the core or in the
*        CM. This is a non-blocking operation so the producer CPU
*        continues to execute).
*     4. Producer CPU executes SYNC_RELASE (flushes all previous
*        requests. Waits only until CM has accepted the last request,
*        so the cohUpgrade from step 3 is still in flight in the CM).
*     5. Producer CPU writes lock to clear (no external request required
*        since it's modified in its L1)
*     6. Consumer CPU's read of lock from step 2 is seen by producer CPU
*        (hits in producer CPU's L1, so it's a cache-to-cache transfer
*        back to the consumer CPU)
*     7. Consumer CPU issues SYNC_ACQUIRE (doesn't really do anything
*        in this case)
*     8. Consumer CPU reads data (this data is stale because the
*        cohUpgrade from step 3 has been seen yet) *** ERROR ***
*     9. Consumer CPU sees cohUprade from step 3 (too late!)
*
* @note
*   WORKAROUND:
*   ~~~~~~~~~~
*   Use a SYNC 2 instead of SYNC 4, SYNC 16, SYNC 18 and SYNC 19.  SYNC 17
*   is not affected, because 1004k is an in-order processor.
*
*/

/*---- Compile Options -----------------------------------------------------*/
#ifndef _CPUHAL_H
#define _CPUHAL_H

/*---- Include Files -------------------------------------------------------*/
#include "cpuhal_plat_cfg.h"
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "cpuhal_asm.h"
#include "cpuhal_hw.h"
#include "cpuhal_api.h"
#include "tmr_sys_plat.h"

#include <stdlib.h>
#ifdef USE_SDE_TOOLS
#include <mips/cpu.h>
#endif


/*---- Constant and Type Declarations --------------------------------------*/

/* Co processor defines for the MIP 1004K */
#define C0_MVPControl $0,1
#define C0_MVPConf0   $0,2
#define C0_VPEControl $1,1
#define C0_YQMask     $1,4
#define C0_TCStatus   $2,1
#define C0_TCBind     $2,2
#define C0_TCHalt     $2,4
#define C0_SRSConf0   $6,1
#define CO_Status     $12,0
#define C0_IntCtl     $12,1
#define C0_SRSCtl     $12,2
#define C0_SRSMap     $12,3
#define C0_EBase      $15,1
#define C0_TraceControl     $23,1
#define C0_TraceControl2    $23,2
#define C0_TraceControl3    $24,2

#define CPUHAL_CP0_ERRCTL_PE_BITMSK     0x80000000
#define CPUHAL_CP0_ERRCTL_PO_BITMSK     0x40000000
#define CPUHAL_CP0_ERRCTL_PCO_BITMSK    0x08000000
#define CPUHAL_CP0_ERRCTL_L2P_BITMSK    0x00800000
#define CPUHAL_CP0_ERRCTL_L1ECC_BITMSK  0x00400000
#define CPUHAL_CP0_ERRCTL_WST_BITMSK    0x20000000
#define CPUHAL_CP0_ERRCTL_SPR_BITMSK    0x10000000
#define CPUHAL_CP0_ERRCTL_ITC_BITMSK    0x04000000
#define CPUHAL_CP0_ERRCTL_SE_BITMSK     0x00100000
#define CPUHAL_CP0_ERRCTL_FE_BITMSK     0x00080000
#define CPUHAL_CP0_ERRCTL_PCI_BITMSK    0x0007E000
#define CPUHAL_CP0_ERRCTL_PCP_BITMSK    0x00001000
#define CPUHAL_CP0_ERRCTL_PI_BITMSK     0x00000FF0
#define CPUHAL_CP0_ERRCTL_PD_BITMSK     0x0000000F

#define CPUHAL_CP0_CONFIG1_REG          16
#define CPUHAL_CP0_CONFIG1_SEL          1

#define CPUHAL_CP0_CAUSE_REG            13
#define CPUHAL_CP0_CAUSE_SEL            0

#define CPUHAL_CP0_CONFIG1_DS_BITMSK    0x0000e000
#define CPUHAL_CP0_CONFIG1_DS_BITOFF    13
#define CPUHAL_CP0_CONFIG1_DL_BITMSK    0x00001c00
#define CPUHAL_CP0_CONFIG1_DL_BITOFF    10
#define CPUHAL_CP0_CONFIG1_DA_BITMSK    0x00000380
#define CPUHAL_CP0_CONFIG1_DA_BITOFF    7

#define CPUHAL_CP0_CACHEERR_REG         27
#define CPUHAL_CP0_CACHEERR_SEL         0

#define CPUHAL_CP0_ERRCTL_REG           26
#define CPUHAL_CP0_ERRCTL_SEL           0

#define CPUHAL_CP0_CACHEERR_REG         27
#define CPUHAL_CP0_CACHEERR_SEL         0

#define CPUHAL_CP0_ITAGLO_REG           28
#define CPUHAL_CP0_ITAGLO_SEL           0
#define CPUHAL_CP0_IDATALO_REG          28
#define CPUHAL_CP0_IDATALO_SEL          1
#define CPUHAL_CP0_IDATAHI_REG          29
#define CPUHAL_CP0_IDATAHI_SEL          1
#define CPUHAL_CP0_DTAGLO_REG           28
#define CPUHAL_CP0_DTAGLO_SEL           2
#define CPUHAL_CP0_DTAGHI_REG           29
#define CPUHAL_CP0_DTAGHI_SEL           2
#define CPUHAL_CP0_DDATALO_REG          28
#define CPUHAL_CP0_DDATALO_SEL          3

#define CPUHAL_CP0_DTAGLO_TAG_BITMSK    0xfffff800
#define CPUHAL_CP0_ITAGLO_TAG_BITMSK    0xfffff800

/* If DPSRAM is ECC protected */
#define CPUHAL_CP0_DTAGHI_DPSRAM_ECC_BITMSK 0x000FFFFF
#define CPUHAL_CP0_DTAGHI_DPSRAM_ECC_BITOFF 0

/* If L1 D$ is ECC protected */
#define CPUHAL_CP0_DTAGHI_L1_D_ECC_BITMSK   0x0001FC00
#define CPUHAL_CP0_DTAGHI_L1_D_ECC_BITOFF   10
#define CPUHAL_CP0_DTAGHI_L1_T_ECC_BITMSK   0x0000003F
#define CPUHAL_CP0_DTAGHI_L1_T_ECC_BITOFF   0

/* Trace Control register bits */
#define CPUHAL_CP0_TRCTRL_BITMSK_ON             0x00000001
#define CPUHAL_CP0_TRCTRL_BITMSK_TIM            0x00000002
#define CPUHAL_CP0_TRCTRL_BITMSK_TLSM           0x00000004
#define CPUHAL_CP0_TRCTRL_BITMSK_TFCR           0x00000008
#define CPUHAL_CP0_TRCTRL_BITMSK_G              0x00000010
#define CPUHAL_CP0_TRCTRL_BITMSK_U              0x00200000
#define CPUHAL_CP0_TRCTRL_BITMSK_S              0x00400000
#define CPUHAL_CP0_TRCTRL_BITMSK_K              0x00800000
#define CPUHAL_CP0_TRCTRL_BITMSK_E              0x01000000
#define CPUHAL_CP0_TRCTRL_BITMSK_TS             0x80000000

#define CPUHAL_CP0_TRCTRL2_BITMSK_MODE_PC       0x00000080
#define CPUHAL_CP0_TRCTRL2_BITMSK_MODE_LD_ADDR  0x00000100
#define CPUHAL_CP0_TRCTRL2_BITMSK_MODE_LD_DATA  0x00000200
#define CPUHAL_CP0_TRCTRL2_BITMSK_MODE_ST_ADDR  0x00000400
#define CPUHAL_CP0_TRCTRL2_BITMSK_MODE_ST_DATA  0x00000800

/* CP0 Register 13, Select 0. Cause register bits*/
#define CPUHAL_CP0_CAUSE_EXC_CODE_BITMSK        0x0000007c
#define CPUHAL_CP0_CAUSE_EXC_CODE_BITOFF        2


/**
* @brief
*   This macro shall be used to creates error codes for the M1004KHAL module.
*   Each M1004KHAL error code has 32 bits, is of type "PMCFW_ERROR".
*   The format is as follows:
*   [0 (4-bit) | M1004KHAL error base (16-bit) | error code suffix (12-bit)]
*
* @brief
*   where
*     M1004KHAL error base    - 16-bit PMCFW_ERR_BASE_M34KHAL, defined in "pmcfw_err.h".
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
#define CPUHAL_ERR_CODE_CREATE(err_suffix)    ((PMCFW_ERR_BASE_CPUHAL) | (err_suffix))

#define CPUHAL_ERR_ADDR_NOT_CACHELINE_ALIGNED                   CPUHAL_ERR_CODE_CREATE(0x001)
#define CPUHAL_ERR_ADDR_INVALID                                 CPUHAL_ERR_CODE_CREATE(0x002)
#define CPUHAL_ERR_US_TO_SYSCLK                                 CPUHAL_ERR_CODE_CREATE(0x003)
#define CPUHAL_ERR_SYSCLK_TO_US_INVALID_FREQ                    CPUHAL_ERR_CODE_CREATE(0x004)
#define CPUHAL_ERR_GIC_INT_NUM                                  CPUHAL_ERR_CODE_CREATE(0x005)
#define CPUHAL_ERR_GIC_NO_CB_FCN                                CPUHAL_ERR_CODE_CREATE(0x006)
#define CPUHAL_ERR_VPE_ID_UNKNOWN                               CPUHAL_ERR_CODE_CREATE(0x007)
#define CPUHAL_ERR_GIC_VEC_NUM                                  CPUHAL_ERR_CODE_CREATE(0x008)
#define CPUHAL_ERR_GIC_VEC_PRIO                                 CPUHAL_ERR_CODE_CREATE(0x009)
#define CPUHAL_ERR_DSPRAM_NOT_PRESENT                           CPUHAL_ERR_CODE_CREATE(0x00A)
#define CPUHAL_ERR_DSPRAM_CFG_INVALID                           CPUHAL_ERR_CODE_CREATE(0x00B)
#define CPUHAL_ERR_NOT_SUPPORTED                                CPUHAL_ERR_CODE_CREATE(0x00C)
#define CPUHAL_ERR_INT_DEST_UNKNWON                             CPUHAL_ERR_CODE_CREATE(0x00D)
#define CPUHAL_ERR_BAD_CFG                                      CPUHAL_ERR_CODE_CREATE(0x00E)
#define CPUHAL_ERR_INVALID_TLB_PAGE_SIZE                        CPUHAL_ERR_CODE_CREATE(0x00F)
#define CPUHAL_ERR_INVALID_TLB_MEM_REGION_SIZE                  CPUHAL_ERR_CODE_CREATE(0x010)
#define CPUHAL_ERR_TLB_ENTRY_OVERFLOW                           CPUHAL_ERR_CODE_CREATE(0x011)
#define CPUHAL_ERR_INVALID_TLB_PHYSICAL_ADDRESS                 CPUHAL_ERR_CODE_CREATE(0x012)
#define CPUHAL_ERR_INVALID_TLB_VIRTUAL_ADDRESS                  CPUHAL_ERR_CODE_CREATE(0x013)
#define CPUHAL_ERR_INVALID_TLB_CACHE_SETTING                    CPUHAL_ERR_CODE_CREATE(0x014)
#define CPUHAL_ERR_INITIALIZED                                  CPUHAL_ERR_CODE_CREATE(0x015)
#define CPUHAL_ERR_UNINITIALIZED                                CPUHAL_ERR_CODE_CREATE(0x016)
#define CPUHAL_ERR_BAD_PARAM                                    CPUHAL_ERR_CODE_CREATE(0x017)
#define CPUHAL_ERR_UNREGISTERED                                 CPUHAL_ERR_CODE_CREATE(0x018)
#define CPUHAL_ERR_RESOURCE_EXHAUSTED                           CPUHAL_ERR_CODE_CREATE(0x019)

/**
* @brief
*   This enum specifies the cache coherency attributes (cca) that are
*   supported.
* @note
*   This is the coherency page attribute defined by MIPS 1004K.  Please
*   refer to MIPS documentation MD00605 for more information.
*
*/
typedef enum hal_tlb_cca_e
{
    HAL_TLB_CCA_UNCACHED = 2,
    HAL_TLB_CCA_CACHED_NOT_COHERENT = 3,
    HAL_TLB_CCA_CACHED_COH_READ_MISS_EXCLUSIVE = 4,
    HAL_TLB_CCA_CACHED_COH_READ_MISS_SHARED = 5,
    HAL_TLB_CCA_UNCACHED_ACCELERATED = 7
} hal_tlb_cca_enum;

/**
* @brief This enumeration specifies all possible MIPS 1004K CPC command.
*/
typedef enum {
    hal_cpc_cmd_unknown  = 0,  /**< Unknown */
    hal_cpc_cmd_clockoff = 1,  /**< Clock off */
    hal_cpc_cmd_pwrdown  = 2,  /**< Power down */
    hal_cpc_cmd_pwrup    = 3,  /**< Power up */
    hal_cpc_cmd_reset    = 4,  /**< Reset */
} hal_cpc_cmd_enum;

/**
* @brief
*   This enumeration specifies all possible MIPS 1004K CPC
*   sequencer state.
*/
typedef enum {
    hal_cpc_state_pwrdwn   = 0,  /**< D0 - Power down */
    hal_cpc_state_vddok    = 1,  /**< U0 - VddOK */
    hal_cpc_state_updelay  = 2,  /**< U1 - Up delay */
    hal_cpc_state_uclkoff  = 3,  /**< U2 - Up clock off */
    hal_cpc_state_reset    = 4,  /**< U3 - Reset */
    hal_cpc_state_resetdly = 5,  /**< U4 - Reset delay */
    hal_cpc_state_noncoh   = 6,  /**< U5 - Non-coherent execution */
    hal_cpc_state_coh      = 7,  /**< U6 - Coherent execution */
    hal_cpc_state_isolate  = 8,  /**< D1 - Isolate */
    hal_cpc_state_clrbus   = 9,  /**< D3 - Bus clearance */
    hal_cpc_state_dclkoff  = 10, /**< D2 - Down clock off */
} hal_cpc_state_enum;

/**
* @brief
*   This enumeration specifies all possible destinations that an interrupt
*   source can be routed to via the global interrupt controller.
*/
typedef enum
{
    HAL_GIC_INT_DEST_VPE_PIN   = 0,  /**< Per vpe interrupt pin */
    HAL_GIC_INT_DEST_VPE_NMI   = 1,  /**< NMI pin */
    HAL_GIC_INT_DEST_CORE_YQ   = 2,  /**< Yield qualifier pin */
} hal_gic_int_dest_enum;

/**
* @brief
*   This enumeration specifies all possible destination vpes that an interrupt
*   source can be routed to via the global interrupt controller.
*/
typedef enum
{
    HAL_GIC_INT_DEST_VPE_0   = 0,
    HAL_GIC_INT_DEST_VPE_1   = 1,
    HAL_GIC_INT_DEST_VPE_2   = 2,
    HAL_GIC_INT_DEST_VPE_3   = 3,
    HAL_GIC_INT_DEST_VPE_4   = 4,
    HAL_GIC_INT_DEST_VPE_5   = 5
} hal_gic_int_vpe_enum;

/**
* @brief
*   This enumeration specifies the possible interrupt trigger types that
*   can be assigned to each interrupt vector in the GIC.
*/
typedef enum
{
    HAL_GIC_INT_TRIGGER_LEVEL = 0, /**< Level trigger type */
    HAL_GIC_INT_TRIGGER_EDGE,      /**< Edge trigger type */

} hal_gic_trigger_type_enum;


/**
* @brief
*   This enumeration specifies all possible local interrupt supported
*   by the global interrupt controller.
*/
typedef enum
{
    HAL_GIC_LOCAL_INT_WD = 0,      /**< watch dog */
    HAL_GIC_LOCAL_INT_CMP = 1,     /**< count/compare */
    HAL_GIC_LOCAL_INT_TMR = 2,     /**< timer */
    HAL_GIC_LOCAL_INT_PERFCNT = 3, /**< performance counter */
    HAL_GIC_LOCAL_INT_SWINT0 = 4,  /**< swint0 */
    HAL_GIC_LOCAL_INT_SWINT1 = 5,  /**< swint1 */
    HAL_GIC_LOCAL_INT_FDC = 6,     /**< Fast Debug Channel */
    HAL_GIC_LOCAL_INT_MAX     = 7

} hal_gic_local_int_enum;

/**
* @brief
*   This enumeration specifies the possible interrupt priority levels that
*   can be assigned to each interrupt vector in the GIC.
*/
typedef enum
{
    HAL_GIC_VEC_PRIO_0 = 0, /**< Priority level 0 (lowest) */
    HAL_GIC_VEC_PRIO_1,     /**< Priority level 1 */
    HAL_GIC_VEC_PRIO_2,     /**< Priority level 2 */
    HAL_GIC_VEC_PRIO_3,     /**< Priority level 3 */
    HAL_GIC_VEC_PRIO_4,     /**< Priority level 4 */
    HAL_GIC_VEC_PRIO_5,     /**< Priority level 5 (highest non EIC) */
    HAL_GIC_VEC_PRIO_6,     /**< Priority level 6 (EIC only) ... */
    HAL_GIC_VEC_PRIO_7,
    HAL_GIC_VEC_PRIO_8,
    HAL_GIC_VEC_PRIO_9,
    HAL_GIC_VEC_PRIO_10,
    HAL_GIC_VEC_PRIO_11,
    HAL_GIC_VEC_PRIO_12,
    HAL_GIC_VEC_PRIO_13,
    HAL_GIC_VEC_PRIO_14,
    HAL_GIC_VEC_PRIO_15,
    HAL_GIC_VEC_PRIO_16,
    HAL_GIC_VEC_PRIO_17,
    HAL_GIC_VEC_PRIO_18,
    HAL_GIC_VEC_PRIO_19,
    HAL_GIC_VEC_PRIO_20,
    HAL_GIC_VEC_PRIO_21,
    HAL_GIC_VEC_PRIO_22,
    HAL_GIC_VEC_PRIO_23,
    HAL_GIC_VEC_PRIO_24,
    HAL_GIC_VEC_PRIO_25,
    HAL_GIC_VEC_PRIO_26,
    HAL_GIC_VEC_PRIO_27,
    HAL_GIC_VEC_PRIO_28,
    HAL_GIC_VEC_PRIO_29,
    HAL_GIC_VEC_PRIO_30,
    HAL_GIC_VEC_PRIO_31,
    HAL_GIC_VEC_PRIO_32,
    HAL_GIC_VEC_PRIO_33,
    HAL_GIC_VEC_PRIO_34,
    HAL_GIC_VEC_PRIO_35,
    HAL_GIC_VEC_PRIO_36,
    HAL_GIC_VEC_PRIO_37,
    HAL_GIC_VEC_PRIO_38,
    HAL_GIC_VEC_PRIO_39,
    HAL_GIC_VEC_PRIO_40,
    HAL_GIC_VEC_PRIO_41,
    HAL_GIC_VEC_PRIO_42,
    HAL_GIC_VEC_PRIO_43,
    HAL_GIC_VEC_PRIO_44,
    HAL_GIC_VEC_PRIO_45,
    HAL_GIC_VEC_PRIO_46,
    HAL_GIC_VEC_PRIO_47,
    HAL_GIC_VEC_PRIO_48,
    HAL_GIC_VEC_PRIO_49,
    HAL_GIC_VEC_PRIO_50,
    HAL_GIC_VEC_PRIO_51,
    HAL_GIC_VEC_PRIO_52,
    HAL_GIC_VEC_PRIO_53,
    HAL_GIC_VEC_PRIO_54,
    HAL_GIC_VEC_PRIO_55,
    HAL_GIC_VEC_PRIO_56,
    HAL_GIC_VEC_PRIO_57,
    HAL_GIC_VEC_PRIO_58,
    HAL_GIC_VEC_PRIO_59,
    HAL_GIC_VEC_PRIO_60,
    HAL_GIC_VEC_PRIO_61,
    HAL_GIC_VEC_PRIO_62,
    HAL_GIC_VEC_PRIO_63,    /**< Priority level 63 (EIC only) */
} hal_gic_vec_prio_enum;



#define HAL_MEM_CACHE_BYTES_TO_LINES(num_bytes) ((num_bytes) / HAL_MEM_NUMBYTES_CACHE_LINE + \
                                                (((num_bytes) % HAL_MEM_NUMBYTES_CACHE_LINE) != 0 ? 1 : 0))

/*
* Converts bytes to cache lines but takes into consideration the start address and potential
* need to account for cache line overlaps.
*/
#define HAL_CACHE_UNALIGNED_BYTES_TO_LINES(addr, num_bytes)  \
    (((num_bytes + (addr & 0x1F)) % HAL_MEM_NUMBYTES_CACHE_LINE) != 0 ? \
     ((num_bytes + (addr & 0x1F) + 32) / HAL_MEM_NUMBYTES_CACHE_LINE) : \
     ((num_bytes + (addr & 0x1F)) / HAL_MEM_NUMBYTES_CACHE_LINE))


#define HAL_MEM_L2_CACHE_BYTES_TO_LINES(num_bytes) ((num_bytes) / HAL_MEM_NUMBYTES_L2_CACHE_LINE + \
                                                (((num_bytes) % HAL_MEM_NUMBYTES_L2_CACHE_LINE) != 0 ? 1 : 0))


/* This rounds the size of a structure up to a round multiple of cache lines */
#define HAL_MEM_CACHE_LINE_ROUNDED_SIZEOF(x)  \
         (((sizeof(x)) + HAL_MEM_NUMBYTES_CACHE_LINE - 1) & HAL_MEM_CACHE_LINE_MASK)

/* Rounds a 32bit address up to the nearest cache-aligned value. */
#define HAL_MEM_CACHE_ALIGN_ROUNDUP(x)  \
         (((x) + HAL_MEM_NUMBYTES_CACHE_LINE - 1) & HAL_MEM_CACHE_LINE_MASK)


/**
* @brief
*   Macro for saving/disabling interrupts in spinlock asmleaf's
*
* @param[in]
*   tmp - temporary register
*
* @note
*   If we are a TC that is exempt from interrupt processing (IXMT bit
*   set), do not disable interrupts.  Specifically used for non-Threadx
*   TC's.  Interrupt disabling is VPE-wide, and ThreadX assumes a single TC
*   instance bound to a VPE has exclusive use of the interrupt mask register
*   Thus non-ThreadX TC's must not touch this mask to avoid creating deadlock
*   issues.
*/
#define CPUHAL_SAVE_DISABLE_INTS(tmp)                                   \
    mfc0    tmp, C0_TCStatus ; /*Fetch irq mask from TCStatus reg*/     \
    ext     tmp, tmp, 10, 1  ; /*If IXMT set, don't disable ints */     \
    bnez    tmp, 1f          ;                                          \
    nop                      ;                                          \
    di      tmp              ; /* Disable interrupts, store orig SR*/   \
    ehb                      ; /* barrier for irq disable */            \
1:                           ;

/**
* @brief
*   Macro for restoring/enabling interrupts in spinlock asmleaf's
*   Write lock tocken back in branch delay slot.
*
* @param[in]
*   lock_ptr - register with address where a zero will be written in
*       order to release a lock. If this macro is not used in the context
*       of a spinlock, simply pass in a dummy address where a zero
*       can be written
*   irq_msk - register with the interrupt enable mask (CP0 SRstatus
*       register) to be restored when interrupts are enabled
*   tmp- temporary register
*
* @note
*   If we are a TC that is exempt from interrupt processing (IXMT bit
*   set), do not re-enable.  See CPUHAL_DISABLE_INTS above.
*/
#define CPUHAL_RESTORE_ENABLE_INTS(lock_ptr, irq_msk, tmp)              \
    mfc0    tmp, C0_TCStatus;  /*Fetch irq mask from TCStatus reg */    \
    ext     tmp, tmp, 10, 1 ;  /*fetch IXMT bit */                      \
    bnez    tmp, 1f         ;                                           \
    sw      $0, 0(lock_ptr) ; /* release spinlock */                    \
    mtc0    irq_msk, $12    ; /* Restore irq msk in SR */               \
1:                          ;                                           \
    ehb                     ; /* barrier for irq enable */

/**
* @brief
*   Define L2 cache ops depending on whether L2 cache is enabled or not
*/
#if (CPUHAL_L2_CACHE_ENABLED == 1)
#define HAL_L2_CACHE_OP_WB_LINE(addr)           cache 0x1B, 0(addr)
#define HAL_L2_CACHE_OP_INV_LINE(addr)          cache 0x13, 0(addr)
#define HAL_L2_CACHE_OP_WB_INV_LINE(addr)       cache 0x17, 0(addr)
#define HAL_L2_CACHE_OP_LOCK_LINE(addr)         cache CPUHAL_SCACHE_FETCH_LOCK, 0(addr)
#else
#define HAL_L2_CACHE_OP_WB_LINE(addr)
#define HAL_L2_CACHE_OP_INV_LINE(addr)
#define HAL_L2_CACHE_OP_WB_INV_LINE(addr)
#define HAL_L2_CACHE_OP_LOCK_LINE(addr)
#endif

/**
* @brief
*   create a cache line w/o a fetch, mark it as dirty
*
* @param[in] addr   - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_ASM_CREATE_LINE(void *addr)
{
%reg addr
    /*
    ** prefetch code 30 is PrepareForStore; creates a cache line and marks it
    ** as dirty
    */
    pref 30, 0(addr)
%nearsp
/* %nearspel */
    /* Same as above, but load the address off the stack first */
    lw $1, %SPOFF(addr)($sp)
    pref 30, 0($1)
%error
} /* HAL_CACHE_ASM_CREATE_LINE() */
/**
* @brief
*   prefetch a cacheline as "retained" (less chance of eviction)
*
*
* @param[in] addr    - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_ASM_CREATE_LINE_RETAINED(void *addr)
{
%reg addr
    pref 6, 0(addr)
%mem addr
    lw $1, addr
    pref 6, 0($1)
%error
} /* HAL_MEM_ASM_CACHE_LINE_PREF_LOAD_RETAINED() */

#define HAL_MEM_ASM_CACHE_LINE_PREF_LOAD_RETAINED HAL_CACHE_ASM_CREATE_LINE_RETAINED

__asmleaf void HAL_CACHE_ASM_ZERO_LINE(void *addr)
{
%reg addr
    /*
    ** fast write of a cacheline to all-zeros.  10x faster than memset().
    */
        sw $0, 0(addr)
        sw $0, 4(addr)
        sw $0, 8(addr)
        sw $0, 12(addr)
        sw $0, 16(addr)
        sw $0, 20(addr)
        sw $0, 24(addr)
        sw $0, 28(addr)
%error
} /* HAL_CACHE_ASM_CREATE_LINE() */

/**
* @brief
*   loads tag and data from a cache line
*
* @param[in]  addr    - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_ASM_INDEX_LOAD_TAG_DATA(void *addr)
{
%reg addr
    /*
    ** Read the tag for the cache line at the specified
    ** index into the TagLo0 Coprocessor 0 register.
    ** Read the data corresponding to the word
    ** index into the DataLo1 register.
    ** Data array parity bits are also read into the
    ** ErrCtl register.
    ** operate on d-cache.
    ** 001 01
    */
    cache 0x05, 0(addr)
%error
} /* HAL_CACHE_ASM_INDEX_LOAD_TAG_DATA() */

/**
* @brief
*   stores tag in a cache line
*
* @param[in] addr    - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_ASM_INDEX_STORE_TAG(void *addr)
{
%reg addr
    /*
    ** Stores the tag from the cache line and store tag
    ** in TagLo0.
    ** operate on d-cache
    ** 010 01
    */
    cache 0x09, 0(addr)
%error
} /* HAL_CACHE_ASM_INDEX_STORE_TAG() */

/**
* @brief
*   invalidates L1 and create a zero init cache line
*   w/o a fetch or writeback, mark it as dirty
*
* @param[in]  addr    - The address for the cache operation
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf void HAL_CACHE_L1_ASM_INV_CREATE_LINE(void *addr)
{
%reg addr

    /*
    ** cache op 4 is hit-invalidate - on a cache hit, it invalidates
    ** the cache line and but does not write it back to memory even
    ** if it is dirty, operate on d-cache
    ** 100 01
    */
    cache 0x11, 0(addr)

    /*
    ** prefetch code 30 is PrepareForStore; creates a cache line and marks it
    ** as dirty; if cache miss occurs, a zero filled cache line is created
    */
    pref 30, 0(addr)
%nearsp
/* %nearspel */
    /* Same as above, but load the address off the stack first */
    lw $2, %SPOFF(addr)($sp)
    cache 0x11, 0($2)
    pref 30, 0($2)
%error
} /* HAL_CACHE_L1_ASM_INV_CREATE_LINE() */

/**
* @brief
*   writeback a L1 cache line
*
* @param[in] addr    - The address for the cache operation
*
* @note
*       NOTE: This macro does NOT include the sync necessary to maintain ordering and
*       coherency.  It should normally not called directly; instead the
*       hal_cache functions should be called. This macro should only be called
*       directly in performance-tuned code, in which case care must be taken to
*       directly issue the appropriate syncs.
*/
__asmleaf void HAL_CACHE_L1_ASM_WB_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 6 is hit-writeback - on a cache hit, it does not invalidate
    ** the cache line but writes it back to memory, operates on d-cache
    ** 110 01
    */
    cache 0x19, 0(addr)
%mem addr
    /*
    ** cache op 6 is hit-writeback - on a cache hit, it does not invalidate
    ** the cache line but writes it back to memory, operates on d-cache
    ** 110 01
    */
    lw $1, addr
    cache 0x19, 0($1)

%error
} /* HAL_CACHE_L1_ASM_WB_LINE() */

/**
* @brief
*   invalidate a L1 cache line
*
* @param[in]  addr   - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_L1_ASM_INV_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 4 is hit-invalidate - on a cache hit, it invalidates
    ** the cache line and but does not write it back to memory even
    ** if it is dirty, operate on d-cache
    ** 100 01
    */
    cache 0x11, 0(addr)
%mem addr
    /*
    ** cache op 4 is hit-invalidate - on a cache hit, it invalidates
    ** the cache line and but does not write it back to memory even
    ** if it is dirty, operate on d-cache
    ** 100 01
    */
    lw $1, addr
    cache 0x11, 0($1)
%error
} /* HAL_CACHE_L1_ASM_INV_LINE() */

/**
* @brief
*   writeback/invalidate a L1 cache line
*
* @param[in]  addr   - The address for the cache operation
*
* @brief
*   To guarantee ordering, perform a cache_wb followed by a cache_inv. The
*   cache_wb_inv instruction DOES NOT guarantee ordering.
*
* @note
*       This macro does NOT include the sync necessary to maintain ordering and
*       coherency.  It should normally not called directly; instead the
*       hal_cache functions should be called. This macro should only be called
*       directly in performance-tuned code, in which case care must be taken to
*       directly issue the appropriate syncs.
*
*/
__asmleaf void HAL_CACHE_L1_ASM_WB_INV_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 6 is hit-writeback - on a cache hit, it does not invalidate
    ** the cache line but writes it back to memory, operates on d-cache
    ** 110 01
    */
    cache 0x19, 0(addr)

    /*
    ** cache op 4 is hit-invalidate - on a cache hit, it invalidates
    ** the cache line, operates on d-cache
    ** 100 01
    */
    cache 0x11, 0(addr)
%error
} /* HAL_CACHE_L1_ASM_WB_INV_LINE() */

/**
* @brief
*   fetch and lock a L1 cache line
*
* @param[in] addr    - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_L1_ASM_FETCH_LOCK_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 7 is fetch & lock - on a cache hit, it locks the
    ** cache line; otherwise, it fetches from memory and lock the
    ** cache line.
    */
    cache CPUHAL_DCACHE_FETCH_LOCK, 0(addr)
%mem addr
        /*
        ** cache op 7 is fetch & lock - on a cache hit, it locks the
        ** cache line; otherwise, it fetches from memory and lock the
        ** cache line.
        */
    lw $1, addr
    cache CPUHAL_DCACHE_FETCH_LOCK, 0($1)
%error
} /* HAL_CACHE_L1_ASM_FETCH_LOCK_LINE() */

/**
* @brief
*   Index load tag on a dcache line.  Read the tag for a cache line.
*
* @param[in] idx - cache index
*                  15:6 cache line index
*                  5:0  byte offset into 32-byte cache line
*
*/
__asmleaf void HAL_DCACHE_L1_ASM_INDEX_LOAD_TAG(void *idx)
{
%reg idx
    /*
    ** Read tag for cache line @ specificed index into
    ** DTagLo Register
    **
    ** Read the data corresponding to the word index into
    ** DDataLo register
    **
    ** Operate on L1 dcache.
    ** 001 01
    */
    cache 0x05, 0(idx)
    sync  2
%error
} /* HAL_DCACHE_L1_ASM_INDEX_LOAD_TAG() */

/**
* @brief
*   Index store tag on a dcache line.  Write the tag for a cache line.
*
* @param[in] idx - cache index
*                  15:6 cache line index
*                  5:0  byte offset into 32-byte cache line
*
*/
__asmleaf void HAL_DCACHE_L1_ASM_INDEX_STORE_TAG(void *idx)
{
%reg idx
    /*
    ** Write tag for cache line & DTagLo register
    **
    ** If ErrCtlPO set, parity/ECC bits from DTagLo written
    **
    ** operate on L1 dcache
    ** 010 01
    */
    cache 0x09, 0(idx)
    sync  2
%error
} /* HAL_DCACHE_L1_ASM_INDEX_STORE_TAG() */


/**
* @brief
*   Index load tag on a icache line.  Read the tag for a cache line.
*
* @param[in] idx - cache index
*                  15:6 cache line index
*                  5:0  byte offset into 32-byte cache line
*
*/
__asmleaf void HAL_ICACHE_L1_ASM_INDEX_LOAD_TAG(void *idx)
{
%reg idx
    /*
    ** Read tag for cache line at the specified index into
    ** ITagLo Register
    **
    ** Read the data corresponding to the dword index into
    ** the IDataLo and IDataHi registers
    **
    **  If parity is implemented, read the parity bits
    **  corresponding to the data into ErrCtlPI field
    **
    ** Operate on L1 icache.
    ** 001 00
    */
    cache 0x04, 0(idx)
    sync  2
%error
} /* HAL_ICACHE_L1_ASM_INDEX_LOAD_TAG() */

/**
* @brief
*   Index store tag on a icache line.  Write the tag for a cache line.
*
* @param[in] idx - cache index
*                  15:6 cache line index
*                  5:0  byte offset into 32-byte cache line
*
*/
__asmleaf void HAL_ICACHE_L1_ASM_INDEX_STORE_TAG(void *idx)
{
%reg idx
    /*
    ** Write tag for cache line & ITagLo register
    ** For test purposes, the parity bits from ITagLo
    ** will be used if ErrCtlPO is set.
    **
    ** Parity value is automatically calculated
    **
    ** operate on L1 icache
    ** 010 00
    */
    cache 0x08, 0(idx)
    sync  2
%error
} /* HAL_ICACHE_L1_ASM_INDEX_STORE_TAG() */

/**
* @brief
*   Index store data on a dcache line.  Write the data for a cache line.
*
* @param[in] idx - cache index
*                  15:6 cache line index
*                  5:0  byte offset into 32-byte cache line
*
*/
__asmleaf void HAL_DCACHE_L1_ASM_INDEX_STORE_DATA(void *idx)
{
%reg idx
    /*
    ** Writes the DDataLo register to cache
    **
    ** operate on L1 dcache
    ** 011 01
    */
    cache 0x0D, 0(idx)
    sync  2
%mem idx
    lw $1, idx
    cache 0x0D, 0($1)
    sync  2
%error
} /* HAL_DCACHE_L1_ASM_INDEX_STORE_TAG() */

/**
* @brief
*   Index store data on an icache line.  Write the data for a cache line.
*
* @param[in] idx - cache index
*                  15:6 cache line index
*                  5:0  byte offset into 32-byte cache line
*
*/
__asmleaf void HAL_ICACHE_L1_ASM_INDEX_STORE_DATA(void *idx)
{
%reg idx
    /*
    ** Writes the IDataHi and IDataLo register to the way
    ** and dword index specified
    **
    ** If ErrCtlPO set, ErrCtlPI is used for the parity
    ** value. Otherwise, the parity value is calculated
    ** for the write data.
    **
    ** operate on L1 dcache
    ** 011 01
    */
    cache 0x0C, 0(idx)
    sync  2
%error
} /* HAL_ICACHE_L1_ASM_INDEX_STORE_DATA() */


/**
* @brief
*   Index load tag on an L2 line.  Read the tag for a cache line.
*
* @param[in] idx - cache index
*
*/
__asmleaf void HAL_CACHE_L2_ASM_INDEX_LOAD_TAG(void *idx)
{
%reg idx
    /*
    ** Read the tag for the cache line at the specified
    ** index into the L23TagLo Coprocessor 0 register.
    ** Read the data corresponding to the word
    ** index into the L23DataLo, L23DataHi register.
    **
    ** If (WST = 1) L2 Way select operation is performed
    ** Read the ws ram for the cache line at the specified
    ** index into L23TagLo Coprocessor 0 register
    **
    ** Read the tag for the cache line at the specified
    ** index into the TagLo2 Coprocessor 0 register.
    **
    ** Read the data corresponding to the dword index into
    ** the L23DataLo and L23DataHi registers
    **
    ** Operate on L2 (secondary) cache.
    ** 001 11
    */
    cache 0x07, 0(idx)
    sync  2
%error
} /* HAL_CACHE_L2_ASM_INDEX_LOAD_TAG() */

/**
* @brief
*   Index store tag a L2 cache line.  Write the tag for a cache line.
*
* @param[in] idx - cache index
*
*/
__asmleaf void HAL_CACHE_L2_ASM_INDEX_STORE_TAG(void *idx)
{
%reg idx
    /*
    ** ECC generated by HW placed in L2 Data Array along with data
    ** from L23DataLo, L23DataHi
    **
    ** (ErrCtl PO=0): Parity generated by HW placed in L2 WS Array
    ** along with data from L23TagLo register
    **
    ** Write tag for cache line & L23TagLo register
    ** For test purposes, the parity bits from L23TagLo
    ** will be used if ErrCtlPO is set.
    **
    ** Parity value is automatically calculated
    **
    ** operate on L2 cache
    ** 010 11
    */
    cache 0x0B, 0(idx)
    sync  2
%error
} /* HAL_CACHE_L2_ASM_INDEX_STORE_TAG() */

/**
* @brief
*   Index store data on an L2 cache line.  Write the data for a cache line.
*
* @param[in] idx - cache index
*
*/
__asmleaf void HAL_CACHE_L2_ASM_INDEX_STORE_DATA(void *idx)
{
%reg idx
    /*
    ** If WST=1, writes the L23DataLo register to the ECC bits at the way
    ** and dword index specified.
    ** If WST=0, writes the L23DataLo register contents, ECC bits are
    ** generated by the hardware (if present)
    **
    ** operate on L2 cache
    ** 011 11
    */
    cache 0x0F, 0(idx)
    sync  2
%error
} /* HAL_CACHE_L2_ASM_INDEX_STORE_ECC() */

/**
* @brief
*   fetch and lock a L2 cache line
*
*@param[in] addr    - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_L2_ASM_FETCH_LOCK_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 7 is fetch & lock - on a cache hit, it locks the
    ** cache line; otherwise, it fetches from memory and lock the
    ** cache line.
    */
    cache CPUHAL_SCACHE_FETCH_LOCK, 0(addr)
%error
} /* HAL_CACHE_L2_ASM_FETCH_LOCK_LINE() */


/**
* @brief
*   prefetch a cacheline for normal read use
*
* @param[in]  addr    - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_ASM_PREF_LINE(void *addr)
{
%reg addr
    pref 0, 0(addr)
%nearsp
/* %nearspel */
    /* Same as above, but load the address off the stack first */
    lw $2, %SPOFF(addr)($sp)
    pref 0, 0($2)
%error
} /* HAL_CACHE_ASM_PREF_LINE() */

/**
* @brief
*   invalidates L1 & L2 and create a zero init cache line
*   w/o a fetch or writeback, mark it as dirty
*
* @param[in]  addr     - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_L1_L2_ASM_INV_CREATE_LINE(void *addr)
{
%reg addr

    /*
    ** cache op 4 is hit-invalidate - on a cache hit, it invalidates
    ** the cache line and but does not write it back to memory even
    ** if it is dirty, operate on d-cache
    ** 100 01
    */
    cache 0x11, 0(addr)

    /* invalidate L2 cache */
    HAL_L2_CACHE_OP_INV_LINE(addr)

    /*
    ** prefetch code 30 is PrepareForStore; creates a cache line and marks it
    ** as dirty; if cache miss occurs, a zero filled cache line is created
    */
    pref 30, 0(addr)
%nearsp
/* %nearspel */
    /* Same as above, but load the address off the stack first */
    lw $2, %SPOFF(addr)($sp)
    cache 0x11, 0($2)

    HAL_L2_CACHE_OP_INV_LINE($2)
    pref 30, 0($2)
%error
} /* HAL_CACHE_L1_L2_ASM_INV_CREATE_LINE() */

/**
* @brief
*   writeback a L1 and L2 cache line
*
* @brief
*   Note:
*   This ASM macro does not include a SYNC(3).  It is HIGHLY recommended that
*   users call hal_cache_L1_L2_wb_lines or hal_cache_L1_L2_wb_bytes
*   instead so that the SYNC(3) is handled.
*
* @param[in]  addr     - The address for the cache operation
*
* @note
*       This macro does NOT include the sync necessary to maintain ordering and
*       coherency.  It should normally not called directly; instead the
*       hal_cache functions should be called. This macro should only be called
*       directly in performance-tuned code, in which case care must be taken to
*       directly issue the appropriate syncs.

*/
__asmleaf void HAL_CACHE_L1_L2_ASM_WB_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 6 is hit-writeback - on a cache hit, it does not invalidate
    ** the cache line but writes it back to memory, operates on d-cache
    ** 110 01
    */
    cache 0x19, 0(addr)
    HAL_L2_CACHE_OP_WB_LINE(addr)

%error
} /* HAL_CACHE_L1_L2_ASM_WB_LINE() */



/**
* @brief
*   invalidate a L1 and L2 cache line
*
* @param[in]  addr     - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_L1_L2_ASM_INV_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 4 is hit-invalidate - on a cache hit, it invalidates
    ** the cache line and but does not write it back to memory even
    ** if it is dirty, operate on d-cache
    ** 100 01
    */
    cache 0x11, 0(addr)
    HAL_L2_CACHE_OP_INV_LINE(addr)
%error
} /* HAL_CACHE_L1_L2_ASM_INV_LINE() */

/**
* @brief
*   writeback/invalidate a L1 and L2 cache line
*
* @brief
*   Note:
*   This ASM macro does not include a SYNC(3).  It is HIGHLY recommended that
*   users call hal_cache_L1_L2_wb_inv_lines or hal_cache_L1_L2_wb_inv_bytes
*   instead so that the SYNC(3) is handled.
*
* @param[in]  addr    The address for the cache operation
*
* @brief
*   To guarantee ordering, perform a cache_wb followed by a cache_inv. The
*   cache_wb_inv instruction DOES NOT guarantee ordering.
*
* @note
*       This macro does NOT include the sync necessary to maintain ordering and
*       coherency.  It should normally not called directly; instead the
*       hal_cache functions should be called. This macro should only be called
*       directly in performance-tuned code, in which case care must be taken to
*       directly issue the appropriate syncs.
*/
__asmleaf void HAL_CACHE_L1_L2_ASM_WB_INV_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 6 is hit-writeback - on a cache hit, it does not invalidate
    ** the cache line but writes it back to memory, operates on d-cache
    ** 110 01
    */
    cache 0x19, 0(addr)
    /*
    ** cache op 4 is hit-invalidate - on a cache hit, it invalidates
    ** the cache line, operates on d-cache
    ** 100 01
    */
    cache 0x11, 0(addr)
    HAL_L2_CACHE_OP_WB_INV_LINE(addr)

%error
} /* HAL_CACHE_L1_L2_ASM_WB_INV_LINE() */

/**
* @brief
*   fetch and lock a L1 and L2 cache line
*
* @param[in]  addr     - The address for the cache operation
*
*/
__asmleaf void HAL_CACHE_L1_L2_ASM_FETCH_LOCK_LINE(void *addr)
{
%reg addr
    /*
    ** cache op 7 is fetch & lock - on a cache hit, it locks the
    ** cache line; otherwise, it fetches from memory and lock the
    ** cache line.
    */
    cache CPUHAL_DCACHE_FETCH_LOCK, 0(addr)
    HAL_L2_CACHE_OP_LOCK_LINE(addr)
%error
} /* HAL_CACHE_L1_L2_ASM_FETCH_LOCK_LINE() */


/**
* @brief
*   For coherent cache, add a hazard after a 1 or more invalidates to ensure
*   the inavlidates finish before the invalidated cache line is accessed by
*   the 1004k.

*
*/
__asmleaf void HAL_CACHE_COH_INVALIDATE_HAZARD()
{
    /**
     * For coherency, this is required to ensure that all invalidates are
     * completed before we can use the cache line(s) that were invalidated
     * above.  Only one "sync 2" is required to guarantee this.
     *
     * Here is an excerpt from MD00597-1D-CMP-USM:
     *  Globalizing a cache operation changes its timing, compared to a local
     *  operation. The external request must be made, serialized in the
     *  Coherence Manager, and then sent to the cores on the intervention port.
     *  This is not a blocking action, and subsequent instructions on the
     *  requesting CPU will continue to execute. In order to guarantee that the
     *  operation has been completed, a SYNC instruction must be executed prior
     *  to any instruction that requires the updated state. This can be a single
     *  SYNC after a series of cache operations. This SYNC should also be used
     *  on non-coherent cores in the Cluster to ensure maximum compatibility
     *  moving forward.
     */
    sync 2
} /* HAL_CACHE_COH_INVALIDATE_HAZARD() */

/**
* @brief
*   Reads a coprocessor register to get the number of sets in
*   the L2 cache. The size of the cache is # Sets * Association
*   * cache line size
* @param[in]
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_l2_cache_num_sets_get(void)
{
    mfc0    $3, $16, 2
    ext     $2, $3, 8, 4
    li      $3, 64
    sllv    $2, $3, $2
}

/**
* @brief
*   Reverses the bits of an UINT8
*
* @param[in] arg      - The value to reverse bits
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT8 hal_reverse_u8(UINT8 arg)
{
%reg arg
    bitrev  $2, arg     // Bit reverse the lower 16 bits
    srl     $2, $2, 8   // shift everything right 8 bits
%error
}

/**
* @brief
*   Reverses the bits of an UINT16
*
* @param[in] arg      The value to reverse bits
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT16 hal_reverse_u16(UINT16 arg)
{
%reg arg
    bitrev  $2, arg     // Bit reverse the lower 16 bits
%error
}

/**
* @brief
*   Reverses the bits of an UINT32
*
* @param[in] arg      - The value to reverse bits
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_reverse_u32(UINT32 arg)
{
%reg arg
    bitrev  $2, arg     // Bit reverse the lower 16 bits (zero upper bits)
    srl     $1, arg, 16 // set $1 to contain original upper bits
    sll     $2, $2, 16  // The bit reversed lower bits, now get moved to upper
    bitrev  $1, $1      // bit reverse the original lower bits
    or      $2, $1, $2  // or the 2 halves together
%error
}

/**
* @brief
*   Counts how many 0's are in the most significant bits, before a 1 is found
*
* @param[in] arg      - The value to examine for leading zeroes
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_count_lead_zeroes(UINT32 arg)
{
%reg arg
    clz     $2, arg
%error
}

/**
* @brief
*   Counts how many 1's are in the most significant bits, before a 0 is found
*
* @param[in] arg      The value to examine for leading ones
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_count_lead_ones(UINT32 arg)
{
%reg arg
    clo     $2, arg
%error
}

/**
* @brief
*   SYNC(0).  Specified memory instructions (loads or stores or both) that
*   occur before the SYNC(0) must be completed before the specified memory
*   instructions after the SYNC(0) are allowed to start.
*
* @brief
*   SYNC(0) is a heavyweight SYNC.  If in doubt of which SYNC(0), SYNC(2),
*   SYNC(3) to use, you can use SYNC(0).  Note that choosing SYNC(0) may
*   impact the performance that you desire.
*
*
* @note
*     Please refer to MIPS documentation MD00605 for more information.
*
*/
__asmleaf void hal_mem_sync(void)
{
    sync
}

/**
* @brief
*   SYNC(2).  Coherency intervention completion barrier.  This sync is sent
*   to the Coherency Manager.  The CM responds when all older coherent requests
*   have completed their interventions.
*
*   When coherence is enabled, SYNC(2) generates a CoherentSync request. The
*   CoherenceManager will respond to the SYNC(2) when the interventions for
*   all older coherent requests have been completed. If coherence is not
*   enabled, will default to SYNC(0).
*
*   SYNC(2) should be used when you want to ensure that coherent writebacks
*   that occur before SYNC(2) are complete before the specified memory
*   instructions (e.g. UNCACHED writes) after the SYNC(2) are allowed to
*   start.  Coherent writebacks (Config7.NCWB=0) can be either natural
*   cache evictions or writeback cacheops.  Without SYNC(2), coherent
*   writebacks may be out of order relative to uncached accesses.
*
*   If in doubt of which SYNC(0), SYNC(2), SYNC(3) to use, you can use
*   SYNC(0).  Note that choosing SYNC(0) may impact the performance that
*   you desire.
*
*
* @note
*   Please refer to MIPS documentation MD00605 for more information.
*
*/
__asmleaf void hal_mem_sync_2(void)
{
    sync    2
}

/**
* @brief
*   SYNC(3).  Memory sync completion barrier.  This sync is sent to the
*   L2/memory interface to enforce ordering through the system.
*
*   Used to order cached writes with uncached writes.  Any cached writes
*   that occur before the SYNC(3) must be completed before UNCACHED writes
*   after the SYNC(3) are allowed to start.  SYNC(3) guarantees that the
*   UNCACHED write will not pass the cache writeback through MIPS IP on the
*   way to the L2B bridge.  Use a SYNC(3) between an L2 cacheop and an
*   UNCACHED write to ensure the UNCACHED write follows the L2 cacheop.
*
*   When coherence is enabled, SYNC(3) also provides the same functionality
*   as the SYNC(2).  The SYNC(3) generates a CoherentSync request. When
*   interventions for all older coherent requests have completed, SYNC(3)
*   will be sent to memory interface unit. All pending transactions will
*   be sent out. If the next level device (L2 or system) supports legacy
*   SYNC transactions, as indicated by SyncTxEn = 1, an external SYNC
*   request will also be generated. The CM will send a response to the
*   CPU when all prior requests have completed and a SYNC response is
*   received (if it was externalized).
*
*   If in doubt of which SYNC(0), SYNC(2), SYNC(3) to use, you can use
*   SYNC(0).  Note that choosing SYNC(0) may impact the performance that
*   you desire.

*
* @note
*   Please refer to MIPS documentation MD00605 for more information.
*
*/
__asmleaf void hal_mem_sync_3(void)
{
    sync    3
}

/**
* @brief
*   Performs a SYNC_WMB (sync 4).  Older stores are completed before
*   completing younger stores.

*
*
* @note
*   __asmleaf allow the useage of $1, $2 and $3.  Please refer to MIPS
*   documentation MD00605 for more information.
*
*/
__asmleaf void hal_mem_sync_wmb(void)
{
    sync    4
}

/**
* @brief
*   Performs a SYNC_MB (sync 16).  Older load/stores are completed before
*   completing younger load/stores.
*
* @note
*   __asmleaf allow the useage of $1, $2 and $3.  Please refer to MIPS
*   documentat6ion MD00605 for more information.
*
*/
__asmleaf void hal_mem_sync_mb(void)
{
    sync    16
}

/**
* @brief
*   Performs a SYNC_RMB (sync 19).  Older load are completed before
*   completing younger load.
*
*
*
* @note
*   __asmleaf allow the useage of $1, $2 and $3.  Please refer to MIPS
*   documentation MD00605 for more information.
*
*/
__asmleaf void hal_mem_sync_rmb(void)
{
    sync    19
}

/**
* @brief
*   Optimization barrier to the C compiler. Assembly language instructions
*   before the barrier are not mixed (by the C compiler) with assembly
*   language instructions after the barrier. Note that the CPU itself can
*   still mix the execution of instructions. This is stopped by a memory
*   barrier such as hal_mem_sync().
*
*
*
* @note
*   From the MULTI build mips manual: "Asm macros act as an optimization
*   barrier to the compiler, since the compiler is unable to interpret
*   the actions or understand the instructions in the asm macro.
*
*/
asm void hal_optimizer_barrier(void)
{
    nop
}

/**
* @brief
*   Clears execution hazards from writing to co-processor registers
*
*/
__asmleaf void hal_coprocessor_ehb(void)
{
    ehb
}

/**
* @brief
*   Reads a coprocessor register. An instruction hazard barrier may be
*   required. See the MIPS programmers guide for a description of the
*   different co-processor registers
*
* @param[in] reg_num     - Coprocessor register number
* @param[in] sel          - Coprocessor register select
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_coprocessor_read(UINT8 reg_num, UINT8 sel)
{
%con reg_num %con sel
    mfc0    $2, $reg_num, sel
%error
}

/**
* @brief
*   Reads a coprocessor register. An instruction hazard barrier may be
*   required. See the MIPS programmers guide for a description of the
*   different co-processor registers
*
* @param[in] reg_num     - Coprocessor register number
* @param[in] sel         - Coprocessor register select
* @param[in] value       - The value to write to the coprocessor
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf void hal_coprocessor_write(UINT8 reg_num, UINT8 sel, UINT32 value)
{
%con reg_num %con sel %con value
    li      $2, value
    mtc0    $2, $reg_num, sel
%con reg_num %con sel %reg value
    mtc0    value, $reg_num, sel
%error
    ehb
}

/**
* @brief
*  Retrieve cp0 count value.
*
*
* @return
*   Value of CP0 Count register.
*
* @note
*
*/

inline UINT32 hal_cp0_counter_get(void)
{
    return hal_coprocessor_read(9,0);
}


/**
* @brief
*   Set cp0 compare register
*
*
* @return
*   Nothing.
*
* @note
*
*/
inline void hal_cp0_compare_set(UINT32 value)
{
    hal_coprocessor_write(11,0,value);
}

/**
* @brief
*   Sets the targeted thread context for hal_coprocessor_tc_read() or
*   hal_coprocessor_tc_write() API.
*
* @param[in] tc          - Targeted thread context
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf void hal_coprocessor_tc_set(UINT32 tc)
{
%reg tc
    mfc0    $2, $1, 1
    and     $2, $2, 0xffffff00
    or      $2, $2, tc
    mtc0    $2, $1, 1
    ehb
%con tc
    mfc0    $2, $1, 1
    and     $2, $2, 0xffffff00
    ori     $2, $2, tc
    mtc0    $2, $1, 1
    ehb
%error
}

/**
* @brief
*   Reads a GPR register in the targeted thread context.
*
* @param[in] reg_num     - Coprocessor register number
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_gpr_tc_read(UINT8 reg_num)
{
%con reg_num
    mftgpr    $2, $reg_num
%error
}

/**
* @brief
*   Reads a coprocessor register in the targeted thread context.
*   An instruction hazard barrier may be required. See the MIPS
*   programmers guide for a description of the different co-processor
*   registers
*
* @param[in] reg_num     - Coprocessor register number
* @param[in] sel         - Coprocessor register select
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_coprocessor_tc_read(UINT8 reg_num, UINT8 sel)
{
%con reg_num %con sel
    mftc0    $2, $reg_num, sel
%error
}

/**
* @brief
*   Writes a coprocessor register in the targeted thread context.
*   An instruction hazard barrier may be required.  See the MIPS
*   programmers guide for a description of the different co-processor
*   registers
*
* @param[in]  reg_num     - Coprocessor register number
* @param[in]  sel         - Coprocessor register select
* @param[in]  value       - The value to write to the coprocessor
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf void hal_coprocessor_tc_write(UINT8 reg_num, UINT8 sel, UINT32 value)
{
%con reg_num %con sel %con value
    li      $2, value
    mttc0   $2, $reg_num, sel
    ehb
%con reg_num %con sel %reg value
    mttc0   value, $reg_num, sel
    ehb
%error
}


/**
* @brief
*   Disables multiple virtual processor execution
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_disable_mvpe()
{
    dvpe $2
    ehb
}

/**
* @brief
*   Enables multiple virtual processor execution
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_enable_mvpe()
{
    evpe $2
    ehb
}

/**
* @brief
*   Restores multiple processor execution back to a previous state
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf void hal_restore_mvpe(UINT32 state)
{
%reg state
    mtc0    state, C0_MVPControl
    ehb
%mem state
    lw      $2, state
    mtc0    $2, C0_MVPControl
    ehb
%error
}

/**
* @brief
*   Disables multi-threaded execution
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_disable_mtc()
{
    dmt $2
    ehb
}

/**
* @brief
*   Enables multi-threaded execution
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_enable_mtc()
{
    emt $2
    ehb
}

/**
* @brief
*   Restores multi-threaded execution back to a previous state
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf void hal_restore_mtc(UINT32 state)
{
%reg state
    mtc0    state, C0_VPEControl
    ehb
%mem state
    lw      $2, state
    mtc0    $2, C0_VPEControl
    ehb
%error
}


/**
* @brief
*  Fetches current TC ID on the current core.
*
*
*  @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_core_tc_id_get(void)
{
    mfc0    $1, C0_TCBind   //Fetch the TCBind co-processor Register
    ext     $2, $1, 21, 8   //Extract bits 21:28 for the TC ID
}

/**
* @brief
*   Fetches the number of vpe in the specified core.
* @param[in]  core_id - Core id
*
*
* @return
*   Number of vpe in the specified core
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_core_num_of_vpe_get(UINT32 core_id)
{
%reg core_id
    dvpe    $1
    di      $3
    ehb

    rotr    core_id, core_id, 16
    lui     $2, %hi(CPUHAL_GCR_REG_BASE_ADDR)
    sw      core_id, 0x2018($2)
    rotr    core_id, core_id, 16

    lw      $2, 0x4010($2)
    ext     $2, $2, 0, 10
    addiu   $2, $2, 1

    mtc0    $3, CO_Status
    mtc0    $1, C0_MVPControl
    ehb
%error
}

/**
* @brief
*   Fetches the number of tc in the specified core.
*
* @param[in]  core_id - Core id
*
*
* @return
*   Number of tc in the specified core
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3.  When GCR implements
*   PTC in the core control block, we should get it from GCR.
*
*/
__asmleaf UINT32 hal_core_num_of_tc_get(UINT32 core_id)
{
%reg core_id
    mfc0    $2, C0_MVPConf0
    ext     $2, $2, 0, 8
    addiu   $2, $2, 1
%error
}

/**
* @brief
*   Fetches the number core in the system.
*
* @return
*   Number of core in the system
*
*
* @note
*     __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_sys_num_of_core_get(void)
{
    li      $2, CPUHAL_GCR_REG_BASE_ADDR
    lw      $2, 0($2)
    ext     $2, $2, 0, 8
    addiu   $2, $2, 1
}

EXTERN UINT32 hal_sys_num_of_vpe_get(void);

/**
* @brief
*   Fetches the number of tc in the system.
*
*
* @return
*   Number of tc in the system
*
*
* @note
*   None
*
*/
PRIVATE inline UINT32 hal_sys_num_of_tc_get(void)
{
    UINT32  num_of_core;
    UINT32  num_of_tc;
    UINT32  core_id;

    num_of_core = hal_sys_num_of_core_get();
    num_of_tc   = 0;

    for (core_id = 0; core_id < num_of_core; core_id++)
    {
        num_of_tc += hal_core_num_of_tc_get(core_id);
    }

    return num_of_tc;
}

/**
* @brief
*   Fetches current core ID in the system.
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_sys_core_id_get(void)
{
    li      $2, CPUHAL_GCR_REG_BASE_ADDR
    lw      $2, 0x2028($2)
}


/**
* @brief
*  Fetches current TC system ID.  System tc_id is unique across all cores
*
*  @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
PRIVATE inline UINT32 hal_sys_tc_id_get(void)
{
    UINT32 tc_id;
    unsigned core;

    tc_id = 0;
    for (core = 0; core < hal_sys_core_id_get(); core ++)
    {
        tc_id += hal_core_num_of_tc_get(core);
    }
    tc_id += hal_core_tc_id_get();
    return tc_id;
}


/**
* @brief
*   Check if current running TC is running "bare metal", i.e. is not
*   part of the operating system.
*   Assumes that the operating system owns TCs 0:N-1 where N = # of VPEs in a
*   core, and that these TCs are bound to the VPEs in order.  Thus if the
*   current TC number is > current VPE number, this TC resides outside of the OS.
*
* @return
*   0 if TC is owned by OS, else 1
*
 @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_sys_tc_is_bare(void)
{
    mfc0    $1, C0_TCBind   //Fetch the TCBind co-processor Register
    ext     $2, $1, 0, 4    //Extract bits 3:0 for the VPE ID
    ext     $3, $1, 21, 8   // Extract bits 28:21 for the TC id
    subu    $2, $3, $2      // subtract tc_id - vpe_id
    beq     $2, $0, 1f
    nop
    ori     $2, $0, 1       // yup, this is a bare metal TC.
1:
    nop
}

/**
* @brief
*   yield current running TC on a set of yq pins
*
* @param[in]  yqmask - mask of YQ pins on which to yield
*
* @return
*   masked state of YQ pins
*
*/
__asmleaf UINT32 hal_yq_yield_asm(UINT32 yqmask)
{
%reg yqmask
    yield $2, yqmask
    and   $2, $2, yqmask
%error
}


/**
* @brief
*   read current state of YQ pins
*
* @return
*   bitmap of YQ pins.
*
*/
__asmleaf UINT32 hal_yq_read(void)
{
    addiu $1, $0, -2
    yield $2, $1
}


/**
* @brief
*   Fetches current CPU ID (VPE ID) in the system.
*
*
* @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_sys_cpu_id_get(void)
{
    mfc0    $2, $15, 1      //Fetch the EBase co-processor Register
    andi    $2, $2, 0x3FF   //Extract bits 0:9 for the CPUNum
}

/**
* @brief
*  Fetches current gp.
*
*
*  @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf UINT32 hal_gp_register_get(void)
{
    move	$2, $gp
    ehb
}

/**
* @brief
*  Writes to gp.
*
*
*  @note
*   __asmleaf allow the usage of $1, $2 and $3
*
*/
__asmleaf VOID hal_gp_register_set(UINT32 value)
{
%reg value
    move	$gp, value
    ehb
%mem value
    lw      $2, value
    move	$gp, $2
    ehb
%error
}

/**
* @brief
*   Get the core ID given the cpu ID (vpe ID).
* @note
*   - The entire chip is called the "1004K processor".
*   - There are three "cores" on the "1004K processor"
*   - There are two "cpu" in each "core".
*/
PRIVATE inline UINT32 hal_sys_cpu_id_to_core_id(UINT32 cpu_id)
{
    return (cpu_id >> 1);
}


/**
* @brief
*   Fetches the OS managed TC ID from VPE ID
*   Assumes that the operating system owns TCs 0:N-1 where N = # of VPEs in a
*   core, and that these TCs are bound to the VPEs in order.
*
* @return
*   OS managed TC ID
*
* @note
*   None
*
*/
PRIVATE inline UINT32 hal_sys_os_managed_tc_id_get(UINT32 vpe_id)
{
    return ((hal_sys_cpu_id_to_core_id(vpe_id) * hal_core_num_of_vpe_get(hal_sys_cpu_id_to_core_id(vpe_id))) + vpe_id);

}/*hal_sys_os_managed_tc_id_get*/

/**
* @brief
*   Fetches the VPE ID from OS managed TC ID
*   Assumes that
*   1) the operating system owns TCs 0:N-1 where N = # of VPEs in a
*      core, and that these TCs are bound to the VPEs in order.
*   2) TC ID has been validated before calling this function and found to be
*      managed by OS
*
* @return
*   VPE ID
*
* @note
*   None
*
*/
PRIVATE inline UINT32 hal_sys_os_managed_vpe_id_get(UINT32 tc_id)
{
    UINT32 vpe_id;

    for (vpe_id = 0; vpe_id < hal_sys_num_of_vpe_get(); vpe_id++)
    {
        if (hal_sys_os_managed_tc_id_get(vpe_id) == tc_id)
        {
            return vpe_id;
        }

    }
    return 0;
} /* hal_sys_os_managed_vpe_id_get() */


/*---- Function Declarations -----------------------------------------------*/

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
typedef void (*hal_cache_L1_L2_wb_lines_fn_ptr_type)(void *start_ptr, UINT32 num_lines);
EXTERN hal_cache_L1_L2_wb_lines_fn_ptr_type hal_cache_L1_L2_wb_lines_fn_ptr;
#define hal_cache_L1_L2_wb_lines (*hal_cache_L1_L2_wb_lines_fn_ptr)

typedef void (*hal_cache_L1_L2_inv_lines_fn_ptr_type)(void *start_ptr, UINT32 num_lines);
EXTERN hal_cache_L1_L2_inv_lines_fn_ptr_type hal_cache_L1_L2_inv_lines_fn_ptr;
#define hal_cache_L1_L2_inv_lines (*hal_cache_L1_L2_inv_lines_fn_ptr)

typedef void (*hal_cache_L1_L2_wb_bytes_fn_ptr_type)(void *start_ptr, UINT32 num_bytes);
EXTERN hal_cache_L1_L2_wb_bytes_fn_ptr_type hal_cache_L1_L2_wb_bytes_fn_ptr;
#define hal_cache_L1_L2_wb_bytes (*hal_cache_L1_L2_wb_bytes_fn_ptr)

typedef void (*hal_cache_L1_L2_inv_bytes_fn_ptr_type)(void *start_ptr, UINT32 num_bytes);
EXTERN hal_cache_L1_L2_inv_bytes_fn_ptr_type hal_cache_L1_L2_inv_bytes_fn_ptr;
#define hal_cache_L1_L2_inv_bytes (*hal_cache_L1_L2_inv_bytes_fn_ptr)

typedef UINT32 (*hal_cp0_timer_freq_get_fn_ptr_type)(void);
EXTERN hal_cp0_timer_freq_get_fn_ptr_type hal_cp0_timer_freq_get_fn_ptr;
#define hal_cp0_timer_freq_get (*hal_cp0_timer_freq_get_fn_ptr)


/* SMP Support */

/* Register functions */
EXTERN VOID hal_reg_write(UINT32 addr, UINT32 value);
EXTERN UINT32 hal_reg_read(UINT32 addr);

EXTERN VOID hal_exit_smp(const UINT32 is_soft_reset, const UINT32 *done_flag, const UINT32 int_num);

/* Cache functions  */
EXTERN VOID hal_cache_create_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_create_bytes(VOID *start_ptr, UINT32 num_bytes);
EXTERN VOID hal_cache_L1_inv_create_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_wb_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_inv_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_wb_inv_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_prefetch_load_retain_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_fetch_lock_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_wb_bytes(VOID *start_ptr, UINT32 num_bytes);
EXTERN VOID hal_cache_L1_inv_bytes(VOID *start_ptr, UINT32 num_bytes);
EXTERN VOID hal_cache_L1_wb_inv_bytes(VOID *start_ptr, UINT32 num_bytes);
EXTERN VOID hal_cache_L1_L2_wb_inv_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_L2_fetch_lock_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_L2_wb_inv_bytes(VOID *start_ptr, UINT32 num_bytes);
EXTERN VOID hal_cache_L2_fetch_lock_lines(VOID *start_ptr, UINT32 num_lines);
EXTERN VOID hal_cache_L1_wb_inv_all(VOID);
EXTERN VOID hal_cache_L2_wb_inv_all(VOID);
EXTERN VOID hal_cache_L1_flush_inst_line(UINT32 index);
EXTERN VOID hal_cache_L1_invalidate_dline(UINT32 index);
EXTERN VOID hal_cache_L1_flush_dindex(UINT32 index);
EXTERN UINT32 hal_cache_L1_taglo_reg_dcache(UINT32 index);
EXTERN VOID hal_cache_L1_clean_dway(UINT32 way);
EXTERN VOID hal_cache_L1_clean_iway(UINT32 index);

/* Interrupt function */
EXTERN void hal_int_vectors_init(UINT32 addr);

/* MIPS co-processor access functions */
/*
*  Notes about using co-processor registers
* For cop1 sel should always be 0x0
* For cop2 since it is implementation specific, there are a couple of possibilies
*  If the implementation matches cop0, then the cmd can be used as is
*  Otherwise reg can be set to 0x0 and sel can be used for the whole 16 bits to cop2
*/

/*
*  Return true on success, false otherwise.  BOOL was used instead of
* PMCFW_ERROR, as it was written in assembly, which could quickly fall out of
* sync with the error codes
*/
EXTERN BOOL hal_cop_reg_write(UINT8 cop, UINT8 reg, UINT8 sel, UINT32 value);
EXTERN BOOL hal_cop_reg_read(UINT8 cop, UINT8 reg, UINT8 sel, UINT32 *value);
EXTERN BOOL hal_cop_reg_bits_set(UINT8 cop, UINT8 reg, UINT8 sel, UINT32 bit_mask);
EXTERN BOOL hal_cop_reg_bits_clear(UINT8 cop, UINT8 reg, UINT8 sel, UINT32 bit_mask);

/* coherency manager function */
EXTERN void hal_syncctl_set(UINT8 num);
EXTERN void hal_coherence_en(void);
EXTERN void hal_coherence_dis(void);

/* TLB */
EXTERN UINT32 hal_tlb_add_low_level(UINT32 virtual_addr,
                                    UINT32 physical_addr,
                                    UINT32 page_info,
                                    UINT32 coh_flag);

/* CPC */
EXTERN void hal_cpc_cmd(UINT32 core_id, UINT32 command);
EXTERN UINT32 hal_cpc_state(UINT32 core_id);

/* GIC */
EXTERN void hal_gic_init(void);
EXTERN UINT32 hal_gic_num_of_interrupt(void);
EXTERN UINT32 hal_gic_num_of_vpe(void);
EXTERN void hal_gic_trigger_type_edge_set(UINT32 interrupt_number);
EXTERN void hal_gic_trigger_type_level_set(UINT32 interrupt_number);
EXTERN UINT32 hal_gic_trigger_type_get(UINT32 interrupt_number);
EXTERN void hal_gic_polarity_set(UINT32 interrupt_number,
                                 UINT32 polarity);
EXTERN void hal_gic_dual_edge_set(UINT32 interrupt_number);
EXTERN void hal_gic_interrupt_dest_id_set(UINT32 interrupt_number,
                                          UINT32 dest_id);
EXTERN void hal_gic_interrupt_mapping_set(UINT32 interrupt_number,
                                          hal_gic_int_dest_enum dest_type,
                                          UINT32 dest_id,
                                          UINT32 pin_number);
EXTERN void hal_gic_rmask_set(UINT32 interrupt_number);
EXTERN void hal_gic_smask_set(UINT32 interrupt_number);
EXTERN BOOL hal_gic_mask_get(UINT32 interrupt_number);
EXTERN BOOL hal_gic_pending_get(UINT32 interrupt_number);
EXTERN void hal_gic_edge_int_trigger_set(UINT32 interrupt_number);
EXTERN void hal_gic_edge_int_trigger_clear(UINT32 interrupt_number);
EXTERN void hal_gic_local_interrupt_mapping_set(UINT32 vpe_id,
                                                hal_gic_int_dest_enum  dest_type,
                                                hal_gic_local_int_enum local_int,
                                                UINT32 pin_number);
EXTERN void hal_gic_local_rmask_set(UINT32 vpe_id,
                                    hal_gic_local_int_enum local_int);
EXTERN void hal_gic_local_smask_set(UINT32 vpe_id,
                                    hal_gic_local_int_enum local_int);
EXTERN BOOL hal_gic_local_mask_get(UINT32 vpe_id,
                                   hal_gic_local_int_enum local_int);
EXTERN BOOL hal_gic_local_pending_get(UINT32 vpe_id,
                                      hal_gic_local_int_enum local_int);
EXTERN BOOL hal_gic_local_eic_get(UINT32 vpe_id);
EXTERN void hal_gic_local_eic_set(UINT32 vpe_id, BOOL enable);

/* CM */
EXTERN void hal_cm_region_cfg(UINT32 region_id,
                              UINT32 start_address,
                              UINT32 mask,
                              UINT32 target,
                              UINT32 cca_value,
                              UINT32 cca_en);


EXTERN void hal_cm_attr_only_region_cfg(UINT32 region_id,
                                        UINT32 start_address,
                                        UINT32 mask,
                                        UINT32 cca_value,
                                        UINT32 cca_en);

/* SPRAM */
EXTERN UINT32 hal_spram_cfg(UINT32 start_address);

/* TLB */
EXTERN VOID hal_tlb_entry_add(UINT32 virtual_addr,
                              UINT32 physical_addr,
                              UINT32 page_size_kb,
                              UINT32 num_pages,
                              hal_tlb_cca_enum cca_setting);

/* CP0 Timer */
typedef UINT32 (*hal_cp0_ref_clk_fn_ptr_type)(void);
EXTERN void hal_cp0_timer_init(hal_cp0_ref_clk_fn_ptr_type freq_ptr);
EXTERN void hal_cp0_timer_register(void);
EXTERN void hal_cp0_timer_isr_register(void);
EXTERN UINT32 hal_cp0_timer_us_to_count(UINT32 time_us);
EXTERN void hal_cp0_timer_yield(UINT16 yq_mask, UINT32 delay_cycles);
EXTERN UINT32 hal_yq_yield(UINT32 yqmask);
EXTERN void hal_cp0_timer_compare_set(UINT32 value);
EXTERN VOID hal_cp0_timer_ram_code_ptr_adjust(UINT32 offset);
EXTERN VOID hal_cache_ram_code_ptr_adjust(UINT32 offset);

#endif /* _CPUHAL_H */

/** @} end addtogroup */


