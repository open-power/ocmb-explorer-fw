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
*   DESCRIPTION :
*     Generated register header file to be used by serdes_config_guide.c.
*     
*     This file is generated from:
*     Foxhound_RevA_Config_Guide_pdox_r0_53.xlsx (0.53)
*     using this command:
*     
*     python ConfigGuideConv.py -gFoxhound_RevA_Config_Guide_pdox_r0_53.xlsx
*     -r0.53 -pFoxhound_RevA_Config_Guide_pdox_r0_53.parsed
*     -nnamespace_exp_p_a_release_047.tcl -cserdes_config_guide.c
*     -hserdes_config_guide.h
*     -xFHCG_init,FHCG_LBinit,FHCG_sim_init,FH_reset_deassert,FH_iddq_deassert,FH_CSU_open_fastsim,FH_TXRX_datapath_fastsim,FH_alignment_fastsim,FH_IQ_Offset_Calibration
*     -fSERDES -a
* 
* 
*   NOTES: 
*     A number of functions were modified to reduce execessively large
*     parameters lists and use pointers to data structures to pass parameter
*     lists.
*
*******************************************************************************/
#ifndef _SERDES_CONFIG_GUIDE_H
#define _SERDES_CONFIG_GUIDE_H

/*
** Include Files
*/




/* Function Prototypes */

EXTERN BOOL SERDES_FH_Diag_HBT(UINT32 mtsb_ctrl_pcbi_base_addr,
                               UINT32 diag_pcbi_base_addr,
                               UINT32 fast_acq,
                               UINT32 time_lim_high,
                               UINT32 time_lim_low,
                               UINT32 event_lim,
                               exp_pqm_horz_bt_struct* hbt_data_ptr);
EXTERN BOOL SERDES_FH_output_clk_start(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_adc_bist(UINT32 mtsb_ctrl_pcbi_base_addr,
                               UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_csu_bist4(UINT32 csu_pcbi_base_addr,
                                UINT32 if_dac_min,
                                UINT32 if_dac_max);
EXTERN BOOL SERDES_FH_TXRX_Adaptation2_normal(UINT32 adsp_pcbi_base_addr,
                                              UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_Adaptation2_dfe_disable(UINT32 adsp_pcbi_base_addr,
                                                   UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_Adaptation2_adaptation_disable(UINT32 adsp_pcbi_base_addr,
                                                          UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_RxTx_Parallel_LB(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_pattmon(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_Channel_PowerUp(UINT32 csu_pcbi_base_addr,
                                      UINT32 mtsb_ctrl_pcbi_base_addr,
                                      UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_PGA_loopback(UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_output_clk_stop(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TxRx_Parallel_LB(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_init_1_generic(UINT32 csu_pcbi_base_addr,
                                         UINT32 icp,
                                         UINT32 int_div,
                                         UINT32 denom_31_0,
                                         UINT32 denom_39_32,
                                         UINT32 frac_div_31_0,
                                         UINT32 frac_div_39_32);
EXTERN BOOL SERDES_FH_CSU_init_1_25g6(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_init_1_23g46(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_init_1_21g328(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_read_adapt(UINT32 adsp_pcbi_base_addr,
                                 UINT32 mtsb_ctrl_pcbi_base_addr,
                                 exp_pqm_rx_adapt_obj_struct* rx_adapt_data_ptr);
EXTERN BOOL SERDES_FH_TXRX_Adaptation1_FW_normal(UINT32 adsp_pcbi_base_addr,
                                                 UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_Adaptation1_FW_dfe_disable(UINT32 adsp_pcbi_base_addr,
                                                      UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_Adaptation1_FW_adaptation_disable(UINT32 adsp_pcbi_base_addr,
                                                             UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_init_2_generic(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_init_2_25g6(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_init_2_23g46(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_init_2_21g328(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TX_alignment(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_pattmon_dis(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_pattmon_en(UINT32 channel_pcbi_base_addr,
                                 UINT32 mode,
                                 UINT32* chk_usr_patt_ptr);
EXTERN BOOL SERDES_FH_TXRX_datapath(UINT32 adsp_pcbi_base_addr,
                                    UINT32 mdsp_pcbi_base_addr,
                                    UINT32 channel_pcbi_base_addr,
                                    UINT32 mtsb_ctrl_pcbi_base_addr,
                                    UINT32 tap_sel,
                                    UINT32 udfe_mode,
                                    UINT32 ph_ofs_t_pre);
EXTERN BOOL SERDES_28G_LOS_Handler(void);
EXTERN BOOL SERDES_FH_RX_alignment(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_write_mabc_threshold(UINT32 mtsb_ctrl_pcbi_base_addr,
                                           UINT32 addr,
                                           UINT32 data);
EXTERN BOOL SERDES_FH_poll_rx_alignment(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_poll_tx_alignment(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_ReAdaptation(UINT32 adsp_pcbi_base_addr,
                                        UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_Channel_PowerDwn(UINT32 csu_pcbi_base_addr,
                                       UINT32 mtsb_ctrl_pcbi_base_addr,
                                       UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_alignment_init_1(UINT32 mtsb_ctrl_pcbi_base_addr,
                                       UINT32 channel_pcbi_base_addr,
                                       UINT32 rx_align_enable);
EXTERN BOOL SERDES_FH_TX_Calibration(UINT32 csu_pcbi_base_addr,
                                     UINT32 mtsb_ctrl_pcbi_base_addr,
                                     UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_PowerUp(UINT32 csu_pcbi_base_addr,
                                   UINT32 mtsb_ctrl_pcbi_base_addr,
                                   UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_fw_init(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_PowerDwn(UINT32 csu_pcbi_base_addr,
                                    UINT32 mtsb_ctrl_pcbi_base_addr,
                                    UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_pattgen_dis(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_pattgen_en(UINT32 channel_pcbi_base_addr,
                                 UINT32 mode,
                                 UINT32* gen_usr_patt_ptr);
EXTERN BOOL SERDES_FH_read_adc_ram(UINT32 mtsb_ctrl_pcbi_base_addr,
                                   UINT32 addr,
                                   UINT32 *ram_data_ptr);
EXTERN BOOL SERDES_FH_csu_bist3(UINT32 csu_pcbi_base_addr,
                                UINT32 vco_mom_min,
                                UINT32 vco_mom_max);
EXTERN BOOL SERDES_FH_write_adc_ram(UINT32 mtsb_ctrl_pcbi_base_addr,
                                    UINT32 addr,
                                    UINT32 data);
EXTERN BOOL SERDES_FH_read_calib(UINT32 mtsb_ctrl_pcbi_base_addr,
                                 exp_pqm_rx_calib_value_struct* rx_calib_data_ptr);

EXTERN BOOL SERDES_FH_pattgen_chkrd(UINT32 channel_pcbi_base_addr,
                                    UINT32* err_cnt_ptr);

EXTERN BOOL SERDES_FH_csu_bist1(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_Diag_VBT(UINT32 mtsb_ctrl_pcbi_base_addr,
                               UINT32 diag_pcbi_base_addr,
                               UINT32 two_d_en,
                               UINT32 two_d_phase,
                               UINT32 outer_eye,
                               UINT32 fast_acq,
                               UINT32 time_lim_high,
                               UINT32 time_lim_low,
                               UINT32 event_lim,
                               exp_pqm_vert_bt_struct* vbt_data_ptr);
EXTERN BOOL SERDES_FH_MTSB_CTRL_rst(UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_RX_Calibration(UINT32 csu_pcbi_base_addr,
                                     UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_overwr_rtrim(UINT32 channel_pcbi_base_addr,
                                   UINT32 rtrim_ovr_en);
EXTERN BOOL SERDES_FH_NETX_Settings_value(UINT32 mtsb_ctrl_pcbi_base_addr,
                                          UINT32 tx_p3a_d1en);
EXTERN BOOL SERDES_FH_NETX_Settings_default(UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_NETX_Settings_calcprepost(UINT32 mtsb_ctrl_pcbi_base_addr,
                                                UINT32 tx_p2b_d2en,
                                                UINT32 tx_p2b_d1en,
                                                UINT32 tx_p2a_pten,
                                                UINT32 tx_p2a_d2en,
                                                UINT32 tx_p2a_d1en,
                                                UINT32 tx_p1b_d2en,
                                                UINT32 tx_p1b_d1en,
                                                UINT32 tx_p1a_pten,
                                                UINT32 tx_p1a_d2en,
                                                UINT32 tx_p1a_d1en,
                                                UINT32 tx_p3a_d1en,
                                                UINT32 tx_p2b_pten,
                                                UINT32 tx_p1b_pten);
EXTERN BOOL SERDES_FH_read_mabc_threshold(UINT32 mtsb_ctrl_pcbi_base_addr,
                                          UINT32 addr,
                                          UINT32 *mabc_data_ptr);
EXTERN BOOL SERDES_FH_read_rtrim(UINT32 channel_pcbi_base_addr,
                                 UINT32 *rtrim_14_0_ptr,
                                 UINT32 *rtrim_34_15_ptr);
EXTERN BOOL SERDES_FH_IQ_Offset_Calibration_1(UINT32 mtsb_ctrl_pcbi_base_addr,
                                              UINT32 iqcor_sel);
EXTERN BOOL SERDES_FH_alignment(UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_IQ_Offset_Calibration_3(UINT32 mdsp_pcbi_base_addr,
                                              UINT32 d_iq_offset,
                                              UINT32 d_iq_off_update,
                                              UINT32 t_iq_offset,
                                              UINT32 t_iq_off_update,
                                              UINT32 u_ph_offset);
EXTERN BOOL SERDES_FH_IQ_Offset_Calibration_2(UINT32 mtsb_ctrl_pcbi_base_addr,
                                              UINT32 iq_done,
                                              UINT32 *iq_done_ptr,
                                              UINT32 *iq_accum_ptr);
EXTERN BOOL SERDES_FH_IQ_Offset_Calibration_4(UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_read_CSU_status(UINT32 csu_pcbi_base_addr,
                                      exp_pqm_csu_calib_value_status_struct* csu_calib_data_ptr);
EXTERN BOOL SERDES_FH_TXRX_Calibration1(UINT32 mdsp_pcbi_base_addr,
                                        UINT32 mtsb_ctrl_pcbi_base_addr,
                                        UINT32 channel_pcbi_base_addr);
EXTERN BOOL SERDES_FH_CSU_read_ppm(UINT32 csu_pcbi_base_addr,
                                   UINT32 *ppm_offset_ptr);
EXTERN BOOL SERDES_FH_csu_bist2(UINT32 csu_pcbi_base_addr,
                                UINT32 vcap_addr_min,
                                UINT32 vcap_addr_max);
EXTERN BOOL SERDES_FH_TXRX_Adaptation1_PE(UINT32 adsp_pcbi_base_addr,
                                          UINT32 mdsp_pcbi_base_addr,
                                          UINT32 mtsb_ctrl_pcbi_base_addr,
                                          UINT32 obj_en_pass3);
EXTERN BOOL SERDES_FH_PGA_init(UINT32 mtsb_ctrl_pcbi_base_addr);
EXTERN BOOL SERDES_FH_csu_bist5(UINT32 csu_pcbi_base_addr);
EXTERN BOOL SERDES_FH_TXRX_AdaptObj_Force(UINT32 adsp_pcbi_base_addr,
                                          UINT32 mtsb_ctrl_pcbi_base_addr,
                                          exp_txrx_adaptobj_force_struct* txrx_adaptobj_force_ptr);

#endif /* _SERDES_CONFIG_GUIDE_H */
