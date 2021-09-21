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
*   DESCRIPTION:
*    This file contains definitions and declarations for the SPB MUX module for
*    use by external applications.
*
*   NOTES:
*
*******************************************************************************/


#ifndef _SPB_MUX_H
#define _SPB_MUX_H

/*
** Include Files
*/

#include "pmcfw_types.h"

/*
** Function Prototypes
*/

EXTERN VOID spb_mux_init(VOID);
EXTERN UINT32 spb_mux_reg_read(const UINT32 reg_num);
EXTERN VOID spb_mux_reg_write(const UINT32 reg_num, const UINT32 reg_val);
EXTERN VOID spb_mux_reg_read_write(const UINT32 reg_num,
                                   const UINT32 mask,
                                   const UINT32 reg_val);

#endif /* _SPB_MUX_H */


