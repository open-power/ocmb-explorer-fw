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
#include "ech_twi_common.h"
#include "top_plat.h"
#include "ocmb.h"
#include "wdt.h"
#include "serdes_cg_supplement.h"
#include "fw_version_info.h"
#include "ccb_api.h"
#include "char_io.h"
#include "app_fw_ddr.h"


/*
* Local Enumerated Types
*/


/*
** Local Constants
*/
#define ECH_REG_64_BIT_MASK                 0xFF000000
#define ECH_TWI_DUMMY_DATA                  0xFF
#define ECH_OCMB_INVALID_ADDR               0xFFFFFFFF


/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/
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
PRIVATE UINT8 ech_twi_status_byte = EXP_TWI_SUCCESS;

/*TWI CMD ID*/
PUBLIC UINT8 twi_cmd_id = EXP_FW_TWI_CMD_NULL;

/* TWI FW mode*/
PUBLIC UINT8 ech_twi_fw_mode_byte = EXP_TWI_STATUS_RUN_TIME |
                                    (EXP_TWI_STATUS_FW_API_VERSION_BITMSK & (FW_API_VERSION_NUMBER << EXP_TWI_STATUS_FW_API_VERSION_BITOFF));

/* TWI RX/TX data management */
PUBLIC UINT32 ech_twi_rx_len = 0;
PUBLIC UINT32 ech_twi_rx_index = 0;
PUBLIC UINT8 ech_twi_rx_buf[EXP_TWI_MAX_BUF_SIZE];
PUBLIC UINT8 ech_twi_tx_buf[EXP_TWI_MAX_BUF_SIZE];


/* TWI activity tracker */
PUBLIC UINT32 twi_activity = TWI_SLAVE_ACTIVITY_NONE;


/*
** Local Variables
*/

/* poll abort flag used to exit infinite loops in config guide code */
PRIVATE BOOL ech_twi_poll_abort_flag = TRUE;

#if (EXPLORER_DDR_TRAIN_PARMS_SAVE_DISABLE == 0)
/* Variable to keep track of the read position in subsequent calls to ech_twi_read_saved_ddr_params */
PRIVATE UINT32 read_ddr_params_offset = 0;
#endif

/*
** see EBCF-10490
** when TWI writes are into 64-bit OCMB memory space, the first 32-bit
** TWI register write is buffered and then written as two back-to-back
** writes that will be atomic as the code is running single threaded
** IBM uses "left" for upper 32-bits and "right" for lower 32-bits of 64-bit
** registers
*/
PRIVATE UINT32 ocmb_reg_left_addr = ECH_OCMB_INVALID_ADDR;
PRIVATE UINT32 ocmb_reg_left_data;


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
    EXP_TWI_PQM_HORZ_BATHTUB_GET_START_CMD_LEN,          /**< Start horizontal bathtub values */
    EXP_TWI_PQM_HORZ_BATHTUB_GET_READ_CMD_LEN,           /**< Read horizontal bathtub values */
    EXP_TWI_PQM_VERT_BATHTUB_GET_START_CMD_LEN,          /**< Start vertical bathtub values */
    EXP_TWI_PQM_VERT_BATHTUB_GET_READ_CMD_LEN,           /**< Read vertical bathtub values */
    EXP_TWI_PQM_TWOD_BATHTUB_GET_START_CMD_LEN,          /**< Start 2D bathtub values */
    EXP_TWI_PQM_TWOD_BATHTUB_GET_READ_CMD_LEN,           /**< Read 2D bathtub values */
    EXP_TWI_PQM_DELAY_LINE_UPDATE_CMD_LEN,               /**< Delay line update */
    EXP_TWI_POLL_ABORT_CMD_LEN,                          /**< Abort command */
    EXP_TWI_FFE_SETTINGS_CMD_LEN,                         /**< FFE settings */
    EXP_TWI_EXP_FW_CDR_OFFSET_FROM_CAL_SET_CMD_LEN,      /**< Change the CDR offset from the calibrated value */
    EXP_TWI_EXP_FW_CDR_BANDWIDTH_SET_CMD_LEN,            /**< Change the CDR bandwidth prior to boot config 0 */
    EXP_TWI_EXP_FW_PRBS_CAL_STATUS_READ_CMD_LEN,         /**< Report further information about the PRBS cal that was performed */
    EXP_TWI_EXP_FW_CONT_SERDES_CAL_DISABLE_CMD_LEN,      /**< Disable / re-enable SerDes continuous calibration */
    EXP_TWI_EXP_FW_READ_ACTIVE_LOGS_CMD_LEN,             /**< Read active logs over TWI interface */
    EXP_TWI_EXP_FW_READ_SAVED_DDR_PARAMS_CMD_LEN         /**< Read DDR parameters that are saved in flash over the TWI interface */
};


/*
* PRIVATE Functions
*/


/*
* Public Functions
*/

/**
* @brief
*   Return the value of the extended poll abort flag This flag is set to TRUE by
*   the host to direct the Explorer firmware to break out of
*   infinite loops waiting on external signals. This flag is set
*   to FALSE before the code enters an infinite loop.
*
* @return
*   FALSE - code will continue in polling mode
*   TRUE - code will break out of polling mode
*
* @note
*/
PUBLIC BOOL ech_twi_poll_abort_flag_get(VOID)
{

#if (EXPLORER_WDT_DISABLE == 0)
    /*
    ** Kick the WDT in this function because this function
    ** is used to determine if the Host wants us to abort
    ** an infinite loop.  This means that this function is
    ** called by every infinite loop and we want to kick
    ** the WDT while in an infinite loop.
    */
    wdt_hardware_tmr_kick();
#endif

    return ech_twi_poll_abort_flag;

} /* ech_twi_poll_abort_flag_get */

/**
* @brief
*   Set extended polling abort flag. This flag is set to TRUE by
*   the host to direct the Explorer firmware to break out of
*   infinite loops waiting on external signals. This flag is set
*   to FALSE before the code enters an infinite loop.
*
* @return Nothing
*
* @note
*/
PUBLIC VOID ech_twi_poll_abort_flag_set(BOOL abort_flag)
{
    ech_twi_poll_abort_flag = abort_flag;

} /* ech_twi_poll_abort_flag_set*/

/**
* @brief
*   Get the TWI status byte. This byte constains the result of
*   the last TWI message received by the Explorer. Poissble
*   values are:
*       EXP_TWI_SUCCESS (0x00)
*       EXP_TWI_ERROR (0xC0)
*       EXP_TWI_UNSUPPORTED (0xD0)
*       EXP_TWI_BUSY (0xFE)
*
* @params
*   None
*
* @return
*   The TWI status byte.
*
* @note
*/
PUBLIC UINT8 ech_twi_status_byte_get(VOID)
{
    return ech_twi_status_byte;
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
                                                       UINT32 (*func_ptr)(UINT8*, UINT32),
                                                       VOID *buf,
                                                       VOID (*callback_func_ptr)(UINT8),
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
    ** Since this command will be handled by VPE0,
    ** increase rx index to allow the processing
    ** of other I2C commands by VPE1
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
    **  bits 23-19: reserved 
    **  bit 18:     serdes PRBS algorithm supported 
    **  bits 17-16: boot stage
    **  bits 15-8:  status byte
    **  bits 7-0:   status command code
       */

    /* extended error specific to detected error */
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

    /* presume latch succeeds, overwritten if error detected */
    ech_twi_status_byte_set(EXP_TWI_SUCCESS);

    if (OCMB_REGS_BASE_ADDR == (reg_addr & ECH_REG_64_BIT_MASK))
    {
        /* access into OCMB 64-bit memory address */

        if (FALSE == ocmb_reg_addr_valid(reg_addr))
        {
            /* invalid OCMB reg address */
            bc_printf("REG_ADDR_LATCH: Invalid OCMB address 0x%08x\n", reg_addr);

            /* set the extended error code */
            ech_extended_error_code_set(EXP_TWI_REG_RW_ADDR_INVALID);

            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_ERROR);
        }
        else if (TRUE == ocmb_reg_addr_write_only(reg_addr))
        {
            /* write only OCMB reg address */
            bc_printf("REG_ADDR_LATCH: Invalid access to Write-Only OCMB register 0x%08x\n", reg_addr);

            /* set the extended error code */
            ech_extended_error_code_set(EXP_TWI_REG_RW_WRITE_ONLY);

            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_ERROR);
        }
        else
        {
            /* valid OCMB address */

            /* set the extended error code */
            ech_extended_error_code_set(EXP_TWI_SUCCESS);

            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_SUCCESS);
        }

        /* valid or invalid, latch the address */
        ech_latched_reg_addr_set(reg_addr);
    }
    else
    {
        /* access to memory outside OCMB 64-bit address space */

        /* validate the register address */
        if (FALSE == ech_reg_addr_validate(reg_addr))
        {
            /* invalid/restricted address */ 

            /* set the extended error code */
            ech_extended_error_code_set(EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE);

            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_ERROR);
        }
        else
        {
            /* set the extended error code */
            ech_extended_error_code_set(EXP_TWI_SUCCESS);

            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_SUCCESS);
        }

        /* valid or invalid, latch the address */
        ech_latched_reg_addr_set(reg_addr);
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
    /* get the extended error code */
    UINT8 err_code = ech_extended_error_code_get();

    /* get the latched address */
    UINT32 latched_reg_addr = ech_latched_reg_addr_get();

    if (EXP_TWI_SUCCESS != err_code)
    {
        /* invalid register address, set the status byte */
        ech_twi_status_byte_set(EXP_TWI_ERROR);

        /* return errant address to host */
        ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_REG_READ_RSP_DATA_LEN;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] =     (latched_reg_addr >> 24) & 0xFF;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 1] = (latched_reg_addr >> 16) & 0xFF;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 2] = (latched_reg_addr >> 8)  & 0xFF;
        ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 3] = (latched_reg_addr >> 0)  & 0xFF;
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

        /* set status byte */
        ech_twi_status_byte_set(EXP_TWI_SUCCESS);
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

    /* prepare the register address */
    reg_addr = (rx_buf[EXP_TWI_CMD_DATA_OFFSET]     << 24) |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 1] << 16) |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] << 8)  |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 3]);

    /* prepare the register data */
    reg_data = (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 4] << 24) |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 5] << 16) |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 6] << 8)  |
               (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 7]);

    if (OCMB_REGS_BASE_ADDR == (reg_addr & ECH_REG_64_BIT_MASK))
    {
        /* access into OCMB 64-bit memory address */

        /* validate the OCMB address */
        if (FALSE == ocmb_reg_addr_valid(reg_addr))
        {
            /* invalid OCMB address */
            bc_printf("REG_WRITE: Invalid OCMB address 0x%08x\n", reg_addr);

            /* set the extended error code */
            ech_extended_error_code_set(EXP_TWI_REG_RW_ADDR_INVALID);

            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_ERROR);
        }
        else
        {
            /*
            ** see EBCF-10490
            ** when TWI writes are into 64-bit OCMB memory space, the first 32-bit
            ** TWI register write is buffered and then written as two back-to-back
            ** writes that will be atomic as the code is running single threaded
            ** IBM uses "left" for upper 32-bits and "right" for lower 32-bits of 64-bit
            ** registers
            */
            if (ECH_OCMB_INVALID_ADDR == ocmb_reg_left_addr)
            {
                /*
                ** first 32-bit register write of 64-bit write
                ** ensure the address is a left address (upper 32 bits)
                */
                if (TRUE == ocmb_left_address_validate(reg_addr))
                {
                    /* valid left address, record the address and data */
                    ocmb_reg_left_addr = reg_addr;
                    ocmb_reg_left_data = reg_data;

                    /* set status byte */
                    ech_twi_status_byte_set(EXP_TWI_SUCCESS);
                }
                else
                {
                    /* invalid left address */
                    bc_printf("REG_WRITE: Invalid Left OCMB address 0x%08x\n", reg_addr);

                    /* set the extended error code */
                    ech_extended_error_code_set(EXP_TWI_REG_OCMB_LEFT_ADDRESS_INVALID);

                    /* set status byte */
                    ech_twi_status_byte_set(EXP_TWI_ERROR);
                }
            }
            else
            {
                /*
                ** second 32-bit register write of 64-bit write
                ** ensure the address is a right address (lower 32 bits)
                */
                if (TRUE == ocmb_right_address_validate(reg_addr))
                {
                    /* valid right address */

                    /* disable interrupts and disable multi-VPE operation */
                    top_plat_lock_struct lock_struct;
                    top_plat_critical_region_enter(&lock_struct);

                    /* write the left data */
                    *(UINT32*)ocmb_reg_left_addr = ocmb_reg_left_data;

                    /* write the right data */
                    *(UINT32*)reg_addr = reg_data;

                    /* set status byte */
                    ech_twi_status_byte_set(EXP_TWI_SUCCESS);

                    /* restore interrupts and enable multi-VPE operation */
                    top_plat_critical_region_exit(lock_struct);

                    /* invalidate left address */
                    ocmb_reg_left_addr = ECH_OCMB_INVALID_ADDR;
                }
                else
                {
                    /* invalid right address */
                    bc_printf("REG_WRITE: Invalid Right OCMB address 0x%08x\n", reg_addr);

                    /* set the extended error code */
                    ech_extended_error_code_set(EXP_TWI_REG_OCMB_RIGHT_ADDRESS_INVALID);

                    /* set status byte */
                    ech_twi_status_byte_set(EXP_TWI_ERROR);
                }
            }
        }
    }
    else
    {
        /* write to 32-bit memory address */

        /* validate the register address */
        if (FALSE == ech_reg_addr_validate(reg_addr))
        {
            /* 
            ** invalid/restricted address 
            ** set status byte 
            */ 
            ech_twi_status_byte_set(EXP_TWI_ERROR);
        }
        else
        {
            /* write the data */
            *(UINT32*)reg_addr = reg_data;

            /* confirm the write succeeded */
            if (*(UINT32*)reg_addr == reg_data)
            {
                /* set status byte */
                ech_twi_status_byte_set(EXP_TWI_SUCCESS);
            }
            else
            {
                /* set status byte */
                ech_twi_status_byte_set(EXP_TWI_ERROR);
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
    mst_cfg.mst_init_recovery_en = TRUE;
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
*   Process the CDR_OFFSET_FROM_CAL_SET command
*   Adjusts the CDR offset from the calibrated value. Sends a
*   bitmask of the lanes adjusted on the TWI port
* @param [in] rx_buf_ptr  - received data to process  
* @param [in] rx_index - index in buffer of start of received
*                PQM command
* @param [in] port_id - TWI port ID
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_cdr_offset_from_cal_set(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id)
{
    UINT8 applied_bitmask;

    UINT32 rc;

    UINT8 lane_bitmask = rx_buf_ptr[rx_index + 2];
    INT8 cdr_index_offset = rx_buf_ptr[rx_index + 3];

    /* Check that PRBS cal was run */
    if (FALSE == ech_serdes_prbs_cal_state_get()) 
    {
        bc_printf("SerDes PRBS calibration must be run first\n");
        applied_bitmask = 0;

        /* set status byte */
        ech_twi_status_byte_set(EXP_TWI_ERROR);
    }
    else
    {
        rc = SERDES_FH_CDR_Offset_Force_Offset(lane_bitmask, cdr_index_offset, &applied_bitmask);

        if (PMC_SUCCESS != rc) 
        {
            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_ERROR);
        }
        else
        {
            /* set status byte */
            ech_twi_status_byte_set(EXP_TWI_SUCCESS);
        }
    }

    bc_printf("CDR Offset on lanes 0x%02x set to offset %i\n", applied_bitmask, cdr_index_offset);

    /* send the response */
    ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_EXP_FW_CDR_OFFSET_FROM_CAL_SET_RSP_DATA_LEN;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] = applied_bitmask;

    twi_slv_data_put(port_id,
                     ech_twi_tx_buf,
                     EXP_TWI_EXP_FW_CDR_OFFSET_FROM_CAL_SET_RSP_LEN);

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_EXP_FW_CDR_OFFSET_FROM_CAL_SET_CMD_LEN);
}

/**
* @brief
*   Process the CDR_BANDWIDTH_SET command
*   Adjusts the CDR bandwidth from the default value.
* @param [in] rx_buf_ptr  - received data to process  
* @param [in] rx_index - index in buffer of start of received
*                command
* @param [in] port_id - TWI port ID
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_cdr_bandwidth_set(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id)
{
    UINT8 prop_gain = rx_buf_ptr[rx_index + 2];
    UINT8 integ_gain = rx_buf_ptr[rx_index + 3];

    /* Store the prop and accum gains */
    if (ech_serdes_cdr_prop_gain_set(prop_gain) && ech_serdes_cdr_integ_gain_set(integ_gain)) 
    {
        bc_printf("CDR prop gain set to 0x%02x\nCDR integ gain set to 0x%02x\n", prop_gain, integ_gain);

        /* set status byte */
        ech_twi_status_byte_set(EXP_TWI_SUCCESS);
    }
    else
    {
        bc_printf("CDR_BANDWIDTH_SET: prop gain or accum gain is out of range\n");
        
        /* set status byte */
        ech_twi_status_byte_set(EXP_TWI_ERROR);
    }

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_EXP_FW_CDR_BANDWIDTH_SET_CMD_LEN);
}

/**
* @brief
*   Process the CONT_SERDES_CAL_DISABLE command. Disables /
*   enables the periodic serdes calibration. This should be
*   called before generating CDR BER bathtub curves after OMI
*   training is complete.
* @param [in] rx_buf_ptr  - received data to process  
* @param [in] rx_index - index in buffer of start of received
*                command
* @param [in] port_id - TWI port ID
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_continuous_serdes_cal_disable(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id)
{
    UINT8 cal_disable = rx_buf_ptr[rx_index + 2];

    if (cal_disable) 
    {
        bc_printf("Disable periodic SerDes calibration\n");

        /* Disable the continuous calibration */
        serdes_plat_cal_disable(TRUE);
    }
    else
    {
        bc_printf("Starting periodic SerDes calibration\n");

        /* Restart the continuous calibration */
        serdes_plat_cal_disable(FALSE);
    }

    /* set status byte */
    ech_twi_status_byte_set(EXP_TWI_SUCCESS);

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_EXP_FW_CONT_SERDES_CAL_DISABLE_CMD_LEN);
}

/**
* @brief
*   Process the PRBS_CAL_STATUS_READ command reads the
*   information stored during the PRBS calibration sequence.
*   This includes the PRBS monitor detected lane bitmask and cal
*   converged lane bitmask. Three reserved bytes are included
*   for future use.
* @param [in] rx_buf_ptr  - received data to process  
* @param [in] rx_index - index in buffer of start of received
*                command
* @param [in] port_id - TWI port ID
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_prbs_cal_status(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id)
{
    UINT8 pattmon_detected_bitmask = 0x00;
    UINT8 cal_converged_bitmask = 0x00;
    UINT8 reserved_byte_0 = 0x00;
    UINT8 reserved_byte_1 = 0x00;
    UINT8 reserved_byte_2 = 0x00;

    /* Check that PRBS cal was run */
    if (FALSE == ech_serdes_prbs_cal_state_get()) 
    {
        bc_printf("SerDes PRBS calibration must be run first\n");

        /* set status byte */
        ech_twi_status_byte_set(EXP_TWI_ERROR);
    }
    else
    {
        ech_serdes_prbs_cal_data_get(&pattmon_detected_bitmask, &cal_converged_bitmask);

        /* set status byte */
        ech_twi_status_byte_set(EXP_TWI_SUCCESS);
    }

    bc_printf("PRBS detected lane bitmask: 0x%02x, Cal converged lane bitmask: 0x%02x\n", pattmon_detected_bitmask, cal_converged_bitmask);

    /* send the response */
    ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = EXP_TWI_EXP_FW_PRBS_CAL_STATUS_READ_RSP_DATA_LEN;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] = pattmon_detected_bitmask;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 1] = cal_converged_bitmask;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 2] = reserved_byte_0;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 3] = reserved_byte_1;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 4] = reserved_byte_2;

    twi_slv_data_put(port_id,
                     ech_twi_tx_buf,
                     EXP_TWI_EXP_FW_PRBS_CAL_STATUS_READ_RSP_LEN);

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_EXP_FW_PRBS_CAL_STATUS_READ_CMD_LEN);
}

/**
* @brief
*   Process the READ_ACTIVE_LOGS command
*   Reads the active log from ram and passes it back over the
*   TWI interface in pieces. Indicates to the host through the
*   log_continues variable if there is more log to be read.
* @param [in] rx_buf_ptr  - received data to process  
* @param [in] rx_index - index in buffer of start of received
*                command
* @param [in] port_id - TWI port ID
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_read_active_logs(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id)
{

    UINT32 bytes_copied;
    UINT8 log_continues;

    void*  ccb_log_ptr;
    void*  ccb_ctrl_ptr;

    /* Only the runtime log can be read */
    UINT8 channel_id = CHAR_IO_CHANNEL_ID_RUNTIME;

    /* get the CCB control pointer */
    char_io_loc_buffer_info_get(channel_id, &ccb_log_ptr);
    ccb_ctrl_ptr = char_io_ccb_ctrl_get(channel_id);

    if (NULL == ccb_log_ptr)
    {
        /* no active log defined */
        bc_printf("Error: No active log defined\n");

        bytes_copied = 0;
        log_continues = 0;
        ech_twi_status_byte_set(EXP_TWI_ERROR);
    }
    else
    {
        /* Copy data from the log to the TWI buffer */
        bytes_copied = ccb_get(ccb_ctrl_ptr, (CHAR*) (&ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 1]), EXP_TWI_EXP_FW_READ_ACTIVE_LOGS_RSP_DATA_LEN);

        /* Check if there is more to be sent */
        (bytes_copied < EXP_TWI_EXP_FW_READ_ACTIVE_LOGS_RSP_DATA_LEN) ? (log_continues = 0) : (log_continues = 1);

        ech_twi_status_byte_set(EXP_TWI_SUCCESS);
    }

    /* send the response */
    ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = bytes_copied + 1;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] = log_continues;

    twi_slv_data_put(port_id,
                     ech_twi_tx_buf,
                     EXP_TWI_EXP_FW_READ_ACTIVE_LOGS_RSP_LEN);

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_EXP_FW_READ_ACTIVE_LOGS_CMD_LEN);
}

/**
* @brief
*   Process the EXP_FW_READ_SAVED_DDR_PARAMS command
*   Reads the DDR parameters saved in flash and passes them
*   over the TWI interface in pieces. Indicates to the host
*   through the data_continues variable if there is more data to
*   be read.
* @param [in] rx_buf_ptr  - received data to process  
* @param [in] rx_index - index in buffer of start of received
*                command
* @param [in] port_id - TWI port ID
* @return
*   nothing
*
* @note
*/
PUBLIC VOID ech_twi_read_saved_ddr_params(UINT8* rx_buf_ptr, UINT32 rx_index, UINT32 port_id)
{
#if (EXPLORER_DDR_TRAIN_PARMS_SAVE_DISABLE == 0)
    UINT32 bytes_copied;
    UINT8 data_continues;

    UINT8 start_read = rx_buf_ptr[rx_index + 2];

    /* Reset the offset on the first read */
    if (start_read) 
    {
        read_ddr_params_offset = 0;
    }

    /* Copy a section of the parameters to the rsp buffer */
    UINT32 rc = app_fw_ddr_calibration_read(read_ddr_params_offset, 
                                            EXP_TWI_EXP_FW_READ_SAVED_DDR_PARAMS_RSP_DATA_LEN, 
                                            (VOID*) (&ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET + 1]), 
                                            &bytes_copied);

    if (PMC_SUCCESS == rc) 
    {
        /* Keep track of the last offset read */
        (bytes_copied < EXP_TWI_EXP_FW_READ_SAVED_DDR_PARAMS_RSP_DATA_LEN) ? (data_continues = 0) : (data_continues = 1);

        read_ddr_params_offset += bytes_copied;

        ech_twi_status_byte_set(EXP_TWI_SUCCESS);

    }
    else
    {
        bytes_copied = 0;
        data_continues = 0;

        ech_twi_status_byte_set(EXP_TWI_ERROR);
    }

    ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = bytes_copied + 1;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] = data_continues;

#else
    /*
    ** set the status byte 
    ** set the response data length to 1 (no data but 1 byte for data continues field)
    ** set the data continues byte to zero as no more data is available
    */
    ech_twi_status_byte_set(EXP_TWI_SUCCESS);
    ech_twi_tx_buf[EXP_TWI_RSP_LEN_OFFSET] = 1;
    ech_twi_tx_buf[EXP_TWI_RSP_DATA_OFFSET] = 0;

#endif /* EXPLORER_DDR_TRAIN_PARMS_SAVE_DISABLE */

    /* send the response */
    twi_slv_data_put(port_id,
                     ech_twi_tx_buf,
                     EXP_TWI_EXP_FW_READ_SAVED_DDR_PARAMS_RSP_LEN);

    /* increment receive buffer index */
    ech_twi_rx_index_inc(EXP_TWI_EXP_FW_READ_SAVED_DDR_PARAMS_CMD_LEN);
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

    while(ech_twi_rx_len > ech_twi_rx_index)
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

