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
* @brief
*   Contains the base types used by all PMC firmware projects.
*/

#if !defined(PMCFW_TYPES)
#define PMCFW_TYPES
/*
* stddef.h is included here for legacy only. Files should include
* stddef.h directly if they need NULL.
*/
#include <stddef.h>
#include "stdint.h"      /* defines _MAX, _MIN for various types */

#ifdef PMCFW_ALLOW_TYPE_OVERRIDE
#include "pmcfw_types_plat.h"
#endif

#if defined(WIN32)
#include <basetsd.h>
#include <windows.h>
#endif

#if defined(__STDC__)
#include <limits.h>
#endif

typedef void *          PMCFW_HANDLE; /**< Generic handle                 */

#if !defined(CHAR)
#define CHAR    char
#endif

#if !defined(UCHAR)
#define UCHAR   unsigned char
#endif

typedef signed char     INT8;      /**< signed 8-bit quantity            */
#define INT8_BITS       8

typedef unsigned char   UINT8;     /**< unsigned 8-bit quantity          */
#define UINT8_BITS      8

typedef short           INT16;      /**< signed 16-bit quantity           */
#define INT16_BITS      16

typedef unsigned short  UINT16;     /**< unsigned 16-bit quantity         */
#define UINT16_BITS     16

#if !defined(WIN32) /* These OS types already define this type */
#if !defined(INT32)
typedef long            INT32;     /**< signed 32-bit quantity           */
#endif
#endif
#define INT32_BITS      32

#if !defined(WIN32) /* These OS types already define this type */
#if !defined(UINT32)
typedef unsigned long   UINT32;    /**< unsigned 32-bit quantity         */
#endif
#endif
#define UINT32_BITS     32

#if !defined(WIN32) /* These OS types already define this type */
typedef long long       INT64;     /**< signed 64-bit quantity           */
#endif
#define INT64_BITS      64

#if !defined(WIN32) /* These OS types already define this type */
typedef unsigned long long UINT64;    /**< unsigned 64-bit quantity         */
#endif /* !WIN32 */
#define UINT64_BITS     64

#if !defined(BOOL)
#if defined(__cplusplus)
typedef bool             BOOL;      /**< Boolean quantity                 */
#elif defined(WIN32)
typedef int              BOOL;      /**< WIN32 has different boolean defined */
#else
typedef UINT8            BOOL;      /**< Boolean quantity                 */
#endif /* !__cplusplus */
#endif /* !BOOL */

#if !defined(TRUE)
#if defined(cplusplus)
#define TRUE        true
#else
#define TRUE        (1 == 1)
#endif /* !__cplusplus */
#endif /* !TRUE */

#if !defined(FALSE)
#if defined(__cplusplus)
#define FALSE       false
#else
#define FALSE       (1 == 0)
#endif /* !__cplusplus */
#endif /* !FALSE */

#if !defined(PUBLIC)
#define PUBLIC
#endif /* !PUBLIC */

#if !defined(PRIVATE)
#define PRIVATE static
#endif /* !PRIVATE */

/*
#define extern
#include <asm/bitops.h>
*/
#if !defined(EXTERN)
#define EXTERN extern
#endif /* !EXTERN */

#if !defined(min)
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif /* min */

#if !defined(max)
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif /* max */
/* End - Extended Firmware Types */

/* mapping for Linux, WIN32 already defines these macros */
#if !defined(WIN32)
#if !defined(USHORT)
#define USHORT unsigned short
#endif /* USHORT */

#if !defined(UINT)
#define UINT unsigned int
#endif /* UINT */

#if !defined(DWORD)
#define DWORD unsigned long
#endif /* DWORD */

#ifndef STDTYPES_H
#if !defined(HANDLE)
#define HANDLE int
#endif /* HANDLE */
#endif

#if !defined(ULONG)
#define ULONG unsigned long
#endif /* ULONG */

#if !defined(PVOID)
#define PVOID void *
#endif /* PVOID */

#if !defined(SOCKET)
#define SOCKET unsigned int
#endif /* SOCKET */
#endif /* WIN32 */

#if !defined(INVALID_SOCKET)
#define INVALID_SOCKET 0    /* definition different from WIN32 */
#endif /* INVALID_SOCKET */

#if !defined(VOID)
#define VOID void
#endif /* VOID */

#if !defined(INVALID_HANDLE_VALUE)
#define INVALID_HANDLE_VALUE (int)-1    /* definition different from WIN32 */
#endif /* INVALID_HANDLE_VALUE */

#if defined(__ghs__)
#pragma ghs nowarning 826
#endif

/*--------------------------------------------------------------------*/
/* Function Linker Section Definitions                                */
/*--------------------------------------------------------------------*/

// The agains are used to expand the initial call if it needs to be.
// If the agains are not used, then a CONCAT(CONCAT(str,str),str) will
// not work.

#ifdef STDTYPES_H
#define CONCAT_AGAIN(x, y) x ## y
#define CONCAT(x,y) CONCAT_AGAIN(x, y)
#define _PRAGMA_AGAIN(str) _Pragma(#str)
#define _PRAGMA(str) _PRAGMA_AGAIN(str)
#define MAKE_STR_AGAIN(x) #x
#define MAKE_STR(x) MAKE_STR_AGAIN(x)
#define PMCFW_TEXT_SECTION(func) _PRAGMA(CONCAT(ghs section text=,MAKE_STR(CONCAT(.text_,func))))
#define END_PMCFW_TEXT_SECTION() _PRAGMA(ghs section text=default)

#else

#define PMCFW_TEXT_SECTION(func)
#define END_PMCFW_TEXT_SECTION()

#endif

#endif /* PMCFW_TYPES */



