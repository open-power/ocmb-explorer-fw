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
*   DESCRIPTION :
*     Platform-specific definitions for SPI.
*
*   NOTES:
*
*******************************************************************************/



/*
** Include Files
*/
#include "spi_plat_cfg.h"
#include "spi_plat.h"
#include "top.h"
#include "pmcfw_common.h"
#include "sys_timer.h"
#if (SPI_PORT1_L2B_CONFIG == 1)
#include "l2b.h"
#include "l2b_plat.h"
#endif
#include "pmc_plat.h"
#include "spi_api.h"
#include "spi_flash_api.h"
#include "spi_flash_plat.h"

/*
** Local Constants
*/

/* number of chip selects on each port */
#define SPI_PLAT_PORT0_NUM_CS   1

/* physical and virtual memory-mapped address bases */
#define PORT0_CS0_MM_ADDR_BASE_PHYS  FLASH_PHYS_BASE_ADDR
#define PORT0_CS0_MM_ADDR_BASE_VIRT  MIPS_KSEG0(FLASH_PHYS_BASE_ADDR)
#define PORT0_CS1_MM_ADDR_BASE_PHYS  (PORT0_CS0_MM_ADDR_BASE_PHYS + FLASH_PHYS_SIZE)
#define PORT0_CS1_MM_ADDR_BASE_VIRT  (PORT0_CS0_MM_ADDR_BASE_VIRT + FLASH_PHYS_SIZE)

/* Default value for SPI  pin driver strengths */
#define SPI_PLAT_DEFAULT_DRIVER_STRENGTH    0x0

/* Default value for SPI pullups */
#define SPI_PLAT_DEFAULT_PULLUP_VALUE       0x0

/*
** Local Variables
*/

/* port 0 chip select configurations */
spi_cs_config_struct spi_plat_cs_config_port0[SPI_PLAT_PORT0_NUM_CS] = {
    {
        .mm_enable = TRUE,									/* MM enable */
        .mm_addr_base_phys = PORT0_CS0_MM_ADDR_BASE_PHYS,	/* MM physical address base */
        .mm_addr_base = PORT0_CS0_MM_ADDR_BASE_VIRT,		/* MM virtual address base */
        .mm_addr_size = FLASH_PHYS_SIZE						/* memory size */
    },
};

/* top-level SPI configuration */
spi_parms_struct spi_plat_parms[SPI_NUM_PORTS] = {
    {
        .bus_rate = 0,  /* bus rate set dynamically */
        .num_cs = SPI_PLAT_PORT0_NUM_CS,
        .cs_config = spi_plat_cs_config_port0,
        .ecc_config = {
        .enable = FALSE,  /* ECC enable set dynamically for port 0 */
        .addr_base_phys = PORT0_CS0_MM_ADDR_BASE_PHYS,
        .size = FLASH_PHYS_SIZE
        }
    }
};


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
PUBLIC PMCFW_ERROR _spi_plat_pin_driver_strength_set(UINT8 port_id);
PUBLIC spi_plat_pin_driver_strength_set_fn_ptr_type spi_plat_pin_driver_strength_set_fn_ptr = _spi_plat_pin_driver_strength_set;

PUBLIC BOOL _spi_plat_is_boot_quad(VOID);
PUBLIC spi_plat_is_boot_quad_fn_ptr_type spi_plat_is_boot_quad_fn_ptr = _spi_plat_is_boot_quad;

PUBLIC UINT16 _spi_plat_cs_wait_get(UINT8 wait);
PUBLIC spi_plat_cs_wait_get_fn_ptr_type spi_plat_cs_wait_get_fn_ptr = _spi_plat_cs_wait_get;

PUBLIC BOOL _spi_plat_wci_mode_get(VOID);
PUBLIC spi_plat_wci_mode_get_fn_ptr_type spi_plat_wci_mode_get_fn_ptr = _spi_plat_wci_mode_get;

PUBLIC PMCFW_ERROR _spi_plat_flash_poll_write_erase_complete(BOOL write, UINT32 timeout);
PUBLIC spi_plat_flash_poll_write_erase_complete_fn_ptr_type spi_plat_flash_poll_write_erase_complete_fn_ptr = _spi_plat_flash_poll_write_erase_complete;

PUBLIC PMCFW_ERROR _spi_plat_flash_write_pages(UINT8* src_ptr, UINT8* dst_ptr, UINT32 len, UINT32 page_size, UINT32 timeout);
PUBLIC spi_plat_flash_write_pages_fn_ptr_type spi_plat_flash_write_pages_fn_ptr = _spi_plat_flash_write_pages;


/*
** Private Functions
*/

 
/*
** Public Functions
*/

/****************************************************************************
*
* FUNCTION: spi_parms_get
* __________________________________________________________________________
*
* DESCRIPTION:
*   Get platform-specific SPI parameters.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*   Platform-specific SPI parameters as an spi_parms_struct pointer (array of
*   size SPI_NUM_PORTS)
*
* NOTES:
*
*****************************************************************************/
PUBLIC spi_parms_struct *spi_parms_get(VOID)
{
    UINT8 port_id;
    UINT32 bus_rate;
    BOOL ecc_enable;

    /* get the bus rate and ecc state from boostraps: */
    bus_rate = top_efuse_spi_rate_get();
    ecc_enable = top_efuse_spi_ecc_enable_get();

    for (port_id = 0; port_id < SPI_NUM_PORTS; port_id++)
    {
        /* set the bus rate */
        spi_plat_parms[port_id].bus_rate = bus_rate;
    }

    /* set the ECC enable (port 0 only) */
    spi_plat_parms[0].ecc_config.enable = ecc_enable;

    return spi_plat_parms;
}

/****************************************************************************
*
* FUNCTION: spi_plat_init
* __________________________________________________________________________
*
* DESCRIPTION:
*   Platform-specific SPI initialization.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*
* NOTES:
*
*****************************************************************************/
PUBLIC VOID spi_plat_init(VOID)
{

}

/*******************************************************************************
*
* FUNCTION: spi_plat_pin_driver_strength_set()
* ____________________________________________________________________________
*
* DESCRIPTION:
*   Set the pin driver strength.
*
* INPUTS:
*   port_id - port identifier
*
* OUTPUTS:
*
* RETURNS:
*   PMC_SUCCESS: success
*   Others:      failure
*
* NOTES:
*
*******************************************************************************/
PMC_RAM_PROGRAM
PUBLIC PMCFW_ERROR _spi_plat_pin_driver_strength_set(UINT8 port_id)
{
    UINT32 driver_strength;
    UINT32 pullup_value;

    /* configure the pin driver strength as per EFUSE settings*/
    switch (port_id)
    {
        case 0:
            driver_strength = top_efuse_spi_pin_driver_strength_get();

            if(driver_strength != SPI_PLAT_DEFAULT_DRIVER_STRENGTH)
            {
                top_spi_driver_strength_set(driver_strength);
            }

            pullup_value = top_efuse_spi_pin_pullup_get();

            if(pullup_value != SPI_PLAT_DEFAULT_PULLUP_VALUE)
            {
                top_spi_pullup_value_set(pullup_value);
            }
        break;

            default:
                return PMCFW_ERR_INVALID_PARAMETERS;
    }
    sys_timer_busy_wait_us(2);
    return PMC_SUCCESS;
}
PMC_END_RAM_PROGRAM

/*******************************************************************************
*
* FUNCTION: spi_plat_is_boot_quad()
* ____________________________________________________________________________
*
* DESCRIPTION:
*   Check whether quad SPI should be enabled based on the bootstrap register.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*   TRUE if quad mode enabled, FALSE if not
*
* NOTES:
*
*******************************************************************************/
PMC_RAM_PROGRAM
PUBLIC BOOL _spi_plat_is_boot_quad(VOID)
{
    return top_efuse_spi_is_quad();
}
PMC_END_RAM_PROGRAM

/*******************************************************************************
*
* FUNCTION: spi_plat_wci_mode_get()
* ____________________________________________________________________________
*
* DESCRIPTION:
*   Check whether WCI mode is enabled. When WCI mode is 0, interrupts are
*   cleared by reading the interrupt register. When WCI mode is 1, interrupts
*   are cleared by writing a 1 to interrupt register bits.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*   TRUE if WCI mode is enabled, FALSE if not
*
* NOTES:
*
*******************************************************************************/
PMC_RAM_PROGRAM
PUBLIC BOOL _spi_plat_wci_mode_get(VOID)
{
    return (top_spi_wci_mode_get());

}
PMC_END_RAM_PROGRAM

/*******************************************************************************
*
* FUNCTION: spi_plat_cs_wait_get()
* ____________________________________________________________________________
*
* DESCRIPTION:
*   Get the MM_CFG_3 register's SPI_CS_WAIT value for the given wait time.
*
* INPUTS:
*   wait - wait time (ns)
*
* OUTPUTS:
*
* RETURNS:
*   SPI_CS_WAIT value
*
* NOTES:
*
*******************************************************************************/
PMC_RAM_PROGRAM
PUBLIC UINT16 _spi_plat_cs_wait_get(UINT8 wait)
{
    return ((wait * SPI_CLK_RATE) / 1000) - 2;
}
PMC_END_RAM_PROGRAM

/**
* @brief
*   Poll for completed writes or erase to SPI flash.
*
* @param [in] write   - write flag, TRUE if this is a write operation
* @param [in] timeout - Timeout value for write or erase operation
*
* @return
*   PMC_SUCCESS if no error
*   Error specific code otherwise
*
* @note
*/
PMC_RAM_PROGRAM
PUBLIC PMCFW_ERROR _spi_plat_flash_poll_write_erase_complete(BOOL write, UINT32 timeout)
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
                break;
            }
        }
        else
        {
            rc = spi_flash_erase_complete(SPI_FLASH_PORT, SPI_FLASH_CS, &complete);
            if (rc != PMC_SUCCESS)
            {
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
            if (write)
            {
                rc = SPI_ERR_WRITE_FAIL;
            }
            else
            {
                rc = SPI_ERR_ERASE_FAIL;
            }
            break;
        }
    }

    return rc;

} /* _spi_plat_poll_write_erase_complete */
PMC_END_RAM_PROGRAM

/**
* @brief
*   Write data to SPI flash.
*
* @param
*   src_ptr - source address
*   dst_ptr - destination address
*   len - data length in bytes
*   page_size - SPI flash page size in bytes
*   timeout - SPI write timeout
*
*  @return
*   PMC_SUCCESS if no error
*   Error specific code otherwise
*
* @note
*/
PMC_RAM_PROGRAM
PUBLIC PMCFW_ERROR _spi_plat_flash_write_pages(UINT8* src_ptr,
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
                             (UINT8*)((UINT32)dst_ptr & GPBC_FLASH_PHYS_ADDR_MASK),
                             write_len);

        if (rc != PMC_SUCCESS)
        {
            return (rc);
        }

        rc = spi_plat_flash_poll_write_erase_complete(TRUE, timeout);
        if (rc != PMC_SUCCESS)
        {
            return (rc);
        }

        src_ptr += write_len;
        dst_ptr += write_len;
        len -= write_len;

        /* write the remaining data one page at a time */
        write_len = page_size;
    }

    return (PMC_SUCCESS);
}
PMC_END_RAM_PROGRAM

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
PUBLIC VOID spi_plat_ram_code_ptr_adjust(UINT32 offset)
{
    spi_plat_pin_driver_strength_set_fn_ptr         = (spi_plat_pin_driver_strength_set_fn_ptr_type)((UINT32)spi_plat_pin_driver_strength_set_fn_ptr                 - offset);
    spi_plat_is_boot_quad_fn_ptr                    = (spi_plat_is_boot_quad_fn_ptr_type)((UINT32)spi_plat_is_boot_quad_fn_ptr                                       - offset);
    spi_plat_cs_wait_get_fn_ptr                     = (spi_plat_cs_wait_get_fn_ptr_type)((UINT32)spi_plat_cs_wait_get_fn_ptr                                         - offset);
    spi_plat_wci_mode_get_fn_ptr                    = (spi_plat_wci_mode_get_fn_ptr_type)((UINT32)spi_plat_wci_mode_get_fn_ptr                                       - offset);
    spi_plat_flash_poll_write_erase_complete_fn_ptr = (spi_plat_flash_poll_write_erase_complete_fn_ptr_type)((UINT32)spi_plat_flash_poll_write_erase_complete_fn_ptr - offset);
    spi_plat_flash_write_pages_fn_ptr               = (spi_plat_flash_write_pages_fn_ptr_type)((UINT32)spi_plat_flash_write_pages_fn_ptr                             - offset);
}


