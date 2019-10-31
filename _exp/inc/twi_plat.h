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
* @addtogroup TWI
* @{
* @file
* @brief
*   Platform-specific definitions and declarations for TWI.
*
* @note
*/
#ifndef _TWI_PLAT_H
#define _TWI_PLAT_H

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "spb_twi.h"

/*
* Enumerated Types
*/

/**
* @brief 
*  TWI pad mode
*
* @note
*
*/
typedef enum
{
    TWI_PAD_MODE_TWI,    /**< TWI */
    TWI_PAD_MODE_GPIO,   /**< GPIO */
} twi_pad_mode_enum;


/* HARDCODED data for SPD device number in htwi device structure.*/
#define TWI_FWNVRAM_DEVNUM     (1) /* Same as I2C_BOOTSTRAP from hal.h for now, but not necessarily so.  */
#define TWI_SPD_DEVNUM         (4) /* Same as I2C_DEV_FIRST_DRAM_DIMM_SPD from hal.h for now, but not necessarily so.  */ 

#define EXP_TWI_MASTER_PORT         0
#define EXP_TWI_SLAVE_PORT          1

/*
* Structures and Unions
*/

typedef void (*twi_plat_isr_fcn_ptr)(UINT);


/**
* @brief 
*  I2C device parameter structure
*
* @note
*
*/
typedef struct {
    UINT8 device_handle;    /**< Handle used in HAL API for this device */
    UINT8 bus;              /**< I2C bus number, 0 -based */
    UINT8 address;          /**< I2C device address */
    UINT32 size;            /**< I2C device size in bytes */
    UINT32 wp_bytes;        /**< umber of write -protected bytes */
    UINT32 wp_start;        /**< Starting write -protected byte */
    UINT32 speed;           /**< Device speed in KHz */
    UINT32 page_size;       /**< Device page size */
} I2C_device_params;

/**
* @brief 
*  TWI device information structure
*
* @note
*
*/
typedef struct {
    I2C_device_params info;     /**< I2D Device parameters */
    UINT32 addr_access_size;    /**< Address size */
    UINT32 data_access_size;    /**< Data size */
    UINT32 timeout;             /**< TWI timeout */
    UINT16 page_size;           /**< page size, only used if device_type is 0 (NVRAM) */
} twi_device_info_struct;

/*
* Function Prototypes
*/

EXTERN VOID twi_plat_isr_register(twi_plat_isr_fcn_ptr mst_isr_ptr,
                                  twi_plat_isr_fcn_ptr slv_isr_ptr,
                                  twi_plat_isr_fcn_ptr srst_isr_ptr);

EXTERN UINT32 twi_plat_mst_base_address(const UINT port_id);

EXTERN UINT32 twi_plat_slv_base_address(const UINT port_id);

EXTERN PMCFW_ERROR twi_plat_port_mode_set(const UINT port_id,
                                          const twi_pad_mode_enum mode);

EXTERN PMCFW_ERROR twi_plat_pin_read(const UINT port_id,
                                     const spb_twi_pad_enum pad,
                                     BOOL * const value);

EXTERN PMCFW_ERROR twi_plat_pin_write(const UINT port_id,
                                      const spb_twi_pad_enum pad,
                                      const BOOL value);

EXTERN BOOL twi_plat_device_ptr_get(UINT32 device,
                                twi_device_info_struct **twi_device_pptr);

EXTERN VOID twi_plat_pin_driver_strength_set(UINT8 port_id);


#endif /* _TWI_PLAT_H */
/** @} end addtogroup */



