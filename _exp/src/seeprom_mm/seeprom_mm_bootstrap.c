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

#include "target_platform.h"
#if (TARGET_PLATFORM == PALLADIUM)

/**
 * Dummy "SEEPROM" buffer to allow seeprom operations on 
 * Explorer emulation
**/

unsigned char seeprom_mm_bootstrap[256] = {
    0xEF, 0xBE, 0xAD, 0xDE, 0x11, 0xBA, 0x5E, 0xBA
};

#else

/* dummy code to silence the compiler about an empty translation unit */
typedef void __dummy;

#endif /* TARGET_PLATFORM == PALLADIUM */


