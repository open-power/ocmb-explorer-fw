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
*  DESCRIPTION :
*    This file contains definitions and declarations for the SPI flash module
*    for use by external applications.
*
*  NOTES :
*
*******************************************************************************/


#ifndef _SPI_FLASH_API_H
#define _SPI_FLASH_API_H

/*
** Include Files
*/

#include "pmcfw_types.h"

/*
** Constants
*/

/* Error codes */
#define SPI_FLASH_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_SPI_FLASH) | (err_suffix))
#define SPI_FLASH_ERR_SPI_UNINITIALIZED SPI_FLASH_ERR_CODE_CREATE(0x001)  /* SPI not initialized */
#define SPI_FLASH_ERR_CS_UNINITIALIZED  SPI_FLASH_ERR_CODE_CREATE(0x002)  /* port / chip select not initialized */
#define SPI_FLASH_ERR_BAD_PARAM         SPI_FLASH_ERR_CODE_CREATE(0x003)  /* bad parameter */
#define SPI_FLASH_ERR_DEV_SUPPORT       SPI_FLASH_ERR_CODE_CREATE(0x004)  /* flash device not supported */
#define SPI_FLASH_ERR_DEV_STATE         SPI_FLASH_ERR_CODE_CREATE(0x005)  /* flash device in wrong state */
#define SPI_FLASH_ERR_DEVICE_ERROR      SPI_FLASH_ERR_CODE_CREATE(0x006)  /* error indicated by flash device */
#define SPI_FLASH_ERR_WRITE_FAIL        SPI_FLASH_ERR_CODE_CREATE(0x007)  /* flash write failure */
#define SPI_FLASH_ERR_BAD_DEV_CFG       SPI_FLASH_ERR_CODE_CREATE(0x008)  /* invalid flash device configuration */
#define SPI_FLASH_ERR_ADDR_OOB          SPI_FLASH_ERR_CODE_CREATE(0x009)  /* address out of bounds */
#define SPI_FLASH_ERR_ADDR_INVAL        SPI_FLASH_ERR_CODE_CREATE(0x00A)  /* address invalid */
#define SPI_FLASH_ERR_WRITE_LEN         SPI_FLASH_ERR_CODE_CREATE(0x00B)  /* invalid write length */
#define SPI_FLASH_ERR_OPER_SUPPORT      SPI_FLASH_ERR_CODE_CREATE(0x00C)  /* operation not supported */
#define SPI_FLASH_ERR_DEV_UNKNOWN       SPI_FLASH_ERR_CODE_CREATE(0x00D)  /* unknown device */
#define SPI_FLASH_ERR_PROTO_SUPPORT     SPI_FLASH_ERR_CODE_CREATE(0x00E)  /* specified protocol not supported */
#define SPI_FLASH_ERR_DUMMY_SUPPORT     SPI_FLASH_ERR_CODE_CREATE(0x00F)  /* specified dummy cycles not supported */

/*
** Enumerated Types
*/

/* Supported SPI flash devices */
typedef enum
{
    SPI_FLASH_DEV_NONE = 0,        /* no device attached */
    SPI_FLASH_DEV_AUTO,            /* auto-detect device */
    SPI_FLASH_DEV_UNKNOWN,         /* unknown device (auto-detection failed) */
    SPI_FLASH_DEV_MT25QU128ABA,    /* Micron MT25QU128ABA */
    SPI_FLASH_DEV_MT25QU256ABA,    /* Micron MT25QU256ABA */
    SPI_FLASH_DEV_MT25QU01GBBB,    /* Micron MT25QU01GBBB */
    SPI_FLASH_DEV_MX25U25645G,     /* Macronix MX25U25645G */
    SPI_FLASH_DEV_MX25U12835F,     /* Macronix MX25U12835F*/
    SPI_FLASH_DEV_W25Q256JW,       /* Winbond W25Q256JW */
    SPI_FLASH_DEV_S25FS256S,       /* Cypress S25FS256S */
    SPI_FLASH_DEV_ROM_BOOT,        /* Generic device used by a ROM boot agent */
} spi_flash_dev_enum;

/*
** Structures and Unions
*/

/***************************************************************************
*
* STRUCTURE: spi_flash_dev_info_struct
* __________________________________________________________________________
*
* DESCRIPTION:
*   SPI flash device information.
*
* ELEMENTS:
*   sectors                  - number of sectors
*   subsectors_per_sector    - number of subsectors per sector
*   pages_per_subsector      - number of pages per subsector
*   page_size                - physical page size (bytes)
*   max_time_page_prog       - max. time to program a page (us)
*   max_time_subsector_erase - max. time to erase a subsector (us)
*   max_time_sector_erase    - max. time to erase a sector (us)
*   max_time_bulk_erase      - max. time to erase the entire device (us)
*
* NOTES:
*   If the device doesn't have the concept of subsectors, it will be considered
*   to have one subsector per sector.
*
****************************************************************************/
typedef struct
{
    UINT32 sectors;
    UINT32 subsectors_per_sector;
    UINT32 pages_per_subsector;
    UINT32 page_size;
    UINT32 max_time_page_prog;
    UINT32 max_time_subsector_erase;
    UINT32 max_time_sector_erase;
    UINT32 max_time_bulk_erase;
} spi_flash_dev_info_struct;

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
typedef PMCFW_ERROR (*spi_flash_init_fn_ptr)(UINT8 port_id, UINT8 cs_id, spi_flash_dev_enum dev);
EXTERN spi_flash_init_fn_ptr spi_flash_init_ptr;
#define spi_flash_init (*spi_flash_init_ptr)

typedef PMCFW_ERROR (*spi_flash_dev_info_get_fn_ptr)(UINT8 port_id, UINT8 cs_id, spi_flash_dev_enum *dev_ptr, spi_flash_dev_info_struct *dev_info_ptr);
EXTERN spi_flash_dev_info_get_fn_ptr spi_flash_dev_info_get_ptr;
#define spi_flash_dev_info_get (*spi_flash_dev_info_get_ptr)

typedef PMCFW_ERROR (*spi_flash_vendor_ids_get_fn_ptr)(UINT8 port_id, UINT8 cs_id, UINT8 *manuf_id, UINT16 *dev_id);
EXTERN spi_flash_vendor_ids_get_fn_ptr spi_flash_vendor_ids_get_ptr;
#define spi_flash_vendor_ids_get (*spi_flash_vendor_ids_get_ptr)

typedef PMCFW_ERROR (*spi_flash_sector_params_get_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *addr_ptr, UINT8 **sector_ptr, UINT32 *len);
EXTERN spi_flash_sector_params_get_fn_ptr spi_flash_sector_params_get_ptr;
#define spi_flash_sector_params_get (*spi_flash_sector_params_get_ptr)

typedef PMCFW_ERROR (*spi_flash_subsector_params_get_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *addr_ptr, UINT8 **sector_ptr, UINT32 *len);
EXTERN spi_flash_subsector_params_get_fn_ptr spi_flash_subsector_params_get_ptr;
#define spi_flash_subsector_params_get (*spi_flash_subsector_params_get_ptr)

typedef PMCFW_ERROR (*spi_flash_read_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *src_ptr, UINT8 *dst_ptr, UINT32 len);
EXTERN spi_flash_read_fn_ptr spi_flash_read_ptr;
#define spi_flash_read  (*spi_flash_read_ptr)

typedef PMCFW_ERROR (*spi_flash_write_fn_ptr)(UINT8 port_id, UINT8 cs_id, const UINT8 *src_ptr, UINT8 *dst_ptr, UINT32 len);
EXTERN spi_flash_write_fn_ptr spi_flash_write_ptr;
#define spi_flash_write (*spi_flash_write_ptr)

typedef PMCFW_ERROR (*spi_flash_write_pages_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, UINT8* src_ptr, UINT8* dst_ptr, UINT32 len, UINT32 page_size, UINT32 timeout);
EXTERN spi_flash_write_pages_fn_ptr_type spi_flash_write_pages_fn_ptr;
#define spi_flash_write_pages (*spi_flash_write_pages_fn_ptr)

typedef PMCFW_ERROR (*spi_flash_write_complete_fn_ptr)(UINT8 port_id, UINT8 cs_id, BOOL *complete);
EXTERN spi_flash_write_complete_fn_ptr spi_flash_write_complete_ptr;
#define spi_flash_write_complete (*spi_flash_write_complete_ptr)

typedef PMCFW_ERROR (*spi_flash_subsector_erase_fn_ptr)(UINT8 port_id, UINT8 cs_id, UINT8 *addr_ptr);
EXTERN spi_flash_subsector_erase_fn_ptr spi_flash_subsector_erase_ptr;
#define spi_flash_subsector_erase (*spi_flash_subsector_erase_ptr)

typedef PMCFW_ERROR (*spi_flash_sector_erase_fn_ptr)(UINT8 port_id, UINT8 cs_id, UINT8 *addr_ptr);
EXTERN spi_flash_sector_erase_fn_ptr spi_flash_sector_erase_ptr;
#define spi_flash_sector_erase (*spi_flash_sector_erase_ptr)

typedef PMCFW_ERROR (*spi_flash_subsector_erase_wait_fn_ptr_type)(UINT8 port_id, UINT8 cs_id, UINT8 *addr_ptr, UINT32 timeout);
EXTERN spi_flash_subsector_erase_wait_fn_ptr_type spi_flash_subsector_erase_wait_fn_ptr;
#define spi_flash_subsector_erase_wait (*spi_flash_subsector_erase_wait_fn_ptr)

typedef PMCFW_ERROR (*spi_flash_erase_complete_fn_ptr)(UINT8 port_id, UINT8 cs_id, BOOL *complete);
EXTERN spi_flash_erase_complete_fn_ptr spi_flash_erase_complete_ptr;
#define spi_flash_erase_complete (*spi_flash_erase_complete_ptr)


/*
** Function Prototypes
*/
EXTERN PMCFW_ERROR spi_flash_page_size_get(const UINT8 port_id,
                                           const UINT8 cs_id,
                                           const UINT8 * const addr_ptr,
                                           UINT32 * const page_size_ptr);

EXTERN PMCFW_ERROR spi_flash_bulk_erase(UINT8 port_id, UINT8 cs_id);

EXTERN PMCFW_ERROR spi_flash_reset(UINT8 port_id, UINT8 cs_id);

EXTERN VOID spi_flash_ram_code_ptr_adjust(UINT32 offset);
EXTERN VOID spi_n25q_ram_code_ptr_adjust(UINT32 offset);


#endif /* _SPI_FLASH_API_H */



