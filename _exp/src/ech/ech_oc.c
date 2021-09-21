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
#include "ech.h"
#include "ech_loc.h"
#include "mem.h"
#include "crc32.h"
#include "twi.h"
#include "twi_api.h"
#include "app_fw.h"
#include "bc_printf.h"
#include "top_exp_config_guide.h"
#include <string.h>
#include "cpuhal.h"
#include "cpuhal_asm.h"
#include "exp_api.h"
#include "ddr_api.h"



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


/*
** Local Structures and Unions
*/


/*
** Forward declarations
*/


/*
** Global Variables
*/


/*
** Local Variables
*/


/*
* Private Functions
*/


/**
* Public Functions
*/

/**
* @brief
*   Register API function with ECH module.
*
* @return
*   TRUE - ECH initialized
*   FALSE - ECH not initialized
*
* @note
*/
PUBLIC VOID ech_oc_init(VOID)
{
    exp_cmd_enum i;
    UINT32 ctrl_list_sz;
    ech_ctrl_struct* ctrl_ptr;

    /* allocate memory for array of commands */
    ctrl_list_sz = EXP_FW_MAX_CMD * sizeof(ech_ctrl_struct);
    ctrl_ptr = MEM_ALLOC(MEM_TYPE_FREE,
                         ctrl_list_sz,
                         HAL_MEM_NUMBYTES_CACHE_LINE);

    if (NULL == ctrl_ptr)
    {
        /* memory could not be allocated */
        PMCFW_ASSERT(FALSE, EXP_TWI_MEM_ALLOC_ERR);
    }

    /* store pointer to command control structures */
    ech_ctrl_ptr_set(ctrl_ptr);

    /* initialize command control structures */
    for (i = EXP_FW_NULL_CMD; i < EXP_FW_MAX_CMD; i++)
    {
        ctrl_ptr[i].cmd = i;
        ctrl_ptr[i].api_fn_ptr = NULL;
    }

} /* ech_oc_init */


/**
* @brief
*   Register API function with ECH module.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_api_func_register(exp_cmd_enum cmd, ech_cmd_fn_ptr fn_ptr)
{
    ech_ctrl_struct* ctrl = ech_ctrl_ptr_get();

    PMCFW_ASSERT(ctrl != NULL, EXP_INVALID_CTRL_LIST_ERR);
    PMCFW_ASSERT(fn_ptr != NULL, EXP_INVALID_FUNC_PTR_ERR);
    PMCFW_ASSERT(cmd > EXP_FW_NULL_CMD, EXP_INVALID_FUNC_CMD_ERR);
    PMCFW_ASSERT(cmd < EXP_FW_MAX_CMD, EXP_INVALID_FUNC_CMD_ERR);

    /* register the API callback function */
    ctrl[cmd].api_fn_ptr = fn_ptr;
}

/**
* @brief
*   Process messages from the host.
*
* @return
*   TRUE - command was received and process
*   FALSE - otherwise
*
* @note
*/
PUBLIC BOOL ech_oc_cmd_proc(VOID)
{
    ech_ctrl_struct* ctrl_ptr = ech_ctrl_ptr_get();
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT32 crc;

    if (FALSE == ech_cmd_rxd_flag_get())
    {
        /* no command received from host */
        return (FALSE);
    }

    /* clear received message flag */
    ech_cmd_rxd_clr();

    if ((TRUE == !(EXP_FW_NULL_CMD < cmd_ptr->id)) ||
        (TRUE == !(EXP_FW_MAX_CMD > cmd_ptr->id)))
    {
        /* unsupported command */

        /* prepare error response */
        rsp_ptr->parms[0] = EXP_FW_API_FAILURE;
        rsp_ptr->parms[1] = EXP_FW_API_CMD_ERR;

        /* no extended data flag */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        rsp_ptr->ext_data_len = 0;

        /* set the response id, same as the command id */
        rsp_ptr->id = cmd_ptr->id;

        /* set the response request id, same as the command request id */
        rsp_ptr->req_id = cmd_ptr->req_id;

        /* prepare the response */
        ech_oc_rsp_proc();

        /* command processed */
        return (TRUE);
    }

    /* validate the command CRC */
    crc = pmc_crc32((UINT8*)cmd_ptr,
                    (sizeof(exp_cmd_struct) - sizeof(cmd_ptr->crc)),
                    0,
                    TRUE,
                    TRUE);

    if (crc != cmd_ptr->crc)
    {
        /* invalid command header */

        /* prepare error response */
        rsp_ptr->parms[0] = EXP_FW_API_FAILURE;
        rsp_ptr->parms[1] = EXP_FW_API_CMD_CRC_ERR;

        /* no extended data flag */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        rsp_ptr->ext_data_len = 0;

        /* set the response id, same as the command id */
        rsp_ptr->id = cmd_ptr->id;

        /* set the response request id, same as the command request id */
        rsp_ptr->req_id = cmd_ptr->req_id;

        /* prepare the response */
        ech_oc_rsp_proc();

        /* command processed */
        return (TRUE);
    }

    /* validate additional data, if present */
    if (EXP_FW_EXTENDED_DATA_BITMSK == (cmd_ptr->flags & EXP_FW_EXTENDED_DATA_BITMSK))
    {
        crc = pmc_crc32(ech_ext_data_ptr_get(),
                        cmd_ptr->ext_data_len,
                        0,
                        TRUE,
                        TRUE);

        if (cmd_ptr->ext_data_crc != crc)
        {
            /* invalid extended data */

            /* prepare error response */
            rsp_ptr->parms[0] = EXP_FW_API_FAILURE;
            rsp_ptr->parms[1] = EXP_FW_API_CMD_DATA_CRC_ERR;

            /* no extended data flag */
            rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
            rsp_ptr->ext_data_len = 0;

            /* set the response id, same as the command id */
            rsp_ptr->id = cmd_ptr->id;

            /* set the response request id, same as the command request id */
            rsp_ptr->req_id = cmd_ptr->req_id;

            /* prepare the response */
            ech_oc_rsp_proc();

            /* command processed */
            return (TRUE);
        }
    }

    if (NULL == ctrl_ptr[cmd_ptr->id].api_fn_ptr)
    {
        /* no handler function registered for the command */
        PMCFW_ASSERT(FALSE, EXP_INVALID_FUNC_PTR_ERR);
    }

    /* Clear the response buffer */
    memset(rsp_ptr, 0, ech_rsp_size_get());

    /* Copy cmd fields to rsp structure */
    rsp_ptr->id = cmd_ptr->id;
    rsp_ptr->req_id = cmd_ptr->req_id;
    rsp_ptr->host_spad_area = cmd_ptr->host_spad_area;

    /* process the command */
    ctrl_ptr[cmd_ptr->id].api_fn_ptr();

    /* command received and processed */
    return (TRUE);

} /* ech_oc_cmd_proc */

/**
* @brief
*   Process responses to the host.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_oc_rsp_proc(VOID)
{
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();

    /* calculate CRC on additional data, if present */
    if (EXP_FW_EXTENDED_DATA_BITMSK == (rsp_ptr->flags & EXP_FW_EXTENDED_DATA_BITMSK))
    {
        rsp_ptr->ext_data_crc = pmc_crc32(ech_ext_data_ptr_get(),
                                          rsp_ptr->ext_data_len,
                                          0,
                                          TRUE,
                                          TRUE);
    }

    /* calculate response header CRC */
    rsp_ptr->crc = pmc_crc32((UINT8*)rsp_ptr,
                             (sizeof(exp_rsp_struct) - sizeof(rsp_ptr->crc)),
                             0,
                             TRUE,
                             TRUE);

    /* send the interrupt to HOST */
    ech_cmd_txd_flag_set();

} /* ech_proc_rsp */

/** @} end addtogroup */

