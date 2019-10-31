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
*     Generated register header file to be used by top_resets.c.
*
*     This file is generated from:
*     PM8090_revA_Top-Level_Config_Guide.xlsx (PDOX rev 0.08)
*     using this command:
*
*     python ConfigGuideConv.py -gPM8090_revA_Top-Level_Config_Guide.xlsx -r0.08
*     -pPM8090_revA_Top-Level_Config_Guide.parsed -nnamespace_luxor24x_reva.tcl
*     -ctop_resets.c -htop_resets.h
*
*
*   NOTES:
*     None.
*
*******************************************************************************/


#ifndef _TOP_RESETS_H
#define _TOP_RESETS_H

/*
** Include Files
*/



/*******************************************************************************
* Base addresses to be passed into the 'xxx_base_addr' argument of functions
*******************************************************************************/
#define DEV_TOP_TRIMODE_HOST_PHY_BASE_ADDR                                                  (0xBEA80000)
#define DEV_TOP_SAS_TRIMODE_PHY_WRAP_0                                                      (0xBEB00000)
#define DEV_TOP_SAS_TRIMODE_PHY_WRAP_1                                                      (0xBEB0C000)
#define DEV_TOP_SAS_TRIMODE_PHY_WRAP_2                                                      (0xBEB18000)


/* Function Prototypes */

EXTERN BOOL toplevel_sas_csu_init_x24(void);
EXTERN BOOL toplevel_sas_txrx_reset_x16(void);
EXTERN BOOL toplevel_sas_txrx_reset_x24(void);
EXTERN BOOL toplevel_pcie_txrx_resets(UINT32 pm88_60_254_base_addr);
EXTERN BOOL toplevel_sas_txrx_tm2(UINT32 pm88_60_254_base_addr,
                                  UINT32 blk_instance_a);
EXTERN BOOL toplevel_sas_txrx_tm1(UINT32 pm88_60_254_base_addr,
                                  UINT32 blk_instance_a);
EXTERN BOOL toplevel_sas_csu_init_x16(void);
EXTERN BOOL toplevel_sas_txrx_bd(void);
EXTERN BOOL toplevel_pcie_txrx_cal(void);
EXTERN BOOL toplevel_clkGating(void);
EXTERN BOOL toplevel_sas_txrx_tm0(UINT32 pm88_60_254_base_addr,
                                  UINT32 blk_instance_a);
EXTERN BOOL toplevel_pcie_csu_init(void);

/*
** The following are manually created functions, since the PM8090 config guide only
** covers x16 and x24 operation. These functions are from the PM8065 config guide.
*/
EXTERN BOOL toplevel_sas_csu_init_8(void);
EXTERN BOOL toplevel_sas_txrx_reset_8(void);

#endif /* _TOP_RESETS_H */


