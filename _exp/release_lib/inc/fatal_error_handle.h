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


#ifndef _FATAL_ERROR_HANDLE_H
#define _FATAL_ERROR_HANDLE_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "ocmb_erep.h"

/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Strutures and Unions
*/

/*
** Public Function Prototypes
*/

EXTERN VOID fatal_error_handler(void *int_num);
EXTERN VOID non_fatal_error_handler(void *int_num);
EXTERN VOID fatal_to_host_info_set (ocmb_erep_ext_err_struct *  fatal_info_ptr,
                                    ocmb_erep_type_enum         err_type,
                                    UINT32                      sub_category,
                                    UINT32                      reg_offset,
                                    UINT32                      reg_val);
EXTERN VOID fatal_to_host_report_send (ocmb_erep_ext_err_struct to_host_fatal_info);
EXTERN VOID fatal_error_check_and_report (UINT32 reg_val, 
                                          UINT32 mask, 
                                          UINT32 reg_offset, 
                                          UINT32 sub_reg_num,
                                          const char * prefix_string,
                                          ocmb_erep_type_enum err_typ,
                                          ocmb_erep_ext_err_struct * to_host_fatal_info_ptr);


#endif /* _FATAL_ERROR_HANDLE_H */



