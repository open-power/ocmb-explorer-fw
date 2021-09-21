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
* @defgroup FAM: Firmware Authentication Module
* @ingroup 
* @brief 
*     This module contains the public Firmware Authentication
*     Module (FAM) definitions. @{
* @file 
* @brief 
*     User applications should include this file in order to use
*     the FAM services.
* 
* @note 
*/


/*---- Compile Options -----------------------------------------------------*/
#ifndef _FAM_H
#define _FAM_H


/*---- Common Include Files ------------------------------------------------*/

#include "pmcfw_types.h"

/*---- Common Enumerations -------------------------------------------------*/
/**
* @brief
*    status values for image authentication
*/

typedef enum fam_image_status
{
    NOT_TESTED = 0,
    SUCCESS = 1,
    FAIL_KEY_REVOKED = 2,
    FAIL_INVALID_HASH = 3,
    FAIL_INVALID_KEY_IDX = 4,
    FAIL_INVALID_LENGTH = 5,
    FAIL_INVALID_HASH_ALGO = 6,
    FAIL_CONTEXT_DMA = 7,
    FAIL_IMG_AUTH_DMA = 8,
    FAIL_IMG_DMA = 9,
    FAIL_INVALID_SIGNATURE = 10,
    FAIL_SHA_ERR = 11,
    FAIL_PKA_ERR = 12,
    FAIL_INVALID_MEM_TYPE_ERR = 13
} fam_image_status;
    
   
/*---- Common Typedefs -----------------------------------------------------*/

/*---- Constant and Type Declarations --------------------------------------*/
#define HASH_ALGO_SHA_1    0
#define HASH_ALGO_SHA_224  1
#define HASH_ALGO_SHA_256  2
#define HASH_ALGO_SHA_384  3
#define HASH_ALGO_SHA_512  4
#define HASH_ALGO_SHA3_512 5

#define FAM_IMG_MAX_FINGERPRINT_SIZE_BYTES      128    
#define FAM_IMG_MAX_SIGNATURE_SIZE_BYTES        512

#define FAM_FW_PART_IMG_AUTH_SIZE_BYTES         (sizeof(fam_img_auth_blk_struct))
#define FAM_FW_PART_IMG_CTEXT_SIZE_BYTES        (sizeof(fam_img_ctext_blk_struct))

#define FAM_FW_PART_IMG_OFFSET                  (FAM_FW_PART_IMG_AUTH_SIZE_BYTES + FAM_FW_PART_IMG_CTEXT_SIZE_BYTES)

#define FAM_PUB_KEY_LENGTH_BYTES                512

#define FAM_PUB_KEY_EXPONENT                    65537

/**
* @brief
*   This structure defines a list of firmware images locations
*   and sizes.
*/
typedef struct fam_image_desc_struct
{
    UINT8* image_addr;                /**< location for the image */
    UINT8  image_id;                /**< Image ID 'A' or 'B' */
    fam_image_status status;        /**< image status */
    UINT32  extend_err_code;        /**< extended error code */         
} fam_image_desc_struct;

/**
* @brief
*   This structure defines the firmware partition for the Image
*   Authentication block. See the SRCw (Wildfire) Firmware
*   Flash Layout Description document for more details.
*/
typedef struct fam_img_auth_blk_struct
{
    /* 640 Bytes */
    UINT8 image_fingerprint[FAM_IMG_MAX_FINGERPRINT_SIZE_BYTES];
    UINT8 image_signature[FAM_IMG_MAX_SIGNATURE_SIZE_BYTES];
} fam_img_auth_blk_struct;

/**
* @brief
*   This structure defines the first portion of firmware partition for the Image
*   Context block. See the SRCw (Wildfire) Firmware Flash Layout
*   Description document for more details.
*   The context is split across two structs, for optimized cases where the
*   extended section of the context data is not required.
*/
typedef struct fam_img_ctext_blk_struct
{
    UINT32 fw_length;
    UINT32 public_key_selector;
    UINT32 hash_algorithm;
    UINT32 image_comp_flag;
    UINT32 hash_src_comp_flag;
    UINT32 raw_elf_image_flag;
    UINT32 loader_ver;
    UINT8 unused_0[100];
    UINT8 next_image_public_key_1[512];
    UINT8 next_image_public_key_2[512];
    UINT8 unused_1[252];
    UINT32 crc;
} fam_img_ctext_blk_struct;


/*---- Common Macro Definition ---------------------------------------------*/

/*---- Common Log Function Prototypes --------------------------------------*/

/*---- Public Variable Declarations ----------------------------------------*/



/*---- Inline Function Declarations ----------------------------------------*/

/*---- Function Declarations -----------------------------------------------*/

EXTERN void fam_dump_status(const fam_image_desc_struct* const image_list, const UINT32 num_images);
EXTERN fam_image_desc_struct * const fam_authenticate_image(fam_image_desc_struct * const image_list,
                                                            const UINT32 num_images,
                                                            UINT32 * const pub_key_array,
                                                            const UINT32 num_pub_keys,
                                                            const BOOL secure_boot,
                                                            const UINT32 max_image_len);
EXTERN UINT32 fam_image_length_get(fam_image_desc_struct * const image_list, UINT32 image_id);

#endif /* _FAM_H */

/** @} end addtogroup */


