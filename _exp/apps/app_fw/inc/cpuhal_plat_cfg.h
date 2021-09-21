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
*
*   NOTES:
*     Platform specific definitions for cpuhal module.
*
*******************************************************************************/




#ifndef CPUHAL_PLAT_CFG_H_
#define CPUHAL_PLAT_CFG_H_

/* Set to 1 to skip TLB initialization. */
#define CPUHAL_SKIP_TLB_INIT 0

/* Skip cache initialization on sboot. pboot will initialize all L1 i/d cache */
#define CPUHAL_SKIP_L1_CACHE_INIT 0

/* 1: use L2 cache operations, 0: do not use L2 cache operations */
#define CPUHAL_L2_CACHE_ENABLED 0

/* set to 1 to enable CPUHAL CP0 timer support or 0 to disable */
#define CPUHAL_CP0_TIMER_ENABLED  0

/* Define to disable SYSCALL support */
#define CPUHAL_DISABLE_SYSCALL_SUPPORT

/* Set this Flag to 1 if DUAL VPE PIC support is enabled*/
#define CPUHAL_PLAT_PIC_DUAL_VPE_ENABLE        1

/* 
** 1: fill i-cache/d-cache with known values (only useful for cache parity injection test)
** 0: do not fill i-cache/d-cache with known values
*/ 
#define CPUHAL_L1_CACHE_SCRUB   1

/* 
** When enabled, the d-cache initialization in crt0 will also initialize a section of
** physical memory to zero using faster PrepareForStore cahce ops.
** Memory regions is define using the BASE_ADDR and SIZE defines below.
*/ 
#define CPUHAL_CACHED_MEM_INIT_EN           0
#define CPUHAL_CACHED_MEM_INIT_BASE_ADDR    x
#define CPUHAL_CACHED_MEM_INIT_SIZE         x

#endif /* CPUHAL_PLAT_CFG_H_ */


