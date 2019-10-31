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
*  DESCRIPTION :
*    This file contains all the definitions & declarations of the
*    initialization of the GIC for Explorer.
*
*  NOTES:
*    None.
*
*******************************************************************************/


#ifndef _EXP_GIC_H
#define _EXP_GIC_H

/*
** Include Files
*/
#include "pmcfw_types.h"

/*
** Constants
*/

#define FOXHOUND_LANE_0_INT                                     (0)
#define FOXHOUND_LANE_1_INT                                     (1)
#define FOXHOUND_LANE_2_INT                                     (2)
#define FOXHOUND_LANE_3_INT                                     (3)
#define FOXHOUND_LANE_4_INT                                     (4)
#define FOXHOUND_LANE_5_INT                                     (5)
#define FOXHOUND_LANE_6_INT                                     (6)
#define FOXHOUND_LANE_7_INT                                     (7)
#define FOXHOUND_NON_FATAL_INT                                  (8)
#define FOXHOUND_FATAL_INT                                      (9)
#define DDR4_PHY_INT                                            (10)
#define DDR4_PHY_NON_FATAL_INT                                  (11)
#define DDR4_PHY_FATAL_INT                                      (12)
#define TOP_DIGITAL_IO_INT                                      (13)
#define OCMB_IP_INT_0                                           (14)
#define OCMB_IP_INT_1                                           (15)
#define OCMB_IP_INT_2                                           (16)
#define OCMB_IP_INT_3                                           (17)
#define OCMB_IP_INT_4                                           (18)
#define OCMB_IP_INT_5                                           (19)
#define OCMB_IP_INT_6                                           (20)
#define OCMB_IP_INT_7                                           (21)
#define TWI_0_M_INT                                             (22)
#define TWI_0_S_INT                                             (23)
#define TWI_1_M_INT                                             (24)
#define TWI_1_S_INT                                             (25)
#define GPIO_INT_0                                              (26)
#define GPIO_INT_1                                              (27)
#define GPIO_INT_2                                              (28)
#define GPIO_INT_3                                              (29)
#define UART_INT_0                                              (30)
#define SPI_INT_0                                               (31)
#define WDT_INT                                                 (32)
#define GPBC_FATAL_ERROR_INT                                    (33)
#define GPBC_NON_FATAL_ERROR_INT                                (34)
#define GPBC_DEBUG_INT                                          (35)
#define SYS_DCSU_INT                                            (36)
#define TOP_FATAL_INT                                           (37)
#define TOP_NON_FATAL_INT                                       (38)
#define FAIL_N_INT                                              (39)
#define EFUSE_IRQ_INT                                           (40)
#define EFUSE_ECC_SINGLE_ERR_INT                                (41)
#define EFUSE_ECC_DOUBLE_ERR_INT                                (42)
#define SPCS_TS_IRQ_INT                                         (43)
#define SPCS_VM_IRQ_INT                                         (44)
#define SPCS_PD_IRQ_INT                                         (45)
#define SAVE_N_INT                                              (46)
#define PCSE_IRQ_0_INT                                          (47)
#define PCSE_IRQ_1_INT                                          (48)
#define MIPS_DOORBELL_XCBI_INT                                  (49)
#define OCMB_IP_INT_8                                           (50)
#define OCMB_IP_INT_9                                           (51)
#define OCMB_IP_INT_10                                          (52)
#define OCMB_IP_INT_11                                          (53)
#define OPSW_CR_I_INT                                           (54)
#define OPSW_CR_I1_INT                                          (55)
#define TIMER_0_INT                                             (56)
#define TIMER_1_INT                                             (57)
#define TIMER_2_INT                                             (58)
#define TIMER_3_INT                                             (59)
#define EXP_INT_RESERVED_0                                      (60)
#define EXP_INT_RESERVED_1                                      (61)
#define EXP_INT_RESERVED_2                                      (62)
#define EXP_INT_RESERVED_3                                      (63)

#define EXP_NUM_INTERRUPTS                   64

/*
** Macro Definitions
*/

/***************************************************************************
* ASSEMBLY MACRO: exp_init_yq_mask
* __________________________________________________________________________
*
* DESCRIPTION:
*   Initializes the YQ mask
*
*   Note:
*       None
*
* ARGUMENTS:
*       None
*
*
****************************************************************************/
__asmleaf VOID exp_init_yq_mask(void)
{
    /* Setting YQMask bits[15-0] for VPE0 */
    li $9, 0xFFFF
    mtc0    $9, $1, 4
    ehb
}

/*
** Enumerated Types
*/

/*
** Structures and Unions
*/

/*
** Function Prototypes
*/
EXTERN void exp_gic_init(void);
EXTERN void exp_gic_int_enable(UINT32 int_num);
EXTERN void exp_gic_int_disable(UINT32 int_num);
EXTERN void exp_gic_int_clear(UINT32 int_num);

#endif /* _EXP_GIC_H */


