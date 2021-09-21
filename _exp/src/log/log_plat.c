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



/*
** Include files
*/
#include <string.h>
#include "log_plat.h"
#include "log_app_api.h"
#include "pmcfw_common.h"
#include "pmc_plat.h"
#include "app_fw.h"
#include "sys_timer.h"
#include "spi_api.h"
#include "spi.h"
#include "spi_flash.h"
#include "spi_flash_plat.h"
#include "exp_api.h"
#include "ech.h"
#include "bc_printf.h"
#include "char_io.h"
#include "ccb_api.h"
#include "spi_plat.h"

/*
** Local Enumerated Types
*/

/*
** Local Constants
*/
#define LOG_ACTIVE_FW_LOG_IDX   0
#define LOG_STORED_FW_LOG_IDX   1
#define LOG_MAX_FW_LOG_IDX      2

/*
** Local Macro Definitions
*/

/*
** Local Structures and Unions
*/

/*
** Local Variables
*/

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

PRIVATE PMCFW_ERROR _log_spi_flash_partition_erase(UINT8* spi_flash_addr, UINT32 num_bytes);
typedef PMCFW_ERROR (*log_spi_flash_partition_erase_fn_ptr_type)(UINT8* spi_flash_addr, UINT32 num_bytes);
PRIVATE log_spi_flash_partition_erase_fn_ptr_type log_spi_flash_partition_erase_fn_ptr = _log_spi_flash_partition_erase;
#define log_spi_flash_partition_erase (*log_spi_flash_partition_erase_fn_ptr)


/*
** Forward References
*/

/*
** Private Functions
*/

/**
* @brief
*   clear SPI flash firmware log paritition
*
* @param
*   spi_flash_addr - virtual SPI flash address
*   num_bytes - amount of SPI flash to erase
*
*  @return
*   Success - PMC_SUCCESS
*   Failure - failure specific code
*
*/
PMC_RAM_PROGRAM
PRIVATE PMCFW_ERROR _log_spi_flash_partition_erase(UINT8* spi_flash_addr,
                                                   UINT32 num_bytes)
{
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    UINT8* subsector_base;
    UINT32 subsector_len;
    UINT8* erase_addr = (UINT8*)((UINT32)spi_flash_addr & GPBC_FLASH_PHYS_ADDR_MASK);
    PMCFW_ERROR rc;

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);

    if (rc != PMC_SUCCESS)
    {
        return (rc);
    }

    /* halt other VPE so SPI flash can be erased without crashing other VPE */
    hal_disable_mvpe();

    /* erase subsectors until the SPI flash log partition is erased */
    for (UINT32 i = 0; i < num_bytes; i += subsector_len)
    {
        /* get the subsector address */
        rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                            SPI_FLASH_CS,
                                            erase_addr,
                                            &subsector_base,
                                            &subsector_len);

        if (PMC_SUCCESS != rc)
        {
            return (rc);
        }

        if (((UINT32)erase_addr % subsector_len) != 0)
        {
            /* SPI flash log location does not align with subsector boundary */
            return (SPI_FLASH_ERR_BAD_PARAM);
        }

        rc = spi_flash_subsector_erase(SPI_FLASH_PORT,
                                       SPI_FLASH_CS,
                                       subsector_base);

        if (PMC_SUCCESS != rc)
        {
            return (rc);
        }

        /* poll for erase completion */
        rc = spi_plat_flash_poll_write_erase_complete(FALSE, dev_info.max_time_subsector_erase);
        if (rc != PMC_SUCCESS)
        {
            return (rc);
        }

        /* increment the erase subsector address by the length of the subsector */
        erase_addr = (UINT8*)((UINT32)subsector_base + subsector_len);
    }

    /* resume other VPE */
    hal_enable_mvpe();

    /* return success */
    return (PMC_SUCCESS);

} /* log_spi_flash_clear */
PMC_END_RAM_PROGRAM

/**
* @brief
*   read RAM firmware log from newest entries
*
*  @return
*   Nothing
*
* @note
*/
PRIVATE VOID log_ram_read()
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    void*  ccb_log_ptr;
    void*  ccb_ctrl_ptr;
    UINT32 ccb_log_size;

    /* get the CCB size and control pointer */
    ccb_log_size = char_io_loc_buffer_info_get(CHAR_IO_CHANNEL_ID_RUNTIME,
                                               &ccb_log_ptr);
    ccb_ctrl_ptr = char_io_ccb_ctrl_get(CHAR_IO_CHANNEL_ID_RUNTIME);

    if (NULL == ccb_log_ptr)
    {
        /* no active log defined */

        /* prepare response parameters */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        rsp_parms_ptr->status = EXP_FW_API_FAILURE;
        rsp_parms_ptr->err_code = EXP_FW_LOG_OP_NO_ACTIVE_LOG;
        rsp_parms_ptr->num_bytes_returned = 0;
    }
    else
    {
        /* clear the extended data buffer */
        memset(ext_data_ptr,
               0x00,
               ccb_log_size);

        /* Copy CCB to the extended data buffer */
        ccb_get(ccb_ctrl_ptr, (CHAR*) ext_data_ptr, ccb_log_size);

        /* set response parameters */
        rsp_parms_ptr->status = EXP_FW_API_SUCCESS;
        rsp_parms_ptr->err_code = EXP_SUCCESS;
        rsp_parms_ptr->num_bytes_returned = ccb_log_size;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = ccb_log_size;

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_EXTENDED_DATA;

    }

    /* set the response operand, same as command operand */
    rsp_parms_ptr->op = cmd_parms_ptr->op;

    /* send the response */
    ech_oc_rsp_proc();

} /* log_ram_read */

/**
* @brief
*   read saved crash dump log data
*
*  @return
*   Nothing
*
* @note
*/
PRIVATE VOID log_saved_read()
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    UINT8* src_data_ptr;
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();

    /* determine the active image to get the crash dump SPI start address */
    if (cmd_parms_ptr->image == EXP_FW_IMAGE_A)
    {
        /* read request for image A crash dump logfile */

        /* get reference to image A SPI flash crash dump logfile */
        src_data_ptr = (UINT8*)SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_ADDR;
    }
    else
    {
        /* read request for image B crash dump logfile */

        /* get reference to image B SPI flash crash dump logfile */
        src_data_ptr = (UINT8*)SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_ADDR;
    }

    if (cmd_parms_ptr->offset > SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_SIZE)
    {
        /* offset beyond end of logfile, invalid address request */

        /* set failure status */
        rsp_parms_ptr->status = EXP_FW_API_FAILURE;
        rsp_parms_ptr->err_code = EXP_INVALID_ADDR;
        rsp_parms_ptr->num_bytes_returned = 0;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = 0;

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
    }
    else if (cmd_parms_ptr->num_bytes > EXP_EXT_DATA_BUF_SIZE)
    {
        /* excessive number of bytes requested */

        /* set failure status */
        rsp_parms_ptr->status = EXP_FW_API_FAILURE;
        rsp_parms_ptr->err_code = EXP_INVALID_DATA_LENGTH;
        rsp_parms_ptr->num_bytes_returned = 0;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = 0;

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
    }
    else
    {
        /* clear the extended data buffer */
        memset(ext_data_ptr,
               0x00,
               cmd_parms_ptr->num_bytes);

        /* copy SPI crash dump logfile data to extended data buffer */
        memcpy(ext_data_ptr, 
               (src_data_ptr + cmd_parms_ptr->offset), 
               cmd_parms_ptr->num_bytes);

        /* set response parameters */
        rsp_parms_ptr->status = EXP_FW_API_SUCCESS;
        rsp_parms_ptr->err_code = EXP_SUCCESS;
        rsp_parms_ptr->num_bytes_returned = cmd_parms_ptr->num_bytes;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = cmd_parms_ptr->num_bytes;

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_EXTENDED_DATA;
    }

    /* set the response operand, same as command operand */
    rsp_parms_ptr->op = cmd_parms_ptr->op;

    /* send the response */
    ech_oc_rsp_proc();

} /* log_saved_read */

/**
* @brief
*   Erase RAM firmware log.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID log_ram_erase(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    void*  ccb_log_ptr;
    void*  ccb_ctrl_ptr;

    /* get the CCB control pointer and log pointer */
    char_io_loc_buffer_info_get(CHAR_IO_CHANNEL_ID_RUNTIME,
                                                   &ccb_log_ptr);
    ccb_ctrl_ptr = char_io_ccb_ctrl_get(CHAR_IO_CHANNEL_ID_RUNTIME);

    if (NULL == ccb_log_ptr)
    {
        /* no log defined */

        /* set failure status */
        rsp_parms_ptr->status = EXP_FW_API_FAILURE;

        /* set err code */
        rsp_parms_ptr->err_code = EXP_FW_LOG_OP_NO_ACTIVE_LOG;
    }
    else
    {
        /* Clear the circular buffer in RAM */
        ccb_clear(ccb_ctrl_ptr);

        /* set success status */
        rsp_parms_ptr->status = EXP_FW_API_SUCCESS;

        /* set err code */
        rsp_parms_ptr->err_code = EXP_SUCCESS;
    }

    /* set the response operand, same as the command operand */
    rsp_parms_ptr->op = cmd_parms_ptr->op;

    /* set the extended data flag */
    rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* send the response */
    ech_oc_rsp_proc();

} /* log_ram_erase */

/**
* @brief
*   Firmware log command handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID log_cmd_process(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;

    switch (cmd_parms_ptr->op)
    {
        case EXP_FW_LOG_OP_READ_ACTIVE_LOG:
        {
            /* request to read active RAM firmware log from the oldest entry */
            log_ram_read();
        }
        break;

        case EXP_FW_LOG_OP_ACTIVE_CLR:
        {
            /* request to erase RAM log */
            log_ram_erase();
        }
        break;

        case EXP_FW_LOG_OP_READ_SAVED_LOG:
        {
            /* request to read saved crash dump logfile */
            log_saved_read();
        }
        break;

        case EXP_FW_LOG_OP_SAVED_CLR:
        default:
        {
            exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
            exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;

            /* set the response parameters */
            rsp_parms_ptr->status = EXP_FW_API_FAILURE;
            rsp_parms_ptr->err_code = PMCFW_ERR_INVALID_PARAMETERS;
            rsp_parms_ptr->num_bytes_returned = 0;

            /* set the extended data response length */
            rsp_ptr->ext_data_len = 0;

            /* clear the extended data flag */
            rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

            /* set the response operand, same as the command operand */
            rsp_parms_ptr->op = cmd_parms_ptr->op;

            /* send the response */
            ech_oc_rsp_proc();
        }
        break;
    }
} /* log_cmd_handler */


/*
** Public Functions
*/

/**
* @brief
*   clear log memory
*
* @param[in] log_mem_size - log memory size in bytes
* @param[in] log_mem_addr - log memory address
*
*  @return
*   None.
*
*/
PUBLIC void log_mem_clear(UINT32 log_mem_size,
                          void  *log_mem_addr)
{
    memset(log_mem_addr, 0, log_mem_size);

} /* End: log_mem_clear() */

/**
* @brief
*   initialize platform specific features of the log module
*
* @return
*   None.
*
*/
PUBLIC VOID log_plat_init(VOID)
{
    /* register log command handler */
    ech_api_func_register(EXP_FW_LOG, log_cmd_process);

} /* log_plat_init */

/**
* @brief
*   store firmware log to SPI flash.
*
* @param
*   none
*
* @return
*   Success - PMC_SUCCESS
*   Failure - failure specific code
*
*/
PUBLIC PMCFW_ERROR log_spi_flash_store(VOID)
{
    PMCFW_ERROR rc;
    UINT8* log_mem_ptr;
    UINT32 log_size;
    log_cfg_struct* log_hdr_ptr;
    spi_flash_dev_enum dev;
    spi_flash_dev_info_struct dev_info;
    UINT8* spi_log_ptr;

    /* get reference to the firmware log */
    log_app_info_get((VOID*)&log_mem_ptr);

    if (NULL == log_mem_ptr)
    {
        /* no log defined */
        return (EXP_FW_LOG_OP_NO_ACTIVE_LOG);
    }

    log_hdr_ptr = (log_cfg_struct*)log_mem_ptr;
    log_size = sizeof(log_cfg_struct) + (log_hdr_ptr->wr_idx * sizeof(log_app_entry_struct));

    /* determine the active image to erase the correct stored log */
    if ((UINT32*)SPI_FLASH_FW_ACT_IMG_FLAG_ADDR == SPI_FLASH_ACTIVE_IMG_A)
    {
        /* firmware image A is the active image */

        /* get reference to image A SPI flash log*/
        spi_log_ptr = (UINT8*)SPI_FLASH_FW_IMG_A_CFG_LOG_ADDR;
    }
    else
    {
        /* firmware image B is the active image */

        /* get reference to image B SPI flash log*/
        spi_log_ptr = (UINT8*)SPI_FLASH_FW_IMG_B_CFG_LOG_ADDR;
    }

    /* erase the log partition in SPI flash */
    rc = log_spi_flash_partition_erase(spi_log_ptr, log_size);

    if (rc != PMC_SUCCESS)
    {
        /* log partition erase failed */
        return (rc);
    }

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);

    if (rc != PMC_SUCCESS)
    {
        /* error acquiring SPi flash device info */
        return (rc);
    }

    /* write the log one page at a time */
    rc = spi_plat_flash_write_pages(log_mem_ptr,
                                    spi_log_ptr,
                                    log_size,
                                    dev_info.page_size,
                                    dev_info.max_time_page_prog);

    if (PMC_SUCCESS != rc)
    {
        return (rc);
    }

    /* return success */
    return (PMC_SUCCESS);

} /* log_spi_flash_store */

/**
* @brief
*   adjust pointers to functions in RAM to accommodate PIC
*   start-up adding flash offset to initialized function
*   pointers
*
* @param
*   offset - PIC offset to remove from pointers
*
* @return
*   nothing
*/
PUBLIC VOID log_plat_ram_code_ptr_adjust(UINT32 offset)
{
    log_spi_flash_partition_erase_fn_ptr = (log_spi_flash_partition_erase_fn_ptr_type)((UINT32)log_spi_flash_partition_erase_fn_ptr - offset);
}

/* End of File */


