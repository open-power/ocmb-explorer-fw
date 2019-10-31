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
*     This file implements the platform specific functionality for Seeprom driver.
*
*******************************************************************************/


#ifndef _SEEPROM_MM_PLAT_H
#define _SEEPROM_MM_PLAT_H
/*
** Include files
*/

/*
** Constant
*/

/*
** This is only used for Palladium builds to mimic a SEEPROM device
*/
#define SEEPROM_MM_DEVICES \
    {{0, 0xA0, 0x2000, seeprom_mm_bootstrap}}

/*
** External Variable
*/
EXTERN unsigned char seeprom_mm_bootstrap[];

#endif /* _SEEPROM_MM_PLAT_H */


