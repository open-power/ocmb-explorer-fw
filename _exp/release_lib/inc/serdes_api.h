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
*     Header file containing all the OCMB API function definitions.
*
*   NOTES:
*     None.
*
*******************************************************************************/
#ifndef _SERDES_API_H
#define _SERDES_API_H

/*
** Include Files
*/

/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Macro Definitions
*/

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/

EXTERN UINT32 serdes_api_adpt_done_get(UINT32 lane_offset);
EXTERN INT32 serdes_api_d_iq_offset_read(UINT32 lane_offset);
EXTERN INT32 serdes_api_d_iq_offset_write(UINT32 lane_offset, UINT32 d_iq_offset);
EXTERN INT32 serdes_api_t_iq_offset_read(UINT32 lane_offset);
EXTERN VOID serdes_api_ffe_prepost_settings_read(UINT32 lane_offset, 
                                                 UINT32* tx_p2b_d2en_ptr,
                                                 UINT32* tx_p2b_d1en_ptr,
                                                 UINT32* tx_p2a_pten_ptr,
                                                 UINT32* tx_p2a_d2en_ptr,
                                                 UINT32* tx_p2a_d1en_ptr,
                                                 UINT32* tx_p1b_d2en_ptr,
                                                 UINT32* tx_p1b_d1en_ptr,
                                                 UINT32* tx_p1a_pten_ptr,
                                                 UINT32* tx_p1a_d2en_ptr,
                                                 UINT32* tx_p1a_d1en_ptr,
                                                 UINT32* tx_p3a_d1en_ptr,
                                                 UINT32* tx_p2b_pten_ptr,
                                                 UINT32* tx_p1b_pten_ptr);
EXTERN UINT32 serdes_api_fuse_val_stat_1_read(VOID);
EXTERN VOID serdes_api_lane_invert_set(UINT32 lane_offset);

#endif /* _SERDES_API_H */
