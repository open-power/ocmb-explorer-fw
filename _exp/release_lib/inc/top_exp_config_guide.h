/*******************************************************************************
*  Copyright 2020 Microchip Technology Inc. and its subsidiaries.
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
*   DESCRIPTION :
*     Generated register header file to be used by top_exp_config_guide.c.
*     
*     This file is generated from:
*     EXPLORER_CNFG_GUIDE_pdox_r030.xlsx (pdox_r030)
*     using this command:
*     
*     python ConfigGuideConv.py -gEXPLORER_CNFG_GUIDE_pdox_r030.xlsx -rpdox_r030
*     -pEXPLORER_CNFG_GUIDE_pdox_r030.parsed -nnamespace_exp_p_a_release_047.tcl
*     -ctop_exp_config_guide.c -htop_exp_config_guide.h
*     -iserdes_link_width_x4,serdes_link_width_x8,di_enable,di_disable,di_enable_foxhound,di_disable_foxhound,WCIMODE_WC,doorbell_interrupt_controls,deassert_serdes_reset,deassert_phy_ocmb_reset,DDLL
*     -ftop_exp_cfg -a
* 
* 
*   NOTES: 
*     None.
*
*******************************************************************************/
#ifndef _TOP_EXP_CONFIG_GUIDE_H
#define _TOP_EXP_CONFIG_GUIDE_H

/*
** Include Files
*/




/* Function Prototypes */

EXTERN BOOL top_exp_cfg_deassert_phy_ocmb_reset(UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_doorbell_interrupt_controls(UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_WCIMODE_WC(UINT32 pcse_top_regs_base_addr,
                                   UINT32 wrapper_base_addr,
                                   UINT32 efuse_base_addr,
                                   UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_serdes_link_width_x8(UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_di_disable(UINT32 top_xcbi_base_addr,
                                   UINT32 gpbc_sys_xcbi_base_addr,
                                   UINT32 gpbc_peri_xcbi_base_addr,
                                   UINT32 efuse_base_addr,
                                   UINT32 wrapper_base_addr,
                                   UINT32 spi_0_base_addr,
                                   UINT32 dll_base_addr);
EXTERN BOOL top_exp_cfg_deassert_serdes_reset(UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_assert_serdes_reset(UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_DDLL(UINT32 dll_base_addr,
                             UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_serdes_link_width_x4(UINT32 top_xcbi_base_addr);
EXTERN BOOL top_exp_cfg_di_disable_foxhound(UINT32 diag_pcbi_base_addr,
                                            UINT32 adsp_pcbi_base_addr,
                                            UINT32 mdsp_pcbi_base_addr,
                                            UINT32 channel_pcbi_base_addr,
                                            UINT32 csu_pcbi_base_addr,
                                            UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL top_exp_cfg_di_enable(UINT32 top_xcbi_base_addr,
                                  UINT32 gpbc_sys_xcbi_base_addr,
                                  UINT32 gpbc_peri_xcbi_base_addr,
                                  UINT32 efuse_base_addr,
                                  UINT32 wrapper_base_addr,
                                  UINT32 spi_0_base_addr,
                                  UINT32 dll_base_addr);
EXTERN BOOL top_exp_cfg_di_enable_foxhound(UINT32 diag_pcbi_base_addr,
                                           UINT32 adsp_pcbi_base_addr,
                                           UINT32 mdsp_pcbi_base_addr,
                                           UINT32 channel_pcbi_base_addr,
                                           UINT32 csu_pcbi_base_addr,
                                           UINT32 mtsb_ctrl_pcbi_base_addr);

#endif /* _TOP_EXP_CONFIG_GUIDE_H */
