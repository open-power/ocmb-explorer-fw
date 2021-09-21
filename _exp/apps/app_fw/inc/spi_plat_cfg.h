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
*     Platform specific definitions for SPI module.
*
*   NOTES:
*
*******************************************************************************/



#ifndef _SPI_PLAT_CFG_H
#define _SPI_PLAT_CFG_H

/* 
** Set to 1 to disable logging from SPI, or 0 to enable.
*/
#define SPI_LOG_DISABLE         0

/* 
** Set to 1 to disable SPI ECC, or 0 to enable.
*/
#define SPI_ECC_DISABLE         0

/*
** Set to 1 to configure L2B window for port 1 (MRAM)
*/
#define SPI_PORT1_L2B_CONFIG    0

#endif /* _SPI_PLAT_CFG_H */



