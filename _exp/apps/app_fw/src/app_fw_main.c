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

/********************************************************************************
*   DESCRIPTION : This is the top level file for the sboot application.
*                 sboot will load and authenticate images stored in FLASH
*                 memory and bring up the PCIe interface.
*
*   NOTES:        None.
*
*
*******************************************************************************/



/*
** Include Files
*/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "pmcfw_common.h"
#include "bc_printf.h"
#include "crash_dump.h"
#include "pmc_profile.h"
#include "cicint.h"
#include "dcsu_api.h"
#include "mem.h"
#include "spb_mux.h"
#include "spb_twi.h"
#include "twi.h"
#include "exp_api.h"
#include "ddr_api.h"
#include "ech.h"
#include "uart_simple.h"
#include "app_fw.h"
#include "app_fw_ddr.h"
#include "printf.h"
#include "tsh.h"
#include "cmdsvr.h"
#include "exp_cmdsvr.h"
#include "seeprom.h"
#include "exp_gic.h"
#include "spi_plat.h"
#include "spi_api.h"
#include "spi_flash.h"
#include "spi_flash_plat.h"
#include "pmc_plat.h"
#include "cpuhal.h"
#include "log_app_api.h"
#include "log_plat.h"
#include "top_exp_config_guide.h"
#include "reset.h"
#include "temp_sensor_plat.h"
#include "ddr_phy_plat.h"
#include "ccb_api.h"
#include "char_io.h"
#include "spi_flash_plat.h"
#include "log_plat.h"
#include "spi_plat.h"
#include "busio.h"
#include "dcsu_api.h"
#include "spb_spi.h"
#include "spi.h"
#include "top.h"
#include "serdes_cmdsvr.h"
#include "fam_plat.h"
#include "exp_pcse_timer.h"
#include "fatal_plat.h"
#include "serdes_plat.h"
#include "crash_dump_plat.h"
#include "opsw_fatal.h"
#include "opsw_api.h"
#include "twi_cmdsvr.h"
#include "ocmb_plat.h"
#include "ccb_plat.h"
#include "flashloader.h"
#include "flash_partition_info.h"

#if (EXPLORER_BRINGUP == 1)
EXTERN void expl_fca_bringup(void);
#endif

/*
** Local Constants
*/

/* Command Server config */
#define APP_FW_CMDSVR_CMD_LISTS_MAX             10

/* Circular Character Buffer Count for the system */
#define APP_FW_CCB_BUFFER_COUNT                 CHAR_IO_NUM_CHANNELS

/* Circular buffer sizes for both runtime logging and crash logging. */
#define APP_FW_CHAR_IO_RUNTIME_CCB_SIZE         APP_FW_LOG_SIZE
#define APP_FW_CHAR_IO_CRASH_CCB_SIZE           (1*1024)


/*
** Forward References
*/

EXTERN void _start_smp(void);
EXTERN void adapter_info_module_init(void);

PUBLIC volatile BOOL app_fw_oc_ready = FALSE;
PRIVATE volatile BOOL start_second_vpe = FALSE;


/*
** Private Functions
*/

/**
* @brief
*   Adjustment of all pointers to functions in RAM to remove PIC
*   offset from the address.
*
* @param
*
* @return
*
*/
PRIVATE VOID app_fw_ram_code_ptr_adjust(VOID)
{
    UINT32 offset = exp_plat_get_pic_offset();

    /* adjust the log platform function pointers */
    log_plat_ram_code_ptr_adjust(offset);

    /* adjust the SPI platform function pointers */
    spi_plat_ram_code_ptr_adjust(offset);

    /* adjust the busio function pointers */
    busio_ram_code_ptr_adjust(offset);

    /* adjust the HAL timer function pointers */
    hal_cp0_timer_ram_code_ptr_adjust(offset);

    /* adjust the HAL cache memory function pointers */
    hal_cache_ram_code_ptr_adjust(offset);

    /* adjust the DCSU function pointers */
    dcsu_ram_code_ptr_adjust(offset);

    /* adjust the SPB SPI function pointers */
    spb_spi_ram_code_ptr_adjust(offset);

    /* adjust the SPI function pointers */
    spi_ram_code_ptr_adjust(offset);

    /* adjust the SPI control function pointers */
    spi_ctrl_ram_code_ptr_adjust(offset);

    /* adjust the SPI flash function pointers */
    spi_flash_ram_code_ptr_adjust(offset);

    /* adjust the SPI N25Q function pointers */
    spi_n25q_ram_code_ptr_adjust(offset);

    /* adjust the SPI N25Q function pointers */
    sys_timer_ram_code_ptr_adjust(offset);

    /* adjust the top function pointers */
    top_ram_code_ptr_adjust(offset);

    /* adjust the crash dump function pointers */
    crash_dump_plat_ram_code_ptr_adjust(offset);
}


/*
** Public Functions
*/
#pragma ghs section text=".app_fw_start"
/*******************************************************************************
*
*  FUNCTION: main()
*  ___________________________________________________________________________
*
*  DESCRIPTION:
*    Entry point of application FW.
*
*  INPUTS:
*    None.
*
*  OUTPUTS:
*    None.
*
*  RETURNS:
*    None.
*
*******************************************************************************/
PUBLIC int main(void)
{
     _start_smp();
}
#pragma ghs section text=default

/**
* @brief
*   Main loop running on VPE0 for the 34k.
*
* @param
*
* @return
*
*/
void  _34K_VPE0_main(void)
{
    tsh_parms_struct*  tsh_parms_ptr;
    spi_parms_struct*  spi_parms_ptr;
    VOID*  app_log_addr = NULL;
    UINT32 app_log_size = 0;
    PMCFW_ERROR rc;
    
    uart_init(APP_FW_UART_ID,
              APP_FW_UART_BASE_ADDRESS,
              8,
              1,
              UART_PARITY_NONE,
              UART_BAUD_115200,
              FALSE);

    /* Initialise the memory module */
    mem_init(MAX_MEMORY_SECTION,
             (UINT32)__ghsbegin_free_mem,
             (UINT32)__ghsend_free_mem - (UINT32)__ghsbegin_free_mem,
             HAL_MEM_NUMBYTES_CACHE_LINE);

    /*This sections is dedicated for FW authentication module*/
    mem_add_section(AUTH_MEMORY_SECTION_ID,
                    (UINT32)__ghsbegin_fw_auth_mem,
                    (UINT32)__ghsend_fw_auth_mem - (UINT32)__ghsbegin_fw_auth_mem,
                    HAL_MEM_NUMBYTES_CACHE_LINE);

    /* Initialize CP0 timer as system timer */
    hal_cp0_timer_init(dcsu_cpu_clk_freq_get);
    hal_cp0_timer_register();

    /* initialize timer0 as 1 Hz timer */
    exp_timer0_init();

    /* Initialize circular buffer for string buffers. */
    ccb_init(APP_FW_CCB_BUFFER_COUNT);
    char_io_init(APP_FW_CHAR_IO_RUNTIME_CCB_SIZE, APP_FW_CHAR_IO_CRASH_CCB_SIZE);

    /* initialize bc_printf */
    bc_printf_init(APP_FW_UART_ID);

    /* initialize the CMDSVR */
    cmdsvr_init(APP_FW_CMDSVR_CMD_LISTS_MAX, FALSE);

    /* adjust pointers to functions in RAM to remove PIC offset */
    app_fw_ram_code_ptr_adjust();

    /* initialize crash dump */
    crash_dump_init();

    bc_printf("Booting APP_FW %s ....\n",
              ((flash_partition_boot_partition_id_get() == 'A') ? "Image A" : "Image B"));

    /*
    ** Check APP_LOG_SIZE > 0 to enable the classic encoded logging;
    ** otherwise, string logging has been enabled or no logging is enabled
    */
    if (app_log_size > 0)
    {
        app_log_addr = MEM_ALLOC(MEM_TYPE_FREE, app_log_size, HAL_MEM_NUMBYTES_CACHE_LINE);
    }

    /* log initialization called first as all modules may use logs */
    log_app_init(app_log_size, app_log_addr, log_cache_ops_level_L1_L2);

    /* enable logging of high severity events */
    log_app_global_sev_filter_level_set(log_sev_high);

    /* enable logging of highest severity events out to the Hardware STDOUT device*/
    log_app_global_hw_sev_filter_level_set(log_sev_highest);

    /* initialize the reset module */
    app_fw_reset_init();

    /* Initialize the OPSW block - including fatal and non-fatal interrupt enablement. */
    opsw_fatal_init();

    /* initialize the Global interrupt controller */
    /* The function exp_gic_init() also enables interrupts on the PCSe */
    exp_gic_init();

    /* halt other core so SPI Flash driver can be initialized */
    hal_disable_mvpe();

    /* initialize the SPI driver */
    spi_parms_ptr = spi_parms_get();
    spi_init(spi_parms_ptr);

    /* initialize the SPI Flash driver */
    rc = spi_flash_init(SPI_FLASH_PORT,
                        SPI_FLASH_CS,
                        SPI_FLASH_DEV);

    PMCFW_ASSERT(rc == PMC_SUCCESS, rc);

    /*Initialize FAM module*/
    fam_plat_init();    

    /* 
    ** check redundant firmware image against active image
    ** and update redundant if it is older than active
    */
    spi_flash_plat_red_fw_image_update();

    /* resume other core */
    hal_enable_mvpe();

#if (EXPLORER_BRINGUP == 1)
    expl_fca_bringup();
#endif

    /* register SerDes commands with comand server and crash dump */
    serdes_cmdsvr_register();
    serdes_plat_crash_dump_register();

    /* register OCMB with crash dump */
    ocmb_plat_crash_dump_register();

    /* initialize the Explorer command handler */
    ech_init();

    /*
    ** initialize platform specific features of the log module
    ** must be after ech_init
    */
    log_plat_init();

    /* Tiny Shell */
    tsh_parms_ptr = tsh_parms_get(APP_FW_TSH_SHELL_IDX);
    tsh_parms_ptr->uart_port = APP_FW_UART_ID;
    tsh_parms_ptr->cfg.prompt_str = "0x00000000:0001>";
    tsh_init(tsh_parms_ptr);
    tsh_cmdsvr_register(APP_FW_TSH_SHELL_IDX, cmdsvr_command_exec);
    tsh_create(tsh_parms_ptr);

    /* register Explorer-specific CMDSVR commands */
    exp_cmdsvr_init();

    /* register DDR CMDSVR commands */
    app_fw_ddr_cmdsvr_init();

    /* initialize the SEEPROM */
    seeprom_init();
    
    /* Initialize adapter info module*/
    adapter_info_module_init();

    /* DDR PHY platform initialization*/
    ddr_phy_plat_init();

    /* FATAL module platform initialization.*/
    fatal_plat_init();

    /* Initialize Flashloader module */
    flashloader_init();

    /* 
    ** I2C is enabled after all modules but temperature sensor 
    ** module is initialized, which require I2C module to be 
    ** initialized first
    **
    */
    spb_mux_init();
    spb_twi_init(SPB_TWI_INIT_PORT_ALL);
    /* initialize the TWI interface handler */
    ech_twi_init(EXP_TWI_MASTER_PORT, EXP_TWI_SLAVE_PORT, (top_bootstrap_twi_il_addr_get() >>1));

    /* 
    ** Allow second VPE to enter main loop
    ** This will help to respond to FW status command
    ** 
    */
    start_second_vpe = TRUE;

    /* Initialize temperature sensor module */
    temp_sensor_plat_init();

    /* Register TWIM command server */
    twim_cmdsvr_register();

    /*
    ** Write the CP0 value into scratchpad register
    ** This is a 4 x 32BIT register.
    ** Offset 0 has been used for BOOTROM version information
    ** Offset 4 is being used to store the total BOOT TIME
    ** BOOT TIME means: Power-on to first I2C command.
    */
    opsw_scratchpad_set(OPSW_SCRATCHPAD_1, hal_cp0_counter_get());

    PMC_LOOP_FOREVER
    {
        /* must wait for all hardware to be initialized before enabling DI */
        if ((TRUE == app_fw_plat_hw_init_get()) && (FALSE == app_fw_plat_di_enable_get()))
        {
            /* hardware has been initialized but DI has not, enable DI. */
            app_fw_plat_di_enable();
            app_fw_plat_di_enable_set(TRUE);
        }

        /* OpenCapi Command Handler*/
        if(TRUE == app_fw_oc_ready)
        {
            ech_oc_cmd_proc();
        }

        /* I2C Deferred Command Handler*/
        if (ech_def_handler.deferred_cmd_flag &&
            (ech_def_handler.cmd_buf != NULL) &&
            (ech_def_handler.deferred_cmd_handler != NULL) &&
            (ech_def_handler.callback_handler != NULL))
        {
            ech_def_handler.callback_handler((*ech_def_handler.deferred_cmd_handler)(ech_def_handler.cmd_buf, ech_def_handler.cmd_buf_idx));
            ech_def_handler.cmd_buf = NULL;
            ech_def_handler.deferred_cmd_handler = NULL;
            ech_def_handler.callback_handler = NULL;
            ech_def_handler.deferred_cmd_flag = FALSE;
        }

        /* Update temperature sensors */
        temp_sensor_plat_update();

        /* process the UART interface */
        tsh_main_loop(APP_FW_TSH_SHELL_IDX, FALSE);
    }
}

/**
* @brief
*   Main loop running on all VPEs excluding VPE0 for the 34k.
*
* @param
*
* @return
*
*/

void _34K_VPEx_main(void)
{
    while(start_second_vpe == FALSE);

    /* Set the vector table for the VPE with PIC offset */
    hal_int_vectors_init((UINT32)__ghsbegin_image_vec_tlb_ref +  exp_plat_get_pic_offset());

    PMC_LOOP_FOREVER
    {
        ech_twi_slave_proc(EXP_TWI_SLAVE_PORT);
    }
}

