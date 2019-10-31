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
*   DESCRIPTION: This file implements fw_version_info structure
*                This file is first compiled and the using 1) gsrec.exe and
*                2) mot2bin.exe, a binary file is generated, which is then
*                appended to runtime binary image.
*
*
*
*   NOTES:       None.
*
******************************************************************************/




/**
* @addtogroup FW_VERSION_INFO
* @{
* @file
* @brief
*   This file contains fw_version_info structure
*
*/ 


/*
** Include Files
*/
#include "fw_version_info.h"



fw_version_info_struct fw_ver_info =   { FW_VERSION_MAJOR_RELEASE_NUMBER, 
                                         FW_VERSION_MINOR_RELEASE_NUMBER, 
                                         FW_VERSION_PATCH_RELEASE_NUMBER,
                                         FW_VERSION_CL_NUMBER, 
                                         FW_VERSION_BUILD_DATE
                                        };



/** @} end group */



