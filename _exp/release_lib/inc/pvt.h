/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*                                                                               
* Copyright (c) 2020 Microchip Technology Inc. All rights reserved. 
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
*   DESCRIPTION : This file is the public header file for the PVT module.
*
*   NOTES:        None.
*
*******************************************************************************/

/**
 * @addtogroup PVT
 * 
 * @file
 *
 * @brief
 * This file is the local header file for the PVT controller
 *
 */

#ifndef _PVT_H
#define _PVT_H

/*
** Include Files
*/
#include "pmcfw_err.h"


/*
** Enumerated Types 
*/

/**
* @brief This enumeration specifies the clock value for the PVT TS.
* 
*/
typedef enum
{
    PVT_TS_CLK_FREQ_4M = 0,     /**< synthesized clk is set as 4MHz */
    PVT_TS_CLK_FREQ_5M = 1,     /**< synthesized clk is set as 5MHz */
    PVT_TS_CLK_FREQ_6M = 2,     /**< synthesized clk is set as 6MHz */
    PVT_TS_CLK_FREQ_8M = 4,     /**< synthesized clk is set as 8MHz */
    PVT_TS_CLK_FREQ_NUM,        /**< total number of clock frequencies */
} pvt_ts_clk_freq_enum;


/**
* @brief This enumeration specifies the measure type of PVT TS.
* 
*/
typedef enum
{
    PVT_TS_MEASURE_TYPE_ONCE = 0,       /**< run_once measure type  */
    PVT_TS_MEASURE_TYPE_CONTINUOUS = 1, /**< run_cont(continous) measure type */
    PVT_TS_MEASURE_TYPE_NUM,            /**< total measure number */
} pvt_ts_measure_type_enum;


/*
** Macro Definitions
*/

/**
 * This macro creates error codes for PVT module.
 *
 * @param [in] err_suffix      The base error code
 *
 * @returns The complete error code, including module ID and error code.
 */
#define PVT_ERR_CODE_CREATE(err_suffix)     ((PMCFW_ERR_BASE_PVT) | (err_suffix))
#define PVT_ERR_TS_SDIF_UNEXCEPTED_STATUS   PVT_ERR_CODE_CREATE(0x001)
#define PVT_ERR_TS_SAMPLE_NOT_DONE          PVT_ERR_CODE_CREATE(0x002)
#define PVT_ERR_TS_FAULT                    PVT_ERR_CODE_CREATE(0x003)
#define PVT_ERR_TS_ALREADY_STARTED          PVT_ERR_CODE_CREATE(0x004)


/*
* Enumerated Types
*/


/*
** Global variables 
*/


/* 
** Function Prototypes
*/
EXTERN PMCFW_ERROR pvt_ts_centigrade_degree_get(INT16 *centigrade_ptr);
EXTERN PMCFW_ERROR pvt_ts_shutdown(VOID);
EXTERN PMCFW_ERROR pvt_ts_init(pvt_ts_clk_freq_enum ts_clk_id, 
                               pvt_ts_measure_type_enum ts_measure_type);
EXTERN VOID pvt_scratch_pad_set(UINT32 fw_ver_cl_num);


#endif /* _PVT_H */

/** @} end addtogroup */
