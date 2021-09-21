/******************************************************************************
*  Copyright 2019 Microchip Technology Inc. and its subsidiaries.
*  Subject to your compliance with these terms, you may use Microchip
*  software and any derivatives exclusively with Microchip products. It is
*  your responsibility to comply with third party license terms applicable to
*  your use of third party software (including open source software) that may
*  accompany Microchip software.
*  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
*  EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
*  IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
*  PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
*  SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR
*  EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED,
*  EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
*  FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
*  LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
*  EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO
*  MICROCHIP FOR THIS SOFTWARE.
* -------------------------------------------------------------------------- 
*
*  DESCRIPTION :  
*   Section map for runtime FW.
*
*  NOTES:
*
*******************************************************************************/

CONSTANTS 
{
    phy_mask   = 0x1fffffff
    kseg0_bits = 0x80000000
    kseg1_bits = 0xa0000000
    kseg2_bits = 0xc0000000    
    
    handoff_reserve = 64
    heap_reserve = 0
    stack_reserve = 8K
    free_mem_reserve = 19K
    free_mem_authentication = 12K
    /* This is needed for SHA lib in BOOTROM (PBOOT) */
    pboot_sda_reserve = 512

    exe_region = kseg0_bits


    
    /* -------------------------------- FLASH -------------------------------- */
    spi_flash_size = 892K
    spi_flash_image_offset = 0K
    spi_flash_base = 0x10000000
    spi_code_base  = spi_flash_base + spi_flash_image_offset
    
    /******************** 
     * SPI flash section size *
     ********************/

    spi_flash_uncached_size    = 8K
    spi_flash_exe_reg_size     = spi_flash_size - spi_flash_uncached_size
       
    /********************
     * SPI section base *
     ********************/
    
    spi_flash_uncached_base     = spi_code_base
    spi_flash_exe_reg_base      = spi_flash_uncached_base + spi_flash_uncached_size
    spi_flash_end_uncached     = (spi_flash_exe_reg_base & phy_mask) | kseg1_bits

    /* -------------------------------- SRAM -------------------------------- */
    sram_size = 256K
    sram_base = 0x01000000

    /******************** 
     * SRAM section size *
     ********************/
    sram_run_time_data_size     = 191K
    sram_fixed_section_length   = sram_size - sram_run_time_data_size
    sram_msdg_cfg_struct_size   = 512
    sram_fw_boot_cfg_size       = 256
    sram_ext_data_buf_size      = 64K
    sram_rsp_buf_size           = 64
    sram_cmd_buf_size           = 64
    sram_doorbell_size          = 4
       
    /********************
     * SRAM section base *
     ********************/
    
    sram_exe_data_base          = sram_base 
    sram_msdg_cfg_struct_base   = sram_exe_data_base + sram_run_time_data_size
    sram_fw_boot_cfg_base       = sram_msdg_cfg_struct_base + sram_msdg_cfg_struct_size
    sram_ext_data_buf_base      = sram_fw_boot_cfg_base + sram_fw_boot_cfg_size
    sram_rsp_buf_base           = sram_ext_data_buf_base + sram_ext_data_buf_size		
    sram_cmd_buf_base           = sram_rsp_buf_base + sram_rsp_buf_size
    sram_doorbell_base          = sram_cmd_buf_base + sram_cmd_buf_size
    sram_exe_reg_end            = sram_exe_data_base + sram_size
  
}
MEMORY {
    spi_flash_uncached_start  : ORIGIN = (spi_flash_uncached_base & phy_mask) | kseg1_bits, LENGTH = 0
    spi_flash_uncached_memory : ORIGIN = .                                                , LENGTH = spi_flash_uncached_size
    spi_flash_uncached_end    : ORIGIN = .                                                , LENGTH = 0

    spi_flash_exe_start  : ORIGIN = (spi_flash_exe_reg_base & phy_mask) | exe_region, LENGTH = 0
    spi_flash_exe_memory : ORIGIN = .                                               , LENGTH = spi_flash_exe_reg_size
    spi_flash_exe_end    : ORIGIN = .                                               , LENGTH = 0

}


/*
** Program layout for starting in ROM, copying data to RAM, and continuing to
** execute out of ROM.  Some small debugging library functions are located in
**RAM to allow MULTI to set software breakpoints rather than hardware.
*/


MEMORY {
    sram_data_start            : ORIGIN = (sram_exe_data_base & phy_mask)              | exe_region    , LENGTH = 0
    sram_data                  : ORIGIN = .                                                            , LENGTH = sram_run_time_data_size
    sram_fixed_section_start   : ORIGIN = (sram_msdg_cfg_struct_base & phy_mask)       | exe_region    , LENGTH = 0
    sram_fixed_section         : ORIGIN = .                                                            , LENGTH = sram_fixed_section_length
    sram_location_end          : ORIGIN = (sram_exe_reg_end & phy_mask)                | exe_region    , LENGTH = 0
}

SECTIONS
{
/* RAM SECTIONS */
    .handoff_data       ALIGN(32) PAD(handoff_reserve)           : > sram_data
    .pboot_sda_patch    ALIGN(32) PAD(pboot_sda_reserve)         : > .
    .sdabase            ALIGN(32)                                : > .
    .sdata              ALIGN(32)                                : > .
    .sbss               ALIGN(32)                                : > .
    .rosdata            ALIGN(32)                                : > .
    .data               ALIGN(32)                                : > .
    .profile            ALIGN(32)                                : > .
    .bss                ALIGN(32)                                : > .
    .heap               ALIGN(32) PAD(heap_reserve)              : > .
    .stack              ALIGN(32) PAD(stack_reserve)             : > .
    .free_mem           ALIGN(32) PAD(free_mem_reserve)          : > .
    .fw_auth_mem        ALIGN(32) PAD(free_mem_authentication)   : > .

    .text_lib           ALIGN(32)                                :
    {
        libansi.a(ccffs.o(.text))
    }                                                              > .
    .text_rammem        ALIGN(32)                                : > .
    
    .msdg_cfg_struct    ALIGN(32) PAD(sram_msdg_cfg_struct_size) : > sram_fixed_section
    .fw_boot_cfg        ALIGN(32) PAD(sram_fw_boot_cfg_size)     : > .
    .ext_data_buf       ALIGN(32) PAD(sram_ext_data_buf_size)    : > .
    .rsp_buf            ALIGN(32) PAD(sram_rsp_buf_size)         : > .
    .cmd_buf            ALIGN(32) PAD(sram_cmd_buf_size)         : > .
    .doorbell           ALIGN(32) PAD(sram_doorbell_size)        : > .
    .sram_end                                                    : > sram_location_end


/* ROM SECTIONS */

    .image_start_uncached                                                   : > spi_flash_uncached_memory
    .image_bev_reset        ALIGN(4096) MIN_SIZE(0x200) MAX_SIZE(0x200) ABS : > .
    .image_bev_tlb_ref                  MIN_SIZE(0x100) MAX_SIZE(0x100) ABS : > .
    .image_bev_cache_error              MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS : > .
    .image_bev_gen_exc                  MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS : > .
    .image_bev_int                      MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS : > .
    .image_bev_debug                    MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS : > .
    .text_uncached          ALIGN(32)   PAD(2048)                           : > .
    .text_slowmem           ALIGN(32)                                       : > .
    .image_end_uncached     ALIGN(32)                                       : > .
    .image_start_exe_reg                                                    LOAD(. & phy_mask | kseg1_bits)  : > spi_flash_exe_memory
    .image_vec_tlb_ref      ALIGN(4096) MIN_SIZE(0x100) MAX_SIZE(0x100) ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_cache_error              MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_gen_exc                  MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > . 
    .image_vec_int0                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_int1                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_int2                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_int3                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_int4                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_int5                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_int6                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_int7                     MIN_SIZE(0x80)  MAX_SIZE(0x80)  ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_vec_extra                    MIN_SIZE(0x80)  MAX_SIZE(0x100) ABS   LOAD(. & phy_mask | kseg1_bits)  : > .
    .app_fw_start           ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .text                   ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .text_fastmem           ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .robase                 ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .rodata                 ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .secinfo                ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .fixaddr                ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .fixtype                ALIGN(32)                                         LOAD(. & phy_mask | kseg1_bits)  : > .
    .ROM.sdata              ALIGN(32)            ROM(.sdata)                  LOAD(. & phy_mask | kseg1_bits)  : > .
    .ROM.data               ALIGN(32)            ROM(.data)                   LOAD(. & phy_mask | kseg1_bits)  : > .
    .ROM.profile            ALIGN(32)            ROM(.profile)                LOAD(. & phy_mask | kseg1_bits)  : > .
    .image_end_exe_reg                                                        LOAD(. & phy_mask | kseg1_bits)  : > .
    .ddr4_pmu_train_imem            LOAD(. & phy_mask | kseg1_bits)           :{ddr4_pmu_train_imem.bin(.raw)} > .
    .ddr4_pmu_train_dmem            LOAD(. & phy_mask | kseg1_bits)           :{ddr4_pmu_train_dmem.bin(.raw)} > .  
    .ddr4_2d_pmu_train_imem         LOAD(. & phy_mask | kseg1_bits)           :{ddr4_2d_pmu_train_imem.bin(.raw)} > .
    .ddr4_2d_pmu_train_dmem         LOAD(. & phy_mask | kseg1_bits)           :{ddr4_2d_pmu_train_dmem.bin(.raw)} > .
    .ddr4_rdimm_pmu_train_imem      LOAD(. & phy_mask | kseg1_bits)           :{ddr4_rdimm_pmu_train_imem.bin(.raw)} > .
    .ddr4_rdimm_pmu_train_dmem      LOAD(. & phy_mask | kseg1_bits)           :{ddr4_rdimm_pmu_train_dmem.bin(.raw)} > .
    .ddr4_rdimm2d_pmu_train_imem    LOAD(. & phy_mask | kseg1_bits)           :{ddr4_rdimm2d_pmu_train_imem.bin(.raw)} > .
    .ddr4_rdimm2d_pmu_train_dmem    LOAD(. & phy_mask | kseg1_bits)           :{ddr4_rdimm2d_pmu_train_dmem.bin(.raw)} > .
    .ddr4_lrdimm_pmu_train_imem     LOAD(. & phy_mask | kseg1_bits)           :{ddr4_lrdimm_pmu_train_imem.bin(.raw)} > .
    .ddr4_lrdimm_pmu_train_dmem     LOAD(. & phy_mask | kseg1_bits)           :{ddr4_lrdimm_pmu_train_dmem.bin(.raw)} > .
    .ddr4_lrdimm2d_pmu_train_imem   LOAD(. & phy_mask | kseg1_bits)           :{ddr4_lrdimm2d_pmu_train_imem.bin(.raw)} > .
    .ddr4_lrdimm2d_pmu_train_dmem   LOAD(. & phy_mask | kseg1_bits)           :{ddr4_lrdimm2d_pmu_train_dmem.bin(.raw)} > .
    
    .ROM.text_lib           ALIGN(32)            ROM(.text_lib)               LOAD(. & phy_mask | kseg1_bits) : > .
    .ROM.text_rammem        ALIGN(32)            ROM(.text_rammem)            LOAD(. & phy_mask | kseg1_bits) : > .

    /** 
     ** For Explorer following section should be the last section.
     ** !!!!! Please Do NOT Remove this section from the last line !!!!!
     ** This will break Get Adapter Properties Command
     */
    .fw_version_info                LOAD(. & phy_mask | kseg1_bits)           :{app_version.bin(.raw)}  > .

/*
** These special symbols mark the bounds of RAM and ROM memory.
** They are used by the MULTI debugger.
*/
    __ghs_ramstart  = MEMADDR(sram_data_start);
    __ghs_ramend    = MEMENDADDR(sram_location_end);
    __ghs_romstart  = MEMADDR(spi_flash_uncached_start);
    __ghs_romend    = MEMENDADDR(spi_flash_uncached_end);
}

