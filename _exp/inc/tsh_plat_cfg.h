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
*
*   NOTES:
*     Platform specific definitions for the tsh module.
*
*******************************************************************************/



#ifndef _TSH_PLAT_CFG_H
#define _TSH_PLAT_CFG_H

/*
** Set to 0 when using the non-OS version of the Tiny Shell Module
** or 1 when using the standard one.
*/
#define TSH_THREAD_SUPPORT  0

/*
** Set to 0 for no history command lines (to save space).
*/
#define TSH_MAX_HISTORY_LINES   20

/*
** Maximum number of shells that can be instantiated.
*/
#define TSH_SHELLS_MAX          1

/* Tiny Shell configuration */
#define TSH_SHELL_IDX           0        

#endif


