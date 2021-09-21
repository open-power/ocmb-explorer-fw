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
* @defgroup CIC_INT CIC_INT: Central Interrupt Controller
* @brief 
*   This module register ISRs, Enable interrupts and disable interrupts with the CIC
* @{  
* @file 
* @brief 
*   This file contains all the definitions & declarations of the central
*   interrupt controller (CIC) module that are required by external
*   applications.
*
* @note 
*   None
*/ 
#ifndef _CICINT_API_H
#define _CICINT_API_H

/*
* Include Files
*/
#include "pmcfw_types.h"
#include "cpuhal.h"

/*
* Macro Definitions
*/
/**
* @brief
*   This macro maps each row of the configuration table into the
*   CIC module configuration structure for external interrupts.
* 
* @param[in] int_num   - external int number
* @param[in] dest_type - destination type
* @param[in] dest_id   - destination id (vpd id)
* @param[in] prio      - priority level (vector number or yp number)
* @param[in] trigger   - trigger type (level or edge)
* 
* @hideinitializer
*/
#define CICINT_CFG(int_num, dest_type, dest_id, prio, trigger)      \
{                                                                   \
    (dest_type),                                                    \
    (dest_id),                                                      \
    (prio),                                                         \
    (trigger)                                                       \
}
/**
* @brief
*   This macro maps each row of the configuration table into the
*   CIC module configuration structure for local interrupts.
*   The current implementation of local interrupts only supports VPE pins.
* 
* @param[in] int_id    - Local Interrupt ID (hal_gic_local_int_enum)
* @param[in] dest_id   - VPE id Note: Local interrupts are per-VPE.  They cannot be routed
*                        to other VPEs, they are always routed back to the
*                        triggering VPE, or to the triggering core if dest_type == YQ
* @param[in] dest_type - destination type (hal_gic_int_dest_enum) (IRQ/NMI/YQ)
* @param[in] prio      - priority level (vector number or yq number)
*
*@hideinitializer
*/
#define CICINT_LOCAL_CFG(int_id, dest_type, dest_id, prio)          \
{                                                                   \
    (int_id),                                                       \
    (dest_type),                                                    \
    (dest_id),                                                      \
    (prio)                                                          \
}

/*
* Enumerated Types
*/

/*
* Structures and Unions
*/
/** Define CIC interrupt service routine call back function pointer type */
typedef void (*cicint_cback_fcn_ptr)(void *cback_arg);


/**
* @brief 
*   This structure defines the configuration of each interrupt.  All
*   elements are defined to be UINT8 to save memory usage.
*/
typedef struct 
{
    UINT8 dest_type;    /**< destination type (hal_gic_int_dest_enum) */
    UINT8 dest_id;      /**< vpe id */
    UINT8 prio;         /**< vector priority number (hal_gic_vec_prio_enum) or yp number */
    UINT8 trigger;      /**< trigger type (hal_gic_trigger_type_enum) */

} cicint_config_struct;

/**
* @brief 
*   This structure defines the configuration of each interrupt.  All
*   elements are defined to be UINT8 to save memory usage.
*/
typedef struct 
{
    UINT8 int_id;             /**< Local interrupt id (hal_gic_local_int_enum ) */
    UINT8 dest_type;          /**< destination type (hal_gic_int_dest_enum) */
    UINT8 dest_id;            /**< vpe id */
    UINT8 prio;               /**< vector priority number (hal_gic_vec_prio_enum) or yp number */

} cicint_config_local_struct;

/**
* Function Prototypes
*/
EXTERN void cicint_init(const cicint_config_struct             *cfg_array,
                        const UINT32                       cfg_array_size,
                        const cicint_config_local_struct *cfg_local_array,
                        const UINT32                 cfg_local_array_size);

EXTERN void cicint_int_register(UINT32                int_num, 
                                cicint_cback_fcn_ptr  cback_ptr,
                                void                 *cback_arg);

EXTERN void cicint_nmi_register(UINT32                vpe_idx,
                                cicint_cback_fcn_ptr  cback_ptr,
                                void                 *cback_arg);

EXTERN void cicint_int_enable(UINT32  int_num);

EXTERN void cicint_int_disable(UINT32  int_num);

EXTERN void cicint_int_set(UINT32  int_num);

EXTERN void cicint_int_clear(UINT32  int_num);

EXTERN BOOL cicint_int_status_get(UINT32  int_num);

EXTERN BOOL cicint_int_enable_status_get(UINT32  int_num);

EXTERN void cicint_int_vpe_routing_set(UINT32 int_num,
                                       UINT32 vpe_id);

EXTERN void cicint_int_yq_routing_set(UINT32 int_num,
                                      UINT32 vpe_id);

EXTERN void cicint_local_int_register(UINT32                 vpe_id,
                                      hal_gic_local_int_enum  local_int, 
                                      cicint_cback_fcn_ptr   cback_ptr,
                                      void                  *cback_arg);

EXTERN void cicint_local_int_enable(UINT32                 vpe_id,
                                    hal_gic_local_int_enum  local_int);

EXTERN void cicint_local_int_disable(UINT32                 vpe_id,
                                     hal_gic_local_int_enum  local_int);

EXTERN BOOL cicint_local_int_status_get(UINT32                 vpe_id,
                                        hal_gic_local_int_enum  local_int);

EXTERN void cicint_nmi(void);
EXTERN void cicint_set_core(UINT32 core_id);

#endif /* _CICINT_API_H */
/** @} end DEFgroup */


