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
*   DESCRIPTION : This is a TWI platform specific file implementing various
*                 platform functions
*
*
*   NOTES:        None.
*
*
*******************************************************************************/




/**
* @addtogroup TWI
* @{
* @file
* @brief
*   Platform-specific definitions for TWI.
*
* @note
*/

/*
* Include Files
*/

#include "pmcfw_types.h"
#include "twi_plat.h"
#include "twi_plat_cfg.h"
#include "twi_api.h"
#include "spb_twi.h"
#include "top.h"
#include "pmc_hw.h"
#if (TWI_INTERRUPT_MODE_ENABLE == 1)
#include "cicint.h"
#include "cicint_plat.h"
#endif
#include "sys_timer.h"

/*
* Structures and Unions
*/

/* 
* Local Constants 
*/ 
/* Set the TWI timeout value to 1 second - TWI has 100us multiplier */
#define TWI_TIMEOUT         10000
#define TWI_SPD_TIMEOUT     500

/* Default value for TWI  pin driver strengths */
#define TWI_PLAT_DEFAULT_DRIVER_STRENGTH    0x0


/*
* Local Variables
*/

#if (TWI_INTERRUPT_MODE_ENABLE == 1)
/* master, slave, and slave reset ISR function pointers */
#if (TWI_MASTER_MODE_ENABLE == 1)
PRIVATE twi_plat_isr_fcn_ptr twi_mst_isr_fptr = NULL;
#endif
#if (TWI_SLAVE_MODE_ENABLE == 1)
PRIVATE twi_plat_isr_fcn_ptr twi_slv_isr_fptr = NULL;
PRIVATE twi_plat_isr_fcn_ptr twi_srst_isr_fptr = NULL;
#endif
#endif

#if (TWI_PORT0_ONLY == 0)
/* TWI device map table - TWI_FWNVRAM_DEVNUM address will be overridden by bootstrap address setting */
PUBLIC twi_device_info_struct twi_devices_array[] =
{
    /* HANDLE,                   BUS, ADDR, SIZE,   WP_STRT,WP_STRT,SPEED, PAGE_SIZE, ADDR_SIZE,  DATA_SIZE,   TWI_TIMEOUT,        PAGE_SIZE */
    { TWI_FWNVRAM_DEVNUM,         0,   0xA0, 0x2000,  0,      0,      0,     0x20,     2,          1,           TWI_TIMEOUT,    0x20},   /* DEVICE 1 - BOOTSRAP 24C16 */
    { TWI_SPD_DEVNUM,             4,   0xAA, 0x200,   0,      0,      0,     0x08,     1,          1,           TWI_SPD_TIMEOUT,0x08},   /* DEVICE 4 - DDR3 SPD */
};

PRIVATE UINT8 twi_devices_array_size = sizeof(twi_devices_array)/sizeof(twi_device_info_struct);
#endif

/*
* Private Functions
*/

#if (TWI_INTERRUPT_MODE_ENABLE == 1)
/**
* @brief
*   ISR for TWI master interrupts.
*
* @param[in] cback_arg - callback parameter (port ID)
*
* @return
*
* @note
*/
#if (TWI_MASTER_MODE_ENABLE == 1)
PRIVATE VOID twi_plat_mst_isr(VOID *cback_arg)
{
    UINT port_id = (UINT)cback_arg;

    /*
    * check for master interrupt
    * NOTE: there are different TWI interrupt enable registers for PCS and SPCS
    */
    if ((SPB_READ(SPB_TWISS_INT_REG) & SPB_READ(SPB_TWISS_INT_EN_REG) &
         (1 << (port_id * 2))) != 0)
    {
        PMCFW_ASSERT(twi_mst_isr_fptr != NULL, TWI_ERR_MST_ISR_NOT_INSTALLED); 
        twi_mst_isr_fptr(port_id);
    }
}
#endif /* (TWI_MASTER_MODE_ENABLE == 1) */

/**
* @brief
*   ISR for TWI slave and slave reset interrupts.
*
* @param[in] cback_arg - callback parameter (port ID)
*
* @return
*
* @note
*/
#if (TWI_SLAVE_MODE_ENABLE == 1)
PRIVATE VOID twi_plat_slv_isr(VOID *cback_arg)
{
    UINT port_id = (UINT)cback_arg;

    /*
    * check for slave interrupt
    * NOTE: there are different TWI interrupt enable registers for PCS and SPCS
    */
    if ((SPB_READ(SPB_TWISS_INT_REG) & SPB_READ(SPB_TWISS_INT_EN_REG) &
         (1 << ((port_id * 2) + 1))) != 0)
    {
        PMCFW_ASSERT(twi_slv_isr_fptr != NULL, TWI_ERR_SLV_ISR_NOT_INSTALLED); 
        twi_slv_isr_fptr(port_id);
    }

    /* check for slave reset interrupt */
    if ((SPB_READ(SPB_TWI_SRST_INT_REG) & SPB_READ(SPB_TWI_SRST_INT_EN_REG) &
         (1 << port_id)) != 0)
    {
        /* clear and handle the interrupt */
        SPB_WRITE(SPB_TWI_SRST_INT_REG, (1 << port_id));
        PMCFW_ASSERT(twi_srst_isr_fptr != NULL, TWI_ERR_SRST_ISR_NOT_INSTALLED); 
        twi_srst_isr_fptr(port_id);
    }
}
#endif /* (TWI_SLAVE_MODE_ENABLE == 1) */

/**
* @brief
*   ISR for TWI interrupts (master, slave, and slave reset).
*
* @param[in] cback_arg - callback parameter (port ID)
*
* @return
*
* @note
*/
PRIVATE VOID twi_plat_isr(VOID *cback_arg)
{
#if (TWI_MASTER_MODE_ENABLE == 1)
    twi_plat_mst_isr(cback_arg);
#endif

#if (TWI_SLAVE_MODE_ENABLE == 1)
    twi_plat_slv_isr(cback_arg);
#endif
}
#endif /* (TWI_INTERRUPT_MODE_ENABLE == 1) */

#if (TWI_PORT0_ONLY == 0)
/**
* @brief
*   This function overrides information within the specified twi
*   device number's device information structure.
*
* @param[in] device - device identification 
* @param[in] index - index into twi device array 
*
* @return
*
* @note 
*/
PRIVATE VOID twi_plat_device_ptr_override(UINT32 device, UINT32 index)
{
}
#endif

/*
* Public Functions
*/

#if (TWI_INTERRUPT_MODE_ENABLE == 1)
/**
* @brief
*   Register TWI ISRs.
*
* @param[in] mst_isr_ptr  - master ISR
* @param[in] slv_isr_ptr  - slave ISR
* @param[in] srst_isr_ptr - slave reset ISR
*
* @return
*
* @note
*/
PUBLIC VOID twi_plat_isr_register(twi_plat_isr_fcn_ptr mst_isr_ptr,
                                  twi_plat_isr_fcn_ptr slv_isr_ptr,
                                  twi_plat_isr_fcn_ptr srst_isr_ptr)
{
    UINT int_num;

    twi_mst_isr_fptr = mst_isr_ptr;
    twi_slv_isr_fptr = slv_isr_ptr;
    twi_srst_isr_fptr = srst_isr_ptr;

#if (TWI_MASTER_MODE_ENABLE == 1)
    /*
    * register the TWI master ISR for each port, using the port ID as the 
    * callback argument 
    */
    for (int_num = CICINT_FP_INT_GPBC_TWI_0_M;
         int_num <= CICINT_FP_INT_GPBC_TWI_15_M;
         int_num++)
    {
        cicint_int_register(int_num, twi_plat_isr,
                            (VOID *)(int_num - CICINT_FP_INT_GPBC_TWI_0_M));
        cicint_int_enable(int_num);
    }
#endif

#if (TWI_SLAVE_MODE_ENABLE == 1)
    /*
    * register the TWI slave ISR for each port, using the port ID as the 
    * callback argument 
    */
    for (int_num = CICINT_FP_INT_GPBC_TWI_0_S;
         int_num <= CICINT_FP_INT_GPBC_TWI_15_S;
         int_num++)
    {
        cicint_int_register(int_num, twi_plat_isr,
                            (VOID *)(int_num - CICINT_FP_INT_GPBC_TWI_0_S));
        cicint_int_enable(int_num);
    }
#endif
}
#endif /* (TWI_INTERRUPT_MODE_ENABLE == 1) */

/**
* @brief
*   Get the TWI master base register address for the given port.
*
* @param[in] port_id - port identifier
*
* @return
*   TWI master base address
*
* @note
*/
PUBLIC UINT32 twi_plat_mst_base_address(const UINT port_id)
{
    return TWI_MS_BASE_ADDR + (port_id * TWI_SIZE);
}

/**
* @brief
*   Get the TWI slave base register address for the given port.
*
* @param[in] port_id - port identifier
*
* @return
*   TWI slave base address
*
* @note
*/
PUBLIC UINT32 twi_plat_slv_base_address(const UINT port_id)
{
    return TWI_S_BASE_ADDR + (port_id * TWI_SIZE);
}

/**
* @brief
*   Set the TWI pin mode (TWI or GPIO) for a port.
*
* @param[in] port_id - port identifier
* @param[in] mode    - pin mode
*
* @return
*   PMC_SUCCESS: success
* @return
*   Others:      failure
*
* @note
*/
PUBLIC PMCFW_ERROR twi_plat_port_mode_set(const UINT port_id,
                                          const twi_pad_mode_enum mode)
{
    if (mode == TWI_PAD_MODE_TWI)
    {
        spb_twi_muxing_cfg_gpio_to_twi(port_id);
    }
    else if (mode == TWI_PAD_MODE_GPIO)
    {
        spb_twi_muxing_cfg_twi_to_gpio(port_id);
    }
    else
    {
        return PMCFW_ERR_INVALID_PARAMETERS;
    }

    return PMC_SUCCESS;
}

/**
* @brief
*   Read from a TWI pin when configured for GPIO mode.
*
* @param[in] port_id - port identifier
* @param[in] pad     - pin to read (SCL, SDA, or RST)
*
* @param[out] value  - pin value
*
* @return
*   PMC_SUCCESS: success
* @return
*   Others:      failure
*
* @note
*/
PUBLIC PMCFW_ERROR twi_plat_pin_read(const UINT port_id,
                                     const spb_twi_pad_enum pad,
                                     BOOL * const value)
{
    return spb_twi_pin_read(port_id, pad, value);
}

/**
* @brief
*   Write to a TWI pin when configured for GPIO mode.
*
* @param[in] port_id - port identifier
* @param[in] pad     - pin to write to (SCL, SDA, or RST)
* @param[in] value   - value to write
*
* @return
*   PMC_SUCCESS: success
* @return
*   Others:      failure
*
* @note
*/
PUBLIC PMCFW_ERROR twi_plat_pin_write(const UINT port_id,
                                      const spb_twi_pad_enum pad,
                                      const BOOL value)
{
    return spb_twi_pin_write(port_id, pad, value);
}

#if (TWI_PORT0_ONLY == 0)
/**
* @brief
*   This function gets a pointer to the specified twi device
*   number's device information structure.
*
* @param[in] device - device identification
*
* @return
*   TRUE if the device is found.  FALSE if the device not found.
*
* @note 
*/
PUBLIC BOOL twi_plat_device_ptr_get(UINT32 device,
                                twi_device_info_struct **twi_device_pptr)
{
    BOOL rc = FALSE;
    UINT32 i;

    *twi_device_pptr = NULL;

    for(i = 0; i < twi_devices_array_size; i++)
    {
        if(twi_devices_array[i].info.device_handle == device)
        {
            twi_plat_device_ptr_override(device, i);
            *twi_device_pptr = &(twi_devices_array[i]);
            rc = TRUE;
            break;
        }
    }

    return rc;
}
#endif

/*******************************************************************************
*
* FUNCTION: twi_plat_pin_driver_strength_set()
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
*   None
*
* NOTES:
*
*******************************************************************************/
PUBLIC VOID twi_plat_pin_driver_strength_set(UINT8 port_id)
{
  
    UINT32 efuse_driver_strength;
    UINT32 default_driver_strength = top_twi_pin_default_driver_strength_get();
    UINT32 value =0;
    
    switch (port_id)
    {
        case EXP_TWI_SLAVE_PORT:
            /* 
            ** Read EFUSE settings and XOR with FW default to override any settings.
            ** By default, EFUSE is 0. So, to override the settings, please set the 
            ** corresponding BIT in EFUSE.
            */
            efuse_driver_strength = top_efuse_twi_pin_driver_strength_get();   
            value = efuse_driver_strength ^ default_driver_strength;            
            top_twi_driver_strength_set(value);
        break;

        default:
            return;
    }
    
 
    sys_timer_busy_wait_us(2);
}


/** @} end addtogroup */



