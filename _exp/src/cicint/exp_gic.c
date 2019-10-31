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
*  DESCRIPTION:
*    This file implements the code to initialize the GIC for Explorer.
*
*  NOTES:
*     None.
*
*******************************************************************************/



/*
** Include Files
*/
#include "exp_gic.h"

#include "pmcfw_types.h"
#include "cpuhal.h"
#include "cicint.h"
#include "app_fw.h"

#include "bc_printf.h"

#define PMC_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*
** Global variables
*/
PRIVATE const cicint_config_struct exp_cicint_cfg[] =
{  /*   INT Signal                           Destination Type      Destination VPE     Priority(1 = lowest)  Trigger Type*/
    CICINT_CFG(FOXHOUND_LANE_0,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 0  */
    CICINT_CFG(FOXHOUND_LANE_1,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 1  */
    CICINT_CFG(FOXHOUND_LANE_2,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 2  */
    CICINT_CFG(FOXHOUND_LANE_3,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 3  */
    CICINT_CFG(FOXHOUND_LANE_4,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 4  */
    CICINT_CFG(FOXHOUND_LANE_5,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 5  */
    CICINT_CFG(FOXHOUND_LANE_6,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 6  */
    CICINT_CFG(FOXHOUND_LANE_7,      HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 7  */
    CICINT_CFG(FOXHOUND_NON_FATAL,   HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 8  */
    CICINT_CFG(FOXHOUND_FATAL,       HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 9  */
    CICINT_CFG(DDR4_PHY,             HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 10 */
    CICINT_CFG(DDR4_PHY_NON_FATAL,   HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 11 */
    CICINT_CFG(DDR4_PHY_FATAL,       HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 12 */
    CICINT_CFG(TOP_DIGITAL_IO,       HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 13 */
    CICINT_CFG(OCMB_IP_INT_0,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 14 */
    CICINT_CFG(OCMB_IP_INT_1,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 15 */
    CICINT_CFG(OCMB_IP_INT_2,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 16 */
    CICINT_CFG(OCMB_IP_INT_3,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 17 */
    CICINT_CFG(OCMB_IP_INT_4,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 18 */
    CICINT_CFG(OCMB_IP_INT_5,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 19 */
    CICINT_CFG(OCMB_IP_INT_6,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 20 */
    CICINT_CFG(OCMB_IP_INT_7,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 21 */
    CICINT_CFG(TWI_0_M,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 22 */
    CICINT_CFG(TWI_0_S,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 23 */
    CICINT_CFG(TWI_1_M,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 24 */
    CICINT_CFG(TWI_1_S,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 25 */
    CICINT_CFG(GPIO_INT_0,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 26 */
    CICINT_CFG(GPIO_INT_1,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 27 */
    CICINT_CFG(GPIO_INT_2,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 28 */
    CICINT_CFG(GPIO_INT_3,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 29 */
    CICINT_CFG(UART_INT_0,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 30 */
    CICINT_CFG(SPI_INT_0,            HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 31 */
    CICINT_CFG(WDT_INT,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 32 */
    CICINT_CFG(GPBC_FATAL_ERROR,     HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 33 */
    CICINT_CFG(GPBC_NON_FATAL_ERROR, HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 34 */
    CICINT_CFG(GPBC_DEBUG,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 35 */
    CICINT_CFG(SYS_DCSU_INT,         HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 36 */
    CICINT_CFG(TOP_FATAL,            HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 37 */
    CICINT_CFG(TOP_NON_FATAL,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 38 */
    CICINT_CFG(FAIL_N,               HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 39 */
    CICINT_CFG(EFUSE_IRQ,            HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 40 */
    CICINT_CFG(EFUSE_ECC_SINGLE_ERR, HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 41 */
    CICINT_CFG(EFUSE_ECC_DOUBLE_ERR, HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 42 */
    CICINT_CFG(SPCS_TS_IRQ,          HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 43 */
    CICINT_CFG(SPCS_VM_IRQ,          HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 44 */
    CICINT_CFG(SPCS_PD_IRQ,          HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 45 */
    CICINT_CFG(SAVE_N,               HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 46 */
    CICINT_CFG(PCSE_IRQ_0,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 47 */
    CICINT_CFG(PCSE_IRQ_1,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 48 */
    CICINT_CFG(MIPS_DOORBELL_XCBI,   HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 49 */
    CICINT_CFG(OCMB_IP_INT_8,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 50 */
    CICINT_CFG(OCMB_IP_INT_9,        HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 51 */
    CICINT_CFG(OCMB_IP_INT_10,       HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 52 */
    CICINT_CFG(OCMB_IP_INT_11,       HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 53 */
    CICINT_CFG(OPSW_CR_I,            HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 54 */
    CICINT_CFG(OPSW_CR_I1,           HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 55 */
    CICINT_CFG(TIMER_0,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 56 */
    CICINT_CFG(TIMER_1,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 57 */
    CICINT_CFG(TIMER_2,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 58 */
    CICINT_CFG(TIMER_3,              HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_1, HAL_GIC_INT_TRIGGER_EDGE ), /* External Pin 59 */
    CICINT_CFG(EXP_INT_RESERVED_0,   HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_0,  HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 60 */
    CICINT_CFG(EXP_INT_RESERVED_1,   HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_0,  HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 61 */
    CICINT_CFG(EXP_INT_RESERVED_2,   HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_0,  HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 62 */
    CICINT_CFG(EXP_INT_RESERVED_3,   HAL_GIC_INT_DEST_VPE_PIN, HAL_GIC_INT_DEST_VPE_0, HAL_GIC_VEC_PRIO_0,  HAL_GIC_INT_TRIGGER_LEVEL ), /* External Pin 63 */
};

EXTERN UINT32 __ghsbegin_image_vec_tlb_ref[];

/****************************************************************************
*
* FUNCTION: exp_gic_init
* __________________________________________________________________________
*
* DESCRIPTION:
*   This function initializes CIC interrupt module.
*
* INPUTS:
*   None.
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   None.
*
* NOTES:
*   None.
*
*****************************************************************************/
PUBLIC void exp_gic_init(void)
{
    cicint_init(exp_cicint_cfg, PMC_ARRAY_SIZE(exp_cicint_cfg), NULL, 0);

    /* Register this core as core 0 */
    cicint_set_core(0);

    /* Set the vector table */
    hal_int_vectors_init((UINT32)__ghsbegin_image_vec_tlb_ref + exp_plat_get_pic_offset());

    /* Initialize YQ masks*/
    exp_init_yq_mask();

    /* Enabled CPU global interrupt bit */
    hal_int_global_enable();
} /* End: exp_gic_init() */

/****************************************************************************
*
* FUNCTION: exp_gic_int_enable
* __________________________________________________________________________
*
* DESCRIPTION:
*   This function enables the given interrupt in the GIC for Explorer.
*
* INPUTS:
*   int_num - the interrupt number to enable
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   None.
*
* NOTES:
*   None.
*
*****************************************************************************/
PUBLIC void exp_gic_int_enable(UINT32 int_num)
{
    hal_gic_smask_set(int_num);

} /* End: exp_gic_int_enable() */


/****************************************************************************
*
* FUNCTION: exp_gic_int_disable
* __________________________________________________________________________
*
* DESCRIPTION:
*   This function disables the given interrupt in the GIC for Explorer.
*
* INPUTS:
*   int_num - the interrupt number to disable
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   None.
*
* NOTES:
*   None.
*
*****************************************************************************/
PUBLIC void exp_gic_int_disable(UINT32 int_num)
{
    hal_gic_rmask_set(int_num);

} /* End: exp_gic_int_disable() */



/****************************************************************************
*
* FUNCTION: exp_gic_int_clear
* __________________________________________________________________________
*
* DESCRIPTION:
*   This function clears the given interrupt in the GIC for Explorer.
*
* INPUTS:
*   int_num - the interrupt number to clear
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   None.
*
* NOTES:
*   None
*
*****************************************************************************/
PUBLIC void exp_gic_int_clear(UINT32 int_num)
{
    hal_gic_edge_int_trigger_clear(int_num);

} /* End: exp_gic_int_clear() */


