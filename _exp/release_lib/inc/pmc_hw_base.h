/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2018, 2019, 2020 Microchip Technology Inc. All rights reserved. 
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
*   DESCRIPTION : The Explorer Processor's Memory Map
*     These addresses are specific to the CPU (processor) and are not platform
*     or customer specific.
*
*   NOTES: The following values are reflected in the linker script, default.xn.
*
*******************************************************************************/



#ifndef _PMC_BASE_ADDRESS_H
#define _PMC_BASE_ADDRESS_H

/*
** Include Files
*/
#include "cpuhal_asm.h"

/*
** Constants
*/
#define MIPS_KSEG1_BASE_ADDRESS                0xA0000000
#define MIPS_KSEG0_BASE_ADDRESS                0x80000000
/*Place holder - TBD*/
#define CPUHAL_CPC_REG_BASE_ADDR               0x00000000


/*
** Please note, some of these offsets maybe incorrect. They are here so that we can 
** compile the code.
** Once the offsets are verified, we can delete this. 
*/

/******************************************************************************
* The EXPLORER hardware block has the following subblocks:
*
*      SPI_CNTL_0 @ Offset 0x10000000  --->  EXPLORER_SPI_CNTL_0
*      IBM_ADDR_SPACE @ Offset 0x8000000  --->  EXPLORER_IBM_ADDR_SPACE
*      DDR4_PHY @ Offset 0x4000000  --->  EXPLORER_DDR4_PHY
*          WRAPPER @ Offset 0x6000000  --->  EXPLORER_DDR4_PHY_WRAPPER
*          PHY @ Offset 0x4000000  --->  EXPLORER_DDR4_PHY_PHY
*      EFUSE @ Offset 0x20b000  --->  EXPLORER_EFUSE
*      DCSU_16N @ Offset 0x208000  --->  EXPLORER_DCSU_16N
*      SPI_0 @ Offset 0x16000  --->  EXPLORER_SPI_0
*      WDT @ Offset 0x15000  --->  EXPLORER_WDT
*      GPIO @ Offset 0x14000  --->  EXPLORER_GPIO
*      TWI_SS @ Offset 0x8000  --->  EXPLORER_TWI_SS
*          TWI_MS @ Offset 0x8000  --->  EXPLORER_TWI_SS_TWI_MS
*              TWI_S @ Offset 0x8400  --->  EXPLORER_TWI_SS_TWI_MS_TWI_S
*              TWI_M @ Offset 0x8000  --->  EXPLORER_TWI_SS_TWI_MS_TWI_M
*      UART @ Offset 0x5000  --->  EXPLORER_UART
*      GPBC_SYS_XCBI @ Offset 0x4000  --->  EXPLORER_GPBC_SYS_XCBI
*      TOP_XCBI @ Offset 0x2000  --->  EXPLORER_TOP_XCBI
*      GPBC_PERI_XCBI @ Offset 0x0  --->  EXPLORER_GPBC_PERI_XCBI
*
******************************************************************************/


#define EXPLORER_DDR4_PHY_OFFSET             0x04000000
#define EXPLORER_DDR4_PHY_WRAPPER_OFFSET     0x06000000
#define EXPLORER_TWI_SS_OFFSET               0x00008000
#define EXPLORER_TWI_SS_TWI_MS_OFFSET        0x00000000
#define EXPLORER_TWI_SS_TWI_MS_TWI_S_OFFSET  0x00000400
#define EXPLORER_TWI_SS_TWI_MS_TWI_M_OFFSET  0x00000000
#define EXPLORER_GPBC_PERI_XCBI_OFFSET       0x00000000
#define EXPLORER_TOP_XCBI_OFFSET             0x00002000
#define EXPLORER_GPBC_SYS_XCBI_OFFSET        0x00004000
#define EXPLORER_UART_OFFSET                 0x00005000
#define EXPLORER_GPIO_OFFSET                 0x00014000
#define EXPLORER_WDT_OFFSET                  0x00015000
#define EXPLORER_SPI_0_OFFSET                0x00016000
#define EXPLORER_DCSU_16N_OFFSET             0x00208000
#define EXPLORER_PVT_CTRL_OFFSET             0x00209000
#define EXPLORER_EFUSE_OFFSET                0x0020b000
#define EXP_OCMB_OFFSET                      0x08000000
#define EXPLORER_SPI_CNTL_0_OFFSET           0x10000000
#define EXPLORER_TOP_PCSE_TOP_REGS_OFFSET    0x03050000
#define EXPLORER_GIC_OFFSET                  0x03020000
#define EXPLORER_DC_DDLL_16_OFFSET           0x07000000

#define TWI_SIZE                             (0x800)
#define TOP_XCBI_BASE_ADDR                   (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_TOP_XCBI_OFFSET)
#define TWI_MS_BASE_ADDR                     (MIPS_KSEG1(EXPLORER_TWI_SS_OFFSET + EXPLORER_TWI_SS_TWI_MS_TWI_M_OFFSET))
#define TWI_S_BASE_ADDR                      (MIPS_KSEG1(EXPLORER_TWI_SS_OFFSET + EXPLORER_TWI_SS_TWI_MS_TWI_S_OFFSET))
#define UART0_BASEADDR                       (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_UART_OFFSET)

/* GPBC_FLASH */
#define GPBC_FLASH_PHYS_BASE_ADDR            (0x10000000)
#define FLASH_PHYS_BASE_ADDR                 (0x10000000)
#define GPBC_FLASH_PHYS_ADDR_MASK            (0x07FFFFFF)
#define FLASH_PHYS_SIZE                      (0x02000000)
#define GPBC_FLASH_UNCACHE_BASE_ADD          (MIPS_BASE_KSEG1 | GPBC_FLASH_PHYS_BASE_ADDR)
#define GPBC_FLASH_CACHE_BASE_ADDR           (MIPS_BASE_KSEG0 | GPBC_FLASH_PHYS_BASE_ADDR)
#define FLASH_BASE_ADDR                      (GPBC_FLASH_UNCACHE_BASE_ADD)

/* GPBC SPI FLASH: BAR 20 */
#define GPBC_SPI_FLASH_PHYS_BASE_ADDR        (GPBC_FLASH_PHYS_BASE_ADDR)
#define GPBC_SPI_FLASH_PHYS_ADDR_MASK        (GPBC_FLASH_PHYS_ADDR_MASK)
#define GPBC_SPI_FLASH_UNCACHE_BASE_ADD      (MIPS_BASE_KSEG1 | GPBC_SPI_FLASH_PHYS_BASE_ADDR)
#define GPBC_SPI_FLASH_CACHE_BASE_ADDR       (MIPS_BASE_KSEG0 | GPBC_SPI_FLASH_PHYS_BASE_ADDR)

/* Place holder*/
#define CPUHAL_UD_GCR_REG_BASE_ADDR          0xFFFFFFFF
#define PCS_UD_GCR_PHYS_BASE_ADDR            0xFFFFFFFF
#define CPUHAL_GCR_REG_BASE_ADDR             0x00000000
#define CPUHAL_GIC_REG_BASE_ADDR             (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_GIC_OFFSET)
#define PCS_DSPRAM_SIZE                      0xFFFFFFFF
#define CPUHAL_ITC_ADDR_MASK                 0xFFFFFFFF
#define SPI_BASE_ADDR                        MIPS_KSEG1(EXPLORER_SPI_0_OFFSET)
#define SPI_SIZE                             0x0
#define EFUSE_BASE							 MIPS_KSEG1(EXPLORER_EFUSE_OFFSET)
#define SPB_BASE_ADDR_UNCACHED               MIPS_KSEG1_BASE_ADDRESS
#define SPB_TWISS_INT_EN_REG                 0xF8
#define GPIO_BASE_ADDR                       MIPS_KSEG1(EXPLORER_GPIO_OFFSET)
#define ERAAE_CRYPTO_BASE_ADDR               0xFFFFFFFF
#define PCS_DMA_ITU_BASE_ADDR                0xFFFFFFFF
#define PCS_REG_DMA_BASE_ADDR                0xFFFFFFFF
#define OCMB_REGS_BASE_ADDR                  MIPS_KSEG1(EXP_OCMB_OFFSET)


#define SPI_MRAM_PHYS_BASE_ADDR             (0x190C0000)
#define SPI_MRAM_PHYS_SIZE                  (0x20000)
#define PCS_SRAM_PHYS_BASE_ADDR             (0x19040000)
#define PCS_SRAM_CACHE_BASE_ADDR            (MIPS_BASE_KSEG0 | PCS_SRAM_PHYS_BASE_ADDR)

#define DDR_PHYWRAP_REG_UNCACHE_BASE_ADD	(MIPS_KSEG1_BASE_ADDRESS | EXPLORER_DDR4_PHY_WRAPPER_OFFSET)
#define DDR_PHY_REG_UNCACHE_BASE_ADD	    (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_DDR4_PHY_OFFSET)
#define DDR_CTL_REG_UNCACHE_BASE_ADDR		(0x0)
#define L2B_BASE_ADDR					    (0x0)
#define DDR_SUB_REG_UNCACHE_BASE_ADDR		(0x0)


#define OPSW_REG_BASE_ADDR                  MIPS_KSEG1(0x03000000)
#define OPSX_REG_BASE_ADDR                  OPSW_REG_BASE_ADDR

#define EXPLORER_TOP_PCSE_TOP_BASE_ADDR     (MIPS_BASE_KSEG1 | EXPLORER_TOP_PCSE_TOP_REGS_OFFSET)

/* SerDes */
#define SERDES_REGS_BASE_ADDR               (MIPS_KSEG1_BASE_ADDRESS | SERDES_OFFSET)
#define SERDES_CSU_PCBI_BASE_ADDR           (SERDES_REGS_BASE_ADDR | SERDES_CSU_PCBI_OFFSET)
#define SERDES_CHANNEL_PCBI_BASE_ADDR       (SERDES_REGS_BASE_ADDR | SERDES_CHANNEL_PCBI_OFFSET)
#define SERDES_MTSB_CTRL_PCBI_BASE_ADDR     (SERDES_REGS_BASE_ADDR | SERDES_MTSB_CTRL_PCBI_OFFSET)
#define SERDES_DIAG_PCBI_BASE_ADDR          (SERDES_REGS_BASE_ADDR | SERDES_DIAG_PCBI_OFFSET)
#define SERDES_ADSP_PCBI_BASE_ADDR          (SERDES_REGS_BASE_ADDR | SERDES_ADSP_PCBI_OFFSET)
#define SERDES_MDSP_PCBI_BASE_ADDR          (SERDES_REGS_BASE_ADDR | SERDES_MDSP_PCBI_OFFSET)
#define SERDES_OFFSET                       (0x200000)
#define SERDES_MDSP_PCBI_OFFSET             (0xa00)
#define SERDES_ADSP_PCBI_OFFSET             (0x800)
#define SERDES_DIAG_PCBI_OFFSET             (0x600)
#define SERDES_MTSB_CTRL_PCBI_OFFSET        (0x400)
#define SERDES_CHANNEL_PCBI_OFFSET          (0x200)
#define SERDES_CSU_PCBI_OFFSET              (0x0)

/* DDLL */
#define DDLL_REGS_BASE_ADDR                 (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_DC_DDLL_16_OFFSET)

/* GPBCE SYS*/
#define GPBCE_SYS_REGS_BASE_ADDR            (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_GPBC_SYS_XCBI_OFFSET)

/* GPBCE PERI*/
#define GPBCE_PERI_REGS_BASE_ADDR           (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_GPBC_PERI_XCBI_OFFSET)

/* WDT */
#define WDT_REGS_BASE_ADDR                  (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_WDT_OFFSET)

/* PVT */
#define PVT_REGS_BASE_ADDR                  (MIPS_KSEG1_BASE_ADDRESS | EXPLORER_PVT_CTRL_OFFSET)

#endif /* define _PMC_BASE__ADDRESS_H */




