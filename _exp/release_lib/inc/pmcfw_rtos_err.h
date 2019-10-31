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


/**  COPYRIGHT (C) 2002 PMC-SIERRA, INC. ALL RIGHTS RESERVED.                **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to PMC-Sierra, Inc.                                         **/
/** PMC-Sierra distributes this software to its customers pursuant to the    **/ 
/** terms and conditions of the Device Driver Software License Agreement     **/ 
/** contained in the text file software.lic that is distributed along with   **/ 
/** the device driver software. This software can only be utilized if all    **/ 
/** terms and conditions of the Device Driver Software License Agreement are **/ 
/** accepted. If there are any questions, concerns, or if the Device Driver  **/
/** Software License Agreement text file, software.lic, is missing please    **/
/** contact PMC-Sierra for assistance.                                       **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/
/*******************************************************************************
**  MODULE      :  PMCFW RTOS
**
**  FILE        :  pmcfw_rtos_err.h
**      
**  DESCRIPTION :  This file contains error code definitions.
**
**  NOTES       :
**
*******************************************************************************/
#ifndef _PMCFW_RTOS_ERR_H
#define _PMCFW_RTOS_ERR_H


/******************************************************************************/
/* DEFINES AND TYPEDEFS                                                       */
/******************************************************************************/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "pmcfw_types.h"
#include "pmcfw_util.h"
#include "pmcfw_err.h"


/* PMCFW RTOS Error Codes */
#define PMCFW_RTOS_ERR_TIMER_EXPIRED                    (PMCFW_ERR_BASE_PMCFW_RTOS + 0x000)
#define PMCFW_RTOS_ERR_INVALID_TIMER_HANDLE             (PMCFW_ERR_BASE_PMCFW_RTOS + 0x001)
#define PMCFW_RTOS_ERR_INVALID_TIMER_HANDLE_PTR         (PMCFW_ERR_BASE_PMCFW_RTOS + 0x002)
#define PMCFW_RTOS_ERR_INVALID_TIMER_EVENT_HANDLE       (PMCFW_ERR_BASE_PMCFW_RTOS + 0x003)
#define PMCFW_RTOS_ERR_INVALID_TIMER_EVENT_HANDLE_PTR   (PMCFW_ERR_BASE_PMCFW_RTOS + 0x004)
#define PMCFW_RTOS_ERR_TIMER_EVENT_NOT_FOUND            (PMCFW_ERR_BASE_PMCFW_RTOS + 0x005)

EXTERN sPMCFW_UTIL_NAMED_VALUE PMCFW_RTOS_ErrorArray[];


EXTERN INT32 PMCFW_RTOS_ErrorStringFind( INT32   RetCode,
                                        CHAR    **pErrorString );

#endif /* _PMCFW_RTOS_ERR_H */




