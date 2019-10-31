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
* @ingroup PlatformDriversModulePlatform
* @brief
* CPU Hardware Abstraction Layer provides low-level hardware control functions 
* that can be used without the presence of the operating system. 
* 
* @{ 
* @file 
* @brief
*     This file contains the public hardware abstraction layer (hal)
*     definitions for the mips core to be used by the applications.
* @note 
*/ 

/*---- Compile Options -----------------------------------------------------*/
#ifndef _CPUHAL_API_H
#define _CPUHAL_API_H

/*---- Include Files -------------------------------------------------------*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"

/* Constants */
/** Abstract L1 L2 cache line size */
#define HAL_MEM_NUMBYTES_CACHE_LINE             32

/* Interrupt function */
/**
* @brief
*   Enables all CPU core interrupts (set Status(IE) to 1).
*
* @return
*   Original SR value.
*
*
*/
EXTERN UINT32 hal_int_global_enable(void);

/**
* @brief
*   Disables all CPU core interrupts (set Status(IE) to 1).
*
* @return
*   Original SR value.
*
*
*/
EXTERN UINT32 hal_int_global_disable(void);
/**
* @brief
*   Set Status(IE) status to the IE bit value in the input.
*   This should be used with hal_int_global_disable() to begin
*   and end a critical section.
*
* @param[in] ie_bit_val   Register value where you want to 
*                         restore the IE bit status to.
*
* @return
*   Original SR value.
*
*/
EXTERN UINT32 hal_int_global_restore(UINT32 ie_bit_val);


#endif /* _CPUHAL_API_H */

/** @} end addtogroup */


