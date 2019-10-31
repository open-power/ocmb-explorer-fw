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
*   DESCRIPTION:
*    This file contains definitions and declarations for the SPB SPI module for
*    use by external applications.
*
*   NOTES:
*
*******************************************************************************/



#ifndef _SPB_SPI_H
#define _SPB_SPI_H

/*
** Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_mid.h"

/*
** Constants
*/

/* Error codes */
#define SPB_SPI_ERR_CODE_CREATE(err_suffix) ((PMCFW_ERR_BASE_SPB_SPI) | (err_suffix))

#define SPB_SPI_ERR_BAD_PARAM_1     SPB_SPI_ERR_CODE_CREATE(0x001)  /* invalid function parameter value */
#define SPB_SPI_ERR_BAD_PARAM_2     SPB_SPI_ERR_CODE_CREATE(0x002)  /* invalid function parameter value */
#define SPB_SPI_ERR_BAD_PARAM_3     SPB_SPI_ERR_CODE_CREATE(0x003)  /* invalid function parameter value */

/*
** Local Macro Definitions
*/

/******************************************************************************
**
**  MACRO: SPB_SPI_ECC_REGION_REG
**  ___________________________________________________________________________
**
**  DESCRIPTION:    Get a SPIx_CPUy_REGIONz_ register address given the
**                  region 0 address and the desired region.
**
**  INPUTS:         region  - region number
**                  reg     - port 0 register
**
**  RETURNS:        SPIx_CPUy_REGIONz_ register address for the given region
**
******************************************************************************/
#define SPB_SPI_ECC_REGION_REG(region, reg)     ((reg) + ((region) * 0x8))

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
typedef VOID (*spb_spi_ecc_init_fn_ptr)(const UINT8 port_id, const BOOL enable, const UINT32 addr_base_phys, const UINT32 size);
EXTERN spb_spi_ecc_init_fn_ptr spb_spi_ecc_init_ptr;
#define spb_spi_ecc_init (*spb_spi_ecc_init_ptr)

typedef BOOL (*spb_spi_ecc_err_check_fn_ptr)(const UINT8 port_id);
EXTERN spb_spi_ecc_err_check_fn_ptr spb_spi_ecc_err_check_ptr;
#define spb_spi_ecc_err_check (*spb_spi_ecc_err_check_ptr)

typedef void (*spb_spi_2bit_ecc_err_inj_en_set_fn_ptr)(const UINT8 port_id, const BOOL enable);
EXTERN spb_spi_2bit_ecc_err_inj_en_set_fn_ptr spb_spi_2bit_ecc_err_inj_en_set_ptr;
#define spb_spi_2bit_ecc_err_inj_en_set (*spb_spi_2bit_ecc_err_inj_en_set_ptr)

typedef void (*spb_spi_mem_addr_size_set_fn_ptr)(const UINT8 port_id, const UINT8 cs_id, const UINT32 mm_addr_base_phys, const UINT32 mm_addr_size);
EXTERN spb_spi_mem_addr_size_set_fn_ptr spb_spi_mem_addr_size_set_ptr;
#define spb_spi_mem_addr_size_set (*spb_spi_mem_addr_size_set_ptr)

/*
** Function Prototypes
*/
EXTERN VOID spb_spi_ecc_int_prop_en(const UINT8 port_id);
EXTERN VOID spb_spi_ram_code_ptr_adjust(UINT32 offset);

#endif /* define _SPB_SPI_H */



