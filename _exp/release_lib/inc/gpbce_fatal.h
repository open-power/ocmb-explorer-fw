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
*     Platform-specific definitions and declarations for GPBCE FATAL.
*
*******************************************************************************/


#ifndef _GPBCE_FATAL_H
#define _GPBCE_FATAL_H

/*
** Include Files
*/
#include "pmc_hw_base.h"


/*
** Enumerated Types 
*/


/*
** Constants 
*/

#define SPB_GPBC_CHK_IC0_DI_NUM_REGS        5
#define SPB_GPBC_CHK_IC0_DI_REG_OFFSET      0x4C

#define SPB_GPBC_CHK_IC2_DI_NUM_REGS        4
#define SPB_GPBC_CHK_IC2_DI_REG_OFFSET      0x5C

#define SPB_GPBC_SLV_IC0_DI_NUM_REGS        2
#define SPB_GPBC_SLV_IC0_DI_REG_OFFSET      0x3C

#define SPB_GPBC_PERI_DI_INTR_REG_NUM_REGS  4
#define SPB_GPBC_PERI_DI_INTR_REG_OFFSET    0x3C

/*
** Macro Definitions
*/

/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/

EXTERN VOID gpbce_sys_fatal_error_handler(void);
EXTERN VOID gpbce_peri_fatal_error_handler(void);
EXTERN VOID gpbce_fatal_init(void);


#endif /* _GPBCE_FATAL_H */



