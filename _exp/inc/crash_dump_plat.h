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
*
*   DESCRIPTION :
*     Platform-specific definitions and declarations for crash dump
*
*******************************************************************************/
#ifndef _CRASH_DUMP_PLAT_H
#define _CRASH_DUMP_PLAT_H

/*
** Include Files
*/

#include "crash_dump.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Macro Definitions
*/

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/

EXTERN void crash_dump_plat_init(void);
EXTERN UINT32 crash_dump_plat_remaining_spi_space_get(void);
EXTERN UINT32 crash_dump_plat_data_space_used(void);
EXTERN PMCFW_ERROR crash_dump_plat_data_put(UINT32 buffer_size, void *src_buffer_ptr);
PUBLIC PMCFW_ERROR crash_dump_plat_header_flush(void);
EXTERN PMCFW_ERROR crash_dump_plat_data_flush(void);
EXTERN PMCFW_ERROR crash_dump_plat_data_section_get(crash_dump_header *crash_dump_header_ptr, UINT32 crash_dump_offset, UINT8 *dest_buffer, UINT32 dest_buffer_size);
EXTERN VOID crash_dump_plat_ram_code_ptr_adjust(UINT32 offset);
EXTERN PMCFW_ERROR crash_dump_plat_header_put(crash_dump_header *crash_dump_header_ptr);
EXTERN PMCFW_ERROR crash_dump_plat_header_entry_get(UINT32 header_index, crash_dump_header *crash_dump_header_ptr);
EXTERN UINT32 crash_dump_plat_raw_header_get(UINT8* dest_buffer, UINT32 dest_buffer_size, UINT32 offset);
EXTERN UINT32 crash_dump_plat_raw_data_get(UINT8* dest_buffer, UINT32 dest_buffer_size, UINT32 offset);


#endif /* _CRASH_DUMP_PLAT_H */

