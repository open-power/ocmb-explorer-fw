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
*
* @addtogroup Crash dump plat
* @{
* @file
* @brief
*   Implementation of the crash dump platform specific functions
*
* @note
*   The crash dump platform specific module will handle the writing and reading of the crash dump
*   to and from SPI flash. The crash dump is organized into two sections a 4K header block and the
*   remaining space as a data block.
*
*   The 4K header block will be filled with a series of crash_dump_header structures which contain information
*   corresponding to crash dump data section that is stored in the data block.
*
*   This modules supports non-contiguous SPI memory for saving crash dumps. Additional
*   SPI flash memory blocks can be registered with this module. Once registered the
*   crash dump module will handle writing to SPI memory in 4K chunks by using 4K
*   RAM buffers to temporarily hold the data. Until a 4K block of crash dump
*   data is collected.
*/

/*
* Include Files
*/
#include <string.h>
#include "crash_dump.h"
#include "ech.h"
#include "spi_flash_plat.h"
#include "spi_api.h"
#include "spi_plat.h"
#include "mem_api.h"
#include "pmc_plat.h"
#include "spi_flash_api.h"
#include "fam.h"
#include "fw_version_info.h"
#include "flash_partition_info.h"

/*
* Local Constants
*/

#define CRASH_DUMP_HEADER_SECTION_SIZE (4 * 1024)
#define CRASH_DUMP_MAX_WRITE_SIZE (4 * 1024)

/*
* Local Macro Definitions
*/

/*
* Local Structures and Unions
*/
typedef struct crash_dump_spi_section crash_dump_spi_section;

/**
* @brief
*   Structure for an entity in a linked list of SPI flash sections to be used
*   for the crash dump
*/
struct crash_dump_spi_section
{
    UINT32 start_addr;         /**< Starting SPI flash address */
    UINT32 size;               /**< Size of SPI section in bytes */
    crash_dump_spi_section* next_section_ptr; /**< Pointer to the next entry in the linked list */
};

/**
* @brief
*   Structure for the RAM buffers required by the crash dump module to buffer 4K page
*   before writing to SPI
*/
typedef struct crash_dump_ram_buffer
{
    UINT8 *write_ptr; /**< Current write pointer into the RAM buffer */
    UINT8 *start_ptr; /**< Pointer to the start of the RAM buffer */
    UINT8 *end_ptr;   /**< Pointer to the end of the RAM buffer */
} crash_dump_ram_buffer;

/**
* @brief
*   Structure for handling access into the crash_dump_spi_section linked list
*/
typedef struct crash_dump_spi_section_access_handler
{
    crash_dump_spi_section* current_spi_section; /**< Pointer to the SPI section currently being accessed */
    UINT32                  current_spi_address; /**< SPI address within the section that is currently being accessed */
} crash_dump_spi_section_access_handler;

/*
* Local Variables
*/

/* Local variables for remembering the start of the crash dump and header sections in SPI */
PRIVATE crash_dump_spi_section* head_crash_dump_section = NULL;
PRIVATE crash_dump_spi_section* tail_crash_dump_section = NULL;
PRIVATE UINT32 spi_flash_header_address = 0;

/* SPI write handler to keep track of current write position in SPI flash */
PRIVATE crash_dump_spi_section_access_handler spi_write_handler;

/* Two RAM buffers for crash dump data and header */
PRIVATE crash_dump_ram_buffer data_buffer;
PRIVATE crash_dump_ram_buffer header_buffer;

/*
** Forward Reference Function Prototypes and Pointers to Functions in RAM
**
** To accommodate PIC code executing in SPI flash and non-PIC code executing in
** RAM, the functions in RAM are accessed through pointers. The changes that were made:
**
**      - original function name:     $type func_name($type, $type)
**        changed with prepended '_': $type _func_name($type, $type)
**      - define a typedef for the function pointer: typedef $type (_func_name_fn_ptr)($type, $type)
**      - public function pointers initialized in source code files:
**        PUBLIC _func_name_fn_ptr (*func_name_ptr) =  _func_name;
**      - private function pointers initialized in source code files:
**        PRIVATE _func_name_fn_ptr (*func_name_ptr) = _func_name;
**      - for public functions new define for original function name in header file:
**        #define func_name (*func_name_ptr)
**      - for private functions new define for original function name in source code file:
**        #define func_name (*func_name_ptr)
*/

PRIVATE PMCFW_ERROR _crash_dump_write(UINT8* data_ptr, UINT32 data_size, UINT32 spi_dest_addr);
typedef PMCFW_ERROR (*crash_dump_write_fn_ptr_type)(UINT8* data_ptr, UINT32 data_size, UINT32 spi_dest_addr);
PRIVATE crash_dump_write_fn_ptr_type crash_dump_write_fn_ptr = _crash_dump_write;
#define crash_dump_spi_write (*crash_dump_write_fn_ptr)

/*
* Public Variables
*/

/*
* Private Functions
*/

/**
* @brief
*   Get the address in SPI flash where the crash dump data should be stored
*
* @return
*   Address in SPI flash where the crash dump is saved.
*
*/
PRIVATE UINT32 crash_dump_plat_crash_dump_spi_addr_get(void)
{
    /* determine the active image to get the crash dump SPI start address */
    if (flash_partition_boot_partition_id_get() == 'A')
    {
        /* firmware image A is the active image */

        /* Make sure the address for the training data is 4K aligned */
        PMCFW_ASSERT(((SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_ADDR)& 0xFFF) == 0, CRASH_DUMP_ERR_CRASH_DUMP_A_ADDR_ALIGN);

        /* get reference to image A SPI flash log*/
        return (UINT32)(SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_ADDR);
    }
    else
    {
        /* firmware image B is the active image */

        /* Make sure the address for the training data is 4K aligned */
        PMCFW_ASSERT(((SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_ADDR)& 0xFFF) == 0, CRASH_DUMP_ERR_CRASH_DUMP_B_ADDR_ALIGN);

        /* get reference to image B SPI flash log*/
        return (UINT32)(SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_ADDR);
    }
}

/**
* @brief
*   Get the size of SPI flash where the crash dump data should be stored
*
* @return
*   Size of SPI flash where the crash dump is saved.
*
*/
PRIVATE UINT32 crash_dump_plat_crash_dump_spi_size_get(void)
{
    /* determine the active image to get the crash dump size */
    if (flash_partition_boot_partition_id_get() == 'A')
    {
        /* firmware image A is the active image */
        return (UINT32)(SPI_FLASH_FW_IMG_A_CFG_LOG_CRASH_DUMP_SIZE);
    }
    else
    {
        /* get reference to image B SPI flash log*/
        return (UINT32)(SPI_FLASH_FW_IMG_B_CFG_LOG_CRASH_DUMP_SIZE);
    }
}

/**
 * @brief
 *   Add a SPI section to be used for saving the crash dump.
 *
 * @param[in] start_address - Start SPI address
 * @param[in] size - Size of SPI section available
 *
 * @return
 *   none
 */
PRIVATE VOID crash_dump_plat_add_spi_section(UINT32 start_address, UINT32 size)
{
    crash_dump_spi_section *new_crash_section_ptr;

    /* Make sure the start address is page aligned. */
    PMCFW_ASSERT(((start_address)& 0xFFF) == 0, CRASH_DUMP_ERR_SPI_ALIGN);

    /* Allocate memory and fill in new entry for the linked list */
    new_crash_section_ptr = MEM_ALLOC(MEM_TYPE_FREE, sizeof(crash_dump_spi_section), 0);
    new_crash_section_ptr->start_addr = start_address;
    new_crash_section_ptr->size = size;
    new_crash_section_ptr->next_section_ptr = NULL;

    /* Append to the end of the linked list. */
    if (head_crash_dump_section == NULL)
    {
        /* Linked list is empty therefore initialize the head and tail of the linked list */
        head_crash_dump_section = new_crash_section_ptr;
        tail_crash_dump_section = head_crash_dump_section;
    }
    else
    {
        /*
        ** Append new crash dump section to the end of the linked list and move the tail
        ** pointer.
        */
        tail_crash_dump_section->next_section_ptr = new_crash_section_ptr;
        tail_crash_dump_section = new_crash_section_ptr;
    }
}

/**
 * @brief
 *   Sets the start address and size for the crash dump RAM buffer.
 *
 * @param[in] buffer_addr - Amount of data to copy from the src_buffer_ptr to the crash dump
 * @param[in] buffer_size - Pointer to the start of source data buffer.
 *
 * @return
 *   none
 */
PRIVATE void crash_dump_data_buffer_set(UINT32 buffer_addr, UINT32 buffer_size)
{
    data_buffer.start_ptr = (UINT8*) buffer_addr;
    data_buffer.write_ptr = data_buffer.start_ptr;
    data_buffer.end_ptr = (UINT8*)((UINT32)data_buffer.start_ptr + buffer_size);
}

/**
 * @brief
 *   Sets the start address and size for the header RAM buffer.
 *
 * @param[in] buffer_size - Amount of data to copy from the src_buffer_ptr to the crash dump
 * @param[in] src_buffer_ptr - Pointer to the start of source data buffer.
 *
 * @return
 *   none
 */
PRIVATE void crash_dump_header_buffer_set(UINT32 buffer_addr, UINT32 buffer_size)
{
    header_buffer.start_ptr = (UINT8*) buffer_addr;
    header_buffer.write_ptr = header_buffer.start_ptr;
    header_buffer.end_ptr = (UINT8*)((UINT32)header_buffer.write_ptr + buffer_size);
}

/**
 * @brief
 *   Update the SPI flash address pointers
 *
 * @param[in] access_handler_ptr - Pointer to the handler to to update
 * @param[in] data_size - Data size written to SPi flash in bytes
 *
 * @return
 *   PMC_SUCCESS or error.
 */
PRIVATE PMCFW_ERROR spi_flash_address_update(crash_dump_spi_section_access_handler *access_handler_ptr, UINT32 data_size)
{
    PMCFW_ASSERT(data_size == CRASH_DUMP_MAX_WRITE_SIZE, 0);

    UINT32 crash_dump_section_end_addr = access_handler_ptr->current_spi_section->start_addr + access_handler_ptr->current_spi_section->size;

    access_handler_ptr->current_spi_address += data_size;

    /* Reached the end of this section move to next */
    if (access_handler_ptr->current_spi_address >= crash_dump_section_end_addr)
    {
        if (access_handler_ptr->current_spi_section->next_section_ptr != NULL)
        {
            access_handler_ptr->current_spi_address = access_handler_ptr->current_spi_section->next_section_ptr->start_addr;
            access_handler_ptr->current_spi_section = access_handler_ptr->current_spi_section->next_section_ptr;
        }
        else
        {
            access_handler_ptr->current_spi_address = crash_dump_section_end_addr;
            return CRASH_DUMP_ERR_SPI_FULL;
        }
    }

    return PMC_SUCCESS;
}

/**
 * @brief
 *   Read from spi flash to a destination buffer
 *
 * @param[in] dest_buffer_ptr - Pointer to save data read from SPI flash
 * @param[in] data_size - Number of bytes to read
 * @param[in] spi_src_addr - SPI address to read from
 *
 * @return
 *   PMC_SUCCESS or error.
 */
PRIVATE PMCFW_ERROR crash_dump_spi_read(UINT8* dest_buffer_ptr, UINT32 data_size, UINT32 spi_src_addr)
{
    PMCFW_ERROR rc = PMC_SUCCESS;

    rc = spi_flash_read(SPI_FLASH_PORT,
                        SPI_FLASH_CS,
                        (UINT8*)((UINT32)spi_src_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                        dest_buffer_ptr,
                        data_size);

    return rc;
}

/**
 * @brief
 *   Write to SPI flash
 *
 * @param[in] data_ptr - Pointer to the source data
 * @param[in] data_size - Size of data
 * @param[in] spi_dest_addr - Address to write to in SPI
 *
 * @return
 *   PMC_SUCCESS if successful. Otherwise return error.
 */
PMC_RAM_PROGRAM
PRIVATE PMCFW_ERROR _crash_dump_write(UINT8* data_ptr, UINT32 data_size, UINT32 spi_dest_addr)
{
    PMCFW_ERROR rc             = PMC_SUCCESS;
    UINT32      spi_flash_addr = spi_dest_addr;

    spi_flash_dev_info_struct dev_info;
    spi_flash_dev_enum        dev;
    UINT8*                    subsector_base;
    UINT32                    subsector_len;

    PMCFW_ASSERT(data_size <= CRASH_DUMP_MAX_WRITE_SIZE, CRASH_DUMP_ERR_SPI_WRITE_TOO_LARGE);

    /* get SPI flash device info */
    rc = spi_flash_dev_info_get(SPI_FLASH_PORT,
                                SPI_FLASH_CS,
                                &dev,
                                &dev_info);
    if (PMC_SUCCESS != rc)
    {
        return CRASH_DUMP_ERR_SPI_ERASE;
    }

    /* get the subsector address */
    rc = spi_flash_subsector_params_get(SPI_FLASH_PORT,
                                        SPI_FLASH_CS,
                                        (UINT8*)(spi_flash_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                                        &subsector_base,
                                        &subsector_len);
    if (PMC_SUCCESS != rc)
    {
        return CRASH_DUMP_ERR_SPI_ERASE;
    }

    /* erase subsector */
    rc = spi_flash_subsector_erase(SPI_FLASH_PORT,
                                   SPI_FLASH_CS,
                                   subsector_base);
    if (PMC_SUCCESS != rc)
    {
        return CRASH_DUMP_ERR_SPI_ERASE;
    }

    /* poll for erase completion */
    rc = spi_plat_flash_poll_write_erase_complete(FALSE, dev_info.max_time_subsector_erase);
    if (PMC_SUCCESS != rc)
    {
        return CRASH_DUMP_ERR_SPI_ERASE_TIMEOUT;
    }

    rc = spi_plat_flash_write_pages((UINT8*)data_ptr,
                                    (UINT8*)(spi_flash_addr & GPBC_FLASH_PHYS_ADDR_MASK),
                                     data_size,
                                     dev_info.page_size,
                                     dev_info.max_time_page_prog);
    if (PMC_SUCCESS != rc)
    {
        return CRASH_DUMP_ERR_SPI_WRITE;
    }

    return rc;

}
PMC_END_RAM_PROGRAM

/*
* Public Functions
*/

/**
 * @brief
 *   Initialize crash dump platform specific module
 *
 * @return
 *   None.
 */
PUBLIC void crash_dump_plat_init(void)
{
    /* Set the crash dump header address to be before the first SPI section */
    spi_flash_header_address = crash_dump_plat_crash_dump_spi_addr_get();

    /*
    ** Add all the SPI sections used for crash dump. Add an offset of
    ** CRASH_DUMP_HEADER_SECTION_SIZE to accommodate the space required by the header.
    */
    crash_dump_plat_add_spi_section(spi_flash_header_address + CRASH_DUMP_HEADER_SECTION_SIZE,
                                    crash_dump_plat_crash_dump_spi_size_get() - CRASH_DUMP_HEADER_SECTION_SIZE);

    /* Initialize the write address to the start of the crash dump data section */
    spi_write_handler.current_spi_section = head_crash_dump_section;
    spi_write_handler.current_spi_address = head_crash_dump_section->start_addr;

    /* Setup the RAM buffers for the crash dump header and data */
    crash_dump_header_buffer_set((UINT32) ech_ext_data_ptr_get(), CRASH_DUMP_HEADER_SECTION_SIZE);
    crash_dump_data_buffer_set((UINT32) ech_ext_data_ptr_get() + CRASH_DUMP_HEADER_SECTION_SIZE, CRASH_DUMP_MAX_WRITE_SIZE);
}


/**
 * @brief
 *   Get the number of bytes currently used by crash dump.
 *
 * @return
 *   Number of bytes used
 */
PUBLIC UINT32 crash_dump_plat_data_space_used(void)
{
    UINT32 space_used = 0;

    crash_dump_spi_section* crash_dump_section = head_crash_dump_section;

    /* Iterate through all the crash dump sections that have been filled */
    while (crash_dump_section != spi_write_handler.current_spi_section)
    {
        space_used += crash_dump_section->size;

        crash_dump_section = crash_dump_section->next_section_ptr;
    }

    /* Add the space used in the current SPI section */
    space_used += spi_write_handler.current_spi_address - crash_dump_section->start_addr;

    /* Add data in the RAM data buffer */
    space_used += (data_buffer.write_ptr - data_buffer.start_ptr);

    return space_used;
}

/**
 * @brief
 *   Get the number of remaining bytes available in crash dump.
 *
 * @return
 *   Number of bytes remaining
 */
PUBLIC UINT32 crash_dump_plat_remaining_spi_space_get(void)
{
    UINT32 remaining_space = 0;
    crash_dump_spi_section* crash_dump_section = spi_write_handler.current_spi_section->next_section_ptr;

    /* Add the remaining space in the current crash dump section */
    remaining_space += spi_write_handler.current_spi_section->size - (spi_write_handler.current_spi_address - spi_write_handler.current_spi_section->start_addr);

    /* Add space from all unused crash dump sections */
    while (crash_dump_section != NULL)
    {
        remaining_space += crash_dump_section->size;
        crash_dump_section = crash_dump_section->next_section_ptr;
    }

    /* Subtract the amount of space currently used in the RAM buffer */
    remaining_space -= (data_buffer.write_ptr - data_buffer.start_ptr);

    return remaining_space;
}

/**
 * @brief
 *   Append crash dump header to SPI
 *
 * @param[in] crash_dump_header_ptr - Pointer to crash dump header
 *
 * @return
 *   PMC_SUCCESS or error.
 */
PUBLIC PMCFW_ERROR crash_dump_plat_header_put(crash_dump_header *crash_dump_header_ptr)
{
    /* Make sure there is space left for header */
    if (((UINT32) header_buffer.write_ptr + sizeof(crash_dump_header)) > (UINT32) header_buffer.end_ptr)
    {
        return CRASH_DUMP_HEADER_TOO_LARGE;
    }

    /* Copy to header RAM buffer */
    memcpy(header_buffer.write_ptr, crash_dump_header_ptr, sizeof(crash_dump_header));
    header_buffer.write_ptr += sizeof(crash_dump_header);

    return PMC_SUCCESS;
}

/**
 * @brief
 *   Copy data from a buffer to the crash dump.
 *
 * @param[in] buffer_size - Amount of data to copy from the src_buffer_ptr to the crash dump
 * @param[in] src_buffer_ptr - Pointer to the start of source data buffer.
 *
 * @return
 *   PMC_SUCCESS if successful. Otherwise return error.
 */
PUBLIC PMCFW_ERROR crash_dump_plat_data_put(UINT32 buffer_size, void *src_buffer_ptr)
{
    PMCFW_ERROR rc = PMC_SUCCESS;

    /* Make sure there is still space in spi flash to write more data. */
    if (crash_dump_plat_remaining_spi_space_get() == 0)
    {
        return CRASH_DUMP_CHECK_SPI_FLASH_FULL;
    }

    if ((UINT32)data_buffer.write_ptr + buffer_size < (UINT32)data_buffer.end_ptr)
    {
        memcpy(data_buffer.write_ptr, src_buffer_ptr, buffer_size);
        data_buffer.write_ptr += buffer_size;
    }
    else
    {
        UINT32 remaining_ram_buffer_space = (UINT32) data_buffer.end_ptr - (UINT32) data_buffer.write_ptr;
        UINT32 remaining_input_data = buffer_size - remaining_ram_buffer_space;
        void*  remaining_src_buffer_ptr = (void*)((UINT32)src_buffer_ptr + remaining_ram_buffer_space);

        /* Copy the number of bytes from the source buffer to fill the RAM data buffer */
        memcpy(data_buffer.write_ptr, src_buffer_ptr, remaining_ram_buffer_space);

        do
        {
            /* Write to SPI flash */
            rc = crash_dump_spi_write(data_buffer.start_ptr, (UINT32)data_buffer.end_ptr - (UINT32)data_buffer.start_ptr, spi_write_handler.current_spi_address);

            if (rc != PMC_SUCCESS)
            {
                return rc;
            }

            rc = spi_flash_address_update(&spi_write_handler, CRASH_DUMP_MAX_WRITE_SIZE);

            if (rc != PMC_SUCCESS)
            {
                return rc;
            }

            /* Reset and clear data buffer write pointer */
            data_buffer.write_ptr = data_buffer.start_ptr;
            memset(data_buffer.start_ptr, 0, (UINT32)data_buffer.end_ptr - (UINT32)data_buffer.start_ptr);

            /* Fill with remaining data */
            memcpy(data_buffer.write_ptr, remaining_src_buffer_ptr, min(CRASH_DUMP_MAX_WRITE_SIZE, remaining_input_data));
            data_buffer.write_ptr += min(CRASH_DUMP_MAX_WRITE_SIZE, remaining_input_data);
            remaining_input_data -=  min(CRASH_DUMP_MAX_WRITE_SIZE, remaining_input_data);

        } while (remaining_input_data > 0);
    }

    return rc;
}

/**
 * @brief
 *   Send all the contents in the header RAM buffer to SPI flash
 *
 * @return
 *   PMC_SUCCESS if successful. Otherwise return error.
 */
PUBLIC PMCFW_ERROR crash_dump_plat_header_flush(void)
{
    return crash_dump_spi_write(header_buffer.start_ptr, CRASH_DUMP_HEADER_SECTION_SIZE, spi_flash_header_address);
}


/**
 * @brief
 *   Send all the contents in the data RAM buffer to SPI flash
 *
 * @return
 *   PMC_SUCCESS if successful. Otherwise return error.
 */
PUBLIC PMCFW_ERROR crash_dump_plat_data_flush(void)
{
    return crash_dump_spi_write(data_buffer.start_ptr, CRASH_DUMP_MAX_WRITE_SIZE, spi_write_handler.current_spi_address);
}

/**
 * @brief
 *   Get header data from SPI
 *
 * @param[in] header_index - Index for the crash_dump header
 * @param[out] crash_dump_header_ptr - Output pointer to the header data
 *
 * @return
 *   PMC_SUCCESS or error.
 */
PUBLIC PMCFW_ERROR crash_dump_plat_header_entry_get(UINT32 header_index, crash_dump_header *crash_dump_header_ptr)
{
    PMCFW_ERROR rc = PMC_SUCCESS;

    memset(crash_dump_header_ptr, 0, sizeof(crash_dump_header));

    /* Read the header page from SPI flash into the buffer */
    rc = crash_dump_spi_read((void*) crash_dump_header_ptr,
                             sizeof(crash_dump_header),
                             spi_flash_header_address + (sizeof(crash_dump_header) * header_index));

    if (rc != PMC_SUCCESS)
    {
        return rc;
    }

    if (crash_dump_header_ptr->header_key != CRASH_DUMP_HEADER_KEY)
    {
        return CRASH_DUMP_INVALID_HEADER_IDX;
    }

    return rc;
}

/**
 * @brief
 *   Get raw header data from SPI flash
 *
 * @param[in] dest_buffer - Pointer to a buffer to copy header data to
 * @param[in] dest_buffer_size - Size of dest_buffer
 * @param[in] offset - Offset into the header in which to start reading from
 *
 * @return
 *   Number of bytes read
 */
PUBLIC UINT32 crash_dump_plat_raw_header_get(UINT8* dest_buffer, UINT32 dest_buffer_size, UINT32 offset)
{
    PMCFW_ERROR rc = PMC_SUCCESS;
    UINT32 spi_address = spi_flash_header_address + offset;
    UINT32 data_size_to_read = min(dest_buffer_size, CRASH_DUMP_HEADER_SECTION_SIZE - offset);

    if (offset > CRASH_DUMP_HEADER_SECTION_SIZE)
    {
        /* Requested offset larger than header. Return nothing */
        return 0;
    }

    /* Read the header page from SPI flash into the buffer */
    rc = crash_dump_spi_read((void*) dest_buffer,
                             data_size_to_read,
                             spi_address);

    if (rc != PMC_SUCCESS)
    {
        return 0;
    }

    return data_size_to_read;
}

/**
 * @brief
 *   Gets a single crash dump section given a crash_dump_header
 *
 * @param[in] crash_dump_header_ptr - Pointer to the data for the crash dump section to return
 * @param[in] crash_dump_offset - Offset into the crash dump section to return
 * @param[in] dest_buffer - Destination buffer to copy data from the crash dump section
 * @param[in] dest_buffer_size - Size of destination buffer.
 *
 * @return
 *   PMC_SUCCESS if successful. Otherwise return error.
 */
PUBLIC PMCFW_ERROR crash_dump_plat_data_section_get(crash_dump_header *crash_dump_header_ptr, UINT32 crash_dump_offset, UINT8 *dest_buffer, UINT32 dest_buffer_size)
{
    crash_dump_spi_section *current_section = head_crash_dump_section;
    UINT32 read_offset = crash_dump_header_ptr->start_offset + crash_dump_offset;
    UINT32 spi_read_address = current_section->start_addr;
    UINT8 *dest_buffer_write_ptr;
    UINT32 remaining_data;

    if (crash_dump_header_ptr->size < crash_dump_offset)
    {
        /* Offset is larger than the section attempting to read from do nothing and return */
        return PMC_SUCCESS;
    }

    /* Find the start SPI address for the crash dump section */
    while (read_offset != 0)
    {
        if (read_offset > current_section->size)
        {
            read_offset -= current_section->size;

            if (current_section->next_section_ptr == NULL)
            {
                /* This should never happen. Something must have been corrupted */
                return CRASH_DUMP_ERR_GET_OFFSET_TOO_LARGE;
            }
            else
            {
                current_section = current_section->next_section_ptr;
            }
        }
        else
        {
            spi_read_address = current_section->start_addr + read_offset;
            read_offset = 0;
            break;
        }
    }

    /* Try to copy the entire crash dump section or as much as the destination buffer will hold */
    dest_buffer_write_ptr = dest_buffer;
    remaining_data = min((crash_dump_header_ptr->size - crash_dump_offset), dest_buffer_size);
    while (current_section != NULL)
    {
        UINT32 remaining_spi_section_space = (current_section->start_addr + current_section->size) - spi_read_address;
        if (remaining_data <= remaining_spi_section_space)
        {
            crash_dump_spi_read(dest_buffer_write_ptr, remaining_data, spi_read_address);
            return PMC_SUCCESS;
        }
        else
        {
            crash_dump_spi_read(dest_buffer_write_ptr, remaining_spi_section_space, spi_read_address);

            if (current_section->next_section_ptr == NULL)
            {
                /* Should never happen */
                break;
            }

            current_section = current_section->next_section_ptr;
            spi_read_address = current_section->start_addr;
            remaining_data -= remaining_spi_section_space;
            dest_buffer_write_ptr += remaining_spi_section_space;
        }
    }

    return CRASH_DUMP_ERR_GET_SPI_OVERFLOW;
}

/**
 * @brief
 *   Get raw data from SPI flash
 *
 * @param[in] dest_buffer - Pointer to a buffer to copy header data to
 * @param[in] dest_buffer_size - Size of dest_buffer
 * @param[in] offset - Offset into the header in which to start reading from
 *
 * @return
 *   Number of bytes read
 */
PUBLIC UINT32 crash_dump_plat_raw_data_get(UINT8* dest_buffer, UINT32 dest_buffer_size, UINT32 offset)
{
    PMCFW_ERROR rc = PMC_SUCCESS;
    crash_dump_spi_section_access_handler read_handler;
    UINT32 data_read = 0;

    read_handler.current_spi_address = head_crash_dump_section->start_addr;
    read_handler.current_spi_section = head_crash_dump_section;

    if (PMC_SUCCESS != spi_flash_address_update(&read_handler, offset))
    {
        /* Requested offset larger than header. Return nothing */
        return 0;
    }


    /* Keep reading until we fill the requested buffer */
    while(data_read != dest_buffer_size)
    {
        UINT32 spi_address = read_handler.current_spi_address;
        UINT32 remaining_data_spi_section = (read_handler.current_spi_section->start_addr + read_handler.current_spi_section->size) - read_handler.current_spi_address;
        UINT32 data_to_read = min(remaining_data_spi_section, dest_buffer_size);

        /* Read the header page from SPI flash into the buffer */
        rc = crash_dump_spi_read((void*) ((UINT32)dest_buffer + data_read),
                                 data_to_read,
                                 spi_address);

        if (rc != PMC_SUCCESS)
        {
            return data_read;
        }

        data_read += data_to_read;
        rc = spi_flash_address_update(&read_handler, data_read);

        if (rc != PMC_SUCCESS)
        {
            /* Still return success because we hit the end of the SPI flash */
            return data_read;
        }
    }

    return data_read;
}

/**
* @brief
*   adjust pointers to functions in RAM to accommodate PIC
*   start-up adding flash offset to initialized function
*   pointers
*
* @param
*   offset - PIC offset to remove from pointers
*
* @return
*   nothing
*/
PUBLIC VOID crash_dump_plat_ram_code_ptr_adjust(UINT32 offset)
{
    crash_dump_write_fn_ptr = (crash_dump_write_fn_ptr_type)((UINT32)crash_dump_write_fn_ptr - offset);
}

/** @} end addtogroup */
