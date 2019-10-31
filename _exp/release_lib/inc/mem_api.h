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
* @defgroup MEM MEM: Memory Module
* @brief 
*       This module manages memory sections associated with different types
*       of memory in the system. 
*
*       It provides a common API to allocate from
*       these different memory sections. For each allocation request, it
*       ensures that the minimum byte-alignment requirement of the memory
*       section is satisfied. The caller may also specify other byte-alignments
*       when allocating memory blocks, provided that they satisfy the minimum
*       required byte-alignment.
* 
*       -----------------------------------------------------------------
* 
*       In SMP systems, a coherent manager is in place to synchronize the
*       caches from the multiple cores.  If the memories accessed by
*       different cores fall on the same cache line, it could lead to
*       trashing, which impacts system performance. Therefore, memory
*       addresses returned from this module must be cache line aligned.
* 
*       There is only one instance of the Memory Module and it is shared by
*       all cores in the system. The state of the module is stored in a
*       dynamically allocated control structure that is accessible via a
*       global pointer. Any type of access via this global pointer must
*       be performed within a critical region so as to protect against
*       concurrency. A spinlock is used for this purpose.
* 
*       -----------------------------------------------------------------
* 
*       A normal sequence of starting up and using the MEM module is as follows:
* 
*       - mem_init() - Called at system startup. Responsible for setting up
*                      the free memory section and allocating memory
*                      for the global control structure. Only one call
*                      to this function shall be made by one of the cores
*                      in the system.
* 
*       - mem_add_section() - Called by the same core that called mem_init()
*                             to add memory sections to the MEM module.
* 
*       - MEM_ALLOC() - Called by modules to dynamically allocate memory blocks.
*                       Do not call mem_alloc() or mem_alloc_debug() directly.
* 
*       -----------------------------------------------------------------
* 
*       It must be noted that there is no API provided to free-up memory
*       because memories are only dynamically allocated during system
*       initialization time and held by the modules until a system reset
*       occurs.
* @{
* @file
* @brief
*   Public API for MEM
*
* @note
*       None.
*/ 
#ifndef _MEM_API_H
#define _MEM_API_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_common.h"
#include "pmcfw_err.h"

/*
** Enumerated Types
*/

/*
** Constants
*/
/** Flag to indicate whether to align or not */
#define MEM_ALIGN_DONT_CARE 0

/* By convention type 0 is internal .free_mem section for all platforms */
#define MEM_TYPE_FREE 0

/*
** Macro Definitions
*/

#define MEM_ALLOC(mem_section, mem_size, byte_align) \
    mem_alloc(mem_section, mem_size, byte_align, 0, NULL)

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN void *mem_init(const UINT32 max_sections, const UINT32 free_sect_0_start, const UINT32 free_sect_0_size, const UINT32 free_sect_0_min_align);
EXTERN void mem_add_section(const UINT32 mem_section, const UINT32 sect_start, const UINT32 sect_size, const UINT32 min_alignment);
EXTERN void *mem_alloc(const UINT32 mem_section, const UINT32 mem_size, const UINT32 byte_align, const BOOL lock_in_l2, UINT32 *bytes_wasted_ptr);
EXTERN void mem_info(const UINT32 mem_section, UINT32 *sect_start_ptr, UINT32 *sect_max_size_ptr, UINT32 *sect_used_ptr, UINT32 *sect_wasted_ptr);
EXTERN void mem_reset_section(const UINT32 mem_section, const UINT32 sect_start, const UINT32 sect_size, const UINT32 min_alignment);
EXTERN UINT32 mem_adrs_is_valid(UINT32 adrs);
#endif /* _MEM_API_H */
/** @} end addtogroup */


