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
*     Platform-specific definitions and declarations for temp sensor driver.
*
*******************************************************************************/
#ifndef _TEMP_SENSOR_DRIVER_PLAT_H
#define _TEMP_SENSOR_DRIVER_PLAT_H

/*
** Include Files
*/
#include "pmc_hw_base.h"
#include "twi_api.h"

/*
** Enumerated Types 
*/

/* Register address definitions for the EMC1412 temperature sensor */
typedef enum
{
    TEMP_REG_INTERNAL_DIODE_DATA_HIGH_BYTE         = 0x00,   /**< Stores the integer data for the Internal Diode */
    TEMP_REG_EXTERNAL_DIODE_DATA_HIGH_BYTE         = 0x01,   /**< Stores the integer data for the External Diode */
    TEMP_REG_STATUS                                = 0x02,   /**< Stores status bits for the Internal Diode and External Diode */
    TEMP_REG_CONFIGURATION_0                       = 0x03,   /**< Controls the general operation of the device (mirrored at address 09h) */
    TEMP_REG_CONVERSION_RATE_0                     = 0x04,   /**< Controls the conversion rate for updating temperature data (mirrored at address 0Ah) */
    TEMP_REG_INTERNAL_DIODE_HIGH_LIMIT_0           = 0x05,   /**< Stores the 8-bit high limit for the Internal Diode (mirrored at address 0Bh) */
    TEMP_REG_INTERNAL_DIODE_LOW_LIMIT_0            = 0x06,   /**< Stores the 8-bit low limit for the Internal Diode (mirrored at address 0Ch) */
    TEMP_REG_EXTERNAL_DIODE_HIGH_LIMIT_HIGH_BYTE_0 = 0x07,   /**< Stores the integer portion of the high limit for the External Diode (mirrored at register 0Dh) */
    TEMP_REG_EXTERNAL_DIODE_LOW_LIMIT_HIGH_BYTE_0  = 0x08,   /**< Stores the integer portion of the low limit for the External Diode (mirrored at register 0Eh) */
    TEMP_REG_CONFIGURATION_1                       = 0x09,   /**< Controls the general operation of the device (mirrored at address 03h) */
    TEMP_REG_CONVERSION_RATE_1                     = 0x0A,   /**< Controls the conversion rate for updating temperature data (mirrored at address 04h) */
    TEMP_REG_INTERNAL_DIODE_HIGH_LIMIT_1           = 0x0B,   /**< Stores the 8-bit high limit for the Internal Diode (mirrored at address 05h) */
    TEMP_REG_INTERNAL_DIODE_LOW_LIMIT_1            = 0x0C,   /**< Stores the 8-bit low limit for the Internal Diode (mirrored at address 06h) */
    TEMP_REG_INTERNAL_DIODE_HIGH_LIMIT_HIGH_BYTE_1 = 0x0D,   /**< Stores the integer portion of the high limit for the External Diode (mirrored at register 07h) */
    TEMP_REG_INTERNAL_DIODE_LOW_LIMIT_HIGH_BYTE_1  = 0x0E,   /**< Stores the integer portion of the low limit for the External Diode (mirrored at register 08h) */
    TEMP_REG_ONE_SHOT                              = 0x0F,   /**< A write to this register initiates a one shot update */
    TEMP_REG_EXTERNAL_DIODE_DATA_LOW_BYTE          = 0x10,   /**< Stores the fractional data for the External Diode */
    TEMP_REG_SCRATCHPAD_0                          = 0x11,   /**< Scratchpad register for software compatibility */
    TEMP_REG_SCRATCHPAD_1                          = 0x12,   /**< Scratchpad register for software compatibility */
    TEMP_REG_EXTERNAL_DIDOE_HIGH_LIMIT_LOW_BYTE    = 0x13,   /**< Stores the fractional portion of the high limit for the External Diode */
    TEMP_REG_EXTERNAL_DIODE_LOW_LIMIT_LOW_BYTE     = 0x14,   /**< Stores the fractional portion of the low limit for the External Diode */
    TEMP_REG_EXTERNAL_DIODE_THERM_LIMIT            = 0x19,   /**< Stores the 8-bit critical temperature limit for the External Diode */
    TEMP_REG_CHANNEL_MASK_REGISTER                 = 0x1F,   /**< Controls the masking of individual channels */
    TEMP_REG_INTERNAL_DIODE_THERM_LIMIT            = 0x20,   /**< Stores the 8-bit critical temperature limit for the Internal Diode */
    TEMP_REG_THERM_HYSTERESIS                      = 0x21,   /**< Stores the 8-bit hysteresis value that applies to all Therm limits */
    TEMP_REG_CONSECUTIVE_ALERT                     = 0x22,   /**< Controls the number of out-of-limit conditions that must occur before an interrupt is asserted */
    TEMP_REG_EXTERNAL_DIODE_BETA_CONFIGURATION     = 0x25,   /**< Stores the Beta Compensation circuitry setting for External Diode */
    TEMP_REG_EXTERNAL_DIODE_IDEALITY_FACTOR        = 0x27,   /**< Stores the ideality factor for the External Diode */
    TEMP_REG_INTERNAL_DIODE_DATA_LOW_BYTE          = 0x29,   /**< Stores the fractional data for the Internal Diode */
    TEMP_REG_FILTER_CONTROL                        = 0x40,   /**< Controls the digital filter setting for the External DIode channel */
    TEMP_REG_PRODUCT_ID                            = 0xFD,   /**< Stores a fixed value that identifies the device */
    TEMP_REG_MANUFACTURER_ID                       = 0xFE,   /**< Stores a fixed value that represents Microchip */
    TEMP_REG_REVISION                              = 0xFF,   /**< Stores a fixed value that represents the revision number */
} emc1412_reg_addr;

/* Register address definitions for JEDEC compliant temperature sensors */
typedef enum
{
    TEMP_REG_CAPABILITY                  = 0x00,   /**< Capability register */
    TEMP_REG_CONFIGURATION               = 0x01,   /**< Configuration register */
    TEMP_REG_ALARM_TEMP_UPPER_BOUND_TRIP = 0x02,   /**< Upper Boundary Alarm Trip register */
    TEMP_REG_ALARM_TEMP_LOWER_BOUND_TRIP = 0x03,   /**< Lower Boundary Alarm Trip register */
    TEMP_REG_CRITICAL_TEMP_TRIP          = 0x04,   /**< Critical Alarm Trip register */
    TEMP_REG_TEMPERATURE                 = 0x05,   /**< Temperature register */
    TEMP_REG_MANUFACTUER_ID              = 0x06,   /**< Manufacturer ID register */
    TEMP_REG_DEVICE_ID_REVISION          = 0x07    /**< Device ID/Revision register */
} jedec_temp_reg_addr;

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


EXTERN VOID temp_sensor_driver_plat_init(VOID);
EXTERN PMCFW_ERROR temp_sensor_driver_plat_onchip_read(UINT8   twi_addr,
                                                       twi_offset_size_enum twi_offset_size,
                                                       UINT16* twi_reg_offset_list,
                                                       UINT8   twi_reg_offset_list_length,
                                                       UINT8   twi_reg_data_length,
                                                       UINT16* value_ptr);
EXTERN PMCFW_ERROR temp_sensor_driver_plat_jedec_read(UINT8 twi_addr, UINT8 reg_addr, UINT8 register_read_len, UINT8* value_ptr);

#endif /* _TEMP_SENSOR_DRIVER_PLAT_H */

