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
* @addtogroup ECH
* @{
* @file
* @brief
*   Explorer Command Handler.
*
* @note
*/

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_api.h"
#include "pmc_profile.h"
#include "ech.h"
#include "ech_loc.h"
#include "mem.h"
#include "crc32.h"
#include "twi.h"
#include "twi_api.h"
#include "app_fw.h"
#include "top_exp_config_guide.h"
#include "ocmb_config_guide.h"
#include "ocmb_config_guide_mchp.h"
#include <string.h>
#include "serdes_plat.h"
#include "pmc_plat.h"

/*
* Local Enumerated Types
*/


/*
** Local Constants
*/
#define ECH_REG_64_BIT_MASK                 0xFF000000
#define ECH_REG_64_BIT_ADDRESS              0xA8000000
#define ECH_TWI_DUMMY_DATA                  0xFF


/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/
PUBLIC PMCFW_ERROR ech_twi_slv_cmd_rx(UINT32 port_id, UINT8* rx_buf_ptr, UINT32* rx_len_ptr, UINT32* actvity_ptr);
PUBLIC VOID ech_twi_status_proc(UINT32 port_id);
PUBLIC UINT32 ech_twi_boot_config_proc(UINT8* rx_buf, UINT32 rx_index);
PUBLIC VOID ech_twi_reg_addr_latch_proc(UINT8* rx_buf);
PUBLIC VOID ech_twi_reg_read_proc(UINT32 port_id, UINT8* rx_buf);
PUBLIC VOID ech_twi_reg_write_proc(UINT8* rx_buf);
PUBLIC VOID ech_twi_command_id_set(UINT8 id);
EXTERN VOID ech_twi_plat_slave_proc(UINT32 port_id, UINT8 *rx_buf_ptr, UINT8 rx_index);


/*
** Global Variables
*/
PUBLIC ech_twi_deferred_cmd_handler_struct ech_def_handler;

PUBLIC UINT32 g_count_exp_fw_twi_cmd_status    = 0;
PUBLIC UINT32 g_exp_fw_twi_cmd_boot_config     = 0;
PUBLIC UINT32 g_exp_fw_twi_cmd_reg_addr_latch  = 0;
PUBLIC UINT32 g_exp_fw_twi_cmd_reg_read        = 0;
PUBLIC UINT32 g_exp_fw_twi_cmd_reg_write       = 0;
PUBLIC UINT8 ech_twi_deferred_cmd_buf[EXP_TWI_MAX_BUF_SIZE];
/* TWI status byte recording result of last TWI command */
PUBLIC UINT8 ech_twi_status_byte = EXP_TWI_SUCCESS;

/*TWI CMD ID*/
PUBLIC UINT8 twi_cmd_id = EXP_FW_TWI_CMD_NULL;

/* TWI FW mode*/
PUBLIC UINT8 ech_twi_fw_mode_byte = EXP_TWI_STATUS_RUN_TIME;


/* TWI RX/TX data management */
PUBLIC UINT32 ech_twi_rx_len = 0;
PUBLIC UINT32 ech_twi_rx_index = 0;
PUBLIC UINT8 ech_twi_rx_buf[EXP_TWI_MAX_BUF_SIZE];
PUBLIC UINT8 ech_twi_tx_buf[EXP_TWI_MAX_BUF_SIZE];

/* Product Qualification Mode */
PUBLIC BOOL ech_pqm_mode = FALSE;


/* TWI activity tracker */
PUBLIC UINT32 twi_activity = TWI_SLAVE_ACTIVITY_NONE;

/*
** Local Variables
*/


/* 
** lengths of TWI commands 
** these entries must be kept aligned with exp_twi_cmd_enum 
*/ 
PRIVATE UINT32 ech_twi_cmd_len[EXP_FW_TWI_CMD_MAX] = 
{
    0,                                                  /**< Null command */
    EXP_TWI_BOOT_CFG_CMD_LEN,                           /**< Boot config */
    EXP_TWI_STATUS_CMD_LEN,                             /**< Status */
    EXP_TWI_REG_ADDR_LATCH_CMD_LEN,                     /**< Address latch */
    EXP_TWI_REG_READ_CMD_LEN,                           /**< Register read */
    EXP_TWI_REG_WRITE_CMD_LEN,                          /**< Register write */
    EXP_TWI_FW_DOWNLOAD_CMD_LEN,                        /**< Firmware download */
    EXP_TWI_CONT_READ_CMD_LEN,                          /**< Continuous read */
    EXP_TWI_CONT_WRITE_CMD_LEN,                         /**< Continuous write */
    EXP_TWI_BYPASS_4SEC_TIMEOUT,                        /**< Skip the 4 sec wait */
    EXP_TWI_PQM_LANE_SET_CMD_LEN,                        /**< Set SerDes lane configuration */ 
    EXP_TWI_PQM_LANE_GET_CMD_LEN,                        /**< Get SerDes lane configuration */
    EXP_TWI_PQM_FREQ_SET_CMD_LEN,                        /**< Set SerDes clock frequency */
    EXP_TWI_PQM_FREQ_GET_CMD_LEN,                        /**< Get SerDes clock frequency */
    EXP_TWI_PQM_LANE_TRAIN_CMD_LEN,                      /**< Initiate SerDes training */
    EXP_TWI_PQM_LANE_RETRAIN_CMD_LEN,                    /**< Reset SerDes training */
    EXP_TWI_PQM_RX_ADAPAT_OBJ_START_CMD_LEN,             /**< Start RX adaptation object */
    EXP_TWI_PQM_RX_ADAPAT_OBJ_READ_CMD_LEN,              /**< Read RX adaptation object */
    EXP_TWI_PQM_RX_CALIB_VALUE_START_CMD_LEN,            /**< Start RX calibration read */
    EXP_TWI_PQM_RX_CALIB_VALUE_READ_CMD_LEN,             /**< Read RX calibration read */
    EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_START_CMD_LEN,    /**< Start CSU calibration status */
    EXP_TWI_PQM_CSU_CALIB_VALUE_STATUS_READ_CMD_LEN,     /**< Read CSU calibration status */
    EXP_TWI_PQM_PRBS_PATTERN_MODE_SET_CMD_LEN,           /**< Set SerDes pattern generator mode */
    EXP_TWI_PQM_PRBS_USER_PATTERN_SET_CMD_LEN,           /**< Set user defined SerDes pattern generator */
    EXP_TWI_PQM_PRBS_MONITOR_CONTROL_CMD_LEN,            /**< Enable SerDes PRBS monitoring engine */
    EXP_TWI_PQM_PRBS_GENERATOR_CONTROL_CMD_LEN,          /**< Enable SerDes PRBS generation engine */
    EXP_TWI_PQM_PRBS_ERR_COUNT_START_CMD_LEN,            /**< Start SerDes error count register */
    EXP_TWI_PQM_PRBS_ERR_COUNT_READ_CMD_LEN,             /**< Read SerDes error count register */
    EXP_TWI_PQM_HORZ_BATHTUB_GET_START_CMD_LEN,          /**< Read horizontal bathtub values */
    EXP_TWI_PQM_HORZ_BATHTUB_GET_READ_CMD_LEN,           /**< Start vertical bathtub values */
    EXP_TWI_PQM_VERT_BATHTUB_GET_START_CMD_LEN,          /**< Read vertical bathtub values */
    EXP_TWI_PQM_VERT_BATHTUB_GET_READ_CMD_LEN,           /**< Start 2d bathtub values */
    EXP_TWI_PQM_TWOD_BATHTUB_GET_START_CMD_LEN,          /**< Read 2d bathtub values */
    EXP_TWI_PQM_VERT_BATHTUB_GET_READ_CMD_LEN

};


/*
* PRIVATE Functions
*/


/*
* Public Functions
*/

/**
* @brief
*   Get the TWI status byte
* @params
*   None
*   
* @return
*   Nothing
*
* @note
*/
PUBLIC UINT8 ech_twi_status_byte_get(VOID)
{
    return ech_twi_status_byte;
}


/**
* @brief
*   Set Deferred TWI command processing structure 
* @param [in] cmd_id            - Command ID
* @param [in] func_ptr          - Pointer to deferred command handler
* @param [in] buf               - reference to received data buffer
* @param [in] callback_func_ptr - Callback function pointer
* @param [in] len               - Command length
* @param [in] rx_index          - index in buffer of start of received PQM command
*   
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_deferred_cmd_processing_struct_set(exp_twi_cmd_enum cmd_id, 
                                                        UINT32 (*func_ptr)(UINT8 *, UINT32), 
                                                        VOID *buf, 
                                                        VOID (*callback_func_ptr) (UINT8),
                                                        UINT32 len,
                                                        UINT32 rx_index)
{
    ech_def_handler.command_id = cmd_id;
    ech_def_handler.cmd_buf = buf;
    ech_def_handler.cmd_buf_idx = rx_index;
    ech_def_handler.callback_handler = callback_func_ptr;
    ech_def_handler.deferred_cmd_handler = func_ptr;
    ech_def_handler.deferred_cmd_flag = TRUE;

    /* 
    ** Since this command will be handled in VPE0,
    ** increase rx index to allow the processing
    ** of other I2C command.
    ** 
    */
    ech_twi_rx_index_inc(len);
}

/**
* @brief
*   Check for any commands from host on TWI slave interface.
*
* @param [in]  port_id         - TWI port to check for activity
* @param [out] rx_buf_ptr      - Pointer to receive data buffer
* @param [out] rx_len_ptr      - Pointer containing receive data length
* @param [out] activity_ptr    - pointer containing various TWI states
*
* @return
*   PMC_SUCCESS - For SUCCESS, otherwise error code
*   
*
* @note
*/
PUBLIC PMCFW_ERROR ech_twi_slv_cmd_rx(UINT32 port_id,
                                       UINT8* rx_buf_ptr,
                                       UINT32* rx_len_ptr,
                                       UINT32* activity_ptr)
{
    UINT32 data_len;
    PMCFW_ERROR rc;

    /* check for any TWI activity */
    rc = twi_slv_poll(port_id, activity_ptr);

    if (PMC_SUCCESS != rc)
    {
        /* port not configured as a slave */
        PMCFW_ASSERT(FALSE, EXP_TWI_SLAVE_PORT_CFG_ERR);
    }

    if (TWI_SLAVE_ACTIVITY_NONE == *activity_ptr)
    {
        /* no activity on TWI slave port */
        return (rc);
    }

    /* check for RX / TX errors */
    if ((*activity_ptr & TWI_SLAVE_ACTIVITY_RX_ERROR) ||
        (*activity_ptr & TWI_SLAVE_ACTIVITY_TX_ERROR))
    {
        return (rc);
    }

    /* check for received data */
    if (*activity_ptr & TWI_SLAVE_ACTIVITY_RX_REQ)
    {
        /*
        ** data has been received but this does not mean that an entire command
        ** has been received, firmware must manage command data spanning
        ** multiple polling loops
        */

        /* get the received data length */
        rc = twi_slv_data_len_get(port_id, &data_len);
        if (rc != PMC_SUCCESS)
        {
            return (rc);
        }

        if (data_len > EXP_TWI_MAX_BUF_SIZE)
        {
            return (rc);
        }

        /* get the data */
        rc = twi_slv_data_get(port_id, &rx_buf_ptr[*rx_len_ptr], data_len);
        if (rc != PMC_SUCCESS)
        {
            return (rc);
        }

        /* add the received bytes to the received data length */
        *rx_len_ptr += data_len;
    }

    return (rc);
}

/**
* @brief
*   Set the TWI command ID
* @param [in]  id - Command ID from HOST 
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_command_id_set(UINT8 id)
{
    twi_cmd_id = id;
}

/**
* @brief
*   Get the TWI command ID
* @params
*   None
* @return
*   TWI Command ID
*
* @note
*/
PUBLIC UINT8 ech_twi_command_id_get(void)
{
    return twi_cmd_id;
}

/**
* @brief
*   Process TWI status command.
*
* @param [in] port_id - TWI port to send data
*
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_status_proc(UINT32 port_id)
{
    /* prepare the status response */

    /* response length */
    ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_STATUS_RSP_DATA_LEN;

    /*
    ** send 32-bit response MSB first:
    **  bits 24-31: Extended error code
    **  bits 23-18: reserved
    **  bits 17-16: boot stage
    **  bits 15-8:  status byte
    **  bits 7-0:   status command code
       */

    /* reserved */
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] = ech_extended_error_code_get();

    /* boot mode */
    /* fixed value for now */
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 1] = ech_twi_fw_mode_get(); 

    /* status code */
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 2] = ech_twi_status_byte_get();

    /* command ID */
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 3] = ech_twi_command_id_get();

    /* send the response */
    twi_slv_data_put(port_id,
                     ech_twi_tx_buf,
                     EXP_TWI_STATUS_RSP_LEN);

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_STATUS_CMD_LEN);

} /* ech_twi_status_proc */


/**
* @brief
*   Process TWI Register Address Latch command.
*
* @param [in] rx_buf - received data to process
*
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_reg_addr_latch_proc(UINT8* rx_buf)
{
    /* record the register address */
    UINT32 reg_addr = (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 0] << 24) |
                      (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 1] << 16) |
                      (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] << 8) |
                      (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 3]);

    /* validate the register address */
    if (FALSE == ech_reg_addr_validate(reg_addr))
    {
        /* invalid/restricted address */
        /* latch the "address" */
        ech_latched_reg_addr_set(EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE);

        /* failure status */
        ech_twi_status_byte = EXP_TWI_ERROR;
    }
    else
    {
        /* latch the address */
        ech_latched_reg_addr_set(reg_addr);

        /* set success status */
        ech_twi_status_byte = EXP_TWI_SUCCESS;
    }

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_REG_ADDR_LATCH_CMD_LEN);

} /* ech_twi_reg_addr_latch_proc */

/**
* @brief
*   Process TWI Register Read command.
*
* @param [in] port_id - TWI port ID
* @param [in] rx_buf  - received data to process
*
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_reg_read_proc(UINT32 port_id, UINT8* rx_buf)
{
    UINT32 latched_reg_addr = ech_latched_reg_addr_get();

    if ((EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE == latched_reg_addr) ||
        (EXP_TWI_REG_RW_ADDR_PROHIBITED == latched_reg_addr))
    {
        /* invalid register address */
        ech_twi_status_byte = latched_reg_addr;

        /* prepare errant value */
        ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_REG_READ_RSP_DATA_LEN;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] =     0xBA;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 1] = 0xDB;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 2] = 0x10;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 3] = 0x0D;
    }
    else
    {
        UINT32 *reg_ptr = (UINT32 *)latched_reg_addr;

        /* prepare the response */
        ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_REG_READ_RSP_DATA_LEN;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] =     (*reg_ptr >> 24) & 0xFF;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 1] = (*reg_ptr >> 16) & 0xFF;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 2] = (*reg_ptr >> 8) & 0xFF;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 3] = (*reg_ptr >> 0) & 0xFF;

        /* set success status */
        ech_twi_status_byte = EXP_TWI_SUCCESS;
    }

    /* send the response */
    twi_slv_data_put(port_id,
                     ech_twi_tx_buf,
                     EXP_TWI_REG_READ_RSP_LEN);

    /* invalidate the latched address */
    ech_latched_reg_addr_set(EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE);

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_REG_READ_CMD_LEN);

} /* ech_twi_reg_read_proc */

/**
* @brief
*   Process TWI Register Write command.
*
* @param [in] rx_buf - received data to process
*
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_reg_write_proc(UINT8* rx_buf)
{
    UINT32 reg_addr;
    UINT32 reg_data;

    /* prepare the register address  response */
    reg_addr = (rx_buf[EXP_TWI_CMD_DATA_OFFSET]     << 24) |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 1] << 16) |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] << 8)  |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 3]);

    /* validate the register address */
    if (FALSE == ech_reg_addr_validate(reg_addr))
    {
        /* invalid/restricted address */
        ech_twi_status_byte = EXP_TWI_ERROR;
    }
    else
    {
        /* prepare the data */
        reg_data = (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 4] << 24) |
                   (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 5] << 16) |
                   (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 6] << 8)  |
                   (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 7]);

        /* write the data */
         *(UINT32*)reg_addr = reg_data;


        /* 
        ** If the address space is in IBM 64 BIT register location
        ** by-pass the read check because write will only take effect
        ** when complete 64 BIT data have been written.
        */
        if((reg_addr & ECH_REG_64_BIT_MASK) == ECH_REG_64_BIT_ADDRESS)
        {            
            /* set success status */
            ech_twi_status_byte = EXP_TWI_SUCCESS;
        }
        else
        {
            /* confirm the write succeeded */
            if (*(UINT32*)reg_addr == reg_data)
            {
                /* set success status */
                ech_twi_status_byte = EXP_TWI_SUCCESS;
            }
            else
            {
                /* set failure status */
                ech_twi_status_byte = EXP_TWI_ERROR;
            }
        }
    }

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_REG_WRITE_CMD_LEN);

} /* ech_twi_reg_write_proc */

/*
* Public Functions
*/

/**
* @brief
*   Initialize the TWI interfaces for ECH.
*
* @param [in] mst_port - TWI master port
* @param [in] slv_port - TWI slave port
* @param [in] slv_addr - TWI slave address
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_init(UINT32 mst_port, UINT32 slv_port, UINT32 slv_addr)
{
    twi_port_cfg_struct mst_cfg;
    twi_port_cfg_struct slv_cfg;

    /* Initialize the structures with 0*/
    memset((void *)&mst_cfg, 0, sizeof(mst_cfg));
    memset((void *)&slv_cfg, 0, sizeof(slv_cfg));
    
    /* initialize the TWI module */
    twi_init(TWI_OPER_MODE_POLLING);

    /* configure the TWI slave port */
    slv_cfg.slave_cfg.tx_buf_len = EXP_TWI_MAX_BUF_SIZE;
    slv_cfg.slave_cfg.addr = slv_addr;
    slv_cfg.slave_cfg.addr_size = EXP_TWI_ADDR_SIZE;
    slv_cfg.slave_cfg.cback = NULL;
    slv_cfg.slave_cfg.cback_data_ptr = NULL;
    slv_cfg.mst_init_line_reset_en = TRUE;
    slv_cfg.mst_init_recovery_en = FALSE;
    slv_cfg.proto = TWI_PROTO_NONE;
    slv_cfg.ms_mode = TWI_PORT_MS_MODE_SLAVE;

    twi_port_init(slv_port, &slv_cfg);

    /* configure the master TWI port */
    mst_cfg.clk_rate = TWI_CLK_RATE_400KHZ;
    mst_cfg.mst_callback_fptr = NULL;
    mst_cfg.mst_callback_data_ptr = NULL;
    mst_cfg.mst_init_line_reset_en = TRUE;
    mst_cfg.mst_init_recovery_en = FALSE;
    mst_cfg.proto = TWI_PROTO_NONE;
    mst_cfg.ms_mode = TWI_PORT_MS_MODE_MASTER;

    twi_port_init(mst_port, &mst_cfg);
}

/**
* @brief
*   Set the Exp FW Mode
* @param [in] fw_mode_byte - FW mode ( APP FW, BOOTROM etc.)
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_fw_mode_set(UINT8 fw_mode_byte)
{
    ech_twi_fw_mode_byte = fw_mode_byte;
}

/**
* @brief
*   Get the Exp FW Mode
* @params
*   None
* @return
*   FW mode
*
* @note
*/
PUBLIC UINT8 ech_twi_fw_mode_get(void)
{
    return ech_twi_fw_mode_byte;
}



/**
* @brief
*   Set the TWI status byte
* @param [in] status_byte - FW Status BYTE
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_status_byte_set(UINT8 status_byte)
{
    ech_twi_status_byte = status_byte;
}

/**
* @brief
*   Return a pointer to the TWI transmit buffer
* @params
*   None
* @return
*   a pointer to the TWI transmit buffer
*
* @note
*/
PUBLIC UINT8* ech_twi_tx_buf_get(VOID)
{
    return (ech_twi_tx_buf);
}

/**
* @brief
*   Increment the TWI receive index, based on the amount of data
*   processed from receive buffer
* @param [in] bytes_processed - Number of Bytes processed from the command buffer
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_rx_index_inc(UINT32 bytes_processed)
{
    ech_twi_rx_index += bytes_processed;
}

/**
* @brief
*   Process TWI slave interface to the host.
*
* @param [in] port_id - TWI port to check for activity
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_slave_proc(UINT32 port_id)

{
    static BOOL dummy_data_send_flag = TRUE;
    /* check TWI interface for any activity */
    if (PMC_SUCCESS != ech_twi_slv_cmd_rx(port_id, ech_twi_rx_buf, &ech_twi_rx_len, &twi_activity))
    {
        /* TO DO: implement error handling */
        return;
    }

    if ((twi_activity & TWI_SLAVE_ACTIVITY_RX_REQ ) != TWI_SLAVE_ACTIVITY_RX_REQ) 
    {
        /*
        ** The following code handles a race condition where
        ** HOST I2C master is waiting for data bytes and the I2C command
        ** is lost from BOOTROM to APP FW transition. In this scenario
        ** the I2C state machine is stuck in RD request, causing clock
        ** stretching in the I2C bus. To overcome this, the reasonable 
        ** workaround is to send dummy data bytes(0xFF) to HOST till it
        ** satisfies the HOST command response length. After that, the
        ** dummy_data_send_flag is set to FALSE and FW will never send
        ** dummy bytes.
        */
        if (dummy_data_send_flag == TRUE) 
        {
            twi_port_dummy_data_send(port_id, (UINT8)ECH_TWI_DUMMY_DATA);
        }
        return;
    }

    while((ech_twi_rx_len - ech_twi_rx_index) > 0)
    {
        /*
        ** First check that the command is in valid range before accessing the array to find out desired command length.
        ** If controller has not received the full packet, just return and wait for further TWI_SLAVE_ACTIVITY_RX_REQ.
        ** The illegal / unsupported commands will be handled as a part of default case.
        */ 
        if ((ech_twi_rx_buf[ech_twi_rx_index] < EXP_FW_TWI_CMD_MAX) &&
            (ech_twi_rx_len - ech_twi_rx_index) < ech_twi_cmd_len[ech_twi_rx_buf[ech_twi_rx_index]])
        {
            return;
        }

        /* 
        ** Store the in-process command so that proper command ID can be returned 
        ** for the FW STATUS command
        */ 
        if(ech_twi_rx_buf[ech_twi_rx_index] != EXP_FW_TWI_CMD_STATUS )
        {
            ech_twi_command_id_set(ech_twi_rx_buf[ech_twi_rx_index]);
        }

        /*
        ** If the FW is here, it means, we have satisfied missing read request
        ** for the lost command. So, reset dummy_data_send_flag.
        */
        dummy_data_send_flag = FALSE;
        
        ech_twi_plat_slave_proc(port_id, ech_twi_rx_buf , ech_twi_rx_index);

        if ((ech_twi_rx_index != 0) &&
            (ech_twi_rx_len != 0) &&
            (ech_twi_rx_index == ech_twi_rx_len))
        {
            /*
            ** all data in receive buffer has been processed
            ** invalidate the receive buffer and
            ** reset received length and processing index
            */
            ech_twi_rx_buf[0] = 0xFF;
            ech_twi_rx_len = 0;
            ech_twi_rx_index = 0;
        }
        
    } /* ech_twi_slave_proc() */
}

