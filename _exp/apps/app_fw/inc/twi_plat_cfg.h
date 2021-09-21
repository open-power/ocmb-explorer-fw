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
* @addtogroup TWI
* @{
* @file
* @brief
*   Platform-specific configuration definitions for TWI.
*
* @note
*/
#ifndef _TWI_PLAT_CFG_H
#define _TWI_PLAT_CFG_H

/*
** Set to 1 to enable TWI master support 
*/
#define TWI_MASTER_MODE_ENABLE  1

/*
** Set to 1 to enable TWI slave support 
*/
#define TWI_SLAVE_MODE_ENABLE   1

/*
** Set to 1 when using TWI interrupts (versus polling)
*/
#define TWI_INTERRUPT_MODE_ENABLE  0

/*
** Set to 1 when using SMBUS protocol
*/
#define TWI_SMBUS_PROTO_ENABLE  0

/*
** Set to 1 when using MCTP protocol
*/
#define TWI_MCTP_PROTO_ENABLE  0

/* TWI log disable: 1 to disable logging, 0 to enable */
#define TWI_LOG_DISABLE     0

/*
** Set to 1 to only use TWI port 0, or 0 to use all ports. This is useful for
** memory-constrained environments where only port 0 is needed as it saves a
** large amount of DRAM and a small amount of IRAM.
*/
#define TWI_PORT0_ONLY  0

/*
** Set to 1 to enable spinlocks when in polling mode, or 0 to disable. This
** can be used to disable polling mode spinlocks even if the spin module is
** configured to enable spinlocks.
*/
#define TWI_POLLING_SPINLOCK_ENABLE 0

/* set to 1 to enable master recovery, or 0 to disable */
#define TWI_MST_RECOV_ENABLE  0

/* 
** Set to 1 to enable operation with ThreadX, or 0 to operate without ThreadX 
*/ 
#define TWI_THREADX_MODE_ENABLE  0

#endif /* _TWI_PLAT_CFG_H */
/** @} end addtogroup */



