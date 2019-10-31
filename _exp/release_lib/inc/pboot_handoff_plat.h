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
*       Platform definitions for pboot data handed to sboot.
*
*******************************************************************************/

#ifndef _PBOOT_HANDOFF_PLAT_H
#define _PBOOT_HANDOFF_PLAT_H

/*
** Structures
*/

/**
* @brief
*   Define the data that needs to be handed off from pboot to sboot.
*/
typedef struct 
{
    UINT32 cp0_count;                       /**< CP0 Count value when pboot is finished */
    UINT32 num_images;                      /**< Number of images for authentication */
    BOOL flash_auto_detect_failed;          /**< flash auto-detection failed. */
    
    fam_image_desc_struct image_list[3];    /**< Array of image descriptors - max 3 images for authentication: 
                                                 primary, secondary, and HBR */
                                                    
    UINT32 boot_stage;                      /**< PBOOT stage recorder */

} pboot_handoff_data_struct;

#endif /* _PBOOT_HANDOFF_PLAT_H */



