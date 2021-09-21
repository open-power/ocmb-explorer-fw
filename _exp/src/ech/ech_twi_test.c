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
* @addtogroup TWI
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
#include "twi_plat.h"
#include "twi.h"
#include "spin.h"
#include "sys_timer.h"
#include "bc_printf.h"
#include "twi_hw.h"
#include "twi_regs.h"
#include <string.h>


/*
* Local Enumerated Types
*/


/*
* Local Constants
*/


/* number of ECH TWI slave tests */
#define ECH_TWI_SLAVE_TEST_NUM_MAX      5

/* timeout (us) to wait for the slave to finish */
#define ECH_TWI_TEST_SLAVE_TIMEOUT_US   1000000

/* TX/RX size (bytes) */
#define ECH_TWI_TEST_TX_RX_SIZE         32

/* selection of UART output for test: set to 1 to turn on UART output, set to 0 to turn off UART output */
#define ECH_TWI_TEST_MODE_UART_OUTPUT   1


/*
* Local Macro Definitions
*/


/*
* Local Structures and Unions
*/


/*
* Local Variables
*/

/* slave parameters */
PRIVATE twi_slave_struct ech_twi_test_slave;

/* record of test passes/fails */
PRIVATE BOOL test_result[ECH_TWI_SLAVE_TEST_NUM_MAX];

/* transmit/receive buffers */
UINT8 tx_buf[EXP_TWI_MAX_BUF_SIZE];
UINT8 rx_buf[EXP_TWI_MAX_BUF_SIZE];


/*
* Global Variables
*/
UINT8 test_buf[8] = {0xEF, 0xBE, 0xAD, 0xDE};


/*
* Private Functions
*/

/**
* @brief
*   TWI status command test
*
* @return
*   TRUE  - success
* @return
*   FALSE - failure
*/
#if 0
UINT32 twi_tx_result[10];
UINT32 twi_rx_result[10];
UINT32 twi_count = 0;
UINT32 rx_index = 0;
#endif
PUBLIC BOOL ech_twi_status_test(BOOL sub_test)
{
    PMCFW_ERROR rc = PMC_SUCCESS;

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    if (FALSE == sub_test)
    {
        bc_printf("Performing Status Test\n");
    }
#endif

    /* clear the TX / RX buffers */
    memset(tx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);
    memset(rx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);

    /* set the Status command byte */
    tx_buf[EXP_TWI_CMD_OFFSET] = EXP_FW_TWI_CMD_STATUS;

    /* transmit Status command to the slave */
    rc = twi_mst_tx(&ech_twi_test_slave,
                    tx_buf,
                    EXP_TWI_STATUS_CMD_LEN,
                    TRUE);

    if (rc != PMC_SUCCESS)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error 0x%x in twi_mst_tx\n", rc);
#endif
        return (FALSE);
    }

    /* read Status command response from the slave */
    rc = twi_mst_rx(&ech_twi_test_slave,
                    rx_buf,
                    EXP_TWI_STATUS_RSP_LEN,
                    TRUE);

    if (rc != PMC_SUCCESS)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error 0x%x in twi_mst_rx\n", rc);
#endif
        return (FALSE);
    }

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
{                                                           
    UINT32 i;
    for (i = 0; i < EXP_TWI_STATUS_RSP_LEN; i++)
    {
        bc_printf("rx byte %d: 0x%02X\n", i, rx_buf[i]);
    }
}
#endif

    if (EXP_TWI_STATUS_RSP_DATA_LEN != rx_buf[EXP_TWI_RSP_LEN_OFFSET])
    {
        return (FALSE);
    }

    if (EXP_TWI_STATUS_RUN_TIME != rx_buf[EXP_TWI_RSP_DATA_OFFSET + 1])
    {
        return (FALSE);
    }

    if (EXP_TWI_SUCCESS != rx_buf[EXP_TWI_RSP_DATA_OFFSET + 2])
    {
        return (FALSE);
    }

    if (EXP_FW_TWI_CMD_STATUS != rx_buf[EXP_TWI_RSP_DATA_OFFSET + 3])
    {
        return (FALSE);
    }

    return (TRUE);

} /* ech_twi_status_test */

/**
* @brief
*   TWI boot config command test
*
* @return
*   TRUE  - success
* @return
*   FALSE - failure
*/
PUBLIC BOOL ech_twi_boot_config_test(VOID)
{
    BOOL pass_flag = TRUE;
    PMCFW_ERROR rc;
    UINT16 cfg_flags =  (EXP_TWI_BOOT_CFG_FW_MANUFACTURING_MODE << EXP_TWI_BOOT_CFG_FW_MODE_BITOFF) |
                        (EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_MODE << EXP_TWI_BOOT_CFG_OCAPI_LOOPBACK_BITOFF) |
                        (EXP_TWI_BOOT_CFG_TL_TWI_MODE << EXP_TWI_BOOT_CFG_TL_MODE_BITOFF) |
                        (EXP_TWI_BOOT_CFG_DL_BOOT_WAIT_FOR_HOST << EXP_TWI_BOOT_CFG_DL_MODE_BITOFF) |
                        (EXP_TWI_BOOT_CFG_HOST_BOOT_MODE << EXP_TWI_BOOT_CFG_BOOT_MODE_BITOFF)|
                        (EXP_TWI_BOOT_CFG_LANE_8 << EXP_TWI_BOOT_CFG_LANE_MODE_BITOFF) |
                        (EXP_SERDES_21_33_GBPS << EXP_TWI_BOOT_CFG_SERDES_FREQ_BITOFF);

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    bc_printf("Performing Boot Config Test cfg_flags = 0x%04X\n", cfg_flags);
#endif

    /* clear the TX / RX buffers */
    memset(tx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);
    memset(rx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);

    /* set the boot config command byte */
    tx_buf[EXP_TWI_CMD_OFFSET] = EXP_FW_TWI_CMD_BOOT_CONFIG;

    /* set the data length */
    tx_buf[EXP_TWI_CMD_LEN_OFFSET] = EXP_TWI_BOOT_CFG_CMD_DATA_LEN;

    /* set the boot config flags */
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x00;
    tx_buf[4] = (UINT8)((cfg_flags & 0xFF00) >> 8);
    tx_buf[5] = (UINT8)(cfg_flags & 0x00FF);

    /* transmit Boot Config command to the slave */
    rc = twi_mst_tx(&ech_twi_test_slave,
                    tx_buf,
                    EXP_TWI_BOOT_CFG_CMD_LEN,
                    TRUE);

    if (rc != PMC_SUCCESS)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error 0x%x in twi_mst_tx\n", rc);
#endif
        return (FALSE);
    }

    /* check status */
    if (FALSE == ech_twi_status_test(TRUE))
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error boot configuration did not occur\n");
#endif
        return (FALSE);
    }

    /* ensure all configuration flags and values recorded correctly */
    if (FALSE == ech_mfg_mode_get())
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Fail: unexpected manufacturing mode = FALSE\n");
#endif
        pass_flag = FALSE;
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Pass: expected manufacturing mode = TRUE\n");
#endif
    }

    if (FALSE == ech_ocapi_loopback_get())
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Fail: unexpected loopback mode = FALSE\n");
#endif
        pass_flag = FALSE;
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Pass: expected loopback mode = TRUE\n");
#endif
    }

    if (EXP_TWI_BOOT_CFG_TL_TWI_MODE != ech_tl_mode_get())
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Fail: unexpected transport layer mode = 0x%02X\n", ech_tl_mode_get());
#endif
        pass_flag = FALSE;
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Pass: expected transport layer mode = 0x%02X\n", ech_tl_mode_get());
#endif
    }

    if (EXP_TWI_BOOT_CFG_DL_BOOT_WAIT_FOR_HOST != ech_dl_boot_mode_get())
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Fail: unexpected data link layer mode = 0x%02X\n", ech_dl_boot_mode_get());
#endif
        pass_flag = FALSE;
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Pass: expected data link layer mode = 0x%02X\n", ech_dl_boot_mode_get());
#endif
    }

    if (FALSE == ech_host_boot_mode_get())
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Fail: unexpected host boot mode = FALSE\n");
#endif
        pass_flag = FALSE;
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Pass: expected host boot mode = TRUE\n");
#endif
    }

    if (EXP_SERDES_21_33_GBPS != ech_serdes_freq_get())
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Fail: unexpected SERDES frequency selection = 0x%02X\n", ech_serdes_freq_get());
#endif
        pass_flag = FALSE;
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Pass: expected SERDES frequency selection = 0x%02X\n", ech_serdes_freq_get());
#endif
    }

    if (EXP_TWI_BOOT_CFG_LANE_8 != ech_lane_cfg_get())
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Fail: unexpected lane configuration = 0x%02X\n", ech_lane_cfg_get());
#endif
        pass_flag = FALSE;
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Pass: expected lane configuration = 0x%02X\n", ech_lane_cfg_get());
#endif
    }

    return (pass_flag);

} /* ech_twi_boot_config_test */

/**
* @brief
*   TWI register address latch test
*
* @return
*   TRUE  - success
* @return
*   FALSE - failure
*/
PUBLIC BOOL ech_twi_reg_addr_latch_test(BOOL subtest)
{
    PMCFW_ERROR rc = PMC_SUCCESS;

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    if (FALSE == subtest)
    {
        bc_printf("Performing Register Address Latch Test\n");
    }
#endif

    /* clear the TX / RX buffers */
    memset(tx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);
    memset(rx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);

    /* set the Register Address Latch command byte */
    tx_buf[EXP_TWI_CMD_OFFSET] = EXP_FW_TWI_CMD_REG_ADDR_LATCH;

    /* set the data length */
    tx_buf[EXP_TWI_CMD_LEN_OFFSET] = EXP_TWI_REG_ADDR_LATCH_CMD_DATA_LEN;

    /* set the register address */
    tx_buf[EXP_TWI_CMD_DATA_OFFSET]     = ((UINT32)test_buf >> 24) & 0xFF;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 1] = ((UINT32)test_buf >> 16) & 0xFF;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] = ((UINT32)test_buf >> 8) & 0xFF;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 3] = ((UINT32)test_buf >> 0) & 0xFF;

    /* transmit resgister address latch command to the slave */
    rc = twi_mst_tx(&ech_twi_test_slave,
                    tx_buf,
                    EXP_TWI_REG_ADDR_LATCH_CMD_LEN,
                    TRUE);

    if (rc != PMC_SUCCESS)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error 0x%x in twi_mst_tx\n", rc);
#endif
        return (FALSE);
    }

    /* check status */
    if (FALSE == ech_twi_status_test(TRUE))
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error register address latch did not occur\n");
#endif
        return (FALSE);
    }

    if (ech_latched_reg_addr_get() != (UINT32)test_buf)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error register addressed not latched: sent address = 0x%08X  latched address = 0x%08X",
                   (UINT32)test_buf,
                   ech_latched_reg_addr_get());
#endif
        return (FALSE);
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Success register addressed latched: sent address = 0x%08X  latched address = 0x%08X\n",
                   (UINT32)test_buf,
                   ech_latched_reg_addr_get());
#endif
    }

    return (TRUE);

} /* ech_twi_reg_addr_latch_test */

/**
* @brief
*   TWI register read test
*
* @return
*   TRUE  - success
* @return
*   FALSE - failure
*/
PUBLIC BOOL ech_twi_reg_read_test(VOID)
{
    PMCFW_ERROR rc = PMC_SUCCESS;
    UINT32 reg_val;

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    bc_printf("Performing Register Read Test\n");
#endif

    /* clear the TX / RX buffers */
    memset(tx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);
    memset(rx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);

    /* latch the register address */
    if (FALSE == ech_twi_reg_addr_latch_test(TRUE))
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error register addressed not latched: sent address = 0x%08X  latched address = 0x%08X",
                   (UINT32)test_buf,
                   ech_latched_reg_addr_get());
#endif
        return (FALSE);
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Success register addressed latched: sent address = 0x%08X  latched address = 0x%08X\n",
                   (UINT32)test_buf,
                   ech_latched_reg_addr_get());
#endif
    }

    /* set the Register Read command byte */
    tx_buf[EXP_TWI_CMD_OFFSET] = EXP_FW_TWI_CMD_REG_READ;

    /* set the data length */
    tx_buf[EXP_TWI_CMD_LEN_OFFSET] = EXP_TWI_REG_READ_CMD_DATA_LEN;

    /* set the register address, "don't care" as per fimrware arch doc */
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 0] = 0;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 1] = 0;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] = 0;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 3] = 0;

    /* transmit Register Read command to the slave and read the result */
    rc = twi_mst_tx_rx(&ech_twi_test_slave,
                       tx_buf,
                       EXP_TWI_REG_READ_CMD_LEN,
                       rx_buf,
                       EXP_TWI_REG_READ_RSP_LEN);

    if (rc != PMC_SUCCESS)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error 0x%x in twi_mst_tx_rx\n", rc);
#endif
        return (FALSE);
    }

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
{
    UINT32 i;
    for (i = 0; i < EXP_TWI_REG_READ_RSP_LEN; i++)
    {
        bc_printf("rx byte %d: 0x%02X\n", i, rx_buf[i]);
    }
}
#endif

    /* construct the register value */
    reg_val = (rx_buf[EXP_TWI_RSP_DATA_OFFSET + 0] << 24) |
              (rx_buf[EXP_TWI_RSP_DATA_OFFSET + 1] << 16) |
              (rx_buf[EXP_TWI_RSP_DATA_OFFSET + 2] << 8)  |
              (rx_buf[EXP_TWI_RSP_DATA_OFFSET + 3]);

    if (reg_val != *(UINT32*)test_buf)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error register value incorrect: expected value = 0x%08X  read value = 0x%08X\n",
                   *(UINT32*)test_buf,
                   reg_val);
#endif
        return (FALSE);
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Success register value correct: expected value = 0x%08X  read value = 0x%08X\n",
                   *(UINT32*)test_buf,
                   reg_val);
#endif
    }

    return (TRUE);

} /* ech_twi_reg_read_test */

PUBLIC BOOL ech_twi_reg_write_test(VOID)
{
    PMCFW_ERROR rc = PMC_SUCCESS;

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    bc_printf("Performing Register Write Test\n");
    bc_printf(" ...register address = 0x%08X\n", test_buf);
    bc_printf(" ...register value   = 0x%08X\n", *(UINT32*)test_buf);
#endif

    /* clear the TX / RX buffers */
    memset(tx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);
    memset(rx_buf, 0xAA, EXP_TWI_MAX_BUF_SIZE);

    /* set the Register Write command byte */
    tx_buf[EXP_TWI_CMD_OFFSET] = EXP_FW_TWI_CMD_REG_WRITE;

    /* set the data length */
    tx_buf[EXP_TWI_CMD_LEN_OFFSET] = EXP_TWI_REG_WRITE_CMD_DATA_LEN;

    /* set the register address */
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 0] = ((UINT32)test_buf >> 24) & 0xFF;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 1] = ((UINT32)test_buf >> 16) & 0xFF;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 2] = ((UINT32)test_buf >> 8) & 0xFF;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 3] = ((UINT32)test_buf >> 0) & 0xFF;

    /* set the register value */
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 4] = 0xBA;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 5] = 0xDB;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 6] = 0x10;
    tx_buf[EXP_TWI_CMD_DATA_OFFSET + 7] = 0x0D;

    rc = twi_mst_tx(&ech_twi_test_slave,
                    tx_buf,
                    EXP_TWI_REG_WRITE_CMD_LEN,
                    TRUE);

    if (rc != PMC_SUCCESS)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error 0x%x in twi_mst_tx_rx\n", rc);
#endif
        return (FALSE);
    }

    /* check status */
    if (FALSE == ech_twi_status_test(TRUE))
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error register write did not occur\n");
#endif
        return (FALSE);
    }

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    bc_printf(" ...register written\n");
    bc_printf(" ...register value   = 0x%08X\n", *(UINT32*)test_buf);
#endif

    /* confirm the write data is as expected */
    if (0xBADB100D != *(UINT32*)test_buf)
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Error register value incorrect: expected value = 0x%08X  read value = 0x%08X\n",
                   0xBADB100D,
                   *(UINT32*)test_buf);
#endif
        return (FALSE);
    }
    else
    {
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
        bc_printf("Success register value correct: expected value = 0x%08X  read value = 0x%08X\n",
                   0xBADB100D,
                   *(UINT32*)test_buf);
#endif
    }

    return (TRUE);

} /* ech_twi_reg_write_test */

/**
* @brief
*   Run the specified test and display the result.
*
* @param
*   test_num - test number
*
* @return
*   TRUE  - success
* @return
*   FALSE - failure
*/
PRIVATE VOID ech_twi_slave_test_run(const UINT test_num)
{
    BOOL pass;

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    bc_printf("\nRunning test %d...\n", test_num);
#endif

    /* run the test */
    switch (test_num)
    {
        case 0:
            /* Status Command */
            pass = ech_twi_status_test(FALSE);
            break;

        case 1:
            /* Boot Config Command */
            pass = ech_twi_boot_config_test();
            break;

        case 2:
            /* Register Address Latch Command */
            pass = ech_twi_reg_addr_latch_test(FALSE);
            break;

        case 3:
            /* Register Read Command */
            pass = ech_twi_reg_read_test();
            break;

        case 4:
            /* Register Write Command */
            pass = ech_twi_reg_write_test();
            break;

        default:
#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
            bc_printf("Unsupported test number: %d\n", test_num);
#endif
            /* force pass condition on unused tests */
            pass = TRUE;
            break;
    }

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    /* display the result */
    if (pass == TRUE)
    {
        bc_printf("Test %d PASSED\n", test_num);
    }
    else
    {
        bc_printf("Test %d FAILED\n", test_num);
    }
#endif

    /* record the test result */
    test_result[test_num] = pass;

} /* ech_twi_slave_test_run() */


/*
* Public Functions
*/

/**
* @brief
*   set-up TWI slave for testing.
*
* @param
*
* @return
*
*/
PUBLIC VOID ech_twi_slave_setup(UINT32 mst_port_id,
                                UINT32 slave_addr)
{
    /* slave parameters */
    ech_twi_test_slave.port_id = mst_port_id;
    ech_twi_test_slave.addr = slave_addr;
    ech_twi_test_slave.addr_size = EXP_TWI_ADDR_SIZE;
#if 1
    ech_twi_test_slave.stretch_timeout_ms = EXP_TWI_STRETCH_TIMEOUT_MS;
#else
    ech_twi_test_slave.stretch_timeout_ms = 0;

    /* do not allow slave to clock stretch */
    TWI_S_WRITE(EXP_TWI_SLAVE_PORT, TWI_S_IC_SLAVE_SCL_STRETCH_DISABLE, 1);

    /* issue NACKs until command is processed */
    TWI_S_WRITE(EXP_TWI_SLAVE_PORT, TWI_S_IC_SLV_DATA_NACK_ONLY, 1);
#endif
}

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
PUBLIC VOID ech_twi_slave_test(UINT32 mst_port_id,
                               UINT32 slave_addr)
{
    UINT32 i;

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    bc_printf("\nRunning TWI slave test in polling mode\n");
#endif

    ech_twi_slave_setup(mst_port_id, slave_addr);

    /* run tests */
    for (i = 0; i < ECH_TWI_SLAVE_TEST_NUM_MAX; i++)
    {
        ech_twi_slave_test_run(i);
    }

#if (ECH_TWI_TEST_MODE_UART_OUTPUT == 1)
    bc_printf("\nTWI slave tests completed\n");
#endif

    for (i = 0; i < ECH_TWI_SLAVE_TEST_NUM_MAX; i++)
    {
        if (FALSE == test_result[i])
        {
            /* test failed */
            while (1) {}
        }
    }
} /* twi_unit_test() */

/** @} end addtogroup */



