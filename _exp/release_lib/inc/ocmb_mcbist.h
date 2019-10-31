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
*     Header file containing all the OCMB MCBIST functions.
*
*   NOTES:
*     None.
*
*******************************************************************************/


#ifndef _OCMB_MCBIST_H
#define _OCMB_MCBIST_H

/*
** Include Files
*/


/* Function Prototypes */

EXTERN BOOL ocmb_cfg_exp_mcbist_config_RandCmdSeq(UINT32 ocmb_base_addr);
EXTERN BOOL ocmb_cfg_exp_mcbist_config_W_R(UINT32 ocmb_base_addr);
EXTERN BOOL ocmb_cfg_exp_mcbist_config_W_RW(UINT32 ocmb_base_addr);
EXTERN BOOL ocmb_cfg_exp_mcbist_config_W_RWR(UINT32 ocmb_base_addr);
EXTERN BOOL ocmb_cfg_exp_mcbist_config_W_RWW(UINT32 ocmb_base_addr);
EXTERN BOOL ocmb_cfg_exp_mcbist_config_W_RRW(UINT32 ocmb_base_addr);

#endif /* _OCMB_MCBIST_H */


