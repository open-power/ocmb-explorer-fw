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
*     Header file for the OCMB module.
*
*   NOTES:
*     None.
*
*******************************************************************************/


#ifndef _EXP_DDR_CTRLR_H
#define _EXP_DDR_CTRLR_H

/*
** Include Files
*/

#include "pmcfw_types.h"
#include "pmcfw_err.h"

/*
** Macro Definitions
*/

/*
** Enumerated Types
*/

/***************************************************************************
* ENUM: ocmb_topology_type
* __________________________________________________________________________
*
* DESCRIPTION:
*   Enumerates the types of OCMB topologies.
*
* NOTE:
*
****************************************************************************/
typedef enum
{
    SINGLE_CHANNEL = 0,
    DUAL_CHANNEL   = 1,
    APOLLO_RDIMM   = 2
} ocmb_topology_type;

/***************************************************************************
* ENUM: ocmb_package_type
* __________________________________________________________________________
*
* DESCRIPTION:
*   Enumerates the types of OCMB packages.
*
* NOTE:
*
****************************************************************************/
typedef enum
{
    SDP = 0
} ocmb_package_type;

/*
** Constants
*/

/*
** Macros
*/

#define DIVIDE_CEIL(numerator, denominator)     \
     (numerator + denominator - 1) / denominator;

/*
** Structures and Unions
*/

/*******************************************************************************
* STRUCT: ocmb_spd_config_struct
* __________________________________________________________________________
*
* DESCRIPTION:
*   This structure defines the ODT settings for each rank.
*
* ELEMENTS:
*   See below.
*
* NOTE:
*    ##############################################################
*    # input value to ODT setting table                           #
*    # 0x00-0x03=0x0  0x04-0x07=0x1  0x08-0x0B=0x2  0x0C-0x0F=0x3 #
*    # 0x10-0x13=0x0  0x14-0x17=0x1  0x18-0x1B=0x2  0x1C-0x1F=0x3 #
*    # 0x20-0x23=0x0  0x24-0x27=0x1  0x28-0x2B=0x2  0x2C-0x2F=0x3 #
*    # 0x30-0x33=0x0  0x34-0x37=0x1  0x38-0x3B=0x2  0x3C-0x3F=0x3 #
*    # 0x40-0x43=0x4  0x44-0x47=0x5  0x48-0x4B=0x6  0x4C-0x4F=0x7 #
*    # 0x50-0x53=0x4  0x54-0x57=0x5  0x58-0x5B=0x6  0x5C-0x5F=0x7 #
*    # 0x60-0x63=0x4  0x64-0x67=0x5  0x68-0x6B=0x6  0x6C-0x6F=0x7 #
*    # 0x70-0x73=0x4  0x74-0x77=0x5  0x78-0x7B=0x6  0x7C-0x7F=0x7 #
*    # 0x80-0x83=0x8  0x84-0x87=0x9  0x88-0x8B=0xA  0x8C-0xBF=0xB #
*    # 0x90-0x93=0x8  0x94-0x97=0x9  0x98-0x9B=0xA  0x9C-0x9F=0xB #
*    # 0xA0-0xA3=0x8  0xA4-0xA7=0x9  0xA8-0xAB=0xA  0xAC-0xAF=0xB #
*    # 0xB0-0xB3=0x8  0xB4-0xB7=0x9  0xB8-0xBB=0xA  0xBC-0xBF=0xB #
*    # 0xC0-0xC3=0xC  0xC4-0xC7=0xD  0xC8-0xCB=0xE  0xCC-0xFF=0xF #
*    # 0xE0-0xE3=0xC  0xE4-0xE7=0xD  0xE8-0xEB=0xE  0xEC-0xEF=0xF #
*    # 0xF0-0xF3=0xC  0xF4-0xF7=0xD  0xF8-0xFB=0xE  0xFC-0xFF=0xF #
*    ##############################################################
*
*******************************************************************************/
typedef struct
{
    UINT8 rank0;
    UINT8 rank1;
    UINT8 rank2;
    UINT8 rank3;
    UINT8 rank4;
    UINT8 rank5;
    UINT8 rank6;
    UINT8 rank7;
} ocmb_odt_struct;

/*******************************************************************************
* STRUCT: ocmb_spd_config_struct
* __________________________________________________________________________
*
* DESCRIPTION:
*   This structure defines the SPD configurations
*
* ELEMENTS:
*   See below.
*
* NOTE:
*   None.
*
*******************************************************************************/
typedef struct
{
    UINT16             dram_size;        /* in GB */
    UINT16             dram_width;              
    UINT16             ranks;
    UINT16             rdimm;            /* CDIMM = 0, RDIMM = 1, UDIMM = 2, LRDIMM = 3 */
    UINT16             rdimm_buffer_delay;
    UINT16             dfimrl_clk;
    
    UINT32             clk_freq;
    ocmb_topology_type topo_type;
    ocmb_package_type  package_type;
    UINT16             die_count;        /* 1->9 dies not sure if needed */
    UINT16             v3ds_height;      /* 0/2/4/8 options */
    UINT16             cl;
    UINT16             cwl;
    
    /* Might need to add sync odt start/stop */
    /* Might need to add number of ranks per slot for Apollow RDIMM case*/
    
    /* The following config settings are all in picoseconds */
    UINT32             tRCD;
    UINT32             tRP;
    UINT32             tRAS;
    UINT32             tRC;
    UINT32             tRFC1;
    UINT32             tRFC2;
    UINT32             tRFC3;
    UINT32             tRFC4;
    UINT32             tFAW;
    UINT32             tRRD_S;
    UINT32             tRRD_L;
    UINT32             tCCD_L;
    UINT32             tWR;
    UINT32             tWTR_S;
    UINT32             tWTR_L;
        
    /* Need address map output */
    
    /* Optional Features: */
    UINT8              rcd_par_retry;
    UINT8              v2t_mode;
    
    /* Enterprise and half dimm mode */
    UINT8              enterprise_mode;
    UINT8              half_dimm_mode;

    /* ODT settings */
    ocmb_odt_struct    rd_odt;
    ocmb_odt_struct    wr_odt;

    /* Additional features to be added */
    /* low power features on/off and configs */
    /* address inversion settings */
    /* refersh control settings (per rank refresh?) */ 
    
} exp_ddr_ctrlr_spd_config_struct;


/* Function Prototypes */

EXTERN BOOL exp_ddr_ctrlr_spd_config(UINT32 ocmb_base_addr, exp_ddr_ctrlr_spd_config_struct spd_config);

#endif /* _EXP_DDR_CTRLR_H */


