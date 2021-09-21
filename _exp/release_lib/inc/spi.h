/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2021 Microchip Technology Inc. All rights reserved. 
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
* @defgroup SPI SPI: Serial Peripheral Interface
* @brief
*    Serial Peripheral Interface
*
*    SPI is a syncronous serial communications 4-wire bus intended for short range
*    communications primarily in embedded systems. SPI is single master,
*    multiple slave.
* @{
* @file
* @brief
*    This file contains definitions and declarations for the SPI module for use
*    by internal applications.
*
* @note
*    The caller is responsible for all locking. Locks should be per-port.
*    Accesses to devices on different SPI ports may occur concurrently, but
*    accesses to devices on different chip selects of the same port must be
*    protected.
*/

#ifndef _SPI_H
#define _SPI_H

/*
* Include Files
*/

#include "spi_plat_cfg.h"
#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
* Constants
*/

/*
* Enumerated Types
*/

/**
* Transfer modes. This controls how many data lines will be used. Command,
* address, and data transfers could use different transfer modes from each
* other depending on the SPI protocol used.
*/
typedef enum
{
    SPI_XFR_MODE_SINGLE,
    SPI_XFR_MODE_DUAL,
    SPI_XFR_MODE_QUAD
} spi_xfr_mode_enum;

/*
* Structures and Unions
*/

/**
* @brief
*   Per-chip-select platform-specific SPI controller configuration. Each chip
*   select of each port must have this configured. If memory-mapped access for
*   the device is disabled, no further elements in the structure are applicable.
*/
typedef struct
{
    BOOL mm_enable;           /**< whether to enable memory-mapped accesses for the device */
    UINT32 mm_addr_base_phys; /**< physical base address for memory-mapped accesses */
    UINT32 mm_addr_base;      /**< Virtual base address (post L2B-translation) for */
    UINT32 mm_addr_size;      /**< size of memory-mapped address region */
} spi_cs_config_struct;

/**
* @brief
*   ECC configuration. Only one ECC window is supported, which should be
*   configured to span the entire memory region.
*/
typedef struct
{
    BOOL enable;           /**< TRUE to enable ECC, FALSE to disable */
    UINT32 addr_base_phys; /**< physical address base for the ECC window */
    UINT32 size;           /**< size of the ECC window */
} spi_ecc_config_struct;

/**
* @brief
*   Per-port platform-specific configuration.
*/
typedef struct
{
    UINT32 bus_rate;                  /**< maximum supported bus rate (MHz) */
    UINT8 num_cs;                     /**< number of chip selects on the port */
    spi_cs_config_struct *cs_config;  /**< array of size num_cs of chip select configurations */
    spi_ecc_config_struct ecc_config; /**< ECC configuration */
} spi_parms_struct;

/*
** Function Prototypes and Pointers to Functions in RAM
**
** To accommodate PIC code executing in SPI flash and non-PIC code executing in
** RAM, the functions in RAM are accessed through pointers. The changes that were made:
**
**      - original function name:     $type func_name($type, $type)
**        changed with prepended '_': $type _func_name($type, $type)
**      - define a typedef for the function pointer: typedef $type (_func_name_fn_ptr)($type, $type)
**      - public function pointers initialized in source code files:
**        PUBLIC _func_name_fn_ptr (*func_name_ptr) =  _func_name;
**      - private function pointers intitialized in source code files:
**        PRIVATE _func_name_fn_ptr (*func_name_ptr) = _func_name;
**      - for public functions new define for original function name in header file:
**        #define func_name (*func_name_ptr)
**      - for private functions new define for original function name in source code file:
**        #define func_name (*func_name_ptr)
*/
typedef BOOL (*spi_initialized_fn_ptr_type)(void);
EXTERN spi_initialized_fn_ptr_type spi_initialized_fn_ptr;
#define spi_initialized (*spi_initialized_fn_ptr)

typedef UINT8 (*spi_num_ports_get_fn_ptr_type)(VOID);
EXTERN spi_num_ports_get_fn_ptr_type spi_num_ports_get_fn_ptr;
#define spi_num_ports_get (*spi_num_ports_get_fn_ptr)

typedef UINT8 (*spi_num_cs_get_fn_ptr_type)(UINT8 port_id);
EXTERN spi_num_cs_get_fn_ptr_type spi_num_cs_get_fn_ptr;
#define spi_num_cs_get (*spi_num_cs_get_fn_ptr)

typedef PMCFW_ERROR (*spi_ctrl_read_hw_assist_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, UINT8 *data_ptr, UINT32 len);
EXTERN spi_ctrl_read_hw_assist_fn_ptr_type spi_ctrl_read_hw_assist_fn_ptr;
#define spi_ctrl_read_hw_assist (*spi_ctrl_read_hw_assist_fn_ptr)

typedef PMCFW_ERROR (*spi_ctrl_read_direct_access_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, UINT8 *data_ptr, UINT32 len);
EXTERN spi_ctrl_read_direct_access_fn_ptr_type spi_ctrl_read_direct_access_fn_ptr;
#define spi_ctrl_read_direct_access (*spi_ctrl_read_direct_access_fn_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_direct_access_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, const UINT8 *data_ptr, UINT32 len);
EXTERN spi_ctrl_write_direct_access_fn_ptr_type spi_ctrl_write_direct_access_fn_ptr;
#define spi_ctrl_write_direct_access (*spi_ctrl_write_direct_access_fn_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_read_direct_access_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, const UINT8 *data_out_ptr, UINT32 len_out, UINT8 *data_in_ptr, UINT32 len_in);
EXTERN spi_ctrl_write_read_direct_access_fn_ptr_type spi_ctrl_write_read_direct_access_fn_ptr;
#define spi_ctrl_write_read_direct_access   (*spi_ctrl_write_read_direct_access_fn_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_write_direct_access_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, const UINT8 *data1_ptr, UINT32 len1, const UINT8 *data2_ptr, UINT32 len2);
EXTERN spi_ctrl_write_write_direct_access_fn_ptr_type spi_ctrl_write_write_direct_access_fn_ptr;
#define spi_ctrl_write_write_direct_access (*spi_ctrl_write_write_direct_access_fn_ptr)

typedef PMCFW_ERROR (*spi_ctrl_read_mem_mapped_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, const UINT8 *src_ptr, UINT8 *dst_ptr, UINT32 len);
EXTERN spi_ctrl_read_mem_mapped_fn_ptr_type spi_ctrl_read_mem_mapped_fn_ptr;
#define spi_ctrl_read_mem_mapped (*spi_ctrl_read_mem_mapped_fn_ptr)

typedef BOOL (*spi_validate_port_cs_fn_ptr_type)(UINT8 port_id, UINT8 cs_id);
EXTERN spi_validate_port_cs_fn_ptr_type spi_validate_port_cs_fn_ptr;
#define spi_validate_port_cs (*spi_validate_port_cs_fn_ptr)

typedef UINT32 (*spi_addr_ecc_log_to_phys_fn_ptr_type)(const UINT8 port_id, const UINT32 addr_log);
EXTERN spi_addr_ecc_log_to_phys_fn_ptr_type spi_addr_ecc_log_to_phys_fn_ptr;
#define spi_addr_ecc_log_to_phys (*spi_addr_ecc_log_to_phys_fn_ptr)

typedef UINT32 (*spi_size_phys_to_log_fn_ptr_type)(const UINT8 port_id, const UINT32 size);
EXTERN spi_size_phys_to_log_fn_ptr_type spi_size_phys_to_log_fn_ptr;
#define spi_size_phys_to_log (*spi_size_phys_to_log_fn_ptr)

typedef void (*spi_cache_invalidate_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, UINT8 *addr_ptr, UINT32 size);
EXTERN spi_cache_invalidate_fn_ptr_type spi_cache_invalidate_fn_ptr;
#define spi_cache_invalidate (*spi_cache_invalidate_fn_ptr)


/*
* Function Prototypes
*/
EXTERN void spi_init(spi_parms_struct *parms);
EXTERN UINT32 spi_size_phys_to_log(const UINT8 port_id, const UINT32 size);
EXTERN BOOL spi_fatal_error_handler(void);
EXTERN VOID spi_fatal_init(void);
EXTERN VOID spi_fatal_cecc_enable(void);
EXTERN VOID spi_dump_debug_info(VOID);
EXTERN UINT32 spi_size_log_to_phys(const UINT8 port_id, const UINT32 size);

#endif /* _SPI_H */

/** @} end addtogroup */


