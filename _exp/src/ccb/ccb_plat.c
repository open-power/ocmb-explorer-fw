/********************************************************************************
* MICROCHIP PM8596 EXPLORER FIRMWARE
*
* Copyright (c) 2018, 2019, 2020 Microchip Technology Inc. All rights reserved.
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
* @addtogroup CCB
* @{
* @file
* @brief
*   CCB platform implementation.
*
* @note
*/

/*
* Include Files
*/

#include "ccb_api.h"
#include "char_io.h"
#include "crash_dump.h"
#include "crash_dump_plat.h"

/*
* Local Enumerated Types
*/

/*
* Local Macro Definitions
*/

/*
* Local Constants
*/

/*
* Local Structures and Unions
*/

/*
* Private Functions
*/


/*
* Public Functions
*/

/**
* @brief
*   This function copies the runtime CCB to the crash dump.
*
* @return
*   None
*
* @note
*   None.
*
*/
void ccb_plat_runtime_crash_dump(void)
{
    UINT32 size;
    void *ccb_ctrl_ptr;
    void *addr_ptr;
    UINT8 *cd_ram_ptr;

    /* Get CCB info. */
    ccb_ctrl_ptr = char_io_ccb_ctrl_get(CHAR_IO_CHANNEL_ID_RUNTIME);
    size = ccb_info_get(ccb_ctrl_ptr, &addr_ptr);

    /* Get write pointer to the crash dump holding buffer in SRAM. */
    cd_ram_ptr = crash_dump_plat_ram_buf_wr_ptr_get();

    /* Get the unwrapped contents on the CCB.*/
    ccb_get(ccb_ctrl_ptr, (CHAR *)cd_ram_ptr, size);

    /* Update the write point to the crash dump holding buffer. */
    crash_dump_plat_ram_buf_wr_ptr_update(size);
}

/* End of File */

/** @} end addtogroup */


