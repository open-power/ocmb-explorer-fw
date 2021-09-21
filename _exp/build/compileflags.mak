#******************************************************************************
#  Copyright 2019 Microchip Technology Inc. and its subsidiaries.
#  Subject to your compliance with these terms, you may use Microchip
#  software and any derivatives exclusively with Microchip products. It is
#  your responsibility to comply with third party license terms applicable to
#  your use of third party software (including open source software) that may
#  accompany Microchip software.
#  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
#  EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
#  IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
#  PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
#  SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR
#  EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED,
#  EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
#  FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
#  LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
#  EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO
#  MICROCHIP FOR THIS SOFTWARE.
# --------------------------------------------------------------------------
#
# DESCRIPTION  :  This is the standard rules.mak file describing compiler / 
#                 linker settings
#
# NOTES        :  None.
#
#*******************************************************************************/

ENDIAN = little

########### 
#SDE TOOLS#
###########
ifeq (${TOOLS},sde)
  # GNU make definitions
  export ARFLAGS=-crvs
  export AROUTPUTFLAG=
  export BIGENDIANFLAG=-EB
  export LITTLEENDIANFLAG=-EL
  export MACHINEFLAGS=-mips32r2 -mdsp
  #export WARNINGFLAGS=-Wall -Werror
  export WARNINGFLAGS=-Wall
  export OPTIMIZATIONFLAG?=-O3
  export DEBUGGINGFLAGS=-gdwarf-2 -g2
  export DEPENDENCYFLAG=-MM
  export ASMWITHCPPFLAG=-x assembler-with-cpp 
  export ASMMAKEOBJFLAG=
  ##export TOOLLDFLAGS=-nostdlib
  export SDAFLAG=
  export TOOLLDFLAGS=-lc
  export TOOLFLAG=-DGNU_TOOL
#############
#LINUX TOOLS#
#############
else ifeq (${TOOLS}, linux)
  #include gnu.mak
  # GNU make definitions

  # GNU standard FLAGS
  WARNINGFLAGS=-Wall -Wno-trigraphs
  OPTIMIZATIONFLAG?=-Os
  #DEBUGGINGFLAGS=-g
  DEPENDENCYFLAG=-M

###########
#GHS TOOLS#
###########
else ifeq (${TOOLS}, ghs)
# GHS make definitions
  export ARFLAGS=
  export AROUTPUTFLAG=-o
  export BIGENDIANFLAG=-bigendian
  export LITTLEENDIANFLAG=-littleendian
  export MACHINEFLAGS=-cpu=mips32_34kc
  # --double-check.level=low enables the doublecheck analysis tool
  # --double-check.build=off temporary, until all warnings are cleaned up, when we can 
  #   error out a compilation on these warnings
DBLCHECK ?= none
DBLCHECK_FLAGS = --double_check.level=$(DBLCHECK) -double_check.stop_build=warnings \
	-double_check.config=$(SRCTL)/$(PMC_TOP_LEVEL)/build/doublecheck.cfg
export WARNINGFLAGS=-Wall --quit_after_warnings $(DBLCHECK_FLAGS)
export WARNINGFLAGS_VENDOR = -Wall 
  export DEBUGGINGFLAGS=-G
  export TOOLFLAG=-DGHS_TOOL
  export ASMMAKEOBJFLAG=-c -asm=-mipscompat
  export ASMWITHCPPFLAG=-D__ASSEMBLER__
  export DEPENDENCYFLAG=-Make
# ila linker asserts when -OI is used
# using -O2 significantly increases ila binary size 
  export OPTIMIZATIONFLAG?=-O2 -OI
  export SDAFLAG=-sda=128
  export LISTINGFLAG=-list
  export TOOLLDFLAGS=-no_default_lnk \
          -bsp=generic \
          -G \
          -object_dir=objs \
          -wantprototype \
          -nofloatio \
          -release2 \
          --no_commons \
          -farcallpatch \
          -wfarcallpatch \
          -cpu=mips32_34kc \
          -littleendian \
          -delete \
	      -Man \
	      -keepmap
  export CSTDFLAG=-c99

############
#CMIP TOOLS#
############
else ifeq ($(TOOLS),cmip)
  export ARFLAGS=-crvs
  export AROUTPUTFLAG=
  export BIGENDIANFLAG=-EB
  export LITTLEENDIANFLAG=-EL
  export MACHINEFLAGS=-DCPU_EAMP
  #export WARNINGFLAGS=-Wall -Werror
  export WARNINGFLAGS=-Wall
  export OPTIMIZATIONFLAG?=-O2
  export DEBUGGINGFLAGS=-gdwarf-2
  export DEPENDENCYFLAG=-MM
  export ASMWITHCPPFLAG=-x assembler-with-cpp 
  export SDAFLAG=
  export TOOLLDFLAGS=
  export LINKERFILEPREFIX = -T
  export LISTINGFLAG=-Wa,-a,-ad

#no tools defined
else
  $(error TOOLS has not be defined in the Makefile to sde, linux, ghs, or cmip)
endif

# Endianness
ifeq (${ENDIAN}, big)
  export ENDIANFLAG=${BIGENDIANFLAG}
else ifeq (${ENDIAN}, little)
  export ENDIANFLAG=${LITTLEENDIANFLAG}
else
  $(error No Endian flag is defined in Makefile)
endif


export LDFLAGS+= ${addprefix  $(LINKERFILEPREFIX),${LINKERFILES:.lds=.ld}} \
                ${ENDIANFLAG} \
                ${TOOLLDFLAGS} \
                ${addprefix -L,${LIBDIRS}} \
                ${SDAFLAG}


export ASFLAGS+=${ENDIANFLAG} \
                ${MACHINEFLAGS} \
                ${SDAFLAG} \
		-asm3g

export CFLAGS+=${ENDIANFLAG} \
               ${OPTIMIZATIONFLAG} \
               ${WARNINGFLAGS} \
               ${MACHINEFLAGS} \
               ${SDAFLAG}

CPPFLAGS_NOOPT:= $(CPPFLAGS) \
		${ENDIANFLAG} \
                -Omaxdebug \
                ${WARNINGFLAGS} \
                ${MACHINEFLAGS} \
                ${addprefix -I ,${DEFAULT_INCLUDEDIRS}} \
                ${addprefix -I, ${EXTRA_INCLUDEDIRS}} \
                ${TOOLFLAG} \
                ${SDAFLAG} \
                ${CSTDFLAG}

CPPFLAGS_VENDOR := $(CPPFLAGS) \
		${ENDIANFLAG} \
                -Omaxdebug \
                ${WARNINGFLAGS_VENDOR} \
                ${MACHINEFLAGS} \
                ${addprefix -I ,${DEFAULT_INCLUDEDIRS}} \
                ${addprefix -I, ${EXTRA_INCLUDEDIRS}} \
                ${TOOLFLAG} \
                ${SDAFLAG} \
                ${CSTDFLAG}

export CPPFLAGS+=${ENDIANFLAG} \
                ${OPTIMIZATIONFLAG} \
                ${WARNINGFLAGS} \
                ${MACHINEFLAGS} \
                ${addprefix -I ,${DEFAULT_INCLUDEDIRS}} \
                ${addprefix -I, ${EXTRA_INCLUDEDIRS}} \
                ${TOOLFLAG} \
                ${SDAFLAG} \
                ${CSTDFLAG}

export CPPFLAGS_NOOPT

export INCLUDEDIRS

#DEBUGGING=OFF
# debugging option are ON by default: 
# use DEBUGGING=OFF in your makefile to turn it OFF
ifneq (${DEBUGGING}, OFF)
  ASFLAGS+=${DEBUGGINGFLAGS}
  CFLAGS+=${DEBUGGINGFLAGS}  
  CPPFLAGS+=${DEBUGGINGFLAGS} 
endif
