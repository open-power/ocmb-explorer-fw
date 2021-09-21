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

/********************************************************************************
*   DESCRIPTION: This file implements Temperature Sensor command handlers,
*	variables and functions
*
*   NOTES:       None.
*
******************************************************************************/




/**
* @addtogroup TEMP_SENSOR_PLAT
* @{
* @file
* @brief
*   This file contains the Temp sensor handlers.
*
*/ 


/*
** Include Files
*/
#include "pmcfw_common.h"
#include "top.h"
#include "bc_printf.h"
#include "ech.h"
#include "app_fw.h"
#include "temp_sensor_driver_plat.h"
#include "temp_sensor_plat.h"
#include "twi_api.h"
#include "ocmb.h"
#include "exp_gic.h"
#include "cicint_api.h"
#include <string.h>
#include "opsw_timer.h" 
#include "top_plat.h"
#include "pvt.h"
#include "pmc_profile.h"


/*
** Global Variables
*/

/*
** Local Structures and Unions
*/

/* Structure for onboard temperature sensor polling settings */
typedef struct
{
    BOOL   present;        /**< Sensor is present */
    UINT8  twi_addr;       /**< TWI slave address */
    UINT8  twi_reg_offset; /**< TWI register offset */
    UINT8  twi_read_size;  /**< Number of bytes to read */
} onboard_temp_sensor_twi_polling_config;

/* Structure for onchip temperature sensor polling settings */
typedef struct
{
    BOOL   present;                        /**< Sensor is present */
    UINT8  num_twi_reads;                  /**< Number of separate TWI read operations (1 or 2) */
    twi_offset_size_enum  reg_offset_mode; /**< 8-bit or 16-bit register offsets */
    UINT8  twi_addr;                       /**< TWI slave address */
    UINT16 twi_reg_offset[2];              /**< TWI register offset */
    UINT8  twi_read_size;                  /**< Number of bytes to read */
} onchip_temp_sensor_twi_polling_config;

/*
** Local Constants
*/

#define TEMP_SENSOR_CMD_ERR_CODE_BITMASK    0xFFFF  /* Pass through temperature commands are limited to 16-bit error codes */

#define TEMP_SENSOR_UPDATE_TIMER_INT        TIMER_1_INT

/* Flags for updating temperature sensors */
#define TEMP_SENSOR_UPDATE_FLAG 0x1

/*
** Forward Reference
*/


/*
** Private Data
*/

PRIVATE onboard_temp_sensor_twi_polling_config temp_sensor_onboard_dimm0_config;
PRIVATE onboard_temp_sensor_twi_polling_config temp_sensor_onboard_dimm1_config;
PRIVATE onchip_temp_sensor_twi_polling_config  temp_sensor_onchip_config;

/*
** Update frequency for all of the temperature sensors. We will update all the
** temperature sensors at the shortest interval provided by the host.
*/
PRIVATE UINT16 temp_sensor_update_interval_ms;

PRIVATE volatile UINT32 temperature_update_flags = 0;

/*
** Private Functions
*/

/**
* @brief
*   Interrupt handler for the on board temperature sensor update timer
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID temp_sensor_plat_temp_update_int_handler(VOID *int_num)
{
    /* Set flag to update temperature sensors */
    temperature_update_flags |= TEMP_SENSOR_UPDATE_FLAG;

    /* Clear the interrupt */
    cicint_int_clear(TEMP_SENSOR_UPDATE_TIMER_INT);
}

/**
* @brief
*   Initializes the onchip temperature and onboard temperature timer interrupts
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID temp_sensor_plat_timer_int_init(VOID)
{
    /* Initialize interval timer1 */
    opsw_timer1_temp_polling_init();

    /* Register and enable the timer interrupts */
    cicint_int_register(TEMP_SENSOR_UPDATE_TIMER_INT,
                        (cicint_cback_fcn_ptr)temp_sensor_plat_temp_update_int_handler,
                        (VOID*)TEMP_SENSOR_UPDATE_TIMER_INT);

    cicint_int_enable(TEMP_SENSOR_UPDATE_TIMER_INT);
}

/**
* @brief
*   Update interval timer used for periodic temperature updates
*
* @param[in] update_interval_ms - Update interval for the temperature sensors
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID temp_sensor_plat_timer_interval_update(UINT16 update_interval_ms)
{
    opsw_timer1_interval_update(update_interval_ms);
}

/**
* @brief
*   Temp sensor pass through read command handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID temp_sensor_pass_through_read_command_handler(VOID)
{
    UINT8 status = PMC_SUCCESS;
    UINT16 error_code = PMC_SUCCESS;
    BOOL stop_en;
    twi_slave_struct slave_temp_sensor;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_temp_read_cmd_parms_struct* cmd_parms_ptr = (exp_fw_temp_read_cmd_parms_struct*) &cmd_ptr->parms;
    exp_fw_temp_read_rsp_parms_struct* rsp_parms_ptr = (exp_fw_temp_read_rsp_parms_struct*) &rsp_ptr->parms;

    /* Fill in TWI slave structure for the temperature sensor */
    slave_temp_sensor.port_id = EXP_TWI_MASTER_PORT;
    slave_temp_sensor.addr = cmd_parms_ptr->twi_address >> 1;
    slave_temp_sensor.addr_size = TWI_ADDR_SIZE_7BIT;
    slave_temp_sensor.stretch_timeout_ms = EXP_TWI_STRETCH_TIMEOUT_MS;
    slave_temp_sensor.stretch_timeout_ms_offset = 0;
    slave_temp_sensor.offset_size = TWI_OFFSET_SIZE_8BIT;

    /* Check stop enable flag */
    if ((cmd_parms_ptr->twi_cmd_flags & EXP_TWI_TEMP_READ_CMD_FLAG_STOP_BITMSK) == EXP_TWI_TEMP_READ_CMD_FLAG_STOP_BITMSK)
    {
        stop_en = TRUE;
    }
    else
    {
        stop_en = FALSE;
    }

    error_code = twi_mst_rx(&slave_temp_sensor, &rsp_parms_ptr->resp_data[0], cmd_parms_ptr->num_bytes, stop_en);

    if (error_code != PMC_SUCCESS)
    {
        bc_printf("EXP_FW_TEMP_SENSOR_PASS_THROUGH_READ error = 0x%08x\n", error_code);
        status = PMCFW_ERR_FAIL;
    }

    /* Fill response status and error code */
    rsp_parms_ptr->status = status;

    /* set the no extended data flag */
    rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_temp_sensor_pass_through_read */


/**
* @brief
*   Temp sensor pass through write command handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID temp_sensor_pass_through_write_command_handler(VOID)
{
    UINT8 status = PMC_SUCCESS;
    UINT32 error_code = 0;
    BOOL stop_en;
    twi_slave_struct slave_temp_sensor;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_temp_write_cmd_parms_struct* cmd_parms_ptr = (exp_fw_temp_write_cmd_parms_struct*) &cmd_ptr->parms;
    exp_fw_temp_write_rsp_parms_struct* rsp_parms_ptr = (exp_fw_temp_write_rsp_parms_struct*) &rsp_ptr->parms;

    /* Fill in TWI slave structure for the temperature sensor */
    slave_temp_sensor.port_id = EXP_TWI_MASTER_PORT;
    slave_temp_sensor.addr = cmd_parms_ptr->twi_address >> 1;
    slave_temp_sensor.addr_size = TWI_ADDR_SIZE_7BIT;
    slave_temp_sensor.stretch_timeout_ms = EXP_TWI_STRETCH_TIMEOUT_MS;
    slave_temp_sensor.stretch_timeout_ms_offset = 0;
    slave_temp_sensor.offset_size = TWI_OFFSET_SIZE_8BIT;

    /* Check stop enable flag */
    if ((cmd_parms_ptr->twi_cmd_flags & EXP_TWI_TEMP_WRITE_CMD_FLAG_STOP_BITMSK) == EXP_TWI_TEMP_WRITE_CMD_FLAG_STOP_BITMSK)
    {
        stop_en = TRUE;
    }
    else
    {
        stop_en = FALSE;
    }

    error_code = twi_mst_tx(&slave_temp_sensor, &cmd_parms_ptr->data_bytes[0], cmd_parms_ptr->num_bytes, stop_en);

    if (error_code != PMC_SUCCESS)
    {
        bc_printf("EXP_FW_TEMP_SENSOR_PASS_THROUGH_WRITE error = 0x%08x\n", error_code);
        status = PMCFW_ERR_FAIL;
    }

    /* Fill response status and error code */
    rsp_parms_ptr->status = status;

    /* set the no extended data flag */
    rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_temp_sensor_pass_through_write */


/**
* @brief
*   Temp sensor config interval read command handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID temp_sensor_cfg_interval_read_command_handler(VOID)
{
    UINT8 status = PMC_SUCCESS;
    UINT32 error_code = TEMP_SENSOR_INTERVAL_READ_SUCCESS;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_temp_interval_read_cmd_parms_struct* cmd_parms_ptr = (exp_fw_temp_interval_read_cmd_parms_struct*) &cmd_ptr->parms;
    exp_fw_temp_interval_read_rsp_parms_struct* rsp_parms_ptr = (exp_fw_temp_interval_read_rsp_parms_struct*) &rsp_ptr->parms;

    temp_sensor_onboard_dimm0_config.present = (cmd_ptr->flags & EXP_FW_TEMP_DIMM0_PRESENT_BITMSK)? TRUE: FALSE;
    temp_sensor_onboard_dimm1_config.present = (cmd_ptr->flags & EXP_FW_TEMP_DIMM1_PRESENT_BITMSK)? TRUE: FALSE;
    temp_sensor_onchip_config.present = (cmd_ptr->flags & EXP_FW_TEMP_ONCHIP_PRESENT_BITMSK)? TRUE: FALSE;

    if (temp_sensor_onboard_dimm0_config.present)
    {
        temp_sensor_onboard_dimm0_config.twi_addr = cmd_parms_ptr->onboard_dimm0_twi_address;
        temp_sensor_onboard_dimm0_config.twi_reg_offset = cmd_parms_ptr->onboard_dimm0_twi_reg_offset;

        if (cmd_parms_ptr->onboard_dimm0_twi_reg_length == EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_1_BYTE)
        {
            temp_sensor_onboard_dimm0_config.twi_read_size = sizeof(UINT8);
        }
        else if (cmd_parms_ptr->onboard_dimm0_twi_reg_length == EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_2_BYTES)
        {
            temp_sensor_onboard_dimm0_config.twi_read_size = sizeof(UINT16);
        }
        else
        {
            status = PMCFW_ERR_FAIL;
            error_code = TEMP_ERR_ONCHIP_REG_READ_LEN_UNSUPPORTED;
            temp_sensor_onboard_dimm0_config.present = FALSE;
        }
    }

    if (temp_sensor_onboard_dimm1_config.present)
    {
        temp_sensor_onboard_dimm1_config.twi_addr = cmd_parms_ptr->onboard_dimm1_twi_address;
        temp_sensor_onboard_dimm1_config.twi_reg_offset = cmd_parms_ptr->onboard_dimm1_twi_reg_offset;

        if (cmd_parms_ptr->onboard_dimm1_twi_reg_length == EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_1_BYTE)
        {
            temp_sensor_onboard_dimm1_config.twi_read_size = sizeof(UINT8);
        }
        else if (cmd_parms_ptr->onboard_dimm1_twi_reg_length == EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_2_BYTES)
        {
            temp_sensor_onboard_dimm1_config.twi_read_size = sizeof(UINT16);
        }
        else
        {
            status = PMCFW_ERR_FAIL;
            error_code = TEMP_ERR_ONCHIP_REG_READ_LEN_UNSUPPORTED;
            temp_sensor_onboard_dimm1_config.present = FALSE;
        }
    }

    if (temp_sensor_onchip_config.present)
    {
        if (cmd_parms_ptr->onchip_config_flags & EXP_TWI_TEMP_INTERVAL_READ_CMD_SENSOR_TYPE_BITMSK)
        {
            /* 
            ** firmware on-chip temp sensor will be used 
            ** it is auto-configured by the application at start-up 
            */
        }
        else
        {
            /* on-chip temperature diode will be read via external TWI addressable temperature sensor */

            if (EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_1_BYTE == ((cmd_parms_ptr->onchip_config_flags & EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_BITMSK) >>
                                                                                                     EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_BITOFF))
            {
                temp_sensor_onchip_config.twi_read_size = sizeof(UINT8);
            }
            else if (EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_2_BYTES == ((cmd_parms_ptr->onchip_config_flags & EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_BITMSK) >>
                                                                                                           EXP_TWI_TEMP_INTERVAL_READ_CMD_SIZE_BITOFF))
            {
                temp_sensor_onchip_config.twi_read_size = sizeof(UINT16);
            }
            else
            {
                status = PMCFW_ERR_FAIL;
                error_code = TEMP_ERR_ONCHIP_REG_READ_LEN_UNSUPPORTED;
                temp_sensor_onchip_config.present = FALSE;
            }


            if (status == PMC_SUCCESS)
            {
                if (cmd_parms_ptr->onchip_config_flags & EXP_TWI_TEMP_INTERVAL_READ_CMD_NUM_READ_OP_BITMSK)
                {
                    if (temp_sensor_onchip_config.twi_read_size == sizeof(UINT8))
                    {
                        temp_sensor_onchip_config.num_twi_reads = 2;
                    }
                    else
                    {
                        status = PMCFW_ERR_FAIL;
                        error_code = TEMP_ERR_ONCHIP_REG_READ_LEN_UNSUPPORTED;
                        temp_sensor_onchip_config.present = FALSE;
                    }
                }
                else
                {
                    temp_sensor_onchip_config.num_twi_reads = 1;
                }
            }

            if (status == PMC_SUCCESS)
            {
                temp_sensor_onchip_config.reg_offset_mode = (cmd_parms_ptr->onchip_config_flags & EXP_TWI_TEMP_INTERVAL_READ_CMD_REG_OFFSET_BITMSK)?
                                                            TWI_OFFSET_SIZE_16BIT:
                                                            TWI_OFFSET_SIZE_8BIT;

                temp_sensor_onchip_config.twi_addr = cmd_parms_ptr->onchip_twi_address;
                temp_sensor_onchip_config.twi_reg_offset[0] = cmd_parms_ptr->onchip_twi_reg_offset_0;

                if (temp_sensor_onchip_config.num_twi_reads == 2)
                {
                    temp_sensor_onchip_config.twi_reg_offset[1] = cmd_parms_ptr->onchip_twi_reg_offset_1;
                }
            }
        }
    }

    temp_sensor_update_interval_ms = cmd_parms_ptr->read_interval_ms;
    temp_sensor_plat_timer_interval_update(temp_sensor_update_interval_ms);

    /* Set status and error code */
    rsp_parms_ptr->status = status;
    rsp_parms_ptr->err_code = error_code & TEMP_SENSOR_CMD_ERR_CODE_BITMASK;

    /* set the no extended data flag */
    rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_temp_sensor_cfg_interval_read */




/*
** Public Functions
*/

/**
* @brief
*   Temp Sensor Platform initialization
*
* @param
*   None
* @return
*   PMC_SUCCESS or error specific code
*
*/
PUBLIC PMCFW_ERROR temp_sensor_plat_init(VOID)
{
    /* Register the temperature sensor command handler with ECH module*/
    ech_api_func_register(EXP_FW_TEMP_SENSOR_PASS_THROUGH_READ, temp_sensor_pass_through_read_command_handler);
    ech_api_func_register(EXP_FW_TEMP_SENSOR_PASS_THROUGH_WRITE, temp_sensor_pass_through_write_command_handler);
    ech_api_func_register(EXP_FW_TEMP_SENSOR_CONFIG_INTERVAL_READ, temp_sensor_cfg_interval_read_command_handler);

    /* Initialize the timer interrupt */
    temp_sensor_plat_timer_int_init();

    /* 
    ** if operating on FVB, temp sensors are behind TWI expander
    ** if present, enable the expander channels to access the temp sensors 
    ** NOTE: the FVB temp sensors are on different expander channels but since both 
    ** channels will be enabled the temp sensors must be at different addresses
    */

    /* 
    ** ensure the master port recovery sequence is disabled so recovery is not 
    ** attempted if there is no response from the expander 
    */ 
    BOOL mst_init_recovery_en = twi_mst_init_recovery_en_get(EXP_TWI_MASTER_PORT);
    twi_mst_init_recovery_en_set(EXP_TWI_MASTER_PORT, FALSE);

    /* 
    ** enable DIMM0 channel on TWI expander
    ** only required for Explorer FVB, has no impact if expander is not present
    ** expander address is reserved and no other device should be at the address
    */
    PMCFW_ERROR rc = temp_sensor_driver_twi_expander_enable(TWI_SWITCH_CHANNEL_ID_2);
    if (PMC_SUCCESS == rc)
    {
        /* 
        ** expander responded successfully so it is present 
        ** enable DIMM1 channel on TWI expander
        */ 
        temp_sensor_driver_twi_expander_enable(TWI_SWITCH_CHANNEL_ID_3);

#if (EXPLORER_ON_CHIP_TEMP_TWI_ACCESS_DISABLE == 0)
        /* enable on-chip channel on TWI expander */
        temp_sensor_driver_twi_expander_disable(TWI_SWITCH_CHANNEL_ID_4);
#endif
    }

    /* restore the original setting for master port recovery */
    twi_mst_init_recovery_en_set(EXP_TWI_MASTER_PORT, mst_init_recovery_en);

    /* initialize the on-chip temp sensor for continuous measurement */
    return (pvt_ts_init(PVT_TS_CLK_FREQ_5M, PVT_TS_MEASURE_TYPE_CONTINUOUS));
}

/**
* @brief
*   Update the IBM registers with the on board and on chip temperature sensor data
*
* @return
*   None.
*
*/
PUBLIC VOID temp_sensor_plat_update(VOID)
{
    UINT16 dimm0_temp;
    UINT16 dimm1_temp;
    UINT8 twi_data_buffer[2];
    PMCFW_ERROR rc;
    top_plat_lock_struct lock_struct;

    /* Update if the flag is set */
    if (temperature_update_flags & TEMP_SENSOR_UPDATE_FLAG)
    {
        if (temp_sensor_onboard_dimm0_config.present)
        {
            memset(&twi_data_buffer, 0, sizeof(twi_data_buffer));

            rc = temp_sensor_driver_plat_jedec_read(temp_sensor_onboard_dimm0_config.twi_addr, 
                                                    temp_sensor_onboard_dimm0_config.twi_reg_offset, 
                                                    temp_sensor_onboard_dimm0_config.twi_read_size, 
                                                    twi_data_buffer);

            if (rc == PMC_SUCCESS)
            {
                dimm0_temp = twi_data_buffer[0] | (twi_data_buffer[1] << UINT8_BITS);

                /* disable interrupts and disable multi-VPE operation */
                top_plat_critical_region_enter(&lock_struct);

                /* Update the OCMB thermal data register */
                ocmb_api_temp_dimm0_update(dimm0_temp, TRUE, TRUE, FALSE);

                /* restore interrupts and enable multi-VPE operation */
                top_plat_critical_region_exit(lock_struct);
            }
            else
            {
                /* disable interrupts and disable multi-VPE operation */
                top_plat_critical_region_enter(&lock_struct);

                /* Update the OCMB thermal data register with error bit */
                ocmb_api_temp_dimm0_update(0, FALSE, TRUE, TRUE);

                /* restore interrupts and enable multi-VPE operation */
                top_plat_critical_region_exit(lock_struct);
            }
        }
        else
        {
            /* DIMM0 is not present */

            /* disable interrupts and disable multi-VPE operation */
            top_plat_critical_region_enter(&lock_struct);

            /* Update the OCMB thermal data register with present bit unset */
            ocmb_api_temp_dimm0_update(0, FALSE, FALSE, FALSE);

            /* restore interrupts and enable multi-VPE operation */
            top_plat_critical_region_exit(lock_struct);
        }

        if (temp_sensor_onboard_dimm1_config.present)
        {

            memset(&twi_data_buffer, 0, sizeof(twi_data_buffer));

            rc = temp_sensor_driver_plat_jedec_read(temp_sensor_onboard_dimm1_config.twi_addr, 
                                                    temp_sensor_onboard_dimm1_config.twi_reg_offset, 
                                                    temp_sensor_onboard_dimm1_config.twi_read_size, 
                                                    twi_data_buffer);

            if (rc == PMC_SUCCESS)
            {
                dimm1_temp = twi_data_buffer[0] | (twi_data_buffer[1] << UINT8_BITS);

                /* disable interrupts and disable multi-VPE operation */
                top_plat_critical_region_enter(&lock_struct);

                /* Update the OCMB thermal data register */
                ocmb_api_temp_dimm1_update(dimm1_temp, TRUE, TRUE, FALSE);

                /* restore interrupts and enable multi-VPE operation */
                top_plat_critical_region_exit(lock_struct);
            }
            else
            {
                /* disable interrupts and disable multi-VPE operation */
                top_plat_critical_region_enter(&lock_struct);

                /* Update the OCMB thermal data register with error bit */
                ocmb_api_temp_dimm1_update(0, FALSE, TRUE, TRUE);

                /* restore interrupts and enable multi-VPE operation */
                top_plat_critical_region_exit(lock_struct);
            }
        }
        else
        {
            /* DIMM1 is not present */

            /* disable interrupts and disable multi-VPE operation */
            top_plat_critical_region_enter(&lock_struct);

            /* Update the OCMB thermal data register with present bit unset */
            ocmb_api_temp_dimm1_update(0, FALSE, FALSE, FALSE);

            /* restore interrupts and enable multi-VPE operation */
            top_plat_critical_region_exit(lock_struct);
        }

        if (temp_sensor_onchip_config.present)
        {
#if (EXPLORER_ON_CHIP_TEMP_TWI_ACCESS_DISABLE == 0)
            UINT16 chip_temp;
            rc = temp_sensor_driver_plat_onchip_read(temp_sensor_onchip_config.twi_addr,
                                                     temp_sensor_onchip_config.reg_offset_mode,
                                                     temp_sensor_onchip_config.twi_reg_offset,
                                                     temp_sensor_onchip_config.num_twi_reads,
                                                     temp_sensor_onchip_config.twi_read_size,
                                                     &chip_temp);
#else
            /* get the on-chip temperature */
            INT16 chip_temp;
            rc = pvt_ts_centigrade_degree_get(&chip_temp);
#endif

            if (rc == PMC_SUCCESS)
            {
                /* disable interrupts and disable multi-VPE operation */
                top_plat_critical_region_enter(&lock_struct);

                /* Update the OCMB thermal data register */
                ocmb_api_temp_onchip_update(chip_temp, TRUE, TRUE, FALSE);

                /* restore interrupts and enable multi-VPE operation */
                top_plat_critical_region_exit(lock_struct);
            }
            else
            {
                /* disable interrupts and disable multi-VPE operation */
                top_plat_critical_region_enter(&lock_struct);

                /* Update the OCMB thermal data register with error bit */
                ocmb_api_temp_onchip_update(0, FALSE, TRUE, TRUE);

                /* restore interrupts and enable multi-VPE operation */
                top_plat_critical_region_exit(lock_struct);
            }
        }
        else
        {
            /* disable interrupts and disable multi-VPE operation */
            top_plat_critical_region_enter(&lock_struct);

            /* Update the OCMB thermal data register with present bit unset */
            ocmb_api_temp_onchip_update(0, FALSE, FALSE, FALSE);

            /* restore interrupts and enable multi-VPE operation */
            top_plat_critical_region_exit(lock_struct);
       }

        /* Clear temperature sensor update flag */
        temperature_update_flags &= ~TEMP_SENSOR_UPDATE_FLAG;
    }
}
/** @} end group */



