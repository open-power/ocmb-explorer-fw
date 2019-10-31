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


/**  COPYRIGHT (C) 2001 PMC-SIERRA, INC. ALL RIGHTS RESERVED.                **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to PMC-Sierra, Inc.                                         **/
/** PMC-Sierra distributes this software to its customers pursuant to the    **/
/** terms and conditions of the Device Driver Software License Agreement     **/
/** contained in the text file software.lic that is distributed along with   **/
/** the device driver software. This software can only be utilized if all    **/
/** terms and conditions of the Device Driver Software License Agreement are **/
/** accepted. If there are any questions, concerns, or if the Device Driver  **/
/** Software License Agreement text file, software.lic, is missing please    **/
/** contact PMC-Sierra for assistance.                                       **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/

/*******************************************************************************
**
**  MODULE      :  PMC Firmware Driver RTOS Interface
**
**  FILE        :  pmcfw_rtos.h
**
**  DESCRIPTION :  Contains the RTOS specific definitions
**
**  NOTES       :  customers may need to modify this file
**
*******************************************************************************/

#ifndef _PMCFW_RTOS_H
#define _PMCFW_RTOS_H

/*
** include files
*/

#include <stdio.h>
#include "pmcfw_types.h"
#include "pmcfw_rtos_err.h"
#include <assert.h>
#include <string.h>

/*
** Constant Declarations
*/

#define PMCFW_RTOS_LINE_BUFFER_SIZE             256
#define PMCFW_RTOS_FILENAME_MAX                 256
#define PMCFW_RTOS_BUFFER_LENGTH                513     /* max size needed for text buffers */
#define PMCFW_RTOS_AUTOMATIC_DATA_THRESHOLD     256

/*
** Type Definitions
*/
/*******************************************************************************
**
**  STRUCT: sPMCFW_RTOS_FIV
**  ___________________________________________________________________________
**
**  DESCRIPTION :   PALADIN Module File Initialization Vector.
**
**  VALUES      :   pFilename - filename to open.
**                  pMode - mode to open file.  e.g. "r", "w", "rb", "wb"
**
**  NOTES       :
**
*******************************************************************************/
typedef struct
{
    CHAR *pFilename;
    CHAR *pMode;
} sPMCFW_RTOS_FIV;



/*******************************************************************************
**
**  ENUM: ePMCFW_RTOS_FILEMODE
**  ___________________________________________________________________________
**
**  DESCRIPTION :   Enumerated type with which the file was opened.  Stored
**                  internally in PMCFW RTOS File.
**
**  VALUES      :   ePMCFW_RTOS_FILEMODE_BinaryNoFilter - the file is binary
**                  ePMCFW_RTOS_FILEMODE_TextWithFilter-  the file is text and
**                  as values are read from the file comments '#' are stripped
**                  off.
**
**  NOTES       :
**
*******************************************************************************/
typedef enum
{
    ePMCFW_RTOS_FILEMODE_BinaryNoFilter,
    ePMCFW_RTOS_FILEMODE_TextWithFilter
} ePMCFW_RTOS_FILEMODE;



/*******************************************************************************
**
**  STRUCT: sPMCFW_RTOS_FILE
**  ___________________________________________________________________________
**
**  DESCRIPTION :   Structure containing fields for operating on a file.
**
**  VALUES      :
**      StdioInputFile - file handle for the particlar OS.
**      StdioLineNumber - for filtered files, the line number
**                        currently being processed.
**      StdioBuffer - buffer to hold the current line being
**                    processed.
**      StdioInputFileName - string name of the file
**      StdioNextToken - for filtered files, pointer to the
**                       next data to be processed.
**      StdioToken - for filtered files, pointer to the current
**                   token processed.
**      FilterFlag - filtering mode and type of file
**
**  NOTES       :
**
*******************************************************************************/
typedef struct sPMCFW_RTOS_FILE
{
    /* Public fields required to access any i/o */
    FILE *pStdioFile;

    /* Private fields used for stream processing */
    INT32 StdioLineNumber;
    CHAR StdioBuffer[BUFSIZ];
    CHAR StdioInputFileName[PMCFW_RTOS_FILENAME_MAX + 1];
    CHAR *pStdioNextToken;
    CHAR *pStdioToken;
    ePMCFW_RTOS_FILEMODE FilterFlag;
    CHAR target_function_name[PMCFW_RTOS_BUFFER_LENGTH];
    CHAR test_synopsis[PMCFW_RTOS_BUFFER_LENGTH];
    INT32 test_case_number;
    struct sPMCFW_RTOS_FILE* pPrevFile;
    BOOL  eof;

    UINT16 cacheBufferPos;
    UINT16 cacheBufferEnd;
    CHAR   cacheBuffer[BUFSIZ];
} sPMCFW_RTOS_FILE;



/*******************************************************************************
**
**  STRUCT: sPMCFW_RTOS_TIMER_EVENT
**  ___________________________________________________________________________
**
**  DESCRIPTION :   Structure containing fields for a timer event.  A single
**                  hardware timer can have multiple events.
**
**  VALUES      :   Delta - difference between this timer and the next timer
**                  Expiry - remaining expiry time for this timer
**                  Next - next timer in linked list of timers
**                  Prev - previous timer in linked list of timers
**
**  NOTES       :
**
*******************************************************************************/
//typedef struct tPMCFW_RTOS_TIMER_EVENT sPMCFW_RTOS_TIMER_EVENT;
typedef struct tPMCFW_RTOS_TIMER_EVENT
{
    UINT32 Delta;          /* in ticks */
    UINT32 Expiry;         /* in ticks */
    struct tPMCFW_RTOS_TIMER_EVENT *pNext;
    struct tPMCFW_RTOS_TIMER_EVENT *pPrev;
} sPMCFW_RTOS_TIMER_EVENT;



/*******************************************************************************
**
**  STRUCT: sPMCFW_RTOS_TIMER
**  ___________________________________________________________________________
**
**  DESCRIPTION :   Structure containing fields for a single timer which can
**                  service multiple timer events.
**
**  VALUES      :   DelayedTicks - how many ticks delay since timer tick
**                  ElapsedTicks - number of ticks since last processing
**                  ElapsedTicksCritSect - number of ticks waiting for
**                                         critical section to complete.
**                  pFirstRunningTimer - head of linked list of created
**                                       timer events.
**                  StartedList - head of linked list of running timer events
**                  OsTimerHandle - OS specific handle created for timer.
**
**  NOTES       :
**
*******************************************************************************/
typedef struct
{
    UINT32 DelayedTicks;
    UINT32 ElapsedTicks;
    BOOL ElapsedTicksCritSect;
    sPMCFW_RTOS_TIMER_EVENT *pFirstRunningTimer;
    sPMCFW_RTOS_TIMER_EVENT StartedList;
#   ifdef WIN32
      MMRESULT OsTimerHandle;
#   else
    /* insert BSC specific local variables for timers here */
#   endif
} sPMCFW_RTOS_TIMER;



/*******************************************************************************
**
**  STRUCT: sPMCFW_RTOS_NamedValue
**  ___________________________________________________________________________
**
**  DESCRIPTION :
**
**  VALUES      :
**
**  NOTES       :
**
*******************************************************************************/
typedef struct
{
    CHAR    *name;
    UINT32  value;
} sPMCFW_RTOS_NamedValue;

/*
** Global Variables
*/
EXTERN CHAR input_file_path[];
EXTERN UINT8 input_file_path_len;
EXTERN sPMCFW_RTOS_FILE *input_file_ptr;
EXTERN sPMCFW_RTOS_FILE *output_file_ptr;

/*
** Function Definitions
*/

/* File Handling Functions */
EXTERN sPMCFW_RTOS_FILE *PMCFW_RTOS_FileOpen( sPMCFW_RTOS_FIV *pFiv );
EXTERN INT32 PMCFW_RTOS_FileClose( sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FileFeofIs( sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FileReadUint8( UINT8 *pValue, sPMCFW_RTOS_FILE *pFile );
PUBLIC INT32 PMCFW_RTOS_FileReadUint64Hex( UINT64 *pValue, sPMCFW_RTOS_FILE *pFile );
PUBLIC INT32 PMCFW_RTOS_FileReadUint32Hex( UINT32 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FileReadUint8Hex( UINT8 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FileReadHex( UINT32 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FileReadUint32( UINT32 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FileReadUint16( UINT16 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FilePrint( sPMCFW_RTOS_FILE *pFile, CHAR *pMsg, ... );
EXTERN INT32 PMCFW_RTOS_FileReadInt32(INT32 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN INT32 PMCFW_RTOS_FileReadInt16(INT16 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN UINT32 PMCFW_RTOS_ReadString(CHAR *pValue, UINT32 max_length, sPMCFW_RTOS_FILE *pFile);

/* Testing Functions */
EXTERN void PMCFW_RTOS_ValidateTag(sPMCFW_RTOS_FILE *pFile, CHAR *Tag);
EXTERN void PMCFW_RTOS_ValidatePostTestMarker(sPMCFW_RTOS_FILE *pFile);
EXTERN UINT32 PMCFW_RTOS_ReadNamedValue(sPMCFW_RTOS_NamedValue *pNameList, UINT32 *pValue, sPMCFW_RTOS_FILE *pFile );
EXTERN void PMCFW_RTOS_OutputTestResult(sPMCFW_RTOS_FILE *pFile, sPMCFW_RTOS_FILE *pOutputFile, BOOL Passed);
EXTERN void PMCFW_RTOS_SkipRestOfTest(sPMCFW_RTOS_FILE *pFile);
EXTERN BOOL PMCFW_RTOS_ValidateTest(sPMCFW_RTOS_FILE *pInputFile, sPMCFW_RTOS_FILE *pOutputFile);
EXTERN void PMCFW_RTOS_Exit(BOOL Success);
EXTERN UINT32 PMCFW_RTOS_read_data(UINT32 max_len, UINT8 *data_ptr, sPMCFW_RTOS_FILE *file_ptr);
EXTERN BOOL PMCFW_RTOS_validate_data(UINT16 data_len, const UCHAR *data_type, const UINT8 *data_ptr, sPMCFW_RTOS_FILE *file_ptr);
EXTERN void PMCFW_RTOS_parse_arguments(int argc, char **argv);

/* Memory Management Functions */
EXTERN INT32 PMCFW_RTOS_MemAlloc( UINT32 NumBytes, void **ppMem );
EXTERN void PMCFW_RTOS_MemFree( void **ppMem );
EXTERN void *PMCFW_RTOS_MemSet( void *pMem, INT32 val, UINT32 sz );
EXTERN void *PMCFW_RTOS_MemCpy( void *pDst, const void *pSrc, UINT32 sz );

/* Input/Output Functions */
EXTERN void PMCFW_RTOS_PrintError( CHAR *pErrMsg, ... );
EXTERN void PMCFW_RTOS_Print( CHAR *pMsg, ... );

/* String Handling Functions */
EXTERN INT32 PMCFW_RTOS_StrPrint( CHAR *pStr, CHAR *pMsg, ... );

/* Miscellaneous Task Functions */
EXTERN UINT32 PMCFW_RTOS_ReadRestOfLine( CHAR *pValue, UINT32 max_length, sPMCFW_RTOS_FILE *pFile );

/* Timer Management Functions */
EXTERN INT32 PMCFW_RTOS_TimerOpen( sPMCFW_RTOS_TIMER **ppTimer );
EXTERN INT32 PMCFW_RTOS_TimerClose( sPMCFW_RTOS_TIMER **ppTimer );
EXTERN INT32 PMCFW_RTOS_TimerEventCreate(   sPMCFW_RTOS_TIMER           *pTimer,
                                            UINT32                      Duration,
                                            void                        *pFunc,
                                            sPMCFW_RTOS_TIMER_EVENT     **ppTimerEvent );
EXTERN INT32 PMCFW_RTOS_TimerEventDestroy(   sPMCFW_RTOS_TIMER       *pTimer,
                                            sPMCFW_RTOS_TIMER_EVENT **ppTimerEvent );
EXTERN INT32 PMCFW_RTOS_TimerEventStart( sPMCFW_RTOS_TIMER *pTimer, sPMCFW_RTOS_TIMER_EVENT *pTimerEvent);
EXTERN INT32 PMCFW_RTOS_TimerEventStop( sPMCFW_RTOS_TIMER *pTimer, sPMCFW_RTOS_TIMER_EVENT *pTimerEvent);
EXTERN INT32 PMCFW_RTOS_TimerEventNotExpired( sPMCFW_RTOS_TIMER *pTimer, sPMCFW_RTOS_TIMER_EVENT *pTimerEvebt);
EXTERN INT32 PMCFW_RTOS_TimerDelay( sPMCFW_RTOS_TIMER   *pTimer,
                                    UINT32              DelayMs );

#endif /* _PMCFW_RTOS_H */




