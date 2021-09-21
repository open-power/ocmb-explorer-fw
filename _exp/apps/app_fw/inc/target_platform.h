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
*     Specify target platform and various options
*******************************************************************************/



#ifndef _TARGET_PLATFORM_H
#define _TARGET_PLATFORM_H

/*
** Include Files
*/

/*
** Constants
*/

/*
** Target Platform
** SILICON:     Run on silicon device
** PALLADIUM:   Run on Palladium/protium emulation platform
*/
#define TARGET_PLATFORM     SILICON

#define SILICON             0
#define PALLADIUM           1

/* Check target platform compile flag is valid */
#if (TARGET_PLATFORM != SILICON) && \
    (TARGET_PLATFORM != PALLADIUM)
#error TARGET_PLATFORM compile flag is not set to a valid value
#endif

/*
** INCLUDE_ANALOG 0         : Build without analog initialization
** 
** INCLUDE_ANALOG 1         : Normal chip build with analog initialization
*/
#if (TARGET_PLATFORM == SILICON)
#define INCLUDE_ANALOG      1
#elif (TARGET_PLATFORM == PALLADIUM)
#define INCLUDE_ANALOG      0
#endif

/*
** SWITCHTEC_EMULATION_PLATFORM
** Defined when running on emulation platform for switchtec firmware
*/
#if (TARGET_PLATFORM == SILICON)
#undef  SWITCHTEC_EMULATION_PLATFORM
#elif (TARGET_PLATFORM == PALLADIUM)
#define SWITCHTEC_EMULATION_PLATFORM
#endif

#endif /* _TARGET_PLATFORM_H */


