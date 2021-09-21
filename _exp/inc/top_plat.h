/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2018, 2019, 2020 Microchip Technology Inc. All rights reserved. 
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
* @addtogroup TOP
* @{
* @file
* @brief
*   Platform-specific configuration definitions for TOP.
*
* @note
*/
#ifndef _TOP_PLAT_H
#define _TOP_PLAT_H

/*
** Include Files
*/
#include "top.h"

/*
** Constants
*/

/* 
** Set palladium device ID is the same for ROC and SPC
** Use this override to set the device type for palladium
*/ 
#define TOP_PLAT_PALLADIUM_DEV_TYPE     TOP_DEV_TYPE_ROC

#define TOP_DEV_ID_OVERRIDE_FLAG_VALUE  0x7001F1A6

/*
* Structures and Unions
*/

/*
** This structure is used to store the interrupt, 
** MTC, and VPE status during critical sections 
*/
typedef struct  
{
    UINT32 int_status;     /* used for storing/restoring interrupt status */
    UINT32 mtc_status;     /* used for storing/restoring MTC status */
    UINT32 vpe_status;     /* used for storing/restoring VPE status */
} top_plat_lock_struct;

/*
** Function prototypes
*/

EXTERN PMCFW_ERROR top_device_id_override(UINT32* dev_id);
PUBLIC void top_plat_critical_region_enter(top_plat_lock_struct * lock_struct_ptr);
PUBLIC void top_plat_critical_region_exit(top_plat_lock_struct lock_struct);

#endif /* _TOP_PLAT_H */
/** @} end addtogroup */



