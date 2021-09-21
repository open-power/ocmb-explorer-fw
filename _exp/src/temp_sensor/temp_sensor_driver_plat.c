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
* @addtogroup TEMP_SENSOR_DRIVER_PLAT
* @{
* @file
* @brief
*   This file contains the temperature sensor driver. For the Explorer FVB. This
*   is the driver for the EMC1412-1-ACZL and PCA9548ABS.
*
*/


/*
** Include Files
*/
#include "pmcfw_common.h"
#include "top.h"
#include "bc_printf.h"
#include "app_fw.h"
#include "temp_sensor_plat.h"
#include "temp_sensor_driver_plat.h"
#include "twi_api.h"
#include "pmc_profile.h"


/*
** Global Variables
*/

/*
** Local Constants
*/

/* TWI slave address for the MC1412-1-ACZL temperature sensor and PCA9548ABS I2C switch */
#define TWI_SWITCH_SLV_ADDR     (0xEA >> 1)
#define TWI_TEMP_SLV_ADDR       (0x98 >> 1)


/*
** Local Structures and Unions
*/

/*
** Forward Reference
*/

/*
** Private Data
*/

/* TWI slave structure for the I2C switch */
PRIVATE twi_slave_struct twi_switch = {
    .port_id = EXP_TWI_MASTER_PORT,
    .addr = TWI_SWITCH_SLV_ADDR,
    .addr_size = TWI_ADDR_SIZE_7BIT,
    .stretch_timeout_ms = EXP_TWI_STRETCH_TIMEOUT_MS,
    .stretch_timeout_ms_offset = 0,
    .offset_size = TWI_OFFSET_SIZE_8BIT
};

/*
** Private Functions
*/

/**
* @brief
*   Enable a channel on the TWI expander
*
* @param[in] twi_expander_channel - Channel to enable

* @return
*   PMC_SUCCESS or error code.
*
*/
PUBLIC PMCFW_ERROR temp_sensor_driver_twi_expander_enable(UINT32 twi_expander_channel)
{
    UINT8 twi_switch_control_reg;
    PMCFW_ERROR rc = PMC_SUCCESS;

    if (twi_expander_channel > TWI_SWITCH_CHANNEL_ID_7)
    {
        return TEMP_ERR_SWITCH_CHANNEL_ID_INVALID;
    }

    /* Read the control register */
    rc = twi_mst_rx(&twi_switch, &twi_switch_control_reg, sizeof(twi_switch_control_reg), TRUE);

    if (rc != PMC_SUCCESS)
    {
        return rc;
    }

    /* Write the control register */
    twi_switch_control_reg |= (1 << twi_expander_channel);

    rc = twi_mst_tx(&twi_switch, &twi_switch_control_reg, sizeof(twi_switch_control_reg), TRUE);

    if (rc != PMC_SUCCESS)
    {
        return rc;
    }

    return rc;
}

/**
* @brief
*   Disable a channel on the TWI expander
*
* @param[in] twi_expander_channel - Channel to disable

* @return
*   PMC_SUCCESS or error code.
*
*/
PUBLIC PMCFW_ERROR temp_sensor_driver_twi_expander_disable(UINT32 twi_expander_channel)
{
    UINT8 twi_switch_control_reg;
    PMCFW_ERROR rc = PMC_SUCCESS;

    if (twi_expander_channel > TWI_SWITCH_CHANNEL_ID_7)
    {
        return TEMP_ERR_SWITCH_CHANNEL_ID_INVALID;
    }

    /* Read the control register */
    rc = twi_mst_rx(&twi_switch, &twi_switch_control_reg, sizeof(twi_switch_control_reg), TRUE);

    if (rc != PMC_SUCCESS)
    {
        return rc;
    }

    /* Write the control register */
    twi_switch_control_reg &= ~(1 << twi_expander_channel);

    rc = twi_mst_tx(&twi_switch, &twi_switch_control_reg, sizeof(twi_switch_control_reg), TRUE);

    return rc;
}

/*
** Public Functions
*/

/**
 * @brief
 *   Read temperature sensor register from onchip temperature sensors
 *
 * @param[in]  twi_addr - TWI address
 * @param[in]  twi_offset_size - TWI offset size in bytes (1 or 2 bytes)
 * @param[in]  twi_reg_offset_list - List of register offsets to read from
 * @param[in]  twi_reg_offset_list_length - Number of register offsets
 * @param[in]  twi_reg_data_length - Number of data bytes to be read in each read (1 or 2 bytes)
 * @param[out] value_ptr - Pointer to output value
 *
 * @return PMC_SUCCESS or error code.
 */
PUBLIC PMCFW_ERROR temp_sensor_driver_plat_onchip_read(UINT8   twi_addr,
                                                       twi_offset_size_enum twi_offset_size,
                                                       UINT16* twi_reg_offset_list,
                                                       UINT8   twi_reg_offset_list_length,
                                                       UINT8   twi_reg_data_length,
                                                       UINT16* value_ptr)
{
    PMCFW_ERROR rc = PMC_SUCCESS;
    twi_slave_struct slave_temp_sensor;

    /* Construct twi slave structure */
    slave_temp_sensor.port_id = EXP_TWI_MASTER_PORT;
    slave_temp_sensor.addr = twi_addr >> 1;
    slave_temp_sensor.addr_size = TWI_ADDR_SIZE_7BIT;
    slave_temp_sensor.stretch_timeout_ms = EXP_TWI_STRETCH_TIMEOUT_MS;
    slave_temp_sensor.stretch_timeout_ms_offset = 0;
    slave_temp_sensor.offset_size = twi_offset_size;

    if (twi_reg_offset_list_length == 1)
    {
        /* Perform a single read command */
        rc = twi_mst_rx_offset(&slave_temp_sensor, (UINT32)twi_reg_offset_list[0], (UINT8*)value_ptr, twi_reg_data_length);
    }
    else
    {
        /* Perform two seperate read commands */
        rc = twi_mst_rx_offset(&slave_temp_sensor, (UINT32)twi_reg_offset_list[0], (UINT8*)value_ptr, twi_reg_data_length);

        /* 
        ** Only perform the second read operation if the first one succeeded and the 
        ** read data length is 8bit. Since we only support 16bit temperature sensor readings.
        */
        if (twi_reg_data_length == sizeof(UINT8) && rc == PMC_SUCCESS)
        {
            rc = twi_mst_rx_offset(&slave_temp_sensor, (UINT32)twi_reg_offset_list[1], (((UINT8*)value_ptr)+1), twi_reg_data_length);
        }
    }

    return rc;
}

/**
 * @brief
 *   Read temperature sensor register to JEDEC compliant temperature sensor
 *
 * @param[in]  twi_addr - TWI address
 * @param[in]  reg_addr - Register offset
 * @param[in]  register_read_len - Number of bytes to read
 * @param[out] value_ptr - Pointer to output value
 *
 * @return
 */
PUBLIC PMCFW_ERROR temp_sensor_driver_plat_jedec_read(UINT8 twi_addr, UINT8 reg_addr, UINT8 register_read_len, UINT8* value_ptr)
{
    PMCFW_ERROR rc = PMC_SUCCESS;
    twi_slave_struct slave_temp_sensor;

    /* Construct twi slave structure */
    slave_temp_sensor.port_id = EXP_TWI_MASTER_PORT;
    slave_temp_sensor.addr = twi_addr >> 1;
    slave_temp_sensor.addr_size = TWI_ADDR_SIZE_7BIT;
    slave_temp_sensor.stretch_timeout_ms = EXP_TWI_STRETCH_TIMEOUT_MS;
    slave_temp_sensor.stretch_timeout_ms_offset = 0;
    slave_temp_sensor.offset_size = TWI_OFFSET_SIZE_8BIT;

    /* Read register from temperature sensor */
    rc = twi_mst_rx_offset(&slave_temp_sensor, (UINT32)reg_addr, value_ptr, register_read_len);

    return rc;
}

/** @} end group */

