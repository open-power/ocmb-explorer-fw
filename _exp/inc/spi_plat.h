/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2018, 2019, 2020 Microchip Technology Inc. All rights reserved. 
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
*     Platform-specific definitions and declarations for SPI.
*
*******************************************************************************/


#ifndef _SPI_PLAT_H
#define _SPI_PLAT_H

/*
** Include Files
*/

#include "spi_plat_cfg.h"
#include "spi.h"
#include "pmcfw_common.h"

/*
** Local Constants
*/

/* number of SPI ports */
#define SPI_NUM_PORTS  1

/* total potential number of SPI devices on all ports, whether physically present or not */
#define SPI_NUM_DEVS   1

/* SPI clock rate */
#define SPI_CLK_RATE   400

/*
** Macro Definitions
*/

/******************************************************************************
**
**  MACRO: SPI_ARR_IDX
**  ___________________________________________________________________________
**
**  DESCRIPTION:    Calculate the index into a device array given a port ID and
**                  device ID. One-dimensional device arrays are used rather
**                  than two dimensional arrays based on port ID and device ID
**                  since different ports can have different numbers of devices.
**
**  INPUTS:         port_id - port identifier
**                  cs_id   - chip select identifier for the given port
**
**  RETURNS:        Array index
**
******************************************************************************/
#define SPI_ARR_IDX(port_id, cs_id)    ((2 * (port_id)) + (cs_id))

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
typedef PMCFW_ERROR (*spi_plat_pin_driver_strength_set_fn_ptr_type)(UINT8 port_id);
EXTERN spi_plat_pin_driver_strength_set_fn_ptr_type spi_plat_pin_driver_strength_set_fn_ptr;
#define spi_plat_pin_driver_strength_set (*spi_plat_pin_driver_strength_set_fn_ptr)

typedef BOOL (*spi_plat_is_boot_quad_fn_ptr_type)(VOID);
EXTERN spi_plat_is_boot_quad_fn_ptr_type spi_plat_is_boot_quad_fn_ptr;
#define spi_plat_is_boot_quad (*spi_plat_is_boot_quad_fn_ptr)

typedef UINT16 (*spi_plat_cs_wait_get_fn_ptr_type)(UINT8 wait);
EXTERN spi_plat_cs_wait_get_fn_ptr_type spi_plat_cs_wait_get_fn_ptr;
#define spi_plat_cs_wait_get (*spi_plat_cs_wait_get_fn_ptr)

typedef BOOL (*spi_plat_wci_mode_get_fn_ptr_type)(VOID);
EXTERN spi_plat_wci_mode_get_fn_ptr_type spi_plat_wci_mode_get_fn_ptr;
#define spi_plat_wci_mode_get (*spi_plat_wci_mode_get_fn_ptr)

typedef PMCFW_ERROR (*spi_plat_flash_poll_write_erase_complete_fn_ptr_type)(BOOL write, UINT32 timeout);
EXTERN spi_plat_flash_poll_write_erase_complete_fn_ptr_type spi_plat_flash_poll_write_erase_complete_fn_ptr;
#define spi_plat_flash_poll_write_erase_complete (*spi_plat_flash_poll_write_erase_complete_fn_ptr)

EXTERN spi_parms_struct *spi_parms_get(VOID);
EXTERN VOID spi_plat_init(VOID);
EXTERN VOID spi_plat_ram_code_ptr_adjust(UINT32 offset);

#endif /* _SPI_PLAT_H */



