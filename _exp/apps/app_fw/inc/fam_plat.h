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
* 
* RSA/SHA-256 signature verification program
*                                                                               
* Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
*                                                                               
* SPDX-License-Identifier: Apache-2.0
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
*                                                                               
* This file is part of mbed TLS (https://tls.mbed.org)
*                                                                               
* FIPS-180-2 compliant SHA-384/512 implementation
*                                                                               
* Copyright (C) 2006-2015, ARM Limited, All Rights Reserved SPDX-License-
* Identifier: Apache-2.0
*                                                                               
* Licensed under the Apache License, Version 2.0 (the "License"); you may not 
* use this file except in compliance with the License. You may obtain a copy of 
* the License at
*                                                                               
* http://www.apache.org/licenses/LICENSE-2.0 
*                                                                               
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT 
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the 
* License for the specific language governing permissions and limitations under 
* the License.
* 
* This file is part of mbed TLS (https://tls.mbed.org)
*                                                                               
* The SHA-512 Secure Hash Standard was published by NIST in 2002.
* 
* http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
* 
********************************************************************************/
/********************************************************************************
*   DESCRIPTION :
*     Higher level definitions for the FAM. Each platform has
*     their own copy of fam_plat.h  This header file allows platform
*     specific defines WITHOUT the name of platform being part of each
*     define. This makes module re-use much easier. These definitions
*     are higher level than pmc_hw.h and pmc_plat.h is only include
*     by modules that need it.
*
*
*******************************************************************************/


#ifndef _FAM_PLAT_H
#define _FAM_PLAT_H

/*
** Include Files
*/
#include "pmcfw_types.h"
#include "pmcfw_common.h"
#include "top.h"
#include "pmc_plat.h"
#if (CRYPTO_ROUTINE_SOURCE == USE_CRYPTO_LIB)
#include "mbedtls/rsa.h"
#include "mbedtls/oid.h"
#include "mbedtls/sha512.h"
#include "mbedtls/platform.h"
#elif (CRYPTO_ROUTINE_SOURCE == USE_BOOTROM)

/**
 * \file bignum.h
 *
 * \brief Multi-precision integer library
 */
/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

typedef unsigned int  mbedtls_mpi_uint;

/**
 * \file rsa.h
 *
 * \brief This file provides an API for the RSA public-key cryptosystem.
 *
 * The RSA public-key cryptosystem is defined in <em>Public-Key
 * Cryptography Standards (PKCS) #1 v1.5: RSA Encryption</em>
 * and <em>Public-Key Cryptography Standards (PKCS) #1 v2.1:
 * RSA Cryptography Specifications</em>.
 *
 */
/*
 *  Copyright (C) 2006-2018, Arm Limited (or its affiliates), All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of Mbed TLS (https://tls.mbed.org)
 */


/*
 * RSA constants
 */
#define MBEDTLS_RSA_PUBLIC      0 /**< Request private key operation. */
#define MBEDTLS_RSA_PRIVATE     1 /**< Request public key operation. */

#define MBEDTLS_RSA_PKCS_V15    0 /**< Use PKCS#1 v1.5 encoding. */
#define MBEDTLS_RSA_PKCS_V21    1 /**< Use PKCS#1 v2.1 encoding. */

#define MBEDTLS_RSA_SIGN        1 /**< Identifier for RSA signature operations. */
#define MBEDTLS_RSA_CRYPT       2 /**< Identifier for RSA encryption and decryption operations. */

/**
 * \brief          MPI structure
 */
typedef struct
{
    int s;              /*!<  integer sign      */
    size_t n;           /*!<  total # of limbs  */
    mbedtls_mpi_uint *p;          /*!<  pointer to limbs  */
}
mbedtls_mpi;

/**
 * \brief   The RSA context structure.
 *
 * \note    Direct manipulation of the members of this structure
 *          is deprecated. All manipulation should instead be done through
 *          the public interface functions.
 */
typedef struct
{
    int ver;                    /*!<  Always 0.*/
    size_t len;                 /*!<  The size of \p N in Bytes. */

    mbedtls_mpi N;              /*!<  The public modulus. */
    mbedtls_mpi E;              /*!<  The public exponent. */

    mbedtls_mpi D;              /*!<  The private exponent. */
    mbedtls_mpi P;              /*!<  The first prime factor. */
    mbedtls_mpi Q;              /*!<  The second prime factor. */

    mbedtls_mpi DP;             /*!<  <code>D % (P - 1)</code>. */
    mbedtls_mpi DQ;             /*!<  <code>D % (Q - 1)</code>. */
    mbedtls_mpi QP;             /*!<  <code>1 / (Q % P)</code>. */

    mbedtls_mpi RN;             /*!<  cached <code>R^2 mod N</code>. */

    mbedtls_mpi RP;             /*!<  cached <code>R^2 mod P</code>. */
    mbedtls_mpi RQ;             /*!<  cached <code>R^2 mod Q</code>. */

    mbedtls_mpi Vi;             /*!<  The cached blinding value. */
    mbedtls_mpi Vf;             /*!<  The cached un-blinding value. */

    int padding;                /*!< Selects padding mode:
                                     #MBEDTLS_RSA_PKCS_V15 for 1.5 padding and
                                     #MBEDTLS_RSA_PKCS_V21 for OAEP or PSS. */
    int hash_id;                /*!< Hash identifier of mbedtls_md_type_t type,
                                     as specified in md.h for use in the MGF
                                     mask generating function used in the
                                     EME-OAEP and EMSA-PSS encodings. */
}mbedtls_rsa_context;

#else
#error "Please set appropriate value for BOOTROM_CRYPTO_ROUTINE_USE"
#endif



/*
** Enumerated Types
*/

/*
** Constants
*/

/*
** Macro Definitions
*/

/****************************************************************************
*
* FUNCTION: fam_public_key_valid
* __________________________________________________________________________
*
* DESCRIPTION:
*   Determines if the public key is valid
*
* INPUTS:
*   idx - public key index
*
* OUTPUTS:
*   None.
*
* RETURNS:
*   TRUE - public key is valid
*   FALSE - public key is invalid
*
* NOTES:
*
*****************************************************************************/
PRIVATE inline BOOL fam_public_key_valid(const UINT32 idx)
{
    /* Explorer EFUSE states which key is valid. 
    ** There are total 4 keys. EFUSE basically says which index to use.
    */
    return top_public_key_valid(idx);
}



/*
** Structures and Unions
*/

/*
** Global variables
*/

/*
** Function Prototypes
*/
EXTERN VOID (*mbedtls_platform_set_calloc_free_ptr) (VOID * (*calloc_func)( size_t, size_t ), VOID (*free_func)( VOID * ));
EXTERN VOID (*mbedtls_rsa_init_ptr) (mbedtls_rsa_context *ctx, INT32 padding, INT32 hash_id );
EXTERN INT32  (*mbedtls_rsa_public_ptr) (mbedtls_rsa_context *ctx, const UINT8 *input, UINT8 *output );
EXTERN VOID (*mbedtls_sha512_ptr) (const UINT8 *input, size_t ilen, UINT8 output[64], INT32 is384 );
EXTERN VOID fam_plat_init(VOID);
EXTERN const UINT8* fam_plat_pka_pubkey_get(UINT32 key_idx);



#endif /* _FAM_PLAT_H */


