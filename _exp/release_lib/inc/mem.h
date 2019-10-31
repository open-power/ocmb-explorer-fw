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
* @addtogroup MEM
* @{
* @file
* @brief 
*       This module manages memory sections associated with different types
*       of memory in the system. 
*
*       Refer to mem_api.h for details of this module.
*
* @note
*       None.
*/ 
#ifndef _MEM_H
#define _MEM_H

/*
** Include Files
*/
#include "mem_api.h"

/*
** To enable the DEBUG MODE, uncomment the following define
*/
//#define MEM_DEBUG

/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Macro Definitions
*/
#if defined(MEM_DEBUG)

#if defined(MEM_ALLOC) /* undef macro, if defined above in mem_api.h */
#undef MEM_ALLOC
#endif

#define MEM_ALLOC(mem_section, mem_size, byte_align) \
    mem_alloc_debug(mem_section, mem_size, byte_align, FALSE, __FILE__)

#define MEM_ALLOC_L2_LOCK(mem_section, mem_size, byte_align)    \
    mem_alloc_debug(mem_section, mem_size, byte_align, TRUE, __FILE__)

#else /* MEM_DEBUG */

#define MEM_ALLOC_L2_LOCK(mem_section, mem_size, byte_align) \
    mem_alloc(mem_section, mem_size, byte_align, 1, NULL)

#endif /* MEM_DEBUG */


/*
** DEBUG MODE
**
** The MEM module provides a feature which tracks the source of each instance
** of memory allocation. It records the filename and line number of the caller
** to MEM_ALLOC, along with the size of the allocated block and the memory
** section from which the block was allocated.
**
** Compiling with the -DMEM_DEBUG flag will activate this feature.
**
** NOTE: Users of MEM module should always call MEM_ALLOC() to allocate
**       memory, and not directly calling mem_alloc_debug() or mem_alloc().
*/
#if defined(MEM_DEBUG)
EXTERN void *mem_alloc_debug(const UINT32 mem_section, const UINT32 mem_size, const UINT32 byte_align, const BOOL lock_in_l2, const CHAR *filename);
EXTERN void mem_dump_debug_info(void);
#endif

#endif /* _MEM_H */


/** @} end addtogroup */


