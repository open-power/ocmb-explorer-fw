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
* @brief
*   PMC Firmware Utility Program Header
*/
#ifndef _PMCFW_UTIL_H
#define _PMCFW_UTIL_H

/* 
* include files 
*/

#include "pmcfw_types.h"

#include <math.h>

/*
* public type definitions 
*/
typedef struct
{
    CHAR    *pName;
    UINT32  Value;
} sPMCFW_UTIL_NAMED_VALUE;

/* 
* public function definitions 
*/

EXTERN UINT32 PMCFW_UTIL_HexStrToUINT32( CHAR *pChar );
EXTERN UINT32 PMCFW_UTIL_UintStrToUINT32( CHAR *pChar );
EXTERN INT32 PMCFW_UTIL_OptionIs( CHAR *pParam );
EXTERN INT32 PMCFW_UTIL_ValueInNamedValueListFind(  sPMCFW_UTIL_NAMED_VALUE *pNameList, 
                                                    CHAR                    *pTestToken, 
                                                    UINT32                  *pNamedValue );
EXTERN INT32 PMCFW_UTIL_NameInNamedValueListFind(   sPMCFW_UTIL_NAMED_VALUE *pNameList, 
                                                    UINT32                  NamedValue, 
                                                    CHAR                    **ppTestToken );
EXTERN VOID PMCFW_UTIL_CmdlineParse1(   INT32 argc,
                                        CHAR **argv, 
                                        CHAR *input_filename_ptr, 
                                        CHAR *output_filename_ptr);

#endif /* _PMCFW_UTIL_H */




