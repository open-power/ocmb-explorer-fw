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
*
* @brief
*   Header file for pmcfw_getline module
*
* @note
*     None.
*
*/ 
#ifndef _PMCFW_GETLINE_H
#define _PMCFW_GETLINE_H	1

#include <stdio.h>
#include "pmcfw_types.h"

#define GETLINE_NO_LIMIT	-1

EXTERN int pmcfw_getline(char **_lineptr, size_t *_n, FILE *_stream);
EXTERN int pmcfw_getline_safe(char **_lineptr, size_t *_n, FILE *_stream, int limit);

#endif /* _PMCFW_GETLINE_H */


