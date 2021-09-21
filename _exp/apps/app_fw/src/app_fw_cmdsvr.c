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
* @addtogroup SBOOT
* @{
* @file
* @brief
*   sboot command server command function definitions.
*
* @note
*
*/

/*
* Include Files
*/

#include "cmdsvr_plat_cfg.h"
#include "pmcfw_types.h"

#if (CMDSVR_REG_COMMANDS == 1)
#include "pmcfw_common.h"
#include "cmdsvr_func_api.h"
#include "bc_printf.h"


/* 
* Local Constants 
*/

/* 
* Forward References
*/

PRIVATE PMCFW_ERROR sboot_cmd_sboot_lnkstat(CHAR **args,
                                            UINT8 num_args);


/* 
** Private Variables
*/

/* list of command server commands registered by sboot */
#pragma ghs startdata
PRIVATE cmdsvr_cmd_def_struct sboot_cmd_set[] = {
    {
        "sboot_lnkstat",
        "Show the link status of the USP",
        sboot_cmd_sboot_lnkstat,
        NULL,
        FALSE
    }
};
#pragma ghs enddata

/* 
** Private Functions
*/

/**
* @brief
*   Show the link status of the USP.
*
* @return
*   PMC_SUCCESS: success
* @return
*   Others:      failure
*/
PRIVATE PMCFW_ERROR sboot_cmd_sboot_lnkstat(CHAR **args,
                                            UINT8 num_args)
{


    //bc_printf("USP LTSSM state: %s\n", state_str);

    return PMC_SUCCESS;
}
#endif /* (CMDSVR_REG_COMMANDS == 1) */


/* 
** Public Functions
*/

/**
* @brief
*   Register the sboot command server commands.
*
* @return
*   none
*
*/
PUBLIC VOID sboot_cmdsvr_register(VOID)
{
#if (CMDSVR_REG_COMMANDS == 1)
    PMCFW_ERROR rv;

    rv = cmdsvr_func_list_register(sboot_cmd_set, PMC_ARRAY_SIZE(sboot_cmd_set));
    PMCFW_ASSERT(rv == PMC_SUCCESS, rv);
#endif
}



