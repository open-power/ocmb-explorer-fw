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
* @defgroup BUSIO BUSIO: Register read-modify-write functions
* @brief
*   Hardware access devlopers should map block specific macros for
*   register address/read/write access to the appropriate set of
*   macros defined here.
*
*   The macros defined here work for:
*                  8, 16, 32-bit device registers; using
*                  8, 16, 32-bit data-bus accesses; on
*                  8, 16, 32, 64-bit address boundaries.
*
*   These macros work on both little endian and big endian
*                processors and busses.
*
* @{
* @file
* @brief
*   Public API for BUSIO
*
* @note
*   Generally compilers will in-line these functions.
*/
#ifndef _BUSIO_H
#define _BUSIO_H
/*
* Include files
*/
#include "pmcfw_types.h"

/*
* Constants
*/
/*
* Macro Definitions
*/
/*
* Device access drivers can use these symbols to provide conditional
* compilation support for selecting the set of BUSIO access macros to
* use for data and address bus combinations
*/
#define BUSIO_DATA_WIDTH_8          8
#define BUSIO_DATA_WIDTH_16         16
#define BUSIO_DATA_WIDTH_32         32
#define BUSIO_ADDR_WIDTH_8          8
#define BUSIO_ADDR_WIDTH_16         16
#define BUSIO_ADDR_WIDTH_32         32
#define BUSIO_ADDR_WIDTH_64         64


/**
* @brief
*   Return the Absolute address of a register
*
*   BUSIO_ADDRz where z is 8, 16, 32 or 64-bit aligned address (addr-bus-width)
*
* @param[in] base        - base address of TSB
* @param[in] reg_num     - register number as it appears in the hardware data sheet
*
* @return
*   Absolute address of the register.
*
* @note
*   none
*
* @hideinitializer
*/
#define BUSIO_ADDR8(base, reg_num)                                             \
                     ((UINT8*)(base) + ((reg_num) << 0))
#define BUSIO_ADDR16(base, reg_num)                                            \
                     ((UINT8*)(base) + ((reg_num) << 1))
#define BUSIO_ADDR32(base, reg_num)                                            \
                     ((UINT8*)(base) + ((reg_num) << 2))
#define BUSIO_ADDR64(base, reg_num)                                            \
                     ((UINT8*)(base) + ((reg_num) << 3))


/**
* @brief
*     Basic hardware access: read a single device register.
*
*     BUSIO_READx_y_z
*        x -- 8, 16 or 32-bit register            (register-width)
*        y -- 8, 16 or 32-bit read-operation      (data-bus-width)
*        z -- 8, 16, 32 or 64-bit aligned address (addr-bus-width)
*
* @param[in] base        - base address of TSB
* @param[in] reg_num     - register number as it appears in the hardware data sheet
*
* @return
*   value read from the addressed register location
*
* @note
*   none
*
* @hideinitializer
*/
#define BUSIO_READ8_8_8(base, reg_num)                                         \
            ((UINT8)(*(volatile UINT8*)(BUSIO_ADDR8((base), (reg_num)))))
#define BUSIO_READ8_8_16(base, reg_num)                                        \
            ((UINT8)(*(volatile UINT8*)(BUSIO_ADDR16((base), (reg_num)))))
#define BUSIO_READ8_8_32(base, reg_num)                                        \
            ((UINT8)(*(volatile UINT8*)(BUSIO_ADDR32((base), (reg_num)))))
#define BUSIO_READ8_8_64(base, reg_num)                                        \
            ((UINT8)(*(volatile UINT8*)(BUSIO_ADDR64((base), (reg_num)))))

#define BUSIO_READ8_16_8(base, reg_num)                                        \
            ((UINT8)(*(volatile UINT16*)(BUSIO_ADDR8((base), (reg_num)))))
#define BUSIO_READ8_16_16(base, reg_num)                                       \
            ((UINT8)(*(volatile UINT16*)(BUSIO_ADDR16((base), (reg_num)))))
#define BUSIO_READ8_16_32(base, reg_num)                                       \
            ((UINT8)(*(volatile UINT16*)(BUSIO_ADDR32((base), (reg_num)))))
#define BUSIO_READ8_16_64(base, reg_num)                                       \
            ((UINT8)(*(volatile UINT16*)(BUSIO_ADDR64((base), (reg_num)))))

#define BUSIO_READ8_32_8(base, reg_num)                                        \
            ((UINT8)(*(volatile UINT32*)(BUSIO_ADDR8((base), (reg_num)))))
#define BUSIO_READ8_32_16(base, reg_num)                                       \
            ((UINT8)(*(volatile UINT32*)(BUSIO_ADDR16((base), (reg_num)))))
#define BUSIO_READ8_32_32(base, reg_num)                                       \
            ((UINT8)(*(volatile UINT32*)(BUSIO_ADDR32((base), (reg_num)))))
#define BUSIO_READ8_32_64(base, reg_num)                                       \
            ((UINT8)(*(volatile UINT32*)(BUSIO_ADDR64((base), (reg_num)))))

#define BUSIO_READ16_16_8(base, reg_num)                                       \
            ((UINT16)(*(volatile UINT16*)(BUSIO_ADDR8((base), (reg_num)))))
#define BUSIO_READ16_16_16(base, reg_num)                                      \
            ((UINT16)(*(volatile UINT16*)(BUSIO_ADDR16((base), (reg_num)))))
#define BUSIO_READ16_16_32(base, reg_num)                                      \
            ((UINT16)(*(volatile UINT16*)(BUSIO_ADDR32((base), (reg_num)))))
#define BUSIO_READ16_16_64(base, reg_num)                                      \
            ((UINT16)(*(volatile UINT16*)(BUSIO_ADDR64((base), (reg_num)))))

#define BUSIO_READ16_32_8(base, reg_num)                                       \
            ((UINT16)(*(volatile UINT32*)(BUSIO_ADDR8((base), (reg_num)))))
#define BUSIO_READ16_32_16(base, reg_num)                                      \
            ((UINT16)(*(volatile UINT32*)(BUSIO_ADDR16((base), (reg_num)))))
#define BUSIO_READ16_32_32(base, reg_num)                                      \
            ((UINT16)(*(volatile UINT32*)(BUSIO_ADDR32((base), (reg_num)))))
#define BUSIO_READ16_32_64(base, reg_num)                                      \
            ((UINT16)(*(volatile UINT32*)(BUSIO_ADDR64((base), (reg_num)))))

#define BUSIO_READ32_32_8(base, reg_num)                                       \
            ((UINT32)(*(volatile UINT32*)(BUSIO_ADDR8((base), (reg_num)))))
#define BUSIO_READ32_32_16(base, reg_num)                                      \
            ((UINT32)(*(volatile UINT32*)(BUSIO_ADDR16((base), (reg_num)))))
#define BUSIO_READ32_32_32(base, reg_num)                                      \
            ((UINT32)(*(volatile UINT32*)(BUSIO_ADDR32((base), (reg_num)))))
#define BUSIO_READ32_32_64(base, reg_num)                                      \
            ((UINT32)(*(volatile UINT32*)(BUSIO_ADDR64((base), (reg_num)))))

/**
* @brief
*   Basic hardware access: write a single device register.
*
*   BUSIO_READx_y_z
*       x -- 8, 16 or 32-bit register            (register-width)
*       y -- 8, 16 or 32-bit read-operation      (data-bus-width)
*       z -- 8, 16, 32 or 64-bit aligned address (addr-bus-width)
*
* @param[in] base        - base address
* @param[in] reg_num     - register number as it appears in the hardware data sheet
* @param[in] data        - data to be written
*
* @return
*   None
*
* @hideinitializer
*/
#define BUSIO_WRITE8_8_8(base, reg_num, data)                                  \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT8*)(BUSIO_ADDR8((base), (reg_num))) = xreg;     \
            } while(0)
#define BUSIO_WRITE8_8_16(base, reg_num, data)                                 \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT8*)(BUSIO_ADDR16((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE8_8_32(base, reg_num, data)                                 \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT8*)(BUSIO_ADDR32((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE8_8_64(base, reg_num, data)                                 \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT8*)(BUSIO_ADDR64((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE8_16_8(base, reg_num, data)                                 \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR8((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE8_16_16(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR16((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE8_16_32(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR32((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE8_16_64(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR64((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE8_32_8(base, reg_num, data)                                 \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR8((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE8_32_16(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR16((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE8_32_32(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR32((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE8_32_64(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT8 xreg = (data);                                           \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR64((base), (reg_num))) = xreg;   \
            } while(0)

#define BUSIO_WRITE16_16_8(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR8((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE16_16_16(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR16((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE16_16_32(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR32((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE16_16_64(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT16*)(BUSIO_ADDR64((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE16_32_8(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR8((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE16_32_16(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR16((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE16_32_32(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR32((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE16_32_64(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT16 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR64((base), (reg_num))) = xreg;   \
            } while(0)

#define BUSIO_WRITE32_32_8(base, reg_num, data)                                \
            do                                                                 \
            {                                                                  \
                UINT32 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR8((base), (reg_num))) = xreg;    \
            } while(0)
#define BUSIO_WRITE32_32_16(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT32 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR16((base), (reg_num))) = xreg;   \
            } while(0)
#define BUSIO_WRITE32_32_32(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT32 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR32((base), (reg_num))) = xreg;   \
            } while(0)

#define BUSIO_WRITE32_32_64(base, reg_num, data)                               \
            do                                                                 \
            {                                                                  \
                UINT32 xreg = (data);                                          \
                                                                               \
                *(volatile UINT32*)(BUSIO_ADDR64((base), (reg_num))) = xreg;   \
            } while(0)


/*
* Function Prototypes
*/

/**
* @brief
*   These functions perform Read-Modify-Write on an
*   8/16/32-bit register given the base address of a device/tsb
*   the register number, a mask of bits to write and
*   the value to write
*
*   Note: These are split between multiple source files since
*   generally only one set of the functions will be used
*   per device, and it allows the linker to exclude
*   the unused functions.
*
* @param[in] base       - absolute address of device/tsb
* @param[in] reg_num    - register number as it appears in
*                               the hardware data sheet
* @param[in] mask       - mask of bits to write
* @param[in] value      - data to be written
* @return
*    None
*
* @hideinitializer
*/

/*
** Function Prototypes and Pointers to Functions in RAM
**
** To accommodate PIC code executing in SPI flash and non-PIC code executing in
** RAM, the functions in RAM are accessed through pointers. The changes that were made:
**
**      - original function name:     $type func_name($type, $type)
**        changed with prepended '_': $type _func_name($type, $type)
**      - define a typedef for the function pointer: typedef $type (_func_name_fn_ptr)($type, $type)
**      - public function pointers initialized in source code files:
**        PUBLIC _func_name_fn_ptr (*func_name_ptr) =  _func_name;
**      - private function pointers intitialized in source code files:
**        PRIVATE _func_name_fn_ptr (*func_name_ptr) = _func_name;
**      - for public functions new define for original function name in header file:
**        #define func_name (*func_name_ptr)
**      - for private functions new define for original function name in source code file:
**        #define func_name (*func_name_ptr)
*/
/** 32-bit register accesses */
typedef void (*busio_field_write32_32_8_fn_ptr_type)(const void *base, UINT32 reg_num, UINT32 mask, UINT32 value);
EXTERN busio_field_write32_32_8_fn_ptr_type busio_field_write32_32_8_fn_ptr;
#define busio_field_write32_32_8 (*busio_field_write32_32_8_fn_ptr)


/** 8-bit register accesses */
EXTERN void busio_field_write8_8_8   (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_8_16  (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_8_32  (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_8_64  (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);

EXTERN void busio_field_write8_16_8  (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_16_16 (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_16_32 (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_16_64 (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);

EXTERN void busio_field_write8_32_8  (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_32_16 (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_32_32 (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);
EXTERN void busio_field_write8_32_64 (const void *base, UINT32 reg_num, UINT8  mask, UINT8  value);

/** 16-bit register accesses */
EXTERN void busio_field_write16_16_8 (const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);
EXTERN void busio_field_write16_16_16(const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);
EXTERN void busio_field_write16_16_32(const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);
EXTERN void busio_field_write16_16_64(const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);

EXTERN void busio_field_write16_32_8 (const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);
EXTERN void busio_field_write16_32_16(const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);
EXTERN void busio_field_write16_32_32(const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);
EXTERN void busio_field_write16_32_64(const void *base, UINT32 reg_num, UINT16 mask, UINT16 value);

/** 32-bit register accesses */
EXTERN void busio_field_write32_32_16(const void *base, UINT32 reg_num, UINT32 mask, UINT32 value);
EXTERN void busio_field_write32_32_32(const void *base, UINT32 reg_num, UINT32 mask, UINT32 value);
EXTERN void busio_field_write32_32_64(const void *base, UINT32 reg_num, UINT32 mask, UINT32 value);

EXTERN VOID busio_ram_code_ptr_adjust(UINT32 offset);

#endif    /* _BUSIO_H */

/** @} end addtogroup */


