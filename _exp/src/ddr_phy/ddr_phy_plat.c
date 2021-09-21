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
* @addtogroup DDR_PHY_PLAT
* @{
* @file
* @brief
*   This file contains the DDR PHY platform specific command
*   handlers, variables, and functions.
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
#include <string.h>
#include "cpuhal.h"
#include "cpuhal_asm.h"
#include "exp_api.h"
#include "ddr_api.h"
#include "crash_dump.h"
#include "ddr_phy_dump.h"
#include "app_fw_ddr.h"


/*
** Global Variables
*/

/*
** Local Structures and Unions
*/


/*
** Forward Reference
*/


/*
** Private Data
*/

#define DDRPHY_REG_CRASH_DUMP_SIZE 0x80000

PRIVATE BOOL ddr_phy_initialized = FALSE;

/*
** Private Functions
*/

/**
* @brief
*   DDR PHY step-by-step command handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ddr_phy_step_by_step_init_command_handler(VOID)
{
} /* ddr_phy_step_by_step_init_command_handler */

/**
* @brief
*   DDR PHY command handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ddr_phy_init_command_handler(VOID)
{
    BOOL error_status = FALSE;
    UINT32 ext_error_code = PMC_SUCCESS;
    UINT8  error_code = DDR_PHY_INIT_RESP_SUCCESS;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_phy_init_cmd_parms_struct* cmd_parms_ptr = (exp_fw_phy_init_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_phy_init_rsp_parms_struct* rsp_parms_ptr = (exp_fw_phy_init_rsp_parms_struct*)&rsp_ptr->parms;
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    UINT32 ext_data_size = ech_ext_data_size_get();

    PMCFW_ASSERT((ext_data_size >= sizeof(user_2d_eye_response_1_msdg_t)) &&
                 (ext_data_size >= sizeof(user_2d_eye_response_2_msdg_t)) &&
                 (ext_data_size >= sizeof(user_response_msdg_t)),
                 DDR_FW_ERR_API_EYE_CAPTURE_BUFFER_SIZE);

    if ((cmd_ptr->flags & EXP_FW_EXTENDED_DATA_BITMSK) == EXP_FW_EXTENDED_DATA)
    {
        if (cmd_ptr->ext_data_len != sizeof(user_input_msdg_t))
        {
            ext_error_code = DDR_FW_ERR_INCORRECT_DATA_LENGTH;

            /* set the extended data response length */
            rsp_ptr->ext_data_len = 0;

            /* set the extended data flag */
            rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        }
        else
        {
            user_input_msdg_t* ddr_phy_msdg_ptr = (user_input_msdg_t*) ext_data_ptr;

            /* set the PHY configuration parameters sent by host */
            ddr_api_init(ddr_phy_msdg_ptr);

            memset(ext_data_ptr, 0, ext_data_size);

            if (cmd_parms_ptr->phy_init_mode == EXP_FW_PHY_INIT_DEFAULT_TRAIN)
            {
                /* initialize the DDR interface */
                ext_error_code = ddr_api_fw_train();

                /* copy the training results into the extended data buffer */
                memcpy(ext_data_ptr,
                       ddrphy_training_results_get(),
                       sizeof(user_response_msdg_t));

                /* set the extended data response length */
                rsp_ptr->ext_data_len = sizeof(user_response_msdg_t);

                /* set the extended data flag */
                rsp_ptr->flags = EXP_FW_EXTENDED_DATA;

                /* if training failed print error code */
                if (PMC_SUCCESS != ext_error_code)
                {
                    bc_printf("ddr_phy_init_command_handler(): ddr_api_fw_train() failed rc = 0x%08X\n", ext_error_code);
                }

                /* whether training passed or failed, save the calibration results */
                ddr_api_cal_results_get(&app_fw_ddr_saved_data.timing_data,
                                        &app_fw_ddr_saved_data.vref_data);

                /* save calibration results */
                PMCFW_ERROR rc = app_fw_ddr_calibration_save(&app_fw_ddr_saved_data);

                if (PMC_SUCCESS != rc)
                {
                    bc_printf("ddr_phy_init_command_handler(): app_fw_ddr_calibration_save() failed rc = 0x%08X\n", rc);
                }
            }
            else if (cmd_parms_ptr->phy_init_mode == EXP_FW_PHY_INIT_READ_EYE_TRAIN)
            {
                user_2d_eye_response_1_msdg_t* user_2d_eye_response_msdg_ptr = (user_2d_eye_response_1_msdg_t*) ext_data_ptr;

                memset(user_2d_eye_response_msdg_ptr, 0, sizeof(user_2d_eye_response_1_msdg_t));

                /* initialize the DDR interface */
                ext_error_code = ddr_api_fw_read_eye_capture_train((UINT8*) &user_2d_eye_response_msdg_ptr->read_2d_eye_resp);

                user_response_msdg_t *user_response_msdg_ptr = ddrphy_training_results_get();

                /* Copy data from user_response to the 2D eye response structure */
                memcpy(&user_2d_eye_response_msdg_ptr->version_number,
                       &user_response_msdg_ptr->version_number,
                       sizeof(user_2d_eye_response_msdg_ptr->version_number));

                memcpy(&user_2d_eye_response_msdg_ptr->tm_resp,
                       &user_response_msdg_ptr->tm_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->tm_resp));

                memcpy(&user_2d_eye_response_msdg_ptr->err_resp,
                       &user_response_msdg_ptr->err_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->err_resp));

                memcpy(&user_2d_eye_response_msdg_ptr->mrs_resp,
                       &user_response_msdg_ptr->mrs_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->mrs_resp));

                memcpy(&user_2d_eye_response_msdg_ptr->rc_resp,
                       &user_response_msdg_ptr->rc_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->rc_resp));

                /* set the extended data response length */
                rsp_ptr->ext_data_len = sizeof(user_2d_eye_response_1_msdg_t);

                /* set the extended data flag */
                rsp_ptr->flags = EXP_FW_EXTENDED_DATA;
            }
            else if (cmd_parms_ptr->phy_init_mode == EXP_FW_PHY_INIT_WRITE_EYE_TRAIN)
            {
                user_2d_eye_response_2_msdg_t* user_2d_eye_response_msdg_ptr = (user_2d_eye_response_2_msdg_t*) ext_data_ptr;

                memset(user_2d_eye_response_msdg_ptr, 0, sizeof(user_2d_eye_response_2_msdg_t));

                ext_error_code = ddr_api_fw_write_eye_capture_train((UINT8*) &user_2d_eye_response_msdg_ptr->write_2d_eye_resp);

                user_response_msdg_t *user_response_msdg_ptr = ddrphy_training_results_get();

                /* Copy data from user_response to the 2D eye response structure */
                memcpy(&user_2d_eye_response_msdg_ptr->version_number,
                       &user_response_msdg_ptr->version_number,
                       sizeof(user_2d_eye_response_msdg_ptr->version_number));

                memcpy(&user_2d_eye_response_msdg_ptr->tm_resp,
                       &user_response_msdg_ptr->tm_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->tm_resp));

                memcpy(&user_2d_eye_response_msdg_ptr->err_resp,
                       &user_response_msdg_ptr->err_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->err_resp));

                memcpy(&user_2d_eye_response_msdg_ptr->mrs_resp,
                       &user_response_msdg_ptr->mrs_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->mrs_resp));

                memcpy(&user_2d_eye_response_msdg_ptr->rc_resp,
                       &user_response_msdg_ptr->rc_resp,
                       sizeof(user_2d_eye_response_msdg_ptr->rc_resp));

                /* set the extended data response length */
                rsp_ptr->ext_data_len = sizeof(user_2d_eye_response_2_msdg_t);

                /* set the extended data flag */
                rsp_ptr->flags = EXP_FW_EXTENDED_DATA;
            }
            else
            {
                ext_error_code = DDR_FW_ERR_UNSUPPORTED_PHY_INIT_MODE;

                /* set the extended data response length */
                rsp_ptr->ext_data_len = 0;

                /* set the extended data flag */
                rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
            }
        }
    }
    else
    {
        ext_error_code = DDR_FW_ERR_NO_EXTENDED_DATA;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = 0;

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
    }

    if (ext_error_code != DDR_SUCCESS)
    {
        error_status = TRUE;

        /* Set the summary error code for host */
        switch(ext_error_code)
        {
            case DDR_FW_ERR_UNSUPPORTED_PHY_INIT_MODE:
                error_code = DDR_PHY_INIT_UNSUPPORTED_INIT_MODE;
                bc_printf("[DDR_PHY_INIT_UNSUPPORTED_INIT_MODE] ext_error_code: 0x%08x\n", ext_error_code);
                break;
            case DDR_FW_ERR_INCORRECT_DATA_LENGTH:
                error_code = DDR_PHY_INIT_INPUT_MSDG_SIZE_ERROR;
                bc_printf("[DDR_PHY_INIT_INPUT_MSDG_SIZE_ERROR] ext_error_code: 0x%08x\n", ext_error_code);
                break;
            case DDR_FW_ERR_NO_EXTENDED_DATA:
                error_code = DDR_PHY_INIT_INPUT_MSDG_NO_EXT_DATA;
                bc_printf("[DDR_PHY_INIT_INPUT_MSDG_NO_EXT_DATA] ext_error_code: 0x%08x\n", ext_error_code);
                break;
            case DDR_ERR_INVALID_DIMMTYPE:
            case DDR_ERR_INVALID_CSPRESENT:
            case DDR_ERR_INVALID_DRAMDATAWIDTH:
            case DDR_ERR_INVALID_HEIGHT3DS:
            case DDR_ERR_INVALID_ACTIVEDBYTE:
            case DDR_ERR_INVALID_ACTIVENIBBLE:
            case DDR_ERR_INVALID_ADDRMIRROR:
            case DDR_ERR_INVALID_COLUMNADDRWIDTH:
            case DDR_ERR_INVALID_ROWADDRWIDTH:
            case DDR_ERR_INVALID_SPDCLSUPPORTED:
            case DDR_ERR_INVALID_SPDTAAMIN:
            case DDR_ERR_INVALID_RANK4MODE:
            case DDR_ERR_INVALID_ENCODEDQUADCS:
            case DDR_ERR_INVALID_DDPCOMPATIBLE:
            case DDR_ERR_INVALID_TSV8HSUPPORT:
            case DDR_ERR_INVALID_MRAMSUPPORT:
            case DDR_ERR_INVALID_MDSSUPPORT:
            case DDR_ERR_INVALID_NUMPSTATES:
            case DDR_ERR_INVALID_FREQUENCY:
            case DDR_ERR_INVALID_PHYODTIMPEDANCE:
            case DDR_ERR_INVALID_PHYDRVIMPEDANCE:
            case DDR_ERR_INVALID_PHYSLEWRATE:
            case DDR_ERR_INVALID_ATXIMPEDANCE:
            case DDR_ERR_INVALID_ATXSLEWRATE:
            case DDR_ERR_INVALID_CKTXIMPEDANCE:
            case DDR_ERR_INVALID_CKTXSLEWRATE:
            case DDR_ERR_INVALID_ALERTODTIMPEDANCE:
            case DDR_ERR_INVALID_DRAMRTTNOM:
            case DDR_ERR_INVALID_DRAMRTTWR:
            case DDR_ERR_INVALID_DRAMRTTPARK:
            case DDR_ERR_INVALID_DRAMDIC:
            case DDR_ERR_INVALID_DRAMWRITEPREAMBLE:
            case DDR_ERR_INVALID_DRAMREADPREAMBLE:
            case DDR_ERR_INVALID_PHYEQUALIZATION:
            case DDR_ERR_INVALID_INITVREFDQ:
            case DDR_ERR_INVALID_INITPHYVREF:
            case DDR_ERR_INVALID_ODTWRMAPCS:
            case DDR_ERR_INVALID_ODTRDMAPCS:
            case DDR_ERR_INVALID_GEARDOWN:
            case DDR_ERR_INVALID_CALATENCYADDER:
            case DDR_ERR_INVALID_BISTCALMODE:
            case DDR_ERR_INVALID_BISTCAPARITYLATENCY:
            case DDR_ERR_INVALID_RCDDIC:
            case DDR_ERR_INVALID_RCDVOLTAGECTRL:
            case DDR_ERR_INVALID_RCDIBTCTRL:
            case DDR_ERR_INVALID_RCDDBDIC:
            case DDR_ERR_INVALID_RCDSLEWRATE:
            case DDR_ERR_INVALID_DFIMRL_DDRCLK:
            case DDR_ERR_INVALID_ATXDLY:
                error_code = DDR_PHY_INIT_INPUT_MSDG_ERROR;
                bc_printf("[DDR_PHY_INIT_INPUT_MSDG_ERROR] ext_error_code: 0x%08x\n", ext_error_code);
                break;
            default:
                /* For any other error respond with general training error */
                error_code = DDR_PHY_INIT_TRAIN_ERROR;
                bc_printf("[DDR_PHY_INIT_TRAIN_ERROR] ext_error_code: 0x%08x\n", ext_error_code);
                break;
        }
    }

    /* Fill response status and error code */
    rsp_parms_ptr->status = error_status;
    rsp_parms_ptr->ext_err_code = ext_error_code;
    rsp_parms_ptr->err_code = error_code;

    /* send the response */
    ech_oc_rsp_proc();

} /* ddr_phy_init_command_handler */

/**
* @brief
*   Send userInputMsdg structure to the crash dump
*
* @param
*   None
* @return
*   None.
*
*/
PRIVATE void ddr_phy_plat_user_input_msdg_crash_dump(void)
{
    crash_dump_put(sizeof(user_input_msdg_t), (void*) ddr_api_userInputMsdg_get());
}

/*
** Public Functions
*/

/**
* @brief
*   DDR PHY Platform initialization
*
* @param
*   None
* @return
*   None.
*
*/
PUBLIC void ddr_phy_plat_init(VOID)
{
    /* Register the adapter info command handler with ECH module*/
    ech_api_func_register(EXP_FW_DDR_PHY_INIT, ddr_phy_init_command_handler);
    ech_api_func_register(EXP_FW_PHY_STEP_BY_STEP_INIT, ddr_phy_step_by_step_init_command_handler);

    /* Register crash dump */
    crash_dump_register("DDRPHY_MSDG", &ddr_phy_plat_user_input_msdg_crash_dump, CRASH_DUMP_RAW, sizeof(user_input_msdg_t));
    crash_dump_register("DDRPHY_REGS", &ddr_phy_dump_debug_info, CRASH_DUMP_ASCII, DDRPHY_REG_CRASH_DUMP_SIZE);
}


/**
* @brief
*   Set DDR PHY initialization state.
*
* @param[in] is_intialized - Update DDR PHY module to initialized or uninitialized state.
*
* @return
*   None.
*
* @note
*    We need to keep track of the state of the DDR PHY block because we cannot read
*    registers if it is not initialized.
*
*/
PUBLIC VOID ddr_phy_plat_initialized_set(BOOL is_initialized)
{
    ddr_phy_initialized = is_initialized;
}

/**
* @brief
*   Get DDR PHY initialization state.
*
* @return
*   TRUE if DDR PHY is initialized.
*
* @note
*
*/
PUBLIC BOOL ddr_phy_plat_initialized_get(VOID)
{
    return ddr_phy_initialized;
}


/** @} end group */



