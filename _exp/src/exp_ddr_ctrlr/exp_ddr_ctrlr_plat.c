/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*
* Copyright (c) 2018, 2019. 2020 Microchip Technology Inc. All rights reserved.
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
* @addtogroup exp_ddr_ctrlr
* @{
* @file
* @brief
*   Explorer DDR Controller library
*
* @note
*/

#include "pmc_profile.h"
#include "target_platform.h"
#include "pmc_hw_base.h"
#include "exp_ddr_ctrlr.h"
#include "ddr_phy.h"


/*
** Constants
*/

/*
** Local Variables
*/

exp_ddr_ctrlr_spd_config_struct ocmb_config;

/**
* @brief
*   This enumeration specifies the supported SPD configurations.
*/
typedef enum
{
    OCMB_PALLADIUM_SPD_CONFIG,          /**< SPD Configuration used for emulation */
    OCMB_UDIMM_SPD_CONFIG_x8_1R_2666,   /**< SPD Configuration for x8-1R-UDIMM-2666 */
    OCMB_UDIMM_SPD_CONFIG_x8_2R_3200,	/**< SPD Configuration for x8-2R-UDIMM-3200 */
    OCMB_RDIMM_SPD_CONFIG_x4_2R_3200,	/**< SPD Configuration for x4-2R-RDIMM-3200 */
    OCMB_UDIMM_SPD_CONFIG_x16_1R_2666,  /**< SPD Configuration for x16-1R-UDIMM-2666 */
    OCMB_UDIMM_SPD_CONFIG_x8_1R_3200,	/**< SPD Configuration for x8-1R-UDIMM-3200 */
    OCMB_NUM_SPD_CONFIG                 /**< Number of SPD configurations */
} ocmb_spd_config_index_enum;

PRIVATE const exp_ddr_ctrlr_spd_config_struct ocmb_spd_config_array[OCMB_NUM_SPD_CONFIG] = {
    [OCMB_PALLADIUM_SPD_CONFIG] = {
        .dram_size          = 8,
        .dram_width         = 8, // or 4
        .ranks              = 1,
        .rdimm              = 0,
        .rdimm_buffer_delay = 0, // probably do not care?
        .dfimrl_clk         = 0x9,
        .clk_freq           = 1600,      //1600MHz aka DDR3200
        .topo_type          = SINGLE_CHANNEL,
        .package_type       = SDP,
        .die_count          = 10, // 20 for x4
        .v3ds_height        = 0,
        .cl                 = 20,
        .cwl                = 16,

        /* The following config settings are all in ps */ // Check with Riad check that is also running at 3200 MHz
        .tRCD               = 12500,
        .tRP                = 12500,
        .tRAS               = 32000,
        .tRC                = 45750,
        .tRFC1              = 550000,
        .tRFC2              = 350000,
        .tRFC4              = 260000,
        .tFAW               = 21000,
        .tRRD_S             = 3000,     //AE Note: JEDEC spec has min of 4 CLKs
        .tRRD_L             = 4900,     //AE Note: JEDEC spec has min of 4 CLKs
        .tCCD_L             = 5000,
        .tWR                = 15000,
        .tWTR_S             = 2500,      //AE Note: JEDEC spec has min of 2 CLKs
        .tWTR_L             = 7500,      //AE Note: JEDEC spec has min of 4 CLKs

        .rcd_par_retry      = 0,
        .v2t_mode           = 0,

        .enterprise_mode    = 0,
        .half_dimm_mode     = 0,

        .rd_odt.rank0       = 0x7,
        .rd_odt.rank1       = 0x7,
        .rd_odt.rank2       = 0x7,
        .rd_odt.rank3       = 0x7,
        .rd_odt.rank4       = 0x7,
        .rd_odt.rank5       = 0x7,
        .rd_odt.rank6       = 0x7,
        .rd_odt.rank7       = 0x7,

        .wr_odt.rank0       = 0x7,
        .wr_odt.rank1       = 0x7,
        .wr_odt.rank2       = 0x7,
        .wr_odt.rank3       = 0x7,
        .wr_odt.rank4       = 0x7,
        .wr_odt.rank5       = 0x7,
        .wr_odt.rank6       = 0x7,
        .wr_odt.rank7       = 0x7
    },
    [OCMB_UDIMM_SPD_CONFIG_x8_1R_2666] = {
        .dram_size          = 8,
        .dram_width         = 8,
        .ranks              = 1,
        .rdimm              = 0,
        .rdimm_buffer_delay = 0, // probably do not care?
        .dfimrl_clk         = 0xA,
        .clk_freq           = 1333,      //1600MHz aka DDR3200
        .topo_type          = SINGLE_CHANNEL,
        .package_type       = SDP,
        .die_count          = 9, // 20 for x4
        .v3ds_height        = 1,
        .cl                 = 19,
        .cwl                = 14,
        /* The following config settings are all in ps */ // Check with Riad check that is also running at 3200 MHz
        .tRCD               = 13750,
        .tRP                = 13750,
        .tRAS               = 32000,
        .tRC                = 45750,
        .tRFC1              = 350000,
        .tRFC2              = 260000,
        .tRFC4              = 160000,
        .tFAW               = 21000,
        .tRRD_S             = 3000,     //AE Note: JEDEC spec has min of 4 CLKs
        .tRRD_L             = 4900,     //AE Note: JEDEC spec has min of 4 CLKs
        .tCCD_L             = 5000,
        .tWR                = 15000,
        .tWTR_S             = 2500,      //AE Note: JEDEC spec has min of 2 CLKs
        .tWTR_L             = 7500,      //AE Note: JEDEC spec has min of 4 CLKs
        .rcd_par_retry      = 0,
        .v2t_mode           = 0,
        .enterprise_mode    = 0,
        .half_dimm_mode     = 0,
        .rd_odt.rank0       = 0x40,
        .rd_odt.rank1       = 0x80,
        .rd_odt.rank2       = 0x40,
        .rd_odt.rank3       = 0x80,
        .rd_odt.rank4       = 0x04,
        .rd_odt.rank5       = 0x08,
        .rd_odt.rank6       = 0x04,
        .rd_odt.rank7       = 0x08,
        .wr_odt.rank0       = 0xAA,
        .wr_odt.rank1       = 0x40,
        .wr_odt.rank2       = 0x80,
        .wr_odt.rank3       = 0x40,
        .wr_odt.rank4       = 0x08,
        .wr_odt.rank5       = 0x04,
        .wr_odt.rank6       = 0x08,
        .wr_odt.rank7       = 0x04
    },
    [OCMB_UDIMM_SPD_CONFIG_x8_2R_3200] = {
       .dram_size          = 8,
       .dram_width         = 8,
       .ranks              = 2,
       .rdimm              = 0,
       .rdimm_buffer_delay = 0, // probably do not care?
       //.dfimrl_clk         = 0x9, // default and working
       .dfimrl_clk         = 0xA, //As per PREP "509638"
       .clk_freq           = 1600,      //1600MHz aka DDR3200
       .topo_type          = SINGLE_CHANNEL,
       .package_type       = SDP,
       .die_count          = 16,
       .v3ds_height        = 0,
       .cl					= 22,
       .cwl                = 16,
       // The following config settings are all in ps // Check with Riad check that is also running at 3200 MHz
       .tRCD               = 13750,
       .tRP                = 13750,
       .tRAS               = 32000,
       .tRC                = 45750,
       .tRFC1              = 350000,
       .tRFC2              = 260000,
       .tRFC4              = 160000,
       .tFAW               = 21000,
       .tRRD_S             = 2500,  //AE Note: JEDEC spec has min of 4 CLKs
       .tRRD_L             = 4900,  //AE Note: JEDEC spec has min of 4 CLKs
       .tCCD_L             = 5000,
       .tWR                = 15000,
       .tWTR_S             = 2500,      //AE Note: JEDEC spec has min of 2 CLKs
       .tWTR_L             = 7500,      //AE Note: JEDEC spec has min of 4 CLKs
       .rcd_par_retry      = 0,
       .v2t_mode           = 0,
       .enterprise_mode    = 0,
       .half_dimm_mode     = 0,

       .rd_odt.rank0       = 0x44, //working one
       .rd_odt.rank1       = 0x88, //working one
       //.rd_odt.rank0       = 0x88, //Alex new suggestion not working for this config
       //.rd_odt.rank1       = 0x44, //Alex new suggestion not working for this config
       .rd_odt.rank2       = 0x00,
       .rd_odt.rank3       = 0x00,
       .rd_odt.rank4       = 0x00,
       .rd_odt.rank5       = 0x00,
       .rd_odt.rank6       = 0x00,
       .rd_odt.rank7       = 0x00,

       .wr_odt.rank0       = 0x44,
       .wr_odt.rank1       = 0x88,
       .wr_odt.rank2       = 0x00,
       .wr_odt.rank3       = 0x00,
       .wr_odt.rank4       = 0x00,
       .wr_odt.rank5       = 0x00,
       .wr_odt.rank6       = 0x00,
       .wr_odt.rank7       = 0x00
    },
    [OCMB_RDIMM_SPD_CONFIG_x4_2R_3200] = {
        .dram_size          = 8,
        .dram_width         = 4,
        .ranks              = 2,
        .rdimm              = 1,
        .rdimm_buffer_delay = 0, // probably do not care?

        //.dfimrl_clk         = 0x9, //As per ALex Phy config
        .dfimrl_clk         = 0xA, //As per PREP "509638"
        .clk_freq           = 1600,      //1600MHz aka DDR3200

        .topo_type          = SINGLE_CHANNEL,
        .package_type       = SDP,
        .die_count          = 18,
        .v3ds_height        = 0,
        .cl                 = 22,
        .cwl                = 16,
        /* The following config settings are all in ps */ // Check with Riad check that is also running at 3200 MHz
        .tRCD               = 13750,
        .tRP                = 13750,
        .tRAS               = 32500,        // Matching as per Kevin
        .tRC                = 45750,
        .tRFC1              = 350000,
        .tRFC2              = 260000,
        .tRFC4              = 160000,
        .tFAW               = 10000,
        .tRRD_S             = 2500,     //AE Note: JEDEC spec has min of 4 CLKs
        .tRRD_L             = 4900,     //AE Note: JEDEC spec has min of 4 CLKs
        .tCCD_L             = 5000,
        .tWR                = 15000,
        .tWTR_S             = 2500,      //AE Note: JEDEC spec has min of 2 CLKs
        .tWTR_L             = 7500,      //AE Note: JEDEC spec has min of 4 CLKs
        .rcd_par_retry      = 0,
        .v2t_mode           = 0,
        .enterprise_mode    = 0,
        .half_dimm_mode     = 0,

        .rd_odt.rank0       = 0x44, //Working
        .rd_odt.rank1       = 0x88, //Working
        //.rd_odt.rank0       = 0x88, //Alex suggestion working only for x4
        //.rd_odt.rank1       = 0x44, //Alex suggestion
        .rd_odt.rank2       = 0x00,
        .rd_odt.rank3       = 0x00,
        .rd_odt.rank4       = 0x00,
        .rd_odt.rank5       = 0x00,
        .rd_odt.rank6       = 0x00,
        .rd_odt.rank7       = 0x00,

        .wr_odt.rank0       = 0x44, //Kevin-Working
        .wr_odt.rank1		= 0x88, //Kevin-Working
        .wr_odt.rank2		= 0x00,
        .wr_odt.rank3		= 0x00,
        .wr_odt.rank4		= 0x00,
        .wr_odt.rank5		= 0x00,
        .wr_odt.rank6		= 0x00,
        .wr_odt.rank7		= 0x00
    },
    [OCMB_UDIMM_SPD_CONFIG_x16_1R_2666] = {
       //.dram_size          = 4,
       .dram_size          = 8,
       .dram_width         = 16,
       .ranks              = 1,
       .rdimm              = 0,
       .rdimm_buffer_delay = 0, // probably do not care?
       //.dfimrl_clk         = 0x9, // Added by pchandr1
       .dfimrl_clk         = 0xA, //As per PREP "509638"
       .clk_freq           = 1333,      //1600MHz aka DDR3200
       .topo_type          = SINGLE_CHANNEL,
       .package_type       = SDP,
       .die_count          = 4, // 20 for x4
       .v3ds_height        = 0,
       .cl                 = 19,
       .cwl                = 14,
       /* The following config settings are all in ps */ // Check with Riad check that is also running at 3200 MHz
       .tRCD               = 13750,
       .tRP                = 13750,
       .tRAS               = 32000,
       .tRC                = 47000, //Original
       //.tRC                = 45750,
       .tRFC1              = 350000,
       .tRFC2              = 260000,
       .tRFC4              = 160000,
       .tFAW               = 30000,
       .tRRD_S             = 5300,     //AE Note: JEDEC spec has min of 4 CLKs
       .tRRD_L             = 6400,     //AE Note: JEDEC spec has min of 4 CLKs
       .tCCD_L             = 5000,
       .tWR                = 15000,
       .tWTR_S             = 2500,      //AE Note: JEDEC spec has min of 2 CLKs
       .tWTR_L             = 7500,      //AE Note: JEDEC spec has min of 4 CLKs
       .rcd_par_retry      = 0,
       .v2t_mode           = 0,
       .enterprise_mode    = 0,
       .half_dimm_mode     = 0,

       .rd_odt.rank0       = 0x40,
       .rd_odt.rank1       = 0x80,
       .rd_odt.rank2       = 0x40,
       .rd_odt.rank3       = 0x80,
       .rd_odt.rank4       = 0x04,
       .rd_odt.rank5       = 0x08,
       .rd_odt.rank6       = 0x04,
       .rd_odt.rank7       = 0x08,

       .wr_odt.rank0       = 0xAA,
       .wr_odt.rank1       = 0xFF,
       .wr_odt.rank2       = 0xFF,
       .wr_odt.rank3       = 0xFF,
       .wr_odt.rank4       = 0xFF,
       .wr_odt.rank5       = 0xFF,
       .wr_odt.rank6       = 0xFF,
       .wr_odt.rank7       = 0xFF
    },
    [OCMB_UDIMM_SPD_CONFIG_x8_1R_3200] = {
        .dram_size          = 8,
        .dram_width         = 8,
        .ranks              = 1,
        .rdimm              = 0,
        .rdimm_buffer_delay = 0, // probably do not care?
        //.dfimrl_clk         = 0x9, 
        .dfimrl_clk         = 0xA, //As per PREP "509638"
        .clk_freq           = 1600,      //1600MHz aka DDR3200
        .topo_type          = SINGLE_CHANNEL,
        .package_type       = SDP,
        .die_count          = 9,  
        //.v3ds_height        = 1, //original
        .v3ds_height        = 0,
        .cl					= 22, 
        .cwl                = 16, 
        /* The following config settings are all in ps */ // Check with Riad check that is also running at 3200 MHz
        .tRCD               = 13750,
        .tRP                = 13750,
        .tRAS               = 32000,
        .tRC                = 45750,
        .tRFC1              = 350000,
        .tRFC2              = 260000,
        .tRFC4              = 160000,
        .tFAW               = 21000,
        .tRRD_S             = 2500,  //AE Note: JEDEC spec has min of 4 CLKs
        .tRRD_L             = 4900,  //AE Note: JEDEC spec has min of 4 CLKs
        .tCCD_L             = 5000,
        .tWR                = 15000,
        .tWTR_S             = 2500,      //AE Note: JEDEC spec has min of 2 CLKs
        .tWTR_L             = 7500,      //AE Note: JEDEC spec has min of 4 CLKs
        .rcd_par_retry      = 0,
        .v2t_mode           = 0,
        .enterprise_mode    = 0,
        .half_dimm_mode     = 0,
        
		.rd_odt.rank0       = 0x00,
        .rd_odt.rank1       = 0x00,
        .rd_odt.rank2       = 0x00,
        .rd_odt.rank3       = 0x00,
        .rd_odt.rank4       = 0x00,
        .rd_odt.rank5       = 0x00,
        .rd_odt.rank6       = 0x00,
        .rd_odt.rank7       = 0x00,
            
		.wr_odt.rank0       = 0xFF,
        .wr_odt.rank1       = 0x00,
        .wr_odt.rank2       = 0x00,
        .wr_odt.rank3       = 0x00,
        .wr_odt.rank4       = 0x00,
        .wr_odt.rank5       = 0x00,	
        .wr_odt.rank6       = 0x00,
        .wr_odt.rank7       = 0x00
    }
};

/*
** Global Variables
*/

/*
** Local Functions
*/

/*
** Public Functions
*/

/****************************************************************************
*
* FUNCTION: ocmb_sample_spd_config_init
* __________________________________________________________________________
*
* DESCRIPTION:
*   Sets hard-coded SPD values for testing.
*
* INPUTS:
*   ocmb_config - OCMB configuration structure in which to write the hard-coded
*                 values.
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   None.
*
* NOTES:
*   None.
*
*****************************************************************************/
PUBLIC void exp_ddr_ctrlr_sample_spd_config_init(exp_ddr_ctrlr_spd_config_struct *ocmb_spd_config)
{
#if (TARGET_PLATFORM == PALLADIUM)
    *ocmb_spd_config = ocmb_spd_config_array[OCMB_PALLADIUM_SPD_CONFIG];
#else
    *ocmb_spd_config = ocmb_spd_config_array[OCMB_UDIMM_SPD_CONFIG_x8_1R_2666];
#endif
}

/****************************************************************************
*
* FUNCTION: exp_ddr_ctrlr_init
* __________________________________________________________________________
*
* DESCRIPTION:
*   Initialize the Explorer DDR controller.
*
* INPUTS:
*   None.
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   None.
*
* NOTES:
*   None.
*
*****************************************************************************/
PUBLIC void exp_ddr_ctrlr_init(VOID)
{
#if (EXPLORER_BRINGUP == 1)
    /* Load hard coded SPD input values. */
    exp_ddr_ctrlr_sample_spd_config_init(&ocmb_config);

    /* Apply SPD values. */
    exp_ddr_ctrlr_spd_config(OCMB_REGS_BASE_ADDR, ocmb_config);
#endif
}


