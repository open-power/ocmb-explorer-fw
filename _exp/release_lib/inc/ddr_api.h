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
*  DESCRIPTION :
*    This file contains definitions and declarations for the DDR module
*    for use by external applications.
*
*  NOTES :
*
*******************************************************************************/


#ifndef _DDR_API_H
#define _DDR_API_H

/*
** Include Files
*/

#include <stdint.h>
#include "ddrphy_toolbox.h"
#include "ddr_phy.h"

/*
** Constants
*/

/*
** Enumerated Types
*/

/**
 * @brief Enumerated type for the voltage selection being used. 
 *        These enumerations are passed into the margining code
 *        to select the voltage type to measure, adjust, and/or
 *        to print out to the UART for information purposes.
 */
typedef enum
{
    DDR_DRAM_VOLTAGE          = 0,
    DDR_DEVICE_VOLTAGE        = 1,
    DDR_NUM_VOLTAGE_TYPES     = 2
} ddr_voltage_select_e;

/*
** Structures
*/

/**
 *
 * @brief A structure that holds the training timing data for
 *        DDR initialization using the fast boot/restore
 *        sequence.
 *
 */
typedef struct
{
    train_delay_msdg_t  timing_data;
    phy_cdd_t           cdd_data;
} ddr_timing_data_t;


/**
 *
 * @brief A structure that holds the trained Vref data for DDR
 *        initialization using the fast boot/restore sequence.
 *
 */
typedef struct __attribute__ ((__packed__))
{
    train_host_vref_msdg_t  vrefdqDeviceVoltageSetting;
    train_dram_vref_msdg_t  vrefdqDramVoltageSetting;
} ddr_vref_data_t;


/**
 * @brief Structure to hold verbosity settings.
 */
typedef struct
{
    uint32_t     enableAcceptancePrint;
    uint32_t     enableDebugPrint;
    uint32_t     enableHistogramPrint;
} ddr_verbosity_t;

/* DDR PHY reset callback function */
typedef void (*ddr_phy_reset_fcn)(BOOL do_reset);

/*
** Global Variables
*/
EXTERN ddr_timing_data_t ddrTimingData;
EXTERN ddr_vref_data_t   ddrVrefData;

/*
** External Declarations
*/
/*
 * ddr_api_init()
 *
 * @brief Init DDR library
 *
 * @param userInputMsdgPtr - pointer to the MSDG structure
 *
 * @return 0 = success, other = error code
 */
EXTERN uint32_t ddr_api_init(const user_input_msdg_t *userInputMsdgPtr);

/*
 * ddr_api_userInputMsdg_get()
 *
 * @brief Get pointer to userInputMsdg structure
 *
 * @return pointer to userInputMsdg
 */
EXTERN user_input_msdg_t* ddr_api_userInputMsdg_get(void);

/*
 * @brief Measure the timing and voltage windows using data saved from previously firmware training
 *
 * @param ddrTimingDataPtr - pointer to a structure that contains the previous timing results
 * @param ddrVrefDataPtr - pointer to a structure that contains the previous VREF results (for DDR4)
 *
 * @return 0 = success, other = error code.
 */
EXTERN uint32_t ddr_api_saved_margin_results_load( const ddr_timing_data_t *ddrTimingDataPtr,
                                                   const ddr_vref_data_t *ddrVrefDataPtr);

/*
 * @brief Train the DDR PHY settings
 *
 * @return 0 = success, other = error code.
 */
EXTERN uint32_t ddr_api_fw_train(void);

/*
 * @brief Train the DDR PHY until 2D read eye step and save eye results.
 *
 * @return 0 = success, other = error code.
 */
EXTERN uint32_t ddr_api_fw_read_eye_capture_train(uint8_t *eye_data_buffer_ptr);

/*
 * @brief Train the DDR PHY settings from 2D read eye step to 2D write eye step.
 *        Should only be called after first executing ddr_api_fw_read_eye_capture_train()
 *
 * @return 0 = success, other = error code.
 */
EXTERN uint32_t ddr_api_fw_write_eye_capture_train(uint8_t *eye_data_buffer_ptr);

/*
 * @brief Override PHY delay line settings using TOF data
 * 
 * This function is targeted for initial bring-up without training
 *
 * @return 0 = success, other = error code.
 * 
 * Notes: This is experimental test code and is not intended to be used by customers.
 */
EXTERN uint32_t  ddr_api_forced_dl_init(void);

/*
 * @brief Extract timing and VREF data from the register settings
 *
 * The returned training values should be saved to non-volatile storage.
 *
 * @param ddrTimingDataPtr - pointer to a structure that needs to be filled up with timing results
 * @param ddrVrefDataPtr - pointer to a structure that needs to be filled up  with VREF results
 *
 * @return 0 = success, other = error code.
 */
EXTERN uint32_t ddr_api_cal_results_get(   ddr_timing_data_t   *ddrTimingDataPtr,
                                           ddr_vref_data_t     *ddrVrefDataPtr);

#endif /* _DDR_API_H */



