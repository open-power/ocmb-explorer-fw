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
* @addtogroup ECH_CMD_TEST
* @{
* @file
* @brief
*    Explorer Command Handler TWI test mastert. The test configures and
*    initializes one master port.
*    These two ports should be physically connected. Data is sent from the
*    master to the slave, the slave modifies the data and sends it back to the
*    master, and the master validates the modified data.
*
*    This can be configured to run in either interrupt or polling mode by
*    setting the TWI_TEST_MODE_INTERRUPT #define appropriately. Other test
*    parameters can be changed as needed in the other #defines.
*
*    To run the test, add this file to the CSOURCES in twim/build/Makefile:
*
*       ifeq ($(PROCESSOR), iop)
*       CSOURCES += $(MODDIR)/src/twi_unit_test.c
*       endif
*
*    Execute the test by calling twi_unit_test(), for example from a command
*    server. The test must be run with interrupts enabled and threadX active.
*    Test results will be printed to the console.
*
* @note
*/

/*
* Include Files
*/

#include "ech.h"
#include "exp_api.h"
#include "app_fw.h"
#include "crc32.h"
#include "twi_plat.h"
#include "twi.h"
#include "spin.h"
#include "sys_timer.h"
#include "bc_printf.h"
#include "ocmb_plat.h"
#include "spi.h"
#include "spi_flash.h"
#include "spi_flash_plat.h"
#include "spi_api.h"
#include "pmc_plat.h"
#include "log_app_api.h"
#include "app_fw.h"
#include "mem_api.h"
#include "exp_top_xcbi_regs.h"
#include <string.h>


/*
* Local Enumerated Types
*/


/*
* Local Constants
*/


/* number of EXP command tests */
#define ECH_CMD_TEST_NUM_MAX    10

/* ingress/egress doorbell signals */
#define ECH_DOORBELL_CLEAR      0x00000000
#define ECH_DOORBELL_INGRESS    0xBEEFF00D
#define ECH_DOORBELL_EGRESS     0xDEADBEEF

/* which SPI port and chip select to use */
#define SPI_PORT_ID 0
#define SPI_CS_ID   0

/* SPI Flash memory base address */
#define SPI_FLASH_FW_LOG_TEST_MEM_BASE_ADDR   0x00141000
#define SPI_FLASH_UNCACHED_KSEG1_UNCACHED     0xB0000000

/* maximum time (us) to poll for erase / program completion */
#define SPI_ERASE_POLL_TIME_US  3000000
#define SPI_PROG_POLL_TIME_US   10000000


/*
* Local Macro Definitions
*/


/*
* Local Structures and Unions
*/


/*
** Forward declarations
*/
EXTERN UINT8 __ghsbegin_doorbell[];
EXTERN uint32_t ddrphy_userinputmsdg_configure(user_input_msdg_t *ui_msdg_p);

/*
* Local Variables
*/
PRIVATE volatile UINT32* sram_doorbell_ptr = (UINT32*)__ghsbegin_doorbell;

/* record of test passes/fails */
PRIVATE BOOL test_result[EXP_FW_MAX_CMD];

/* SPD and DDR parameters */
PRIVATE ech_spd_ddr_cfg_struct ech_spd_ddr_cfg;

/* firmware log */
PRIVATE log_cfg_struct* log_ptr;
//PRIVATE log_cfg_struct* flash_log_ptr = (log_cfg_struct*)(SPI_FLASH_UNCACHED_KSEG1_UNCACHED | SPI_FLASH_FW_LOG_TEST_MEM_BASE_ADDR);
PRIVATE log_cfg_struct* flash_log_ptr;


/*
* Global Variables
*/

PRIVATE UINT8 ech_read_active_fw_log[APP_FW_LOG_SIZE];
PRIVATE UINT8 ech_read_stored_fw_log[APP_FW_LOG_SIZE];

/* SPI flash device info */
PRIVATE spi_flash_dev_info_struct spi_test_dev_info;


/*
* Private Functions
*/

PMC_RAM_PROGRAM

/**
* @brief
*   Poll for completed writes or erase to SPI flash.
*
* @return
*   PMC_SUCCESS if no error
*   Error specific code otherwise
*
* @note
*/
PRIVATE PMCFW_ERROR poll_write_erase_complete(BOOL write, UINT32 timeout)
{
    UINT_TIME cpu_clk_start;   /* initial CPU clock */
    UINT_TIME cpu_clk_now;     /* current CPU clock */
    UINT_TIME timeout_cpu_clk; /* timeout in CPU clocks */
    BOOL complete;
    PMCFW_ERROR rc;

    cpu_clk_start = sys_timer_read();
    timeout_cpu_clk = sys_timer_us_to_count(timeout);

    while (TRUE)
    {
        cpu_clk_now = sys_timer_read();

        if (write)
        {
            rc = spi_flash_write_complete(SPI_FLASH_PORT, SPI_FLASH_CS, &complete);
            if (rc != PMC_SUCCESS)
            {
                bc_printf(" write_complete error: %08lx\n", rc);
                break;
            }
        }
        else
        {
            rc = spi_flash_erase_complete(SPI_FLASH_PORT, SPI_FLASH_CS, &complete);
            if (rc != PMC_SUCCESS)
            {
                bc_printf(" erase_complete error: %08lx\n", rc);
                break;
            }
        }

        if (complete)
        {
            /* write / erase complete */
            rc = PMC_SUCCESS;
            break;
        }

        if (sys_timer_diff(cpu_clk_start, cpu_clk_now) > timeout_cpu_clk)
        {
            /* timeout */
            bc_printf(" completion timeout\n");
            rc = SPI_ERR_WRITE_FAIL;
            break;
        }
    }

    return rc;

} /* poll_write_erase_complete */

/**
* @brief
*   Write data to SPI flash.
*
* @return
*   PMC_SUCCESS if no error
*   Error specific code otherwise
*
* @note
*/
PRIVATE PMCFW_ERROR write_pages(UINT8* src_ptr,
                                UINT8* dst_ptr,
                                UINT32 len,
                                UINT32 page_size,
                                UINT32 timeout)
{
    UINT32 write_len;
    PMCFW_ERROR rc;

    /* convert to the logical page size, which will be different if ECC is enabled */
    page_size = spi_size_phys_to_log(SPI_FLASH_PORT, page_size);

    /* write up until the end of the current page */
    write_len = page_size - (((UINT32)dst_ptr % page_size));
    while (len > 0)
    {
        write_len = min(write_len, len);

        /* write */
        rc = spi_flash_write(SPI_FLASH_PORT,
                             SPI_FLASH_CS,
                             src_ptr,
                             dst_ptr,
                             write_len);

        if (rc != PMC_SUCCESS)
        {
            bc_printf("Write error: %08lx\n", rc);
            return (rc);
        }

        rc = poll_write_erase_complete(TRUE, timeout);
        if (rc != PMC_SUCCESS)
        {
            bc_printf("poll_write_erase_complete error: %08lx\n", rc);
            return (rc);
        }

        src_ptr += write_len;
        dst_ptr += write_len;
        len -= write_len;

        /* write the remaining data one page at a time */
        write_len = page_size;
    }

    return PMC_SUCCESS;

} /* write_pages */

/**
* @brief
*   Command test handler stub function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_cmd_stub_handler(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* set the extended data flag */
    rsp_ptr->flags = EXP_FW_EXTENDED_DATA;

    /* set the extended data response length */
    rsp_ptr->ext_data_len = 4;

    /* set the extended data */
    ext_data_ptr[0] = 0x55;
    ext_data_ptr[1] = 0xAA;
    ext_data_ptr[2] = 0xBB;
    ext_data_ptr[3] = 0x33;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_cmd_stub_handler */

/**
* @brief
*   Command test stub function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_cmd_stub_test(exp_cmd_enum cmd)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    UINT32 crc;

    bc_printf("Preparing TWI command ... ");

    /* set the command id */
    cmd_ptr->id = cmd;

    /* set the command flags */
    cmd_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* set the request identifier */
    cmd_ptr->req_id = 0xDEAD;

    /* set the extended data length */
    cmd_ptr->ext_data_len = 0;

    /* set the extended data CRC */
    cmd_ptr->ext_data_crc = 0;

    /* set the host area */
    cmd_ptr->host_spad_area = 0;

    /* set the command work area */
    cmd_ptr->ech_spad_area = 0;

    /* clear the padding words */
    memset(cmd_ptr->padding, 0, sizeof(cmd_ptr->padding));

    /* clear the command arguments */
    memset(cmd_ptr->parms, 0, sizeof(cmd_ptr->parms));

    bc_printf("done\n");
    bc_printf("Calculating command CRC ... ");

    /* set the command CRC */
    cmd_ptr->crc = pmc_crc32((UINT8*)cmd_ptr,
                             (sizeof(exp_cmd_struct) - sizeof(cmd_ptr->crc)),
                             0,
                             TRUE,
                             TRUE);

    bc_printf("done\n");

    /* ring the doorbell */
    *sram_doorbell_ptr = ECH_DOORBELL_INGRESS;

    /* wait for the response to be posted */
    while (*sram_doorbell_ptr != ECH_DOORBELL_EGRESS)
        ;

    /* validate the response CRC */
    bc_printf("Validating response CRC ... ");
    crc = pmc_crc32((UINT8*)rsp_ptr,
                    (sizeof(exp_rsp_struct) - sizeof(rsp_ptr->crc)),
                    0,
                    TRUE,
                    TRUE);

    if (crc != rsp_ptr->crc)
    {
        /* invalid response  */
        bc_printf("\nResponse invalid: CRC error\n");

        /* test failed */
        return (FALSE);
    }
    bc_printf("done\n");

    /* validate additional data, if present */
    if (EXP_FW_EXTENDED_DATA_BIT_MASK == (rsp_ptr->flags & EXP_FW_EXTENDED_DATA_BIT_MASK))
    {
        bc_printf("Validating extended data CRC ... ");
        crc = pmc_crc32(ech_ext_data_ptr_get(),
                        rsp_ptr->ext_data_len,
                        0,
                        TRUE,
                        TRUE);

        if (crc != rsp_ptr->ext_data_crc)
        {
            /* invalid response */
            bc_printf("\nResponse invalid: extended data CRC error\n");

            /* test failed */
            return (FALSE);
        }
        bc_printf("done\n");
    }

    /* parse the response */
    if (cmd_ptr->id != rsp_ptr->id)
    {
        /* not the expected response */
        bc_printf("Response invalid: incorrect response ID\n");

        /* test failed */
        return (FALSE);
    }

    if (0xDEAD != rsp_ptr->req_id)
    {
        /* not the expected request ID */
        bc_printf("Response invalid: incorrect request ID\n");

        /* test failed */
        return (FALSE);
    }

    if (4 != rsp_ptr->ext_data_len)
    {
        /* not the expected extended data length */
        bc_printf("Response invalid: incorrect data length\n");

        /* test failed */
        return (FALSE);
    }

    if ((ext_data_ptr[0] != 0x55) ||
        (ext_data_ptr[1] != 0xAA) ||
        (ext_data_ptr[2] != 0xBB) ||
        (ext_data_ptr[3] != 0x33))
    {
        /* not the expected extended data */
        bc_printf("Response invalid: incorrect data\n");

        /* not the expected extended data */
        return (FALSE);
    }


    /* test passed */
    return (TRUE);

} /* ech_fw_cmd_stub_test */

/**
* @brief
*   DDR SPD and PHY data set command test handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_ddr_spd_phy_data_set_handler_test_common(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    ech_spd_ddr_cfg_struct* rx_spd_ddr_cfg_ptr = (ech_spd_ddr_cfg_struct*)ext_data_ptr;
    UINT8* rx_spd_cfg_ptr = (UINT8*)&rx_spd_ddr_cfg_ptr->ocmb_spd;
    UINT8* rx_ddr_cfg_ptr = (UINT8*)&rx_spd_ddr_cfg_ptr->ddr_phy_msdg;
    exp_ddr_ctrlr_spd_config_struct expected_spd_cfg;
    UINT8* expected_spd_cfg_ptr;
    user_input_msdg_t expected_ddr_cfg;
    UINT8* expected_ddr_cfg_ptr;
    UINT32 i;

    /*
    ** get a copy of the hard-coded SPD data
    ** clear stack memory as not all fields of structure are set by OCMB function
    */
    memset((VOID*)&expected_spd_cfg, 0x00, sizeof(exp_ddr_ctrlr_spd_config_struct));
    exp_ddr_ctrlr_sample_spd_config_init(&expected_spd_cfg);

    /* access the data as a byte array */
    expected_spd_cfg_ptr = (UINT8*)&expected_spd_cfg;

    /* compare the received DDR SPD data against expected data */
    bc_printf("Testing received DDR SPD data ");
    for (i = 0; i < sizeof(exp_ddr_ctrlr_spd_config_struct); i++)
    {
        if (expected_spd_cfg_ptr[i] != rx_spd_cfg_ptr[i])
        {
            /* SPD configuration data mismatch */
            bc_printf("\nSPD mismatch at offset %u, expected 0x%02X  received 0x%02X\n",
                      i,
                      expected_spd_cfg_ptr[i],
                      rx_spd_cfg_ptr[i]);
        }
        if (0 == (i % 25))
        {
            bc_printf(".");
        }
    }
    bc_printf(" done\n");

    /*
    ** get a copy of the hard-coded MSDG data
    ** clear structure as some fields may not be set DDR toolbox function
    */
    memset((VOID*)&expected_ddr_cfg, 0x00, sizeof(user_input_msdg_t));
    ddrphy_userinputmsdg_configure(&expected_ddr_cfg);

    /* access the data as a byte array */
    expected_ddr_cfg_ptr = (UINT8*)&expected_ddr_cfg;

    /* compare against received PHY configuration data */
    bc_printf("Testing received DDR PHY ");
    for (i = 0; i < sizeof(user_input_msdg_t); i++)
    {
        if (expected_ddr_cfg_ptr[i] != rx_ddr_cfg_ptr[i])
        {
            /* PHY configuration data mismatch */
            bc_printf("\nDDR PHY mismatch at offset %u, expected 0x%02X  received 0x%02X\n",
                      i, expected_ddr_cfg_ptr[i], rx_ddr_cfg_ptr[i]);
        }
        if (0 == (i % 25))
        {
            bc_printf(".");
        }
    }
    bc_printf(" done\n");

    /* copy data in extended data buffer to location just above */
    bc_printf("Preparing response data ");
    for (i = 0; i < cmd_ptr->ext_data_len; i++)
    {
        ext_data_ptr[cmd_ptr->ext_data_len + i] = ext_data_ptr[i];
        if (0 == (i % 25))
        {
            bc_printf(".");
        }
    }
    bc_printf(" done\n");

    /* set the extended data response length */
    rsp_ptr->ext_data_len = cmd_ptr->ext_data_len * 2;

    /* set the extended data flag */
    rsp_ptr->flags = EXP_FW_EXTENDED_DATA;

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();
}

/**
* @brief
*   DDR SPD and PHY data set command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_ddr_spd_phy_data_set_cmd_test_common(UINT32 cmd_id)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    UINT32 crc;
    UINT32 i;

    /*
    ** get a copy of the hard-coded SPD data
    ** clear structure as some fields may not be set OCMB function
    */
    memset((VOID*)&ech_spd_ddr_cfg.ocmb_spd, 0x00, sizeof(exp_ddr_ctrlr_spd_config_struct));
    bc_printf("Preparing DDR SPD data, size = %d bytes ... ", sizeof(ech_spd_ddr_cfg.ocmb_spd));
    exp_ddr_ctrlr_sample_spd_config_init(&ech_spd_ddr_cfg.ocmb_spd);
    bc_printf("done\n");

    /*
    ** get a copy of the hard-coded MSDG data
    ** clear structure as some fields may not be set DDR toolbox function
    */
    memset((VOID*)&ech_spd_ddr_cfg.ddr_phy_msdg, 0x00, sizeof(user_input_msdg_t));
    bc_printf("Preparing DDR PHY data, size = %d bytes ... ", sizeof(ech_spd_ddr_cfg.ddr_phy_msdg));
    ddrphy_userinputmsdg_configure(&ech_spd_ddr_cfg.ddr_phy_msdg);
    bc_printf("done\n");

    /* set the command id */
    cmd_ptr->id = cmd_id;

    /* set the command flags */
    cmd_ptr->flags = EXP_FW_EXTENDED_DATA;

    /* set the request identifier */
    cmd_ptr->req_id = 0xC0DA;

    /* set the extended data length */
    cmd_ptr->ext_data_len = sizeof(ech_spd_ddr_cfg);

    /* clear the extended data buffer */
    memset(ext_data_ptr, 0x00, (2 * sizeof(ech_spd_ddr_cfg)));

    /* record DDR SPD and PHY data in the extended data buffer */
    memcpy(ext_data_ptr, (VOID*)&ech_spd_ddr_cfg, sizeof(ech_spd_ddr_cfg));

    /* set the extended data CRC */
    cmd_ptr->ext_data_crc = pmc_crc32(ext_data_ptr,
                                      sizeof(ech_spd_ddr_cfg),
                                      0,
                                      TRUE,
                                      TRUE);

    /* set the host area */
    cmd_ptr->host_spad_area = 0;

    /* set the command work area */
    cmd_ptr->ech_spad_area = 0;

    /* clear the padding words */
    memset(cmd_ptr->padding, 0, sizeof(cmd_ptr->padding));

    /* clear the command arguments */
    memset(cmd_ptr->parms, 0, sizeof(cmd_ptr->parms));

    /* set the command CRC */
    cmd_ptr->crc = pmc_crc32((UINT8*)cmd_ptr,
                             (sizeof(exp_cmd_struct) - sizeof(cmd_ptr->crc)),
                             0,
                             TRUE,
                             TRUE);

    /* ring the doorbell */
    *sram_doorbell_ptr = ECH_DOORBELL_INGRESS;

    /* wait for the response to be posted */
    while (*sram_doorbell_ptr != ECH_DOORBELL_EGRESS)
        ;

    /* validate the response CRC */
    bc_printf("Validating response CRC ... ");
    crc = pmc_crc32((UINT8*)rsp_ptr,
                    (sizeof(exp_rsp_struct) - sizeof(rsp_ptr->crc)),
                    0,
                    TRUE,
                    TRUE);

    if (crc != rsp_ptr->crc)
    {
        /* invalid response */
        bc_printf("\nResponse invalid: CRC error\n");

        /* test failed */
        return (FALSE);
    }
    bc_printf("done\n");

    /* check for failure indication */
    if (EXP_FW_API_FAILURE == rsp_ptr->parms[0])
    {
        bc_printf("Command failed, error code: 0x%08X\n", rsp_ptr->parms[1]);
        return (FALSE);
    }

    /* validate additional data, if present */
    if (EXP_FW_EXTENDED_DATA_BIT_MASK == (rsp_ptr->flags & EXP_FW_EXTENDED_DATA_BIT_MASK))
    {
        bc_printf("Validating response extended data CRC ... ");
        crc = pmc_crc32(ext_data_ptr,
                        rsp_ptr->ext_data_len,
                        0,
                        TRUE,
                        TRUE);

        if (crc != rsp_ptr->ext_data_crc)
        {
            /* invalid response */
            bc_printf("\nResponse invalid: extended data CRC error\n");

            /* test failed */
            return (FALSE);
        }
        bc_printf("done\n");
    }

    /* parse the response */
    if (cmd_ptr->id != rsp_ptr->id)
    {
        /* not the expected response */
        bc_printf("Response invalid: incorrect response ID\n");

        /* test failed */
        return (FALSE);
    }

    if (0xC0DA != rsp_ptr->req_id)
    {
        /* not the expected request ID */
        bc_printf("Response invalid: incorrect request ID\n");

        /* test failed */
        return (FALSE);
    }


    if (rsp_ptr->ext_data_len != (cmd_ptr->ext_data_len * 2))
    {
        /* not the expected extended data length */
        bc_printf("Response invalid: incorrect response length\n");

        /* test failed */
        return (FALSE);
    }

    /* ensure returned data is as expected */
    bc_printf("Testing the response data ");
    for (i = 0; i < cmd_ptr->ext_data_len; i++)
    {
        if (ext_data_ptr[i] != ext_data_ptr[cmd_ptr->ext_data_len + i])
        {
            return (FALSE);
        }
        if (0 == (i % 25))
        {
            bc_printf(".");
        }
    }
    bc_printf(" done\n");


    /* test passed */
    return (TRUE);
}

/**
* @brief
*   DDR SPD and PHY interface init command test handler
*   function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_ddr_interface_init_handler_test(VOID)
{
    ech_fw_ddr_spd_phy_data_set_handler_test_common();

} /* ech_fw_ddr_interface_init_handler_test */

/**
* @brief
*   DDR SPD and PHY interface init command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_ddr_interface_init_cmd_test(VOID)
{
    return(ech_fw_ddr_spd_phy_data_set_cmd_test_common(EXP_FW_DDR_INTERFACE_INIT));

} /* ech_fw_ddr_interface_init_cmd_test */

/**
* @brief
*   DDR SPD and PHY data set command test handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_spd_data_set_handler_test(VOID)
{
    ech_fw_ddr_spd_phy_data_set_handler_test_common();

} /* ech_fw_spd_data_set_handler_test */


/**
* @brief
*   DDR SPD and PHY data set command test function.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC BOOL ech_fw_spd_data_set_cmd_test(VOID)
{
    return(ech_fw_ddr_spd_phy_data_set_cmd_test_common(EXP_FW_SPD_DATA_SET));

} /* ech_fw_spd_data_set_cmd_test */

/**
* @brief
*   Temperature sensor pass through read command test handler
*   function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_temp_sensor_pass_through_read_handler_test(VOID)
{
    ech_fw_cmd_stub_handler();

} /* ech_fw_temp_pass_through_read_handler_test */

/**
* @brief
*   Temperature sensor pass through read command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_temp_sensor_pass_through_read_cmd_test(VOID)
{
    return (ech_fw_cmd_stub_test(EXP_FW_TEMP_SENSOR_PASS_THROUGH_READ));

} /* ech_fw_temp_pass_through_read_cmd_test */

/**
* @brief
*   Temperature sensor pass through write command test handler
*   function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_temp_sensor_pass_through_write_handler_test(VOID)
{
    ech_fw_cmd_stub_handler();

} /* ech_fw_temp_pass_through_write_handler_test */

/**
* @brief
*   Temperature sensor pass through write command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_temp_sensor_pass_through_write_cmd_test(VOID)
{
    return (ech_fw_cmd_stub_test(EXP_FW_TEMP_SENSOR_PASS_THROUGH_WRITE));

} /* ech_fw_temp_pass_through_write_cmd_test */

/**
* @brief
*   Temperature sensor config interval read command test handler
*   function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_temp_sensor_cfg_interval_read_handler_test(VOID)
{
    ech_fw_cmd_stub_handler();

} /* ech_fw_temp_cfg_interval_read_handler_test */

/**
* @brief
*   Temperature sensor config interval read command test
*   function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_temp_sensor_cfg_interval_read_cmd_test(VOID)
{
    return (ech_fw_cmd_stub_test(EXP_FW_TEMP_SENSOR_CONFIG_INTERVAL_READ));

} /* ech_fw_temp_cfg_interval_read_cmd_test */

/**
* @brief
*   Go command test handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_go_handler_test(VOID)
{
    ech_fw_cmd_stub_handler();

} /* ech_fw_go_handler_test */

/**
* @brief
*   Go command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_go_cmd_test(VOID)
{
    return (ech_fw_cmd_stub_test(EXP_FW_GO_COMMAND));

} /* ech_fw_go_cmd_test */

/**
* @brief
*   Adapter properties get command test handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_adapter_properties_get_handler_test(VOID)
{
    ech_fw_cmd_stub_handler();

} /* ech_fw_adapter_properties_get_handler_test */

/**
* @brief
*   Adapter properties get command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_adapter_properties_get_cmd_test(VOID)
{
    return (ech_fw_cmd_stub_test(EXP_FW_ADAPTER_PROPERTIES_GET));

} /* ech_fw_adapter_properties_get_cmd_test */

/**
* @brief
*   Binary upgrade command test handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_binary_upgrade_handler_test(VOID)
{
    ech_fw_cmd_stub_handler();

} /* ech_fw_binary_upgrade_handler_test */

/**
* @brief
*   Binary upgrade command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_binary_upgrade_cmd_test(VOID)
{
    return (ech_fw_cmd_stub_test(EXP_FW_BINARY_UPGRADE));

} /* ech_fw_binary_upgrade_cmd_test */

/**
* @brief
*   Flash loader version info command test handler function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_flash_loader_version_info_handler_test(VOID)
{
    ech_fw_cmd_stub_handler();

} /* ech_fw_flash_loader_version_info_handler_test */

/**
* @brief
*   Flash loader version info command test function.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE BOOL ech_fw_flash_loader_version_info_cmd_test(VOID)
{
    return (ech_fw_cmd_stub_test(EXP_FW_FLASH_LOADER_VERSION_INFO));

} /* ech_fw_flash_loader_version_info_cmd_test */

/**
* @brief
*   Read RAM firmware log from base entry.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_log_ram_read_base_handler_test(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    UINT32 rd_cnt;
    static UINT32 entry_offset;
    static UINT32 last_read_entry;

    if (NULL == log_ptr)
    {
        /* set failure status */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        rsp_parms_ptr->status = FALSE;
        rsp_parms_ptr->err_code = PMCFW_ERR_FAIL;
    }
    else
    {
        if (TRUE == cmd_parms_ptr->start)
        {
            /* request to (re)-start read from base of active firmware log */

            /* reset the offset of the entries in the response */
            entry_offset = 0;

            if (0 == log_ptr->wr_idx_wrap)
            {
                /*
                ** log entries have not wrapped in circular buffer
                ** read entries from the start of the logfile
                */
                last_read_entry = 0;
            }
            else
            {
                /*
                ** log entries have wrapped in circular buffer
                ** read entries from oldest entry, which will be at next log write index
                */
                last_read_entry = log_ptr->wr_idx;
            }
        }

        /* clear the extended data buffer */
        memset(ext_data_ptr,
               0x00,
               (cmd_parms_ptr->num_entries * sizeof(log_app_entry_struct)));

        for (rd_cnt = 0; ((rd_cnt < cmd_parms_ptr->num_entries) && (last_read_entry < log_ptr->size_in_entries)); rd_cnt++, last_read_entry++)
        {
            /* copy logfile entries until number of request entries have been read or reach wrap point of logfile */

            /* write entries to the extended data buffer */
            memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                   (VOID*)&log_ptr->app_log_array[last_read_entry],
                   sizeof(log_app_entry_struct));
        }

        if (rd_cnt < cmd_parms_ptr->num_entries)
        {
            /* reached wrap point of logfile but have not read the number of requested entries */

            /* reset read index to the first entry of the logfile */
            last_read_entry = 0;

            for (; ((rd_cnt < cmd_parms_ptr->num_entries) && (last_read_entry < log_ptr->wr_idx)); rd_cnt++, last_read_entry++)
            {
                /* copy logfile entries until number of requested entries have been read or reach end of entries */

                /* write entries to the extended data buffer */
                memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                       (VOID*)&log_ptr->app_log_array[last_read_entry],
                       sizeof(log_app_entry_struct));
            }
        }

        /* set response parameters */
        rsp_parms_ptr->op = cmd_parms_ptr->op;
        rsp_parms_ptr->status = TRUE;
        rsp_parms_ptr->log_entries = log_ptr->size_in_entries;
        rsp_parms_ptr->num_entries = rd_cnt;
        rsp_parms_ptr->offset = entry_offset;

        /* record the offset of the next read */
        entry_offset += rd_cnt;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = rsp_parms_ptr->num_entries * sizeof(log_app_entry_struct);

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_EXTENDED_DATA;
    }

    /* set the response operand, same as command operand */
    rsp_parms_ptr->op = cmd_parms_ptr->op;

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();

} /*ech_fw_log_ram_read_base_handler_test */

/**
* @brief
*   Read RAM firmware log from head entry.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_log_ram_read_head_handler_test(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    UINT32 rd_cnt;
    UINT32 rd_entry;
    UINT32 entries_to_read;
    static UINT32 entry_offset;
    static UINT32 last_read_entry;

    if (NULL == log_ptr)
    {
        /* set failure status */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        rsp_parms_ptr->status = FALSE;
        rsp_parms_ptr->err_code = PMCFW_ERR_FAIL;
    }
    else
    {
        if (TRUE == cmd_parms_ptr->start)
        {
            /* request to (re-)start read from head of active firmware log */

            if (0 == log_ptr->wr_idx_wrap)
            {
                if (cmd_parms_ptr->num_entries >= log_ptr->wr_idx)
                {
                    /*
                    ** read request for more entries than exist in the log
                    ** read from the beginning of the log
                    */
                    last_read_entry = 0;
                    entries_to_read = log_ptr->wr_idx;
                }
                else
                {
                    /* read the requested number of entries */
                    last_read_entry = log_ptr->wr_idx - cmd_parms_ptr->num_entries;
                    entries_to_read = cmd_parms_ptr->num_entries;
                }

                /* set the offset of the logs being read */
                entry_offset = last_read_entry;
            }
            else
            {
                if (cmd_parms_ptr->num_entries > log_ptr->wr_idx)
                {
                    /*
                    ** log entries have wrapped in circular buffer
                    ** calculate the first entry to read
                    */
                    last_read_entry = log_ptr->size_in_entries - (cmd_parms_ptr->num_entries - log_ptr->wr_idx);

                    /* set the offset of the logs being read */
                    entry_offset = log_ptr->size_in_entries - cmd_parms_ptr->num_entries;

                    /* set the number of entries to read */
                    entries_to_read = cmd_parms_ptr->num_entries;

                    if (last_read_entry < log_ptr->wr_idx)
                    {
                        /*
                        ** read request past the oldest entry in the logfile
                        ** start reading from oldest entry
                        */
                        entries_to_read -= log_ptr->wr_idx - last_read_entry;
                        last_read_entry = log_ptr->wr_idx;

                        /* set the offset of the logs being read */
                        entry_offset = 0;
                    }
                }
                else
                {
                    /*
                    ** read request for a number of entries that doesn't
                    ** require log wraparound management
                    */
                    last_read_entry = log_ptr->wr_idx - cmd_parms_ptr->num_entries;

                    /* set the offset of the logs being read */
                    entry_offset = log_ptr->size_in_entries - cmd_parms_ptr->num_entries;

                    /* set the number of entries to read */
                    entries_to_read = cmd_parms_ptr->num_entries;
                }
            }
        }
        else
        {
            /* request to continue reading logs from the head of the active firmware log */

            if (0 == log_ptr->wr_idx_wrap)
            {
                /* log entries have not wrapped in circular buffer */

                if (cmd_parms_ptr->num_entries >= last_read_entry)
                {
                    /*
                    ** read request for more log entries than remain in logfile
                    ** read entries from the start of the logfile
                    */
                    entries_to_read = last_read_entry;
                    last_read_entry = 0;
                }
                else
                {
                    /*
                    ** read request for fewer log entries than exist in logfile
                    ** calculate the next entry to read
                    */
                    last_read_entry = last_read_entry - cmd_parms_ptr->num_entries;
                    entries_to_read = cmd_parms_ptr->num_entries;
                }

                /* set the offset of the logs being read */
                entry_offset = last_read_entry;
            }
            else
            {
                /* log entries have wrapped in circular buffer */

                if ((last_read_entry < log_ptr->wr_idx) &&
                    (cmd_parms_ptr->num_entries >= last_read_entry))
                {
                    /*
                    ** read from below the logical end of the log entries and
                    ** request will wraparound logfile, calculate first entry to read
                    */
                    last_read_entry = log_ptr->size_in_entries - (cmd_parms_ptr->num_entries - last_read_entry);

                    /* set the offset of the logs being read */
                    entry_offset = entry_offset - cmd_parms_ptr->num_entries;

                    /* set the number of entries to read */
                    entries_to_read = cmd_parms_ptr->num_entries;

                    if (last_read_entry < log_ptr->wr_idx)
                    {
                        /*
                        ** read request past the oldest entry in the logfile
                        ** start reading from oldest entry and adjust number of entries to read
                        */
                        entries_to_read -= log_ptr->wr_idx - last_read_entry;
                        last_read_entry = log_ptr->wr_idx;

                        /* set the offset of the logs being read */
                        entry_offset = 0;
                    }
                }
                else if ((last_read_entry > log_ptr->wr_idx) &&
                         (cmd_parms_ptr->num_entries >= (last_read_entry - log_ptr->wr_idx)))
                {
                    /*
                    ** read from above the logical end of the log entries and
                    ** request will go past oldest entry, set the read parameters
                    */
                    entries_to_read = last_read_entry - log_ptr->wr_idx;
                    last_read_entry = log_ptr->wr_idx;

                    /* set the offset of the logs being read */
                    entry_offset = 0;
                }
                else
                {
                    /*
                    ** read request will not wraparound logfile
                    ** calculate the next entry to read
                    */
                    if (last_read_entry > log_ptr->wr_idx)
                    {
                        /*
                        ** reading from above the oldest entry in the log
                        ** ensure the read doesn't access logs before the oldest log
                        */
                        last_read_entry = last_read_entry - cmd_parms_ptr->num_entries;

                        /* set the offset of the logs being read */
                        entry_offset = entry_offset - cmd_parms_ptr->num_entries;

                        /* set the number of entries to read */
                        entries_to_read = cmd_parms_ptr->num_entries;

                        if (last_read_entry < log_ptr->wr_idx)
                        {
                            /*
                            ** read request past the oldest entry in the logfile
                            ** start reading from oldest entry and adjust number of entries to read
                            */
                            entries_to_read -= log_ptr->wr_idx - last_read_entry;
                            last_read_entry = log_ptr->wr_idx;

                            /* set the offset of the logs being read */
                            entry_offset = 0;
                        }
                    }
                    else
                    {
                        last_read_entry = last_read_entry - cmd_parms_ptr->num_entries;

                        /* set the offset of the logs being read */
                        entry_offset = entry_offset - cmd_parms_ptr->num_entries;

                        /* set the number of entries to read */
                        entries_to_read = cmd_parms_ptr->num_entries;
                    }
                }
            }
        }

        /* clear the extended data buffer */
        memset(ext_data_ptr,
               0x00,
               (cmd_parms_ptr->num_entries * sizeof(log_app_entry_struct)));

        for (rd_cnt = 0, rd_entry = last_read_entry; ((rd_cnt < entries_to_read) && (rd_entry < log_ptr->size_in_entries)); rd_cnt++, rd_entry++)
        {
            /* copy logfile entries until number of request entries have been read or reach wrap point of logfile */

            /* write entries to the extended data buffer */
            memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                   (VOID*)&log_ptr->app_log_array[rd_entry],
                   sizeof(log_app_entry_struct));
        }

        if (rd_cnt < entries_to_read)
        {
            /* reached end of logfile but have not read the number of entries to read */

            /* reset read index to the first entry of the logfile */
            rd_entry = 0;

            for (; ((rd_cnt < entries_to_read) && (rd_entry < log_ptr->wr_idx)); rd_cnt++, rd_entry++)
            {
                /* copy logfile entries until number of requested entries have been read or reach end of entries */

                /* write entries to the extended data buffer */
                memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                       (VOID*)&log_ptr->app_log_array[rd_entry],
                       sizeof(log_app_entry_struct));
            }
        }

        /* set response parameters */
        rsp_parms_ptr->op = cmd_parms_ptr->op;
        rsp_parms_ptr->status = TRUE;
        rsp_parms_ptr->log_entries = log_ptr->size_in_entries;
        rsp_parms_ptr->num_entries = rd_cnt;
        rsp_parms_ptr->offset = entry_offset;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = rsp_parms_ptr->num_entries * sizeof(log_app_entry_struct);

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_EXTENDED_DATA;
    }

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();

} /*ech_fw_log_ram_read_head_handler_test */

/**
* @brief
*   Read SPI flash firmware log.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_log_spi_flash_read_base_handler_test(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    UINT32 rd_cnt;
    static UINT32 entry_offset;
    static UINT32 last_read_entry = 0;

    if ((LOG_SIGNATURE_START != flash_log_ptr->signature_start) || (LOG_SIGNATURE_END != flash_log_ptr->signature_end))
    {
        /* set failure status */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        rsp_parms_ptr->status = FALSE;
        rsp_parms_ptr->err_code = PMCFW_ERR_FAIL;
    }
    else
    {
        if (TRUE == cmd_parms_ptr->start)
        {
            /* request to read from start of stored firmware log */

            /* reset the offset of the entries in the response */
            entry_offset = 0;

            if (0 == flash_log_ptr->wr_idx_wrap)
            {
                /*
                ** log entries have not wrapped in circular buffer
                ** read entries from the start of the log buffer
                */
                last_read_entry = 0;
            }
            else
            {
                /*
                ** log entries have wrapped in circular buffer
                ** read entries from oldest entry, which will be at next log write index
                */
                last_read_entry = flash_log_ptr->wr_idx;
            }
        }

        /* clear the extended data buffer */
        memset(ext_data_ptr,
               0x00,
               (cmd_parms_ptr->num_entries * sizeof(log_app_entry_struct)));

        for (rd_cnt = 0; ((rd_cnt < cmd_parms_ptr->num_entries) && (last_read_entry < flash_log_ptr->size_in_entries)); rd_cnt++, last_read_entry++)
        {
            /* copy logfile entries until number of request entries have been read or reach end of logfile */

            /* write entries to the extended data buffer */
            memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                   (VOID*)&flash_log_ptr->app_log_array[last_read_entry],
                   sizeof(log_app_entry_struct));
        }

        if (rd_cnt < cmd_parms_ptr->num_entries)
        {
            /* reached end of logfile but have not read the number of requested entries */

            /* reset read index to the beginning of the logfile */
            last_read_entry = 0;

            for (; ((rd_cnt < cmd_parms_ptr->num_entries) && (last_read_entry < flash_log_ptr->wr_idx)); rd_cnt++, last_read_entry++)
            {
                /* copy logfile entries until number of request entries have been read or reach end of entries */

                /* write entries to the extended data buffer */
                memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                       (VOID*)&flash_log_ptr->app_log_array[last_read_entry],
                       sizeof(log_app_entry_struct));
            }
        }

        /* set response parameters */
        rsp_parms_ptr->op = cmd_parms_ptr->op;
        rsp_parms_ptr->status = TRUE;
        rsp_parms_ptr->log_entries = flash_log_ptr->size_in_entries;
        rsp_parms_ptr->num_entries = rd_cnt;
        rsp_parms_ptr->offset = entry_offset;

        /* record the offset of the next read */
        entry_offset += rd_cnt;

        /* set the number of entries in the response */
        rsp_parms_ptr->num_entries = rd_cnt;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = (rsp_parms_ptr->num_entries) * sizeof(log_app_entry_struct);

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_EXTENDED_DATA;
    }

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_log_spi_flash_read_base_handler_test */

/**
* @brief
*   Read SPI flash firmware log.
*
* @return
*   Nothing
*
* @note
*/
PRIVATE VOID ech_fw_log_spi_flash_read_head_handler_test(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    UINT32 rd_cnt;
    UINT32 rd_entry;
    UINT32 entries_to_read;
    static UINT32 entry_offset;
    static UINT32 last_read_entry;

    if (NULL == log_ptr)
    {
        /* set failure status */
        rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;
        rsp_parms_ptr->status = FALSE;
        rsp_parms_ptr->err_code = PMCFW_ERR_FAIL;
    }
    else
    {
        if (TRUE == cmd_parms_ptr->start)
        {
            /* request to (re-)start read from head of active firmware log */

            if (0 == flash_log_ptr->wr_idx_wrap)
            {
                if (cmd_parms_ptr->num_entries >= flash_log_ptr->wr_idx)
                {
                    /*
                    ** read request for more entries than exist in the log
                    ** read from the beginning of the log
                    */
                    last_read_entry = 0;
                    entries_to_read = flash_log_ptr->wr_idx;
                }
                else
                {
                    /* read the requested number of entries */
                    last_read_entry = flash_log_ptr->wr_idx - cmd_parms_ptr->num_entries;
                    entries_to_read = cmd_parms_ptr->num_entries;
                }

                /* set the offset of the logs being read */
                entry_offset = last_read_entry;
            }
            else
            {
                if (cmd_parms_ptr->num_entries > flash_log_ptr->wr_idx)
                {
                    /*
                    ** log entries have wrapped in circular buffer
                    ** calculate the first entry to read
                    */
                    last_read_entry = flash_log_ptr->size_in_entries - (cmd_parms_ptr->num_entries - flash_log_ptr->wr_idx);

                    /* set the offset of the logs being read */
                    entry_offset = flash_log_ptr->size_in_entries - cmd_parms_ptr->num_entries;

                    /* set the number of entries to read */
                    entries_to_read = cmd_parms_ptr->num_entries;

                    if (last_read_entry < flash_log_ptr->wr_idx)
                    {
                        /*
                        ** read request past the oldest entry in the logfile
                        ** start reading from oldest entry
                        */
                        entries_to_read -= flash_log_ptr->wr_idx - last_read_entry;
                        last_read_entry = flash_log_ptr->wr_idx;

                        /* set the offset of the logs being read */
                        entry_offset = 0;
                    }
                }
                else
                {
                    /*
                    ** read request for a number of entries that doesn't
                    ** require log wraparound management
                    */
                    last_read_entry = flash_log_ptr->wr_idx - cmd_parms_ptr->num_entries;

                    /* set the offset of the logs being read */
                    entry_offset = flash_log_ptr->size_in_entries - cmd_parms_ptr->num_entries;

                    /* set the number of entries to read */
                    entries_to_read = cmd_parms_ptr->num_entries;
                }
            }
        }
        else
        {
            /* request to continue reading logs from the head of the active firmware log */

            if (0 == flash_log_ptr->wr_idx_wrap)
            {
                /* log entries have not wrapped in circular buffer */

                if (cmd_parms_ptr->num_entries >= last_read_entry)
                {
                    /*
                    ** read request for more log entries than remain in logfile
                    ** read entries from the start of the logfile
                    */
                    entries_to_read = last_read_entry;
                    last_read_entry = 0;
                }
                else
                {
                    /*
                    ** read request for fewer log entries than exist in logfile
                    ** calculate the next entry to read
                    */
                    last_read_entry = last_read_entry - cmd_parms_ptr->num_entries;
                    entries_to_read = cmd_parms_ptr->num_entries;
                }

                /* set the offset of the logs being read */
                entry_offset = last_read_entry;
            }
            else
            {
                /* log entries have wrapped in circular buffer */

                if ((last_read_entry < flash_log_ptr->wr_idx) &&
                    (cmd_parms_ptr->num_entries >= last_read_entry))
                {
                    /*
                    ** read from below the logical end of the log entries and
                    ** request will wraparound logfile, calculate first entry to read
                    */
                    last_read_entry = flash_log_ptr->size_in_entries - (cmd_parms_ptr->num_entries - last_read_entry);

                    /* set the offset of the logs being read */
                    entry_offset = entry_offset - cmd_parms_ptr->num_entries;

                    /* set the number of entries to read */
                    entries_to_read = cmd_parms_ptr->num_entries;

                    if (last_read_entry < flash_log_ptr->wr_idx)
                    {
                        /*
                        ** read request past the oldest entry in the logfile
                        ** start reading from oldest entry and adjust number of entries to read
                        */
                        entries_to_read -= flash_log_ptr->wr_idx - last_read_entry;
                        last_read_entry = flash_log_ptr->wr_idx;

                        /* set the offset of the logs being read */
                        entry_offset = 0;
                    }
                }
                else if ((last_read_entry > flash_log_ptr->wr_idx) &&
                         (cmd_parms_ptr->num_entries >= (last_read_entry - flash_log_ptr->wr_idx)))
                {
                    /*
                    ** read from above the logical end of the log entries and
                    ** request will go past oldest entry, set the read parameters
                    */
                    entries_to_read = last_read_entry - flash_log_ptr->wr_idx;
                    last_read_entry = log_ptr->wr_idx;

                    /* set the offset of the logs being read */
                    entry_offset = 0;
                }
                else
                {
                    /*
                    ** read request will not wraparound logfile
                    ** calculate the next entry to read
                    */
                    if (last_read_entry > flash_log_ptr->wr_idx)
                    {
                        /*
                        ** reading from above the oldest entry in the log
                        ** ensure the read doesn't access logs before the oldest log
                        */
                        last_read_entry = last_read_entry - cmd_parms_ptr->num_entries;

                        /* set the offset of the logs being read */
                        entry_offset = entry_offset - cmd_parms_ptr->num_entries;

                        /* set the number of entries to read */
                        entries_to_read = cmd_parms_ptr->num_entries;

                        if (last_read_entry < flash_log_ptr->wr_idx)
                        {
                            /*
                            ** read request past the oldest entry in the logfile
                            ** start reading from oldest entry and adjust number of entries to read
                            */
                            entries_to_read -= flash_log_ptr->wr_idx - last_read_entry;
                            last_read_entry = flash_log_ptr->wr_idx;

                            /* set the offset of the logs being read */
                            entry_offset = 0;
                        }
                    }
                    else
                    {
                        last_read_entry = last_read_entry - cmd_parms_ptr->num_entries;

                        /* set the offset of the logs being read */
                        entry_offset = entry_offset - cmd_parms_ptr->num_entries;

                        /* set the number of entries to read */
                        entries_to_read = cmd_parms_ptr->num_entries;
                    }
                }
            }
        }

        /* clear the extended data buffer */
        memset(ext_data_ptr,
               0x00,
               (cmd_parms_ptr->num_entries * sizeof(log_app_entry_struct)));

        for (rd_cnt = 0, rd_entry = last_read_entry; ((rd_cnt < entries_to_read) && (rd_entry < flash_log_ptr->size_in_entries)); rd_cnt++, rd_entry++)
        {
            /* copy logfile entries until number of request entries have been read or reach wrap point of logfile */

            /* write entries to the extended data buffer */
            memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                   (VOID*)&flash_log_ptr->app_log_array[rd_entry],
                   sizeof(log_app_entry_struct));
        }

        if (rd_cnt < entries_to_read)
        {
            /* reached end of logfile but have not read the number of entries to read */

            /* reset read index to the first entry of the logfile */
            rd_entry = 0;

            for (; ((rd_cnt < entries_to_read) && (rd_entry < flash_log_ptr->wr_idx)); rd_cnt++, rd_entry++)
            {
                /* copy logfile entries until number of requested entries have been read or reach end of entries */

                /* write entries to the extended data buffer */
                memcpy(&ext_data_ptr[rd_cnt * sizeof(log_app_entry_struct)],
                       (VOID*)&flash_log_ptr->app_log_array[rd_entry],
                       sizeof(log_app_entry_struct));
            }
        }

        /* set response parameters */
        rsp_parms_ptr->op = cmd_parms_ptr->op;
        rsp_parms_ptr->status = TRUE;
        rsp_parms_ptr->log_entries = flash_log_ptr->size_in_entries;
        rsp_parms_ptr->num_entries = rd_cnt;
        rsp_parms_ptr->offset = entry_offset;

        /* set the extended data response length */
        rsp_ptr->ext_data_len = rsp_parms_ptr->num_entries * sizeof(log_app_entry_struct);

        /* set the extended data flag */
        rsp_ptr->flags = EXP_FW_EXTENDED_DATA;
    }

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_log_spi_flash_read_head_handler_test */

/**
* @brief
*   Erase SPI flash firmware log.
*
* @return
*   Nothing
* 
* @note
*/
PRIVATE VOID ech_fw_log_ram_erase_handler_test(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;


    if (NULL != log_ptr)
    {
        /* clear the RAM firmware log entries */
        memset(log_ptr->app_log_array, 
               0x00,
               (log_ptr->size_in_entries * sizeof(log_app_entry_struct)));

        /* set success status */
        rsp_parms_ptr->status = TRUE;
        rsp_parms_ptr->err_code = PMC_SUCCESS;
    }
    else
    {
        /* set failure status */
        rsp_parms_ptr->status = FALSE;
        rsp_parms_ptr->err_code = PMCFW_ERR_FAIL;
    }

    /* set the extended data flag */
    rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* set the response operand, same as the command operand */
    rsp_parms_ptr->op = cmd_parms_ptr->op;

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_log_ram_erase_handler_test */

/**
* @brief
*   Erase SPI flash firmware log.
*
* @return
*   Nothing
* 
* @note
*/
PRIVATE VOID ech_fw_log_spi_flash_erase_handler_test(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    UINT8* subsector_base;
    UINT32 subsector_len;
    UINT8* erase_addr = (UINT8*)SPI_FLASH_FW_LOG_TEST_MEM_BASE_ADDR;
    PMCFW_ERROR rc;

    /* set success status, overridden below if an error occurs */
    rsp_parms_ptr->status = TRUE;
    rsp_parms_ptr->err_code = PMC_SUCCESS;

    /* erase SPI flash subsectors over log span */
    for (UINT32 i = 0; i < APP_FW_LOG_SIZE ; i += subsector_len)
    {
        /* get the subsector address and length */
        rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                            SPI_FLASH_CS,
                                            erase_addr,
                                            &subsector_base,
                                            &subsector_len);

        if (rc != PMC_SUCCESS)
        {
            bc_printf("\nSector params error: %08lx\n", rc);

            /* set failure status */
            rsp_parms_ptr->status = FALSE;
            rsp_parms_ptr->err_code = rc;
            break;
        }

        /* erase the subsector */
        rc = spi_flash_subsector_erase(SPI_FLASH_PORT, 
                                       SPI_FLASH_CS, 
                                       subsector_base);

        if (rc != PMC_SUCCESS)
        {
            bc_printf("\nErase error: %08lx\n", rc);

            /* set failure status */
            rsp_parms_ptr->status = FALSE;
            rsp_parms_ptr->err_code = PMCFW_ERR_FAIL;
            break;
        }

        /* poll for completion */
        rc = poll_write_erase_complete(FALSE, spi_test_dev_info.max_time_sector_erase);
        if (rc != PMC_SUCCESS)
        {
            bc_printf("\npoll_write_erase_complete error(): %08lx\n", rc);

            /* set failure status */
            rsp_parms_ptr->status = FALSE;
            rsp_parms_ptr->err_code = rc;
            break;
        }

        /* increment the erase subsector address by the length of the subsector */
        erase_addr = (UINT8*)((UINT32)subsector_base + subsector_len);
    }

    /* set the extended data flag */
    rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* set the response operand, same as the command operand */
    rsp_parms_ptr->op = cmd_parms_ptr->op;

    /* set the response id, same as the command id */
    rsp_ptr->id = cmd_ptr->id;

    /* set the response request id, same as the command request id */
    rsp_ptr->req_id = cmd_ptr->req_id;

    /* send the response */
    ech_oc_rsp_proc();

} /* ech_fw_log_spi_flash_erase_handler_test */

/**
* @brief
*   Parse log commands and invoke supporting functions.
*
* @return
*   Nothing
* 
*
* @note
*/
PUBLIC VOID ech_fw_log_handler_test(VOID)
{
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;

    switch (cmd_parms_ptr->op)
    {
        case EXP_FW_LOG_OP_ACTIVE_RD_BASE:
        {
            /* request to read active firmware log from RAM */
            ech_fw_log_ram_read_base_handler_test();
        }
        break;

        case EXP_FW_LOG_OP_STORED_RD_BASE:
        {
            /* request to read stored firmware log from SPI flash */
            ech_fw_log_spi_flash_read_base_handler_test();
        }
        break;

        case EXP_FW_LOG_OP_ACTIVE_CLR:
        {
            /* request to erase active firmware log from RAM */
            ech_fw_log_ram_erase_handler_test();        
        }
        break;

        case EXP_FW_LOG_OP_STORED_CLR:
        {
            /* request to erase stored firmware log from RAM */
            ech_fw_log_spi_flash_erase_handler_test();        
        }
        break;

        case EXP_FW_LOG_OP_ACTIVE_RD_HEAD:
        {
            /* request to read from head of active firmware log from RAM */
            ech_fw_log_ram_read_head_handler_test();
        }
        break;

        case EXP_FW_LOG_OP_STORED_RD_HEAD:
        {
            /* request to read from head of stored firmware log from SPI flash */
            ech_fw_log_spi_flash_read_head_handler_test();
        }
        break;

        default:
        {
            exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
            exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;

            /* set the response parameters */
            rsp_parms_ptr->status = FALSE;
            rsp_parms_ptr->err_code = PMCFW_ERR_INVALID_PARAMETERS;
            rsp_parms_ptr->log_entries = 0;
            rsp_parms_ptr->num_entries = 0;
            rsp_parms_ptr->offset = 0;

            /* set the extended data response length */
            rsp_ptr->ext_data_len = 0;

            /* clear the extended data flag */
            rsp_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

            /* set the response operand, same as the command operand */
            rsp_parms_ptr->op = cmd_parms_ptr->op;

            /* set the response id, same as the command id */
            rsp_ptr->id = cmd_ptr->id;

            /* set the response request id, same as the command request id */
            rsp_ptr->req_id = cmd_ptr->req_id;

            /* send the response */
            ech_oc_rsp_proc();
        }
        break;
    }
} /* ech_fw_log_read_handler_test */

/**
* @brief
*   Initialize firmware log in RAM.
*
* @return
*   TRUE - SPI flash log initialized
*   FALSE - otherwise
*
* @note
*/
PRIVATE BOOL ech_fw_ram_log_init(VOID)
{
#if 0
    if (NULL == log_ptr)
    {
        /* test application log memory has not been allocated */

        log_ptr = MEM_ALLOC(MEM_TYPE_FREE, APP_FW_LOG_SIZE, HAL_MEM_NUMBYTES_CACHE_LINE);

        if (NULL == log_ptr)
        {
            bc_printf("could not allocate test application log memory");

            return (FALSE);
        }
   }
#else
    /* get the firmware log address */
    log_app_info_get((VOID*)&log_ptr);
#endif

    /* app log buffer allocated, clear memory */
    memset(log_ptr, 0x00, APP_FW_LOG_SIZE);

    /* synthesize log header */
    log_ptr->signature_start = LOG_SIGNATURE_START;
    log_ptr->version         = 1;
    log_ptr->size_in_entries = (APP_FW_LOG_SIZE - sizeof(log_cfg_struct)) / sizeof(log_app_entry_struct);
    log_ptr->wr_idx          = 15;
    log_ptr->frequency_hz    = sys_timer_freq_get();
    log_ptr->header_size     = sizeof(log_cfg_struct);
    log_ptr->app_log_array   = (log_app_entry_struct*)((UINT32)log_ptr + log_ptr->header_size);
    log_ptr->wr_idx_wrap     = 1;
    log_ptr->signature_end   = LOG_SIGNATURE_END;

    bc_printf("Priming active firmware log ... ");
    UINT8* data_ptr = (UINT8*)log_ptr->app_log_array;
    for (UINT32 i = (APP_FW_LOG_SIZE - 1), j = 0; j < (APP_FW_LOG_SIZE - log_ptr->header_size); i--, j++)
    {
        data_ptr[j] = i;
    }
    bc_printf("done\n");

    bc_printf("log_ptr                  = 0x%08X\n", log_ptr);
    bc_printf("log_ptr->signature_start = 0x%08X\n", log_ptr->signature_start);
    bc_printf("log_ptr->version         = 0x%08X\n", log_ptr->version);
    bc_printf("log_ptr->size_in_entries = 0x%08X\n", log_ptr->size_in_entries);
    bc_printf("log_ptr->wr_idx          = 0x%08X\n", log_ptr->wr_idx);
    bc_printf("log_ptr->frequency_hz    = 0x%08X\n", log_ptr->frequency_hz);
    bc_printf("log_ptr->app_log_array   = 0x%08X\n", log_ptr->app_log_array);
    bc_printf("log_ptr->header_size     = 0x%08X\n", log_ptr->header_size);
    bc_printf("log_ptr->wr_idx_wrap     = 0x%08X\n", log_ptr->wr_idx_wrap);
    bc_printf("log_ptr->signature_end   = 0x%08X\n", log_ptr->signature_end);

    return (TRUE);

} /* ech_fw_ram_log_init */

/**
* @brief
*   Initialize firmware log in SPI flash.
*
* @return
*   TRUE - SPI flash log initialized
*   FALSE - otherwise
*
* @note
*/
PRIVATE BOOL ech_spi_flash_log_init(VOID)
{
    UINT8* subsector_base;
    UINT32 subsector_len;
    PMCFW_ERROR rc;
    UINT32 i, j;
    UINT8* flash_log_phy_addr;
    UINT8* erase_addr;

    if (NULL == log_ptr)
    {
        /* app log not initialized */
        bc_printf("\nMust initialize application log before SPI flash log\n");
        return (FALSE);
    }

    /*
    ** before recording the log in SPI flash ensure that pointer to log entries
    ** that is stored in flash is referencing flash
    */
    log_app_entry_struct* ram_app_log_array = log_ptr->app_log_array;

    for (j = 0; j < 2; j++)
    {
        bc_printf("\nPriming Image %s stored firmware log @ ",
                  ((j == 0) ? "A" : "B"));

        if (j == 0)
        {
            flash_log_phy_addr = (UINT8 *)(SPI_FLASH_FW_IMG_A_CFG_LOG_ADDR & GPBC_FLASH_PHYS_ADDR_MASK);
            flash_log_ptr = (log_cfg_struct*)SPI_FLASH_FW_IMG_A_CFG_LOG_ADDR;
            log_ptr->app_log_array = (log_app_entry_struct*)((UINT32)flash_log_ptr + log_ptr->header_size);
        }
        else
        {
            flash_log_phy_addr = (UINT8 *)(SPI_FLASH_FW_IMG_B_CFG_LOG_ADDR & GPBC_FLASH_PHYS_ADDR_MASK);
            flash_log_ptr = (log_cfg_struct*)SPI_FLASH_FW_IMG_B_CFG_LOG_ADDR;
            log_ptr->app_log_array = (log_app_entry_struct*)((UINT32)flash_log_ptr + log_ptr->header_size);
        }
bc_printf("0x%08X ... ", flash_log_phy_addr);

        /* erase the flash partition used for logging */
        erase_addr = flash_log_phy_addr;
        for (i = 0; i < APP_FW_LOG_SIZE; i += subsector_len)
        {
            /* get the subsector address and length */
            rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                                SPI_FLASH_CS,
                                                erase_addr,
                                                &subsector_base,
                                                &subsector_len);

            if (rc != PMC_SUCCESS)
            {
                bc_printf("\nSector params error: %08lx\n", rc);
                return (FALSE);
            }

            /* erase the subsector */
            rc = spi_flash_subsector_erase(SPI_FLASH_PORT, SPI_FLASH_CS, subsector_base);
            if (rc != PMC_SUCCESS)
            {
                bc_printf("\nErase error: %08lx\n", rc);
                return (FALSE);
            }

            /* poll for completion */
            rc = poll_write_erase_complete(FALSE, spi_test_dev_info.max_time_sector_erase);
            if (rc != PMC_SUCCESS)
            {
                bc_printf("\npoll_write_erase_complete error(): %08lx\n", rc);
                return (FALSE);
            }

            /* increment the erase subsector address by the length of the subsector */
            erase_addr = (UINT8*)((UINT32)subsector_base + subsector_len);
        }

        /* write active firmware log to SPI flash */
        rc = write_pages((UINT8*)log_ptr,
                         (UINT8*)flash_log_phy_addr,
                         APP_FW_LOG_SIZE,
                         spi_test_dev_info.page_size,
                         spi_test_dev_info.max_time_page_prog);

        if (rc != PMC_SUCCESS)
        {
            bc_printf("\nwrite_pages error(): %08lx\n", rc);
            return (FALSE);
        }

        /* invalidate the cache so we read the flash memory */
        spi_cache_invalidate(SPI_FLASH_PORT,
                             SPI_FLASH_CS,
                             (UINT8*)flash_log_ptr,
                             APP_FW_LOG_SIZE);

        /* verify the SPI flash data */
        memset(ech_read_stored_fw_log, 0, APP_FW_LOG_SIZE);
        memcpy(ech_read_stored_fw_log,
               (VOID*)flash_log_ptr,
               APP_FW_LOG_SIZE);
        UINT8* flash_log_byte_ptr = (UINT8*)flash_log_ptr;
        for (i = 0; i < APP_FW_LOG_SIZE; i++)
        {
            if (ech_read_stored_fw_log[i] != flash_log_byte_ptr[i])
            {
                bc_printf("\nWrong byte read at %d: 0x%02X (expected 0x%02X)\n",
                          i,
                          ech_read_stored_fw_log[i],
                          flash_log_byte_ptr[i]);
                return (FALSE);
            }
        }

        bc_printf("done\n");

        bc_printf("flash_log_ptr                  = 0x%08X\n", flash_log_ptr);
        bc_printf("flash_log_ptr->signature_start = 0x%08X\n", flash_log_ptr->signature_start);
        bc_printf("flash_log_ptr->version         = 0x%08X\n", flash_log_ptr->version);
        bc_printf("flash_log_ptr->size_in_entries = 0x%08X\n", flash_log_ptr->size_in_entries);
        bc_printf("flash_log_ptr->wr_idx          = 0x%08X\n", flash_log_ptr->wr_idx);
        bc_printf("flash_log_ptr->frequency_hz    = 0x%08X\n", flash_log_ptr->frequency_hz);
        bc_printf("flash_log_ptr->app_log_array   = 0x%08X\n", flash_log_ptr->app_log_array);
        bc_printf("flash_log_ptr->header_size     = 0x%08X\n", flash_log_ptr->header_size);
        bc_printf("flash_log_ptr->wr_idx_wrap     = 0x%08X\n", flash_log_ptr->wr_idx_wrap);
        bc_printf("flash_log_ptr->signature_end   = 0x%08X\n", flash_log_ptr->signature_end);
    }

    /* restore the RAM logfile pointer to entries in RAM */
    log_ptr->app_log_array = ram_app_log_array;

    return (TRUE);

} /* ech_spi_flash_log_init */

/**
* @brief
*   Read firmware log in varying size packets.
*
* @return
*   TRUE - the test passed
*   FALSE - otherwise
*
* @note
*/
PRIVATE BOOL ech_fw_log_erase_test(UINT32 op)
{
    UINT32 i;
    UINT32 crc;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;
    VOID* flash_addr_ptr;

    bc_printf("Erasing %s firmware log ...",
              ((EXP_FW_LOG_OP_ACTIVE_CLR == op) ? "active" : "stored"));

    /* set the command id */
    cmd_ptr->id = EXP_FW_LOG;

    /* set the command flags */
    cmd_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* set the request identifier */
    cmd_ptr->req_id = 0xBABE;

    /* set the extended data length */
    cmd_ptr->ext_data_len = 0;

    /* set the host area */
    cmd_ptr->host_spad_area = 0;

    /* set the command work area */
    cmd_ptr->ech_spad_area = 0;

    /* clear the padding words */
    memset(cmd_ptr->padding, 0, sizeof(cmd_ptr->padding));

    /*
    ** set the command operand
    ** erasing either active log from RAM or stored log in SPI flash
    */
    cmd_parms_ptr->op = op;

    /* set the command CRC */
    cmd_ptr->crc = pmc_crc32((UINT8*)cmd_ptr,
                             (sizeof(exp_cmd_struct) - sizeof(cmd_ptr->crc)),
                             0,
                             TRUE,
                             TRUE);

    /* ring the doorbell */
    *sram_doorbell_ptr = ECH_DOORBELL_INGRESS;

    /* wait for the response to be posted */
    while (*sram_doorbell_ptr != ECH_DOORBELL_EGRESS)
        ;

    /* validate the response CRC */
    crc = pmc_crc32((UINT8*)rsp_ptr,
                    (sizeof(exp_rsp_struct) - sizeof(rsp_ptr->crc)),
                    0,
                    TRUE,
                    TRUE);

    if (crc != rsp_ptr->crc)
    {
        /* invalid response */
        bc_printf("\nResponse invalid: CRC error\n");

        /* test failed */
        return (FALSE);
    }

    if (FALSE == rsp_parms_ptr->status)
    {
        /* erase command failed */
        bc_printf(" failed returned status = 0x%02X  err_code = 0x%08X\n",
                  rsp_parms_ptr->status,
                  rsp_parms_ptr->err_code);
        return (FALSE);
    }

    if (EXP_FW_LOG_OP_ACTIVE_CLR == rsp_parms_ptr->op)
    {
        /* ensure that the active firmware log was cleared */
        UINT8* log_data_ptr = (UINT8*)log_ptr->app_log_array;
        UINT32 log_size_bytes = log_ptr->size_in_entries * sizeof(log_app_entry_struct);
        for (i = 0; i < log_size_bytes; i++)
        {
            if (0 != log_data_ptr[i])
            {
                bc_printf(" failed,RAM not cleared\n");
                return (FALSE);
            }
        }
    }
    else
    {

        if ((UINT32*)SPI_FLASH_FW_ACT_IMG_FLAG_ADDR == SPI_FLASH_ACTIVE_IMG_A)
        {
            /* firmware image A is the active image */

            /* get reference to image A SPI flash log*/
            flash_addr_ptr = (VOID*)SPI_FLASH_FW_IMG_A_CFG_LOG_ADDR;
        }
        else
        {
            /* firmware image B is the active image */

            /* get reference to image B SPI flash log*/
            flash_addr_ptr = (VOID*)SPI_FLASH_FW_IMG_B_CFG_LOG_ADDR;
        }

        /* read flash log memory */
        memcpy(ech_read_stored_fw_log,
               (VOID*)flash_addr_ptr,
               APP_FW_LOG_SIZE);

        /* ensure that the SPI flash log was cleared */
        for (i = 0; i < APP_FW_LOG_SIZE; i++)
        {
            if (0xFF != ech_read_stored_fw_log[i])
            {
                bc_printf(" failed, flash not erased @ offset = %d  val = 0x%02\n", 
                          i, 
                          ech_read_stored_fw_log[i]);
                return (FALSE);
            }
        }
    }

    bc_printf(" passed\n");

    return (TRUE);

} /* ech_fw_log_erase_test */

/**
* @brief
*   Read firmware log from base in varying size entries.
*
* @return
*   TRUE - the test passed
*   FALSE - otherwise
*
* @note
*/
PRIVATE BOOL ech_fw_log_read_base_test(UINT32 op)
{
    UINT32 i, j, k, l, m;
    UINT32 crc;
    UINT32 size_in_entries;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;

    /* clear the extended data buffer */
    memset(ext_data_ptr, 0x00, APP_FW_LOG_SIZE);

    /* set the command id */
    cmd_ptr->id = EXP_FW_LOG;

    /* set the command flags */
    cmd_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* set the request identifier */
    cmd_ptr->req_id = 0xBABE;

    /* set the extended data length */
    cmd_ptr->ext_data_len = 0;

    /* set the host area */
    cmd_ptr->host_spad_area = 0;

    /* set the command work area */
    cmd_ptr->ech_spad_area = 0;

    /* clear the padding words */
    memset(cmd_ptr->padding, 0, sizeof(cmd_ptr->padding));

    /*
    ** set the command operand
    ** accessing either active log from RAM or stored log in SPI flash
    */
    cmd_parms_ptr->op = op;

    if (EXP_FW_LOG_OP_ACTIVE_RD_BASE == op)
    {
        size_in_entries = log_ptr->size_in_entries;
    }
    else
    {
        size_in_entries = flash_log_ptr->size_in_entries;
    }

    /* request the firmware log in varying number of entries */
    for (i = 8; i < size_in_entries; i += 8)
    {
        bc_printf("Reading base of %s firmware log %d entries at a time ... ",
                  ((EXP_FW_LOG_OP_ACTIVE_RD_BASE == op) ? "active" : "stored"),
                  i);

        for (j = 0, m= 0; j < size_in_entries; j += i, m++)
        {
            if (0 == j)
            {
                /* requesting first packet of log data */
                cmd_parms_ptr->start = TRUE;
            }
            else
            {
                /* requesting next packet of log data */
                cmd_parms_ptr->start = FALSE;
            }

            /* set the number of requested log entries */
            cmd_parms_ptr->num_entries = i;

            /* set the command CRC */
            cmd_ptr->crc = pmc_crc32((UINT8*)cmd_ptr,
                                     (sizeof(exp_cmd_struct) - sizeof(cmd_ptr->crc)),
                                     0,
                                     TRUE,
                                     TRUE);

            /* ring the doorbell */
            *sram_doorbell_ptr = ECH_DOORBELL_INGRESS;

            /* wait for the response to be posted */
//            UINT32 counter = 0;
            while (*sram_doorbell_ptr != ECH_DOORBELL_EGRESS)
            {
#if 0
                if (0 == (++counter % 1000))
                {
                    bc_printf("counter = %d\n", counter);
                }
#endif
            }

            /* validate the response CRC */
            crc = pmc_crc32((UINT8*)rsp_ptr,
                            (sizeof(exp_rsp_struct) - sizeof(rsp_ptr->crc)),
                            0,
                            TRUE,
                            TRUE);

            if (crc != rsp_ptr->crc)
            {
                /* invalid response */
                bc_printf("\nResponse invalid: CRC error\n");

                /* test failed */
                return (FALSE);
            }

            /* validate additional data */
            if (EXP_FW_EXTENDED_DATA_BIT_MASK == (rsp_ptr->flags & EXP_FW_EXTENDED_DATA_BIT_MASK))
            {
                crc = pmc_crc32(ext_data_ptr,
                                rsp_ptr->ext_data_len,
                                0,
                                TRUE,
                                TRUE);

                if (crc != rsp_ptr->ext_data_crc)
                {
                    /* invalid response */
                    bc_printf("\nResponse invalid: extended data CRC error\n");

                    /* test failed */
                    return (FALSE);
                }
            }

            /* validate the response operand */
            if (rsp_parms_ptr->op != cmd_parms_ptr->op)
            {
                /* invalid operand */
                bc_printf("\nResponse invalid: response operand invalid\n");

                /* test failed */
                return (FALSE);
            }

            if (FALSE == rsp_parms_ptr->status)
            {
                /* operation failed */
                bc_printf("\nResponse invalid: operation failed err code = 0x%08X\n", rsp_parms_ptr->err_code);

                /* test failed */
                return (FALSE);
            }

            if (EXP_FW_LOG_OP_ACTIVE_RD_BASE == op)
            {
                /* copy the extended data to the read copy of the active firmware buffer */
                memcpy((VOID*)&ech_read_active_fw_log[rsp_parms_ptr->offset * sizeof(log_app_entry_struct)],
                       ext_data_ptr,
                       rsp_ptr->ext_data_len);
            }
            else
            {
                /* copy the extended data to the read copy of the stored firmware buffer */
                memcpy((VOID*)&ech_read_stored_fw_log[rsp_parms_ptr->offset * sizeof(log_app_entry_struct)],
                       ext_data_ptr,
                       rsp_ptr->ext_data_len);
            }

        }

        if (EXP_FW_LOG_OP_ACTIVE_RD_BASE == op)
        {
            /* test read data against active firmware log data */

            /* variables for testing results */
            UINT8 *log_data_ptr = (UINT8 *)log_ptr->app_log_array;
            UINT32 log_size_bytes = log_ptr->size_in_entries * sizeof(log_app_entry_struct);

            if (0 == log_ptr->wr_idx_wrap)
            {
                /* log has not wrapped, start comparison from start of log */
                k = 0;
            }
            else
            {
                /* log has wrapped, start comparison from oldest entry in the log */
                k = log_ptr->wr_idx * sizeof(log_app_entry_struct);
            }

            for (l = 0; l < log_size_bytes; l++)
            {
                if (ech_read_active_fw_log[l] != log_data_ptr[k])
                {
                    bc_printf("failed\n");
                    return (FALSE);
                }
                k = (++k) % log_size_bytes;
            }
            bc_printf("passed\n");
        }
        else
        {
            /* test read data against stored firmware log data */
            /* variables for testing results */
            UINT8 *log_data_ptr = (UINT8 *)flash_log_ptr->app_log_array;
            UINT32 log_size_bytes = flash_log_ptr->size_in_entries * sizeof(log_app_entry_struct);

            if (0 == flash_log_ptr->wr_idx_wrap)
            {
                /* log has not wrapped, start comparison from start of log */
                k = 0;
            }
            else
            {
                /* log has wrapped, start comparison from oldest entry in the log */
                k = flash_log_ptr->wr_idx * sizeof(log_app_entry_struct);
            }

            for (l = 0; l < log_size_bytes; l++)
            {
                if (ech_read_stored_fw_log[l] != log_data_ptr[k])
                {
                    bc_printf("failed\n");
                    return (FALSE);
                }
                k = (++k) % log_size_bytes;
            }
            bc_printf("passed\n");
        }
    }

    return (TRUE);

} /* ech_fw_log_read_base_test */

/**
* @brief
*   Read firmware log from head in varying size entries.
*
* @return
*   TRUE - the test passed
*   FALSE - otherwise
*
* @note
*/
PRIVATE BOOL ech_fw_log_read_head_test(UINT32 op)
{
    UINT32 i, j, k, l;
    UINT32 crc;
    exp_cmd_struct* cmd_ptr = ech_cmd_ptr_get();
    exp_rsp_struct* rsp_ptr = ech_rsp_ptr_get();
    UINT8* ext_data_ptr = ech_ext_data_ptr_get();
    exp_fw_log_cmd_parms_struct* cmd_parms_ptr = (exp_fw_log_cmd_parms_struct*)&cmd_ptr->parms;
    exp_fw_log_rsp_parms_struct* rsp_parms_ptr = (exp_fw_log_rsp_parms_struct*)&rsp_ptr->parms;

    /* clear the extended data buffer */
    memset(ext_data_ptr, 0x00, APP_FW_LOG_SIZE);

    /* set the command id */
    cmd_ptr->id = EXP_FW_LOG;

    /* set the command flags */
    cmd_ptr->flags = EXP_FW_NO_EXTENDED_DATA;

    /* set the request identifier */
    cmd_ptr->req_id = 0xBABE;

    /* set the extended data length */
    cmd_ptr->ext_data_len = 0;

    /* set the host area */
    cmd_ptr->host_spad_area = 0;

    /* set the command work area */
    cmd_ptr->ech_spad_area = 0;

    /* clear the padding words */
    memset(cmd_ptr->padding, 0, sizeof(cmd_ptr->padding));

    /*
    ** set the command operand
    ** accessing either active log from RAM or stored log in SPI flash
    */
    cmd_parms_ptr->op = op;

    /* request the firmware log in varying number of entries */
    for (i = 8; i < log_ptr->size_in_entries; i += 8)
    {
        bc_printf("Reading head of %s firmware log %d entries at a time ... ",
                  ((EXP_FW_LOG_OP_ACTIVE_RD_HEAD == op) ? "active" : "stored"),
                  i);

        for (j = 0; j < log_ptr->size_in_entries; j += i)
        {
            if (0 == j)
            {
                /* requesting first packet of log data */
                cmd_parms_ptr->start = TRUE;
            }
            else
            {
                /* requesting next packet of log data */
                cmd_parms_ptr->start = FALSE;
            }

            /* set the number of requested log entries */
            cmd_parms_ptr->num_entries = i;

            /* set the command CRC */
            cmd_ptr->crc = pmc_crc32((UINT8*)cmd_ptr,
                                     (sizeof(exp_cmd_struct) - sizeof(cmd_ptr->crc)),
                                     0,
                                     TRUE,
                                     TRUE);

            /* ring the doorbell */
            *sram_doorbell_ptr = ECH_DOORBELL_INGRESS;

            /* wait for the response to be posted */
            while (*sram_doorbell_ptr != ECH_DOORBELL_EGRESS)
                ;

            /* validate the response CRC */
            crc = pmc_crc32((UINT8*)rsp_ptr,
                            (sizeof(exp_rsp_struct) - sizeof(rsp_ptr->crc)),
                            0,
                            TRUE,
                            TRUE);

            if (crc != rsp_ptr->crc)
            {
                /* invalid response */
                bc_printf("\nResponse invalid: CRC error\n");

                /* test failed */
                return (FALSE);
            }

            /* validate additional data */
            if (EXP_FW_EXTENDED_DATA_BIT_MASK == (rsp_ptr->flags & EXP_FW_EXTENDED_DATA_BIT_MASK))
            {
                crc = pmc_crc32(ext_data_ptr,
                                rsp_ptr->ext_data_len,
                                0,
                                TRUE,
                                TRUE);

                if (crc != rsp_ptr->ext_data_crc)
                {
                    /* invalid response */
                    bc_printf("\nResponse invalid: extended data CRC error\n");

                    /* test failed */
                    return (FALSE);
                }
            }

            /* validate the response operand */
            if (rsp_parms_ptr->op != cmd_parms_ptr->op)
            {
                /* invalid operand */
                bc_printf("\nResponse invalid: response operand invalid\n");

                /* test failed */
                return (FALSE);
            }

            if (FALSE == rsp_parms_ptr->status)
            {
                /* operation failed */
                bc_printf("\nResponse invalid: operation failed err code = 0x%08X\n", rsp_parms_ptr->err_code);

                /* test failed */
                return (FALSE);
            }

            if (EXP_FW_LOG_OP_ACTIVE_RD_HEAD == op)
            {
                /* copy the extended data to the read copy of the active firmware buffer */
                memcpy((VOID*)&ech_read_active_fw_log[rsp_parms_ptr->offset * sizeof(log_app_entry_struct)],
                       ext_data_ptr,
                       rsp_ptr->ext_data_len);
            }
            else
            {
                /* copy the extended data to the read copy of the stored firmware buffer */
                memcpy((VOID*)&ech_read_stored_fw_log[rsp_parms_ptr->offset * sizeof(log_app_entry_struct)],
                       ext_data_ptr,
                       rsp_ptr->ext_data_len);
            }
        }

        if (EXP_FW_LOG_OP_ACTIVE_RD_HEAD == op)
        {
            /* test read data against active firmware log data */

            /* variables for testing results */
            UINT8 *log_data_ptr = (UINT8 *)log_ptr->app_log_array;
            UINT32 log_size_bytes = log_ptr->size_in_entries * sizeof(log_app_entry_struct);

            if (0 == log_ptr->wr_idx_wrap)
            {
                /* log has not wrapped, start comparison from start of log */
                k = 0;
            }
            else
            {
                /* log has wrapped, start comparison from oldest entry in the log */
                k = log_ptr->wr_idx * sizeof(log_app_entry_struct);
            }

            for (l = 0; l < log_size_bytes; l++)
            {
                if (ech_read_active_fw_log[l] != log_data_ptr[k])
                {
                    bc_printf("failed\n");
                    return (FALSE);
                }
                k = (++k) % log_size_bytes;
            }
            bc_printf("passed\n");
        }
        else
        {
            /* test read data against stored firmware log data */
            /* variables for testing results */
            UINT8 *log_data_ptr = (UINT8 *)flash_log_ptr->app_log_array;
            UINT32 log_size_bytes = log_ptr->size_in_entries * sizeof(log_app_entry_struct);

            if (0 == flash_log_ptr->wr_idx_wrap)
            {
                /* log has not wrapped, start comparison from start of log */
                k = 0;
            }
            else
            {
                /* log has wrapped, start comparison from oldest entry in the log */
                k = flash_log_ptr->wr_idx * sizeof(log_app_entry_struct);
            }

            for (l = 0; l < log_size_bytes; l++)
            {
                if (ech_read_stored_fw_log[l] != log_data_ptr[k])
                {
                    bc_printf("failed\n");
                    return (FALSE);
                }
                k = (++k) % log_size_bytes;
            }
            bc_printf("passed\n");
        }
    }

    return (TRUE);

} /* ech_fw_log_read_head_test */

/**
* @brief
*   Log command test function.
*
* @return
*   TRUE - success
*   FALSE - failure
*
* @note
*/
PRIVATE BOOL ech_fw_log_cmd_test(VOID)
{
    BOOL rc = TRUE;

    /* clear the buffers that will hold read log entries */
    memset(ech_read_active_fw_log, 0, sizeof(ech_read_active_fw_log));
    memset(ech_read_stored_fw_log, 0, sizeof(ech_read_stored_fw_log));

    /* allocate and prime the active firmware log RAM memory */
    if (FALSE == ech_fw_ram_log_init())
    {
        /* error */
        return (FALSE);
    }

    /* halt other core so SPI Flash can be programmed */
    hal_disable_mvpe();

    /* prime the SPI flash firmware log */
    if (FALSE == ech_spi_flash_log_init())
    {
        /* error */
        return (FALSE);
    }

    /* resume other core */
    hal_enable_mvpe();

#if 1
    bc_printf("\n");
    /* repeatedly read from the base of the active firmware log in varying size packets */
    if (FALSE == ech_fw_log_read_base_test(EXP_FW_LOG_OP_ACTIVE_RD_BASE))
    {
        /* failed reading the active firmware log */
        rc = FALSE;
    }
#endif

#if 1
    bc_printf("\n");
    /* repeatedly read from the base of the stored firmware log in varying size packets */
    if (FALSE == ech_fw_log_read_base_test(EXP_FW_LOG_OP_STORED_RD_BASE))
    {
        /* failed reading the active firmware log */
        rc = FALSE;
    }
#endif

#if 1
    bc_printf("\n");
    /* repeatedly read from the head of the active firmware log in varying size packets */
    if (FALSE == ech_fw_log_read_head_test(EXP_FW_LOG_OP_ACTIVE_RD_HEAD))
    {
        /* failed reading the active firmware log */
        rc = FALSE;
    }
#endif

#if 1
    bc_printf("\n");
    /* repeatedly read from the head of the stored firmware log in varying size packets */
    if (FALSE == ech_fw_log_read_head_test(EXP_FW_LOG_OP_STORED_RD_HEAD))
    {
        /* failed reading the active firmware log */
        rc = FALSE;
    }
#endif

    bc_printf("\n");
    /* erase the active firmware log */
    if (FALSE == ech_fw_log_erase_test(EXP_FW_LOG_OP_ACTIVE_CLR))
    {
        /* failed reading the active firmware log */
        rc = FALSE;
    }

    bc_printf("\n");
    /* erase the stored firmware log */
    if (FALSE == ech_fw_log_erase_test(EXP_FW_LOG_OP_STORED_CLR))
    {
        /* failed reading the active firmware log */
        rc = FALSE;
    }

    return (rc);

} /* ech_fw_err_log_get_cmd_test */
PMC_END_RAM_PROGRAM

/**
* @brief
*   Run the specified test and display the result.
*
* @param
*   test_num - test number
*
* @return
*   Nothing
*/
PRIVATE VOID ech_cmd_test_run(const UINT test_num)
{
    BOOL pass;

    bc_printf("\nRunning test %d ...\n", test_num);

    /* run the test */
    switch (test_num)
    {
        case EXP_FW_DDR_INTERFACE_INIT:
            /* firmware DDR interface init */
            pass = ech_fw_ddr_interface_init_cmd_test();
            break;

        case EXP_FW_SPD_DATA_SET:
            /* firmware SPD data set */
            pass = ech_fw_spd_data_set_cmd_test();
            break;

        case EXP_FW_TEMP_SENSOR_PASS_THROUGH_READ:
            /* firmware temperature sensor init */
            pass = ech_fw_temp_sensor_pass_through_read_cmd_test();
            break;

        case EXP_FW_TEMP_SENSOR_PASS_THROUGH_WRITE:
            /* firmware temperature sensor init */
            pass = ech_fw_temp_sensor_pass_through_write_cmd_test();
            break;

        case EXP_FW_TEMP_SENSOR_CONFIG_INTERVAL_READ:
            /* firmware temperature sensor init */
            pass = ech_fw_temp_sensor_cfg_interval_read_cmd_test();
            break;

        case EXP_FW_GO_COMMAND:
            /* firmware go command */
            pass = ech_fw_go_cmd_test();
            break;

        case EXP_FW_ADAPTER_PROPERTIES_GET:
            /* firmware adapter properties get */
            pass = ech_fw_adapter_properties_get_cmd_test();
            break;

        case EXP_FW_BINARY_UPGRADE:
            /* firmware binary upgrade */
            pass = ech_fw_binary_upgrade_cmd_test();
            break;

        case EXP_FW_FLASH_LOADER_VERSION_INFO:
            /* firmware flash loader version */
            pass = ech_fw_flash_loader_version_info_cmd_test();
            break;

        case EXP_FW_LOG:
            /* firmware error log interface init */
            pass = ech_fw_log_cmd_test();
            break;

        default:
            bc_printf("Unsupported test number: %d\n", test_num);

            /* unsupported test, default to pass */
            test_result[test_num] = TRUE;
            return;
    }

    /* display the result */
    if (pass == TRUE)
    {
        bc_printf("\nTest %d PASSED\n\n", test_num);
    }
    else
    {
        bc_printf("\nTest %d FAILED\n\n", test_num);
    }

    /* record the test result */
    test_result[test_num] = pass;

} /* ech_cmd_test_run() */


/*
* Public Functions
*/

/**
* @brief
*   Top-level ECH TWI slave unit test function. Exercises all
*   TWI commands supported by Explorer. TWI ports must be
*   initialized before this function is called.
*
* @param
*
* @return
*
*/
PUBLIC VOID ech_cmd_test(VOID)
{
    UINT32 i;
    spi_flash_dev_enum dev;
    PMCFW_ERROR rc;

    bc_printf("\nRunning OpenCAPI comand tests\n");

    /* clear SRAM doorbell location */
    *sram_doorbell_ptr = ECH_DOORBELL_CLEAR;

    /* register handler functions for the API's being tested */
    ech_api_func_register(EXP_FW_DDR_INTERFACE_INIT, ech_fw_ddr_interface_init_handler_test);
    ech_api_func_register(EXP_FW_SPD_DATA_SET, ech_fw_spd_data_set_handler_test);
    ech_api_func_register(EXP_FW_TEMP_SENSOR_PASS_THROUGH_READ, ech_fw_temp_sensor_pass_through_read_handler_test);
    ech_api_func_register(EXP_FW_TEMP_SENSOR_PASS_THROUGH_WRITE, ech_fw_temp_sensor_pass_through_write_handler_test);
    ech_api_func_register(EXP_FW_TEMP_SENSOR_CONFIG_INTERVAL_READ, ech_fw_temp_sensor_cfg_interval_read_handler_test);
    ech_api_func_register(EXP_FW_GO_COMMAND, ech_fw_go_handler_test);
    ech_api_func_register(EXP_FW_ADAPTER_PROPERTIES_GET, ech_fw_adapter_properties_get_handler_test);
    ech_api_func_register(EXP_FW_BINARY_UPGRADE, ech_fw_binary_upgrade_handler_test);
    ech_api_func_register(EXP_FW_FLASH_LOADER_VERSION_INFO, ech_fw_flash_loader_version_info_handler_test);
//    ech_api_func_register(EXP_FW_LOG, ech_fw_log_handler_test);

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &spi_test_dev_info);
    if (rc != PMC_SUCCESS)
    {
        bc_printf("Tests aborted, spi_flash_dev_info_get() error: %08lx\n", rc);
    }

    /* run all tests */
//    for (i = 0; i < EXP_FW_MAX_CMD; i++)
    for (i = EXP_FW_LOG; i < EXP_FW_MAX_CMD; i++)
    {
        ech_cmd_test_run(i);
    }

    bc_printf("OpenCAPI command tests completed\n");

    for (i = 0; i < EXP_FW_MAX_CMD; i++)
    {
        if (FALSE == test_result[i])
        {
            /* test failed */
            while (1) {}
        }
    }
} /* ech_cmd_test */

PMC_RAM_PROGRAM
/**
* @brief
*   Check if host has set ingress doorbell value to signal new
*   command request and set received command flag if so.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_cmd_test_host_doorbell_check(VOID)
{
    /* check the doorbell, set if command received */
    if (ECH_DOORBELL_INGRESS == *sram_doorbell_ptr)
    {
        ech_cmd_rxd_flag_set();
    }
}

/**
* @brief
*   Signal host that response has been sent.
*
* @return
*   Nothing
*
* @note
*/
PUBLIC VOID ech_cmd_test_host_doorbell_send(VOID)
{
    /* set the door bell to signal response sent */
    *sram_doorbell_ptr = ECH_DOORBELL_EGRESS;
}
PMC_END_RAM_PROGRAM

/** @} end addtogroup */



