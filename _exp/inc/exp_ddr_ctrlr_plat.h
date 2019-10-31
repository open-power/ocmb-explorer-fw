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
* @addtogroup exp_ddr_ctrlr
* @{
* @file
* @brief
*   Header for Explorer DDR controller platform specific library
*
* @note
*/

#ifndef _EXP_DDR_CTRLR_PLAT_H
#define _EXP_DDR_CTRLR_PLAT_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_err.h"
#include "exp_ddr_ctrlr.h"

/*
** Enumerated Types
*/

/*
** External References
*/

EXTERN VOID exp_ddr_ctrlr_sample_spd_config_init(exp_ddr_ctrlr_spd_config_struct *ocmb_config);
EXTERN void exp_ddr_ctrlr_init(VOID);


#endif /* _EXP_DDR_CTRLR_PLAT_H */


