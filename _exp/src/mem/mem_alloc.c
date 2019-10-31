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




/*
* Include Files
*/
#include "mem.h"
#include "pmc_plat.h"
#include <string.h>


/*
* Structures and Unions
*/

/* 
* Local Constants 
*/ 

/*
* Local Variables
*/

/*
* Private Functions
*/

/*
* Public Functions
*/

/**
* @brief
*   memory allocation routine.
*
* @param[in] nitems  - Number of elements to allocate. 
* @param[in] size  - Size of each element
*
* @return
*     pointer to memory buffer.
*
* @note
*/
VOID *platform_calloc(size_t nitems, size_t size)
{
    UCHAR *my_ptr = (UCHAR *)0xFFFFFFFF;
    my_ptr = (UCHAR *)MEM_ALLOC(AUTH_MEMORY_SECTION_ID,nitems*size,4);
    memset(my_ptr, 0, nitems*size);
    return (void *)&my_ptr[0];
}

/*
* Public Functions
*/

/**
* @brief
*   memory free routine.
*
* @param[in] ptr  - pointer to memory buffer. 
*
* @return
*     
*
* @note
*    This is a dummy function to satisfy application.
*/

VOID  platform_free(void *ptr)
{
 
}





