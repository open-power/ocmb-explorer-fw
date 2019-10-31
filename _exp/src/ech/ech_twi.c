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
#include <string.h>
#include "serdes_plat.h"
#include "pmc_plat.h"

/*
* Local Enumerated Types
*/


/*
** Local Constants
*/
#define ECH_OCMB_TRAINING_COMPLETE          0x00100000
#define ECH_OCMB_TRAINING_SM_MASK           0x00007000
#define ECH_OCMB_TRAINING_SM_COMPLETE       0x00007000
#define ECH_OCMB_TRAINING_DELAY_500_US      500
#define ECH_REG_64_BIT_MASK                 0xFF000000
#define ECH_REG_64_BIT_ADDRESS              0xA8000000



/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/
PRIVATE PMCFW_ERROR ech_twi_slv_cmd_rx(UINT32 port_id, UINT8* rx_buf_ptr, UINT32* rx_len_ptr, UINT32* actvity_ptr);
PRIVATE VOID ech_twi_status_proc(UINT32 port_id, UINT8* rx_buf);
PRIVATE UINT32 ech_twi_boot_config_proc(UINT8* rx_buf, UINT32 rx_index);
PRIVATE VOID ech_twi_reg_addr_latch_proc(UINT8* rx_buf);
PRIVATE VOID ech_twi_reg_read_proc(UINT32 port_id, UINT8* rx_buf);
PRIVATE VOID ech_twi_reg_write_proc(UINT8* rx_buf);
PRIVATE VOID ech_twi_command_id_set(UINT8 id);
PRIVATE UINT8 ech_twi_command_id_get(void);


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

/*
** Local Variables
*/

/* TWI status byte recording result of last TWI command */
PRIVATE UINT8 ech_twi_status_byte = EXP_TWI_SUCCESS;

/*TWI CMD ID*/
PRIVATE UINT8 twi_cmd_id = EXP_FW_TWI_CMD_NULL;

/* TWI FW mode*/
PRIVATE UINT8 ech_twi_fw_mode_byte = EXP_TWI_STATUS_RUN_TIME;


/* TWI RX/TX data management */
PRIVATE UINT32 ech_twi_rx_len = 0;
PRIVATE UINT32 ech_twi_rx_index = 0;
PRIVATE UINT8 ech_twi_rx_buf[EXP_TWI_MAX_BUF_SIZE];
PRIVATE UINT8 ech_twi_tx_buf[EXP_TWI_MAX_BUF_SIZE];

/* Product Qualification Mode */
#if (FLASHLOADER_BUILD == 0)
PRIVATE BOOL ech_pqm_mode = FALSE;
#endif

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
* Private Functions
*/

/**
* @brief
*   Get the TWI status byte
*   
* @return
*   Nothing
*
* @note
*/
PRIVATE UINT8 ech_twi_status_byte_get(VOID)
{
    return ech_twi_status_byte;
}
                                                                        
/**
* @brief
*   Set the Product Qualification Mode
* 
* @param[in] setting - TRUE to enable PQM, FALSE to disable PQM
*  
* @return
*   Nothing
*
* @note
*/
#if (FLASHLOADER_BUILD == 0)
PRIVATE VOID ech_twi_pqm_set(BOOL setting)
{
    ech_pqm_mode = setting;
}
#endif

/**
* @brief
*   Get the PQM enable/disable setting
*   
* @return
*   The PQM enable (TRUE) or disable (FASLE) status
*
* @note
*/
#if (FLASHLOADER_BUILD == 0)
PRIVATE BOOL ech_twi_pqm_get(VOID)
{
    return (ech_pqm_mode);
}
#endif
                                                                        
/**
* @brief
*   Set Deferred TWI command processing structure 
*@param
*   cmd_id [in]            - Command ID
*   func_ptr [in]          - Pointer to deferred command handler
*   buf [in]               - reference to received data buffer
*   callback_func_ptr [in] - Callback function pointer
*   len [in]               - Command length
*   rx_index[in]           - index in buffer of start of received PQM command
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
* @param
*   port_id - TWI port to check for activity
*   rx_buf - receive data buffer
*   rx_len_ptr - received data length
*
* @return
*   TRUE - data received
*   FALSE - data not received
*
* @note
*/
PRIVATE PMCFW_ERROR ech_twi_slv_cmd_rx(UINT32 port_id,
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
#if 0
// TO DO: any action required?
        bc_printf("RX error detected\n");
#endif
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
#if 0
// TO DO: any action required?
            bc_printf("Error 0x%x in twi_slv_data_len_get()\n", rc);
#endif
            return (rc);
        }

        if (data_len > EXP_TWI_MAX_BUF_SIZE)
        {
#if 0
// TO DO: any action required?
            bc_printf("Error: rx_len too large: %d\n", rx_len);
#endif
            return (rc);
        }

        /* get the data */
        rc = twi_slv_data_get(port_id, &rx_buf_ptr[*rx_len_ptr], data_len);
        if (rc != PMC_SUCCESS)
        {
#if 0
// TO DO: any action required?
            bc_printf("Error 0x%x in twi_slv_data_get()\n", rc);
#endif
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
*   
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_twi_command_id_set(UINT8 id)
{
    twi_cmd_id = id;
}

/**
* @brief
*   Get the TWI command ID
*   
* @return
*   TWI Command ID
*
* @note
*/
PRIVATE UINT8 ech_twi_command_id_get(void)
{
    return twi_cmd_id;
}

/**
* @brief
*   Process TWI status command.
*
* @param
*   portid - TWI port to send data
*   rx_buf - received data to process
*   rx_len - received data length
*
* @return
*   nothing
*
* @note
*/
PRIVATE VOID ech_twi_status_proc(UINT32 port_id, UINT8* rx_buf)
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
*   Process TWI boot config command.
*
* @param
*   rx_buf[in] - received data to process
*   rx_index[in] - index in buffer of start of received command
*
* @return
*   Status
*
* @note
*/
PRIVATE UINT32 ech_twi_boot_config_proc(UINT8* rx_buf, UINT32 rx_index)
{
#if(FLASHLOADER_BUILD == 0)

    UINT32 rc;
    UINT32 cfg_flags = (rx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] << 8) | rx_buf[EXP_TWI_CMD_DATA_OFFSET + 3];
    UINT8 ech_twi_extended_status = EXP_TWI_SUCCESS ;

    /* 
    ** Do not increment the receive buffer index as
    ** this command is handled from VPE 0 as a deferred processing
    ** command.
    **
    */

    /* process the boot config command */


    /* record the raw boot config flags */
    ech_raw_boot_cfg_set(cfg_flags);

    bc_printf("TWI_BOOT_CONFIG:Received BOOT_CFG_FLAGS = 0x%x\n", cfg_flags);
    
    if (EXP_TWI_BOOT_CFG_FW_PRODUCT_QUALIFICATION_MODE == ((cfg_flags & EXP_TWI_BOOT_CFG_FW_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_FW_MODE_BITOFF))
    {
        /* product qualification mode specified */
        
        bc_printf("TWI_BOOT_CONFIG:Enabling PQM Mode\n");
        /* initialize PQM mode */
        ech_twi_pqm_set(TRUE);        
        /*
        ** Set Adaptation enable/disable depending on bit set in the BOOT_CFG command. 
        */
        ech_adaptation_state_set((cfg_flags & EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_ENABLE_BITMSK) >> EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_BITOFF);
        return EXP_TWI_SUCCESS;
    }

    /*
    ** BOOT CONFIG command has been broken down into two steps
    ** to overcome a hardware bug and race condition.
    ** Step 1: SerDes will be initialized
    ** Step 2: DL train will be initiated
    ** HOST will send two BOOT CONFIG commands back to back
    ** First BOOT CONFIG command will initiate step 1 and 2nd BOOT CONFIG command
    ** will initiate step 2.
    */

    /* BOOT CONFIG Step 0 */
    if (((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK)  >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_BOOT_STEP_0)
    {
        
        bc_printf("TWI_BOOT_CONFIG:Stage 0\n");

        /* 
        ** Validate transport layer mode 
        ** Note: This has no impact on FW. 
        ** This can be used for debugging when customer reports an error
        */
        switch ((cfg_flags & EXP_TWI_BOOT_CFG_TL_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_TL_MODE_BITOFF)
        {
            case EXP_TWI_BOOT_CFG_TL_OCAPI_MODE:                
                bc_printf("TWI_BOOT_CONFIG:Enabling EXP_TWI_BOOT_CFG_TL_OCAPI_MODE Mode\n");
                ech_tl_mode_set(EXP_TWI_BOOT_CFG_TL_OCAPI_MODE);
                break;
        
            case EXP_TWI_BOOT_CFG_TL_TWI_MODE:
                bc_printf("TWI_BOOT_CONFIG:Enabling EXP_TWI_BOOT_CFG_TL_TWI_MODE Mode\n");
                ech_tl_mode_set(EXP_TWI_BOOT_CFG_TL_TWI_MODE);
                break;
        
            case EXP_TWI_BOOT_CFG_TL_JTAG_MODE:                
                bc_printf("TWI_BOOT_CONFIG:Enabling EXP_TWI_BOOT_CFG_TL_JTAG_MODE Mode\n");
                ech_tl_mode_set(EXP_TWI_BOOT_CFG_TL_JTAG_MODE);
                break;
        
            default:
                /* invalid transport layer mode specified */            
                bc_printf("TWI_BOOT_CONFIG:ERR!!! EXP_TWI_BOOT_CFG_UNSUPPORTED_TL_BITMSK\n");
                ech_twi_extended_status |= EXP_TWI_BOOT_CFG_UNSUPPORTED_TL_BITMSK;
                break;
        }

        /*
        ** Validate specified SerDes frequency 
        */
        switch ((cfg_flags & EXP_TWI_BOOT_CFG_SERDES_FREQ_BITMSK)  >> EXP_TWI_BOOT_CFG_SERDES_FREQ_BITOFF)
        {
            case EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS\n");
                ech_serdes_freq_set(EXP_TWI_BOOT_CFG_SERDES_FREQ_2133_GBPS);
                break;

            case EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS\n");
                ech_serdes_freq_set(EXP_TWI_BOOT_CFG_SERDES_FREQ_2346_GBPS);
                break;

            case EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Frequency: EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS\n");
                ech_serdes_freq_set(EXP_TWI_BOOT_CFG_SERDES_FREQ_2560_GBPS);
                break;

            default:                
                bc_printf("TWI_BOOT_CONFIG:Frequency: ERR!!! EXP_TWI_BOOT_CFG_UNSUPPORTED_SERDES_FREQ_BITMSK\n");
                /* invalid SerDes frequency */
                ech_twi_extended_status |= EXP_TWI_BOOT_CFG_UNSUPPORTED_SERDES_FREQ_BITMSK;
                break;
        }

        /* validate the lane configuration */
        switch ((cfg_flags & EXP_TWI_BOOT_CFG_LANE_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_LANE_MODE_BITOFF)
        {
            case EXP_TWI_BOOT_CFG_LANE_8:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Lane: EXP_TWI_BOOT_CFG_LANE_8\n");
                ech_lane_cfg_set(EXP_TWI_BOOT_CFG_LANE_8);
                break;

            case EXP_TWI_BOOT_CFG_LANE_4:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Lane: EXP_TWI_BOOT_CFG_LANE_4\n");
                ech_lane_cfg_set(EXP_TWI_BOOT_CFG_LANE_4);
                break;

            default:                
                bc_printf("TWI_BOOT_CONFIG:Enabling SerDes Lane: ERR!!! EXP_TWI_BOOT_CFG_UNSUPPORT_LANE_CONFIG_BITMSK\n");
                /* invalid lane configuration */
                ech_twi_extended_status |= EXP_TWI_BOOT_CFG_UNSUPPORT_LANE_CONFIG_BITMSK;
                break;
        }

        /* implementation based on EXPLORER and OCMB configuration guides */
        if (EXP_TWI_SUCCESS != ech_twi_extended_status)
        {
            /* Set the extended error code*/
            ech_extended_error_code_set(ech_twi_extended_status);
            /* boot config parameters contain errors, command failed */
            return EXP_TWI_BOOT_CFG_FAILED_BITMSK;
        }

        /*
        ** Set DFE enable/disable depending on bit set in the BOOT_CFG command. 
        ** This MUST be done before intializing SERDES with serdes_plat_low_level_init(). 
        */
        ech_dfe_state_set((cfg_flags & EXP_TWI_BOOT_CFG_DFE_BITMSK) >> EXP_TWI_BOOT_CFG_DFE_BITOFF);

        /*
        ** Set Adaptation enable/disable depending on bit set in the BOOT_CFG command. 
        */
        ech_adaptation_state_set((cfg_flags & EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_ENABLE_BITMSK) >> EXP_TWI_BOOT_CFG_SERDES_ADAPTATION_BITOFF);

        /*
        ** Check if SerDes Initialization has passed
        ** If there is an error, return immediately
        */
        
        bc_printf("TWI_BOOT_CONFIG: SerDes Init Start\n");
        rc = serdes_plat_low_level_init(ech_lane_cfg_get(), 
                                        ech_serdes_freq_get(),
                                        ech_dfe_state_get());

        /* 
        ** Set the flag app_fw_oc_ready to TRUE prior to error checking,
        ** which will enable command polling over OpenCapi.
        ** Since DLx was taken out of reset in serdes_plat_low_level_init,
        ** it is safe to enable command polling. This will enable a path over 
        ** I2C should host wants to use I2C path instead of OpenCapi.
        */
        app_fw_oc_ready = TRUE;
        
        if (rc != PMC_SUCCESS)
        {        
            
            bc_printf("TWI_BOOT_CONFIG: serdes_plat_low_level_init: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;        
        }

        bc_printf("TWI_BOOT_CONFIG: SerDes Init End\n");
        
       
        if (EXP_TWI_BOOT_CFG_FW_MANUFACTURING_MODE == ((cfg_flags & EXP_TWI_BOOT_CFG_FW_MODE_BITMSK) >> EXP_TWI_BOOT_CFG_FW_MODE_BITOFF))
        {            
            bc_printf("TWI_BOOT_CONFIG: FW is in manufacturing mode \n");
            /* manufacturing mode specified */
            ech_mfg_mode_set();
        
            /* test for loopback mode */
            if (EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_MODE == ((cfg_flags & EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_BITMSK) >> EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_BITOFF))
            {                
                bc_printf("TWI_BOOT_CONFIG: Configuring SerDes in Loopback mode \n");
                /* loopback mode selected, set flag */
                ech_ocapi_loopback_set();

                /* Call this function to initialize SerDes so that we can support external loopback */
                rc = serdes_plat_low_level_standalone_init(ech_lane_cfg_get(), ech_dfe_state_get());                
                if (rc != PMC_SUCCESS)
                {                     
                    ech_extended_error_code_set(rc);
                    return EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_FAIL_BITMASK;                    
                }
                /* Run SerDes loopback test */
                rc = serdes_plat_loopback_test(ech_lane_cfg_get());
                if (rc != PMC_SUCCESS)
                {                     
                    ech_extended_error_code_set(rc);
                    return EXP_TWI_BOOT_CFG_SERDES_LOOPBACK_FAIL_BITMASK;                    
                }
            }
        
        }

        return EXP_TWI_SUCCESS;
    }
    
    /* BOOT CONFIG Step 1 */
    else if (((cfg_flags & EXP_TWI_BOOT_CFG_BOOT_STEP_BITMSK)  >> EXP_TWI_BOOT_CFG_BOOT_STEP_BITOFF) == EXP_TWI_BOOT_CFG_BOOT_STEP_1)
    {
        bc_printf("TWI_BOOT_CONFIG: Stage 1\n");

        /* Wait for RX data and run RCLK alignment */
        if ( !ocmb_cfg_RxPatAorB(OCMB_REGS_BASE_ADDR) )
        {                        
            bc_printf("[ERROR] Cannot find pattern A or B\n");  
            ech_extended_error_code_set(EXP_TWI_BOOT_CFG_DLX_CONFIG_PATTERN_A_B_FAILED);
            return EXP_TWI_BOOT_CFG_DLX_CONFIG_FAIL_BITMSK;
        }

        bc_printf("TWI_BOOT_CONFIG: Found Rx Pattern A or B\n");

        /* Call following function to support lane inversion */
        rc =  serdes_plat_lane_inversion_config(ech_lane_cfg_get());
        if (rc != PMC_SUCCESS)
        {        
            bc_printf("TWI_BOOT_CONFIG:  serdes_plat_lane_inversion_config: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_LANE_INVERSION_CONFIG_FAIL_BITMSK;        
        }
        
        rc = serdes_plat_adapt_step1(ech_dfe_state_get(),ech_adaptation_state_get(), ech_lane_cfg_get());
        if (rc != PMC_SUCCESS)
        {            
            bc_printf("TWI_BOOT_CONFIG: Serdes_plat_adapt_step1: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;        
        }

        /* apply OCMB DL reset */
        if (FALSE == ocmb_cfg_DLx_config_FW(OCMB_REGS_BASE_ADDR))
        {
            /* DLx Config FW failed */
            ech_extended_error_code_set(EXP_TWI_BOOT_CFG_DLX_CONFIG_FW_FAILED);
            bc_printf("[ERROR] OCMB DLx_config_FW FAILED\n");
            /* exit */
            return EXP_TWI_BOOT_CFG_DLX_CONFIG_FAIL_BITMSK;
        }

        rc = serdes_plat_adapt_step2(ech_dfe_state_get(), ech_adaptation_state_get(), ech_lane_cfg_get());
        if (rc != PMC_SUCCESS)
        {            
            bc_printf("TWI_BOOT_CONFIG: serdes_plat_adapt_step2: ERR!!! rc=0x%x\n",rc);            
            ech_extended_error_code_set(rc);
            return EXP_TWI_BOOT_CFG_SERDES_INIT_FAIL_BITMASK;        
        }

        /*
        ** IBM recommended registers to monitor for link training complete:
        **
        ** DLX_REG_DL0_CONFIG0_LEFT - bit 0: CFG_DL0_DEBUG_ENABLE
        ** use above bit to enable for more frequent status register updates
        ** did not work as accessing the register hung emaulation
        **
        ** DLX_REG_MC_OMI_FIR_REG_RW_LEFT - bit 20: DL0_TRAINED
        ** monitor above bit and never saw it get set
        **
        ** DLX_REG_DL0_STATUS_RIGHT - bits[14:12]: DL0_STS_TRAINING_STATE_MACHINE
        ** if these bits are 0b111 training is complete
        ** monitoring these bits worked if the fire was connected and the
        ** reset_fire.qel script was executed
        */
#if 0
        UINT32 reg_val = 0;

#if 0
        /* enable debug mode for quicker status register updates */
        OCMB_REG_READ_WRITE(OCMB_DLX_REG_DL0_CONFIG0_LEFT, 0x00000001, 0x00000001);
#endif

#if 0
        while ((reg_val & ECH_OCMB_TRAINING_COMPLETE) != ECH_OCMB_TRAINING_COMPLETE)
        {
            reg_val = OCMB_REG_READ(OCMB_DLX_REG_MC_OMI_FIR_REG_RW_LEFT);
        }
#endif

        while ((reg_val & OCMB_DLX_REG_DL0_STATUS_RIGHT_BITMSK_DL0_STS_TRAINING_STATE_MACHINE) != ECH_OCMB_TRAINING_SM_COMPLETE)
        {
            reg_val = OCMB_REG_READ(OCMB_DLX_REG_DL0_STATUS_RIGHT);
        }
#else
#if 0
        /* 
        ** wait for configuration to complete
        ** test by setting break point on call to ocmb_cfg_poll_value() then use "source reset_fire.qel" script on Palladium
        ** re-start code and wait for register to be set to desired value
        */
        if (FALSE == ocmb_cfg_poll_value(OCMB_REGS_BASE_ADDR,
                                         OCMB_DLX_REG_DL0_STATUS_RIGHT,
                                         OCMB_DLX_REG_DL0_STATUS_RIGHT_BITMSK_DL0_STS_TRAINING_STATE_MACHINE,
                                         ECH_OCMB_TRAINING_SM_COMPLETE,
                                         ECH_OCMB_TRAINING_DELAY_500_US))
        {
            /* boot config command failed */
            ech_twi_status_byte |= EXP_TWI_BOOT_CFG_FAILED_BITMSK;

            /* exit */
            return;
        }
#endif
#endif
        return EXP_TWI_SUCCESS;
    }
#endif

    /* boot config command succeeded */
    return EXP_TWI_SUCCESS;

} /* ech_twi_boot_config_proc */

/**
* @brief
*   Process TWI Register Address Latch command.
*
* @param
*   rx_buf - received data to process
*
* @return
*   nothing
*
* @note
*/
PRIVATE VOID ech_twi_reg_addr_latch_proc(UINT8* rx_buf)
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
#if 0
        /* TO DO: any need to distinguish between out-of-range or protected region? */
        ech_twi_status_byte = EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE;
        ech_twi_status_byte = EXP_TWI_REG_RW_ADDR_PROHIBITED;
#else
        /* latch the "address" */
        ech_latched_reg_addr_set(EXP_TWI_REG_RW_ADDR_OUT_OF_RANGE);

        /* failure status */
        ech_twi_status_byte = EXP_TWI_ERROR;
#endif
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
* @param
*   rx_buf - received data to process
*
* @return
*   nothing
*
* @note
*/
PRIVATE VOID ech_twi_reg_read_proc(UINT32 port_id, UINT8* rx_buf)
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
* @param
*   rx_buf - received data to process
*
* @return
*   nothing
*
* @note
*/
PRIVATE VOID ech_twi_reg_write_proc(UINT8* rx_buf)
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
#if 0
        /* TO DO: any need to distinguish between out-of-range or protected region? */
        ech_twi_status_byte = EXP_TWI_REG_RW_ADDR_OOR;
        ech_twi_status_byte = EXP_TWI_REG_RW_ADDR_PRO;
#else
        ech_twi_status_byte = EXP_TWI_ERROR;
#endif
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
*   
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
*   
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
*   
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
*   
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
* @param
*   portid - TWI port to check for activity
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_twi_slave_proc(UINT32 port_id)

{
    UINT32 twi_activity;

    /* check TWI interface for any activity */
    if (PMC_SUCCESS != ech_twi_slv_cmd_rx(port_id, ech_twi_rx_buf, &ech_twi_rx_len, &twi_activity))
    {
        /* TO DO: implement error handling */
        return;
    }

    if ((twi_activity & TWI_SLAVE_ACTIVITY_RX_REQ ) != TWI_SLAVE_ACTIVITY_RX_REQ) 
    {
        /** 
         * Exit this function if controller has not recived RX request. 
         *  
         * 
         */
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

        switch (ech_twi_rx_buf[ech_twi_rx_index])
        {
            case EXP_FW_TWI_CMD_BOOT_CONFIG:
            {
                /* Boot Config command received, process it */
                memcpy(&ech_twi_deferred_cmd_buf[0],&ech_twi_rx_buf[ech_twi_rx_index],EXP_TWI_BOOT_CFG_CMD_LEN);
                ech_twi_status_byte_set(EXP_TWI_BUSY);
                ech_twi_deferred_cmd_processing_struct_set(EXP_FW_TWI_CMD_BOOT_CONFIG,
                    &ech_twi_boot_config_proc, 
                    (void *)(&ech_twi_deferred_cmd_buf[0]),
                    &ech_twi_status_byte_set,
                    EXP_TWI_BOOT_CFG_CMD_LEN,
                    ech_twi_rx_index);
                g_exp_fw_twi_cmd_boot_config++;
            }
            break;

            case EXP_FW_TWI_CMD_STATUS:
            {
                /* Status command received, process it */
                ech_twi_status_proc(port_id, &ech_twi_rx_buf[ech_twi_rx_index]);
                g_count_exp_fw_twi_cmd_status++;
            }
            break;

            case EXP_FW_TWI_CMD_REG_ADDR_LATCH:
            {
                /* Register Address Latch command received, process it */
                ech_twi_reg_addr_latch_proc(&ech_twi_rx_buf[ech_twi_rx_index]);
                g_exp_fw_twi_cmd_reg_addr_latch++;
            }
            break;

            case EXP_FW_TWI_CMD_REG_READ:
            {
                /* Register Read command received, process it */
                ech_twi_reg_read_proc(port_id, &ech_twi_rx_buf[ech_twi_rx_index]);
                g_exp_fw_twi_cmd_reg_read++;
            }
            break;

            case EXP_FW_TWI_CMD_REG_WRITE:
            {
                /* Register Write command received, process it */
                ech_twi_reg_write_proc(&ech_twi_rx_buf[ech_twi_rx_index]);
                g_exp_fw_twi_cmd_reg_write++;
            }
            break;

#if (FLASHLOADER_BUILD == 0)
            case EXP_FW_PQM_LANE_SET:
            case EXP_FW_PQM_LANE_GET:
            case EXP_FW_PQM_FREQ_SET:
            case EXP_FW_PQM_FREQ_GET:
            case EXP_FW_PQM_LANE_TRAINING:
            case EXP_FW_PQM_TRAINING_RESET:
            case EXP_FW_PQM_RX_CALIBRATION_VALUE_START:
            case EXP_FW_PQM_RX_CALIBRATION_VALUE_READ:
            case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_START:
            case EXP_FW_PQM_CSU_CALIBRATION_VALUE_STATUS_READ:
            case EXP_FW_PQM_PRBS_PATTERN_MODE_SET:
            case EXP_FW_PQM_PRBS_USER_DEFINED_PATTERN_SET:
            case EXP_FW_PQM_PRBS_MONITOR_CONTROL:
            case EXP_FW_PQM_PRBS_GENERATOR_CONTROL:
            case EXP_FW_PQM_PRBS_ERR_COUNT_START:
            case EXP_FW_PQM_PRBS_ERR_COUNT_READ:
            {
                /* Product Qualification Mode Command received */
        
                if (TRUE == ech_twi_pqm_get())
                {
                    /* PQM has been selected */

                    /* process the command */
                    ech_pqm_cmd_proc(ech_twi_rx_buf, ech_twi_rx_index);
                }
                else
                {
                    /* PQM has not been selected, increment command pointer */
                    ech_pqm_cmd_rx_index_increment(ech_twi_rx_buf, ech_twi_rx_index);
                    /* set the Status register */
                    ech_twi_status_byte = EXP_TWI_UNSUPPORTED;
                }
            }
            break;

            /* 
            ** Following 4 PQM commands are made avaiable outside of PQM mode
            ** Please refer to JIRA EDBC-417
            ** Assumption is: SerDes will be initialzied by the HOST prior to the 
            ** execution of the following commands
            */ 
            case EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_START:
            case EXP_FW_PQM_VERTICAL_BATHTUB_GET_START:
            case EXP_FW_PQM_2D_BATHTUB_GET_START:
            case EXP_FW_PQM_HORIZONTAL_BATHTUB_GET_READ:
            case EXP_FW_PQM_VERTICAL_BATHTUB_GET_READ:
            case EXP_FW_PQM_2D_BATHTUB_GET_READ:
            case EXP_FW_PQM_RX_ADAPTATION_OBJ_START:
            case EXP_FW_PQM_RX_ADAPTATION_OBJ_READ:                
                /* process the command */
                ech_pqm_cmd_proc(ech_twi_rx_buf, ech_twi_rx_index);
            break;
            /* 
            ** Even though EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE is a PQM command, 
            ** it needs to be made available outside PQM mode.
            **/
            case EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE:
                /* Force Delay Line update PQM received, process it */
                memcpy(&ech_twi_deferred_cmd_buf[0], &ech_twi_rx_buf[ech_twi_rx_index], EXP_TWI_DELAY_LINE_UPDATE_CMD_LEN);
                ech_twi_status_byte_set(EXP_TWI_BUSY);
                ech_twi_deferred_cmd_processing_struct_set(EXP_FW_PQM_FORCE_DELAY_LINE_UPDATE,
                    &ech_pqm_force_delay_line_update, 
                    (void *)(&ech_twi_deferred_cmd_buf[0]),
                    &ech_twi_status_byte_set,
                    EXP_TWI_DELAY_LINE_UPDATE_CMD_LEN,
                    ech_twi_rx_index);
                
            break;
            
#endif

            case EXP_FW_TWI_CMD_FW_DOWNLOAD:
            case EXP_FW_TWI_CMD_NULL:
            default:
                bc_printf("COMMAND : 0x%x\n", ech_twi_rx_buf[ech_twi_rx_index]);
                bc_printf("twi_activity 0x%08x\n: ", twi_activity);
                bc_printf("ech_twi_rx_len : 0x%08x'n", ech_twi_rx_len);
                bc_printf("ech_twi_rx_index : 0x%08x\n",  ech_twi_rx_index);
                PMCFW_ASSERT(FALSE, EXP_TWI_INVALID_CMD);
            break;
        }
    }

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
