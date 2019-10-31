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
* @defgroup SPI SPI: Serial Peripheral Interface
* @ingroup PlatformDriversModulePlatform
* @brief
*    Serial Peripheral Interface
*
*    SPI is a 4-wire synchronous serial communication bus intended for short range
*    communications primarily in embedded systems. SPI bus can operate with single master
*    and with multiple slave.
* @{
* @file
* @brief
*    This file contains definitions and declarations for the SPI module for use
*    by external applications.
*
* @note
*    The caller is responsible for all locking. Locks should be per-port.
*    Access to devices on different SPI ports may occur concurrently, but
*    access to devices on different chip selects of the same port must be
*    protected.
*/

#ifndef _SPI_API_H
#define _SPI_API_H

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "spi.h"
/*
* Constants
*/

/** Error codes creation Marco of the SPI related errors */
#define SPI_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_SPI) | (err_suffix))
#define SPI_ERR_INITIALIZED      SPI_ERR_CODE_CREATE(0x001) /**< SPI already initialized */
#define SPI_ERR_BAD_PARAM        SPI_ERR_CODE_CREATE(0x002) /**< invalid function parameter value */
#define SPI_ERR_READ_FAIL        SPI_ERR_CODE_CREATE(0x003) /**< SPI read failure */
#define SPI_ERR_WRITE_FAIL       SPI_ERR_CODE_CREATE(0x004) /**< SPI write failure */
#define SPI_ERR_ADDR_OOB         SPI_ERR_CODE_CREATE(0x005) /**< address out of bounds */
#define SPI_ERR_HA_INT_CLEAR     SPI_ERR_CODE_CREATE(0x006) /**< hardware assist interrupts not cleared */
#define SPI_ERR_MM_IDLE          SPI_ERR_CODE_CREATE(0x007) /**< memory map state machine not idle */
#define SPI_ERR_BAD_CTRL_CFG     SPI_ERR_CODE_CREATE(0x008) /**< invalid SPI controller configuration */
#define SPI_ERR_BAD_DEV_CFG      SPI_ERR_CODE_CREATE(0x009) /**< invalid SPI device configuration */
#define SPI_ERR_OPER_SUPPORT     SPI_ERR_CODE_CREATE(0x00A) /**< operation not supported */
#define SPI_ERR_ECC              SPI_ERR_CODE_CREATE(0x00B) /**< ECC error */
#define SPI_ERR_ERASE_FAIL       SPI_ERR_CODE_CREATE(0x00C) /**< SPI erase failure */

/*
* Enumerated Types
*/

/**
* SPI protocol modes. The various modes define which transfer mode the command,
* address, and memory data transfers will use.
*/
typedef enum
{
    SPI_PROTO_UNINITIALIZED, /**< protocol not initialized */
    SPI_PROTO_SINGLE,      /**< single command, address, and memory data */
    SPI_PROTO_QUAD_D,      /**< single command and address; quad memory data */
    SPI_PROTO_QUAD_AD      /**< single command, quad address and memory data */
} spi_proto_enum;

/**
* SPI action protocol modes. The controller can be configured in
* different protocol modes for reads and writes.
*/
typedef enum
{
    SPI_ACTION_PROTO_READ  = 0,
    SPI_ACTION_PROTO_WRITE = 1,
    SPI_ACTION_PROTO_MAX   = 1
} spi_action_proto_enum;

/*
* Structures and Unions
*/

/**
* @brief
*   Configuration information for an SPI device.
* @note
*   Not all fields apply to all device types. Fields that do not apply to a
*   device must be set to 0.
*
*/
typedef struct
{
    UINT8 opcode_read_single;             /**< opcode for single-mode read */
    UINT8 opcode_write_single;            /**< opcode for single-mode write */
    UINT8 opcode_read_quad;               /**< opcode for quad-mode read */
    UINT8 opcode_write_quad;              /**< opcode for quad-mode write */
    UINT8 addr_bytes;                     /**< number of address bytes */
    UINT8 dummy_cycles;                   /**< number of dummy cycles to use */
    BOOL xip_enable;                      /**< whether to use XIP (execute-in-place) mode */
    UINT32 mem_size;                      /**< physical device memory size (bytes) */
    UINT32 bus_rate;                      /**< bus rate (MHz) */
    UINT32 bus_rate_single_max;           /**< maximum bus rate allowable for single mode */
    spi_proto_enum proto_read;            /**< SPI protocol to use for reads */
    spi_proto_enum proto_write;           /**< SPI protocol to use for writes */
    UINT32 ceb_deassert_assert_period_us; /**<
                                          * time to wait between chip select de-assertion and
                                          *  re-assertion (hardware-assist mode)
                                          */
    UINT32 cs_wait;                       /**< MM_CFG_3 register's SPI_CS_WAIT value */
    UINT32 page_size;                     /**< physical page size (bytes) */
} spi_dev_struct;

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
typedef void (*spi_ctrl_init_fn_ptr)(UINT8 port_id, UINT8 cs_id, const spi_dev_struct * const dev_ptr);
EXTERN spi_ctrl_init_fn_ptr spi_ctrl_init_ptr;
#define spi_ctrl_init (*spi_ctrl_init_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_hw_assist_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *data_ptr, UINT32 len);
EXTERN spi_ctrl_write_hw_assist_fn_ptr spi_ctrl_write_hw_assist_ptr;
#define spi_ctrl_write_hw_assist (*spi_ctrl_write_hw_assist_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_read_hw_assist_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *data_out_ptr, UINT32 len_out, UINT8 *data_in_ptr, UINT32 len_in);
EXTERN spi_ctrl_write_read_hw_assist_fn_ptr spi_ctrl_write_read_hw_assist_ptr;
#define spi_ctrl_write_read_hw_assist (*spi_ctrl_write_read_hw_assist_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_dummy_read_hw_assist_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *data_out_ptr, UINT32 len_out, UINT8 *data_in_ptr, UINT32 len_in, UINT8 dummy);
EXTERN spi_ctrl_write_dummy_read_hw_assist_fn_ptr spi_ctrl_write_dummy_read_hw_assist_ptr;
#define spi_ctrl_write_dummy_read_hw_assist (*spi_ctrl_write_dummy_read_hw_assist_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_write_hw_assist_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *data1_ptr, UINT32 len1, const UINT8 *data2_ptr, UINT32 len2);
EXTERN spi_ctrl_write_write_hw_assist_fn_ptr spi_ctrl_write_write_hw_assist_ptr;
#define spi_ctrl_write_write_hw_assist (*spi_ctrl_write_write_hw_assist_ptr)

typedef PMCFW_ERROR (*spi_ctrl_write_mem_mapped_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *src_ptr, UINT8 *dst_ptr, UINT32 len);
EXTERN spi_ctrl_write_mem_mapped_fn_ptr spi_ctrl_write_mem_mapped_ptr;
#define spi_ctrl_write_mem_mapped (*spi_ctrl_write_mem_mapped_ptr)


/*
* Function Prototypes
*/
EXTERN void spi_ctrl_uninit(UINT8 port_id, UINT8 cs_id);
EXTERN VOID spi_ram_code_ptr_adjust(UINT32 offset);
EXTERN VOID spi_ctrl_ram_code_ptr_adjust(UINT32 offset);


#endif /* _SPI_API_H */

/** @} end addtogroup */


