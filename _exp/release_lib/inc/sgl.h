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
*   This file contains the public SGL definitions.
*/

#ifndef _SGL_H_
#define _SGL_H_

#include "pmcfw_types.h"



/*
*      SGL Element Control Bits
*/
#define SGL_BITMSK_CTL_EXT_BIT  0x80000000  /* SGL extension bit mask       */
#define SGL_BITOFF_CTL_EXT_BIT  31          /**< SGL extension bit offset     */
#define SGL_BITMSK_CTL_EOB_BIT  0x40000000  /**< SGL end-of-buffer bit mask   */
#define SGL_BITOFF_CTL_EOB_BIT  30          /**< SGL end-of-buffer bit offset */


/*
*      Structure Definitions
*/
/**
* @brief 
*  The Scatter Gather List element definition.
*/
typedef struct
{
    UINT32 addr_lo;   /**<
                      * The lower 32-bits of the 64-bit address of the data segment that this SGL
                      * element points to.
                      */
    UINT32 addr_hi;   /**<
                      * The upper 32-bits of the 64-bit address of the data segment that this SGL
                      * element points to.
                      */
    UINT32 len;       /**< The number of bytes in the data segment that this SGL element points to. */
    UINT32 control;   /**<
                      * A control DWORD that contains bit masks that indicate any altered
                      * processing that must be done with this element:
                      *           SGL_BITMSK_CTL_EXT_BIT  -This element does not point at a data
                      *                                    segment but at an extended SGL that should
                      *                                    be jumped to in order to continue SGL
                      *                                    processing.
                      *           SGL_BITMSK_CTL_EOB_BIT  -This element is tha last element in the
                      *                                    SGL regardless of whether the number of
                      *                                    bytes expected in the entire list has been
                      *                                    reached or not (if they haven't been
                      *                                    reached then this is an error condition
                      *                                    that should throw an interrupt).
                      */
} sgl_element_struct;



#endif  /*  _SGL_H_  */



