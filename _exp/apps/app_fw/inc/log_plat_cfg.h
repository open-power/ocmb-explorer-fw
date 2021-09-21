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
*     This file contains firmware definitions used to set LOG_COMPILE_LEVEL
*     which is used by application at compiling time. Each application should
*     have its own copy of log_plat_cfg.h.
*
*   NOTES       :
*
*******************************************************************************/



/*---- Compile Options -----------------------------------------------------*/
#ifndef _LOG_PLAT_CFG_H
#define _LOG_PLAT_CFG_H

/*
** Include files
*/


/*
** Choose compile time logging severity level
**  log_sev_disable
**  log_sev_highest
**  log_sev_high
**  log_sev_medium
**  log_sev_low
**  log_sev_lowest
*/
#define LOG_COMPILE_LEVEL log_sev_medium

/**
 * Set LOG_PLAT_USE_STRINGS to 1 if printing out strings to the log buffer. 
 * Set LOG_PLAT_USE_STRINGS to 0 if printing out codes to the log buffer. 
 */
#define LOG_PLAT_USE_STRINGS 1

/*
** Set LOG_PLAT_LOGGING_ENABLED to 1 to enable logging.
** Set LOG_PLAT_LOGGING_ENABLED to 0 to disable logging. 
*/
#define LOG_PLAT_LOGGING_ENABLED 1

#endif /* _LOG_PLAT_CFG_H */


