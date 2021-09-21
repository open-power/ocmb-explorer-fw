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
*     Platform specific definitions for FLASLOADER module.
*
*   NOTES:
*
*******************************************************************************/



/**
* @addtogroup FLASHLOADER
* @{
* @file
* @brief
*   Flashloader platform specific Handler definitions and declarations.
*
* @note
*/

#ifndef _FLASHLOADER_PLAT_H
#define _FLASHLOADER_PLAT_H

#include "spi_flash_plat.h"
#include "flashloader.h"

/*
** Constants
*/

#define FLASH_PLAT_REQ_RESP_BUF_LENGTH  256
#define FLASH_PLAT_VERSION_LENGTH       128
/* HOST will send 256 Bytes of data */
#define FLASH_LOADER_PAGE_BUF_SIZE      256

/* Number of PUBLIC keys available */
#define FLASH_LOADER_PLAT_NUM_PUBLIC_KEYS           4

#define FLASH_LOADER_PLAT_PUB_KEY_LENGTH_BYTES     512


/*
* Enumerated Types
*/


/*
* Structures and Unions
*/

/*
* Global Variables
*/


/*
* Function Prototypes
*/
EXTERN VOID flashloader_plat_init(flashloader_handler_struct *fl);
EXTERN UINT32 flashloader_plat_actv_image_flag_get(void);
EXTERN VOID *flashloader_plat_fw_version_info_get(void);
EXTERN UINT32 flashloader_plat_fw_image_length_get(UINT32 img_id);
EXTERN flashloader_cmd_enum flashloader_plat_flash_command_get(void);
EXTERN VOID flashloader_plat_send_respnse(BOOL return_code, UINT32 ext_err_code, UINT32 ext_resp_length, UINT8* resp_buf);
EXTERN CHAR flashloader_plat_flash_command_partition_id_get(void);
EXTERN UINT32 flashloader_plat_flash_command_packet_sequence_get(void);
EXTERN UINT32 flashloader_plat_flash_command_address_offset_get(void);
EXTERN UINT32 flashloader_plat_flash_command_read_length_get(void);
EXTERN UINT32 flashloader_plat_flash_read_command_validate(INT8 flash_partition_id, UINT32 flash_partition_addr_offset,UINT32 flash_read_length, UINT32 *flash_read_base);
EXTERN VOID flashloader_plat_flash_read(void *dest_start_addr, void * src_start_addr,UINT32 flash_length);
EXTERN UINT32 flashloader_plat_partition_erase(INT8 partition_id, UINT32 *err_code);
EXTERN UINT32 flashloader_plat_flash_image_size_get(void);
EXTERN UINT32 flashloader_plat_flash_image_packet_sequence_get(void);
EXTERN void * flashloader_plat_flash_image_data_buf_address_get(void);
EXTERN UINT32 flashloader_plat_flash_buffer_write(UINT32 flash_write_index, UINT8 *flash_image_data_buf, UINT32 *err_code);
EXTERN UINT32 flashloader_plat_flash_image_validate(UINT32 *err_code);
EXTERN UINT32 flashloader_plat_flash_image_finalize(INT8 flash_partition_id, UINT32 *err_code);

#endif /* _FLASHLOADER_PLAT_H */



