#*******************************************************************************
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
# -----------------------------------------------------------------------------
#
# DESCRIPTION  :  This is the standard rules.mak file
#
# NOTES        :  None.
#
#*******************************************************************************/

ifdef MAKEDEBUG
 OLD_SHELL := $(SHELL)
 SHELL = $(warning [Rebuilding $@ because ($?) is changed])$(OLD_SHELL)
 export SHELL
 export OLD_SHELL
endif

OSTYPE ?= $(shell uname -o | tr 'A-Z' 'a-z')
export OSTYPE

ifeq ($(OSTYPE),cygwin)
  fixpath = $(shell cygpath -m $1)
else
  fixpath = $1
endif



#Ensure that the 1st visible rule is the default rule
default: all
.DEFAULT_GOAL := all

# Define Tools we use

# Common

SED = sed
AWK=awk
#MAKE= make --no-print-directory

TOPLEVEL=FALSE

# recursive test
# if this file is included by a sub-makefile, use the top level init flags
ifeq ($(MAKELEVEL),0)
  TOPLEVEL:=TRUE
endif

ifdef PROGRAM
  TOPLEVEL:=TRUE
else
$(error "PROGRAM not defined")
endif

ifeq ($(TOPLEVEL), TRUE)


#
# Get all the source files from the sub modules
#
ASMSOURCES := $(LOCAL_ASMSOURCES)

# Pull in all VENDOR_MODULES
# these module makefiles might have a CSOURCES += ... line or a CSOURCES_VENDOR += ... line
# force all of these into CSOURCES_VENDOR 
CSOURCES_VENDOR := $(LOCAL_CSOURCES_VENDOR)
CSOURCES :=
BIN2C_BINFILES :=
BIN2ZC_BINFILES :=
CSOURCES_NOOPT :=
EXTRA_LIB_MODULES :=
EXTRA_LIBS :=
include $(foreach mod,$(VENDOR_MODULES),$(SRCTL)/$(mod)/build/Makefile)
CSOURCES_VENDOR += $(CSOURCES)

CSOURCES := $(LOCAL_CSOURCES)

include $(foreach mod,$(CONVERTED_MODULES),$(SRCTL)/$(mod)/build/Makefile)

# basic test for PATH
# test if the PATH to the SRC root is set
ifndef SRCTL
  $(error SRCTL variable is not defined, make sure the SRCTL='PATH to fwcs directory' (i.e. SRCTL=../..) is set in the top-level makefile)
endif	
# test if the PATH to the SRC root is valid
# Path converters
make_posix_path = $(subst \,/,$1)
make_dos_path   = $(subst /,\,$1)


########### 
#SDE TOOLS#
###########
ifeq ($(TOOLS),sde)
  # GNU make definitions
  export CROSS=mips-sde-elf-
  export LD=$(CROSS)ld
  export AS=$(CROSS)gcc
  export CC=$(CROSS)gcc
  export CPP=$(CC)
  export OBJDUMP=$(CROSS)objdump
  export OBJCOPY=$(CROSS)objcopy
  export ELF2BIN=perl ../etc/srecconv.pl
  export AR=$(CROSS)ar
  export DEFAULT_INCLUDEDIRS=../inc .
  export EXTRA_INCLUDEDIRS:=.
#############
#LINUX TOOLS#
#############
else ifeq ($(TOOLS), linux)
  #include gnu.mak
  # GNU make definitions
  LD=ld
  AS=as
  CC=gcc
  CPP=$(CC) -E
  OBJDUMP=objdump
  export DEFAULT_INCLUDEDIRS=../inc .
  export EXTRA_INCLUDEDIRS:=.

###########
#GHS TOOLS#
###########
else ifeq ($(TOOLS), ghs)
export GHS_INSTALL_DIR := $(call fixpath,$(call make_posix_path,$(GHS_INSTALL_DIR)))

  # GHS make definitions
  export CC=$(call make_posix_path,$(GHS_INSTALL_DIR))/ccmips
  export LD=$(call make_posix_path,$(GHS_INSTALL_DIR))/ccmips
  export AS=$(call make_posix_path,$(GHS_INSTALL_DIR))/ccmips
  export GDUMP=$(call make_posix_path,$(GHS_INSTALL_DIR))/gdump
  export GSREC=$(call make_posix_path,$(GHS_INSTALL_DIR))/gsrec
  export GSTRIP=$(call make_posix_path,$(GHS_INSTALL_DIR))/gstrip
  export GMEMFILE=$(call make_posix_path,$(GHS_INSTALL_DIR))/gmemfile
  export AR=$(CC) -archive
  export CPP=$(CC) -E

  export AX=$(GHS_INSTALL_DIR)/ax
  export BIN2C=$(GHS_INSTALL_DIR)/gbin2c
  export BIN2C=$(GHS_INSTALL_DIR)/gbin2c

  export SDK_INCLUDEDIRS

  ifeq ($(findstring /cygdrive/,$(SDK_INCLUDEDIRS)),/cygdrive/)
    SDK_INCLUDEDIRS := $(call fixpath, $(SDK_INCLUDEDIRS))
  endif
  ifdef GHSPATH 
    export EXTRA_INCLUDEDIRS:= $(call fixpath, $(GHS_PATH))
  else
    ifndef GHS_INSTALL_DIR
      $(error GHS_INSTALL_DIR is not defined, please set this variable to the GHS tools path.)
    endif
    GHS_DIR := $(call fixpath, $(GHS_INSTALL_DIR))
  endif
  export EXTRA_INCLUDEDIRS := $(GHS_DIR)/src/libsys $(GHS_DIR)/include/mips
  export DEFAULT_INCLUDEDIRS=../inc . 

else ifeq ($(TOOLS),cmip)
$(error CMIP no longer supported in rules.mak)
else
  #no tools defined
  $(error TOOLS has not be defined in the Makefile to sde or ghs)
endif

#################
#3rd Party TOOLS#
#################
export MOT2BIN=$(SRCTL)/tools/bin/Mot2Bin.exe

# Endianness
ifeq ($(ENDIAN), big)
  export ENDIANFLAG=$(BIGENDIANFLAG)
else ifeq ($(ENDIAN), little)
  export ENDIANFLAG=$(LITTLEENDIANFLAG)
else
  $(warning No Endian flag is defined in Makefile)
endif

include $(SRCTL)/$(PMC_TOP_LEVEL)/build/compileflags.mak

# if top level, it's not a sub-makefile
SUBMAKEFILE:=FALSE
# end of top level configuration)
else

# sub-makefile configuration 
SUBMAKEFILE:=TRUE
# end of recursive configuration
endif

EVERYTHING_DEPENDS_ON += $(SRCTL)/$(PMC_TOP_LEVEL)/build/compileflags.mak
LINTINC=$(addprefix -i, $(INCLUDEDIRS)) $(SDK_INCLUDEDIRS) $(addprefix -i, $(subst \,/,$(GHS_INSTALL_DIR))/ansi ../src ../inc)
CPPINC =$(addprefix -I, $(INCLUDEDIRS) $(SDK_INCLUDEDIRS))

ifndef PROCESSOR
$(error The PROCESSOR variable is not defined in the Makefile)
endif

# set names for processor specific directories
# $(PROCESSOR) should be set in the Makefile
OBJDIR=obj
LIBDIR=lib

export PROCESSOR



# Only used by modules
LIBOUTPUTDIR=../$(LIBDIR)
OBJOUTPUTDIR=../$(OBJDIR)
B2COUTPUTDIR=../bin2c

LOGDEFDIR=$(SRCTL)/_rocw/build/
LOGDEFFILE?=osf_log_def.data


# Display status
WSROOT:=$(abspath $(SRCTL))
prettypath = @echo "(basecode $(PROCESSOR)) $1  $(subst $(WSROOT),.,$(abspath $2))"
BIN2CHOOK   = $(call prettypath,bin2c       ,$<)
BIN2ZCHOOK  = $(call prettypath,zip+bin2c   ,$<)
LIBHOOK     = $(call prettypath,Archiving   ,$@)
ASSEMBLEHOOK= $(call prettypath,Assembling  ,$<)
COMPILEHOOK = $(call prettypath,Compiling   ,$<)
LINKERHOOK  = $(call prettypath,Linking     ,$<)
LINKERFILECREATEHOOK=$(call prettypath,LinkerGen   ,$<)



LINKERFILECREATEHOOK=@echo "Creating $@";
PRECOMPILEDHEADERHOOK=@echo "Creating $@";
HOOKREDIRECTION=\"
ENDHOOK=@echo "$(basename $@).elf and $(basename $@).mem have been successfully created";

# The object files
BIN2C_CFILES  := $(foreach bin_file, $(BIN2C_BINFILES:.bin=.c), $(B2COUTPUTDIR)/$(notdir $(bin_file)) )
BIN2ZC_CFILES := $(foreach bin_file, $(BIN2ZC_BINFILES:.bin=.c), $(B2COUTPUTDIR)/$(notdir $(bin_file)) )

CSOURCES += $(BIN2C_CFILES) $(BIN2ZC_CFILES)

COBJS         := $(foreach source_file, $(CSOURCES:.c=.o), $(OBJOUTPUTDIR)/$(notdir $(source_file)) )
COBJS_NOOPT   := $(foreach source_file, $(CSOURCES_NOOPT:.c=.o), $(OBJOUTPUTDIR)/$(notdir $(source_file)) )
COBJS_VENDOR  := $(foreach source_file, $(CSOURCES_VENDOR:.c=.o), $(OBJOUTPUTDIR)/$(notdir $(source_file)) )
ASMOBJS       := $(foreach source_file, $(ASMSOURCES:.mip=.o), $(OBJOUTPUTDIR)/$(notdir $(source_file)) )
OBJS          := $(COBJS) $(COBJS_NOOPT) $(COBJS_VENDOR) $(ASMOBJS)
LOCAL_COBJS   := $(foreach source_file, $(LOCAL_CSOURCES:.c=.o), $(OBJOUTPUTDIR)/$(notdir $(source_file)) )
LOCAL_ASMOBJS := $(foreach source_file, $(LOCAL_ASMSOURCES:.mip=.o), $(OBJOUTPUTDIR)/$(notdir $(source_file)) )
LOCAL_OBJS    := $(LOCAL_COBJS) $(LOCAL_ASMOBJS)


CSOURCEDIRS   := $(sort $(dir $(CSOURCES) $(CSOURCES_NOOPT) $(CSOURCES_VENDOR)))
ASMSOURCEDIRS := $(sort $(dir $(ASMSOURCES)))
BIN2CBINDIRS  := $(sort $(dir $(BIN2C_BINFILES) $(BIN2ZC_BINFILES)))

vpath
vpath %.c $(CSOURCEDIRS)
vpath %.mip $(ASMSOURCEDIRS)
vpath %.bin $(BIN2CBINDIRS)

#Rules below which are phony
.PHONY: all clean default lib mkobjdir


# Default rule:
# The top level must have PROGRAM variable defined.
ifndef PROGRAM
  $(error "PROGRAM must be defined.")
endif

LIBNAME ?= $(PROCESSOR)

#Implicit Rules
#
# Logic behind the implicit rules
# Everything is broken into steps, so developers can easily debug any stage
# .mip files -> .s files -> .o files
# .c files -> .i files -> .s -> .o files
# make automatically removes .s and .i files, unless explicity requested
#   use .PRECIOUS to keep intermediary files
# the hook variables can be set by the Makefile to run test programs
#.PRECIOUS: %.s %.i

%.ld:   %.lds 
	$(LINKERFILECREATEHOOK)
	$(AT)$(CPP) $(ASMWITHCPPFLAG) $(CPPINC) $(CPPFLAGS) $(LNK_CPPFLAGS) -C $< -o $@.i
	$(AT)$(SED) -r 's/^#.*$$//g' $@.i > $@
	$(AT)rm -f $@.i

#Generate header files from XML files
ALLHEADERS=$(foreach target,$(XMLTARGETS),$(addsuffix -regs.h, $(target)))

##############################
# GNU Make 3.80 or newer in older versions no errors are reported
##############################
define XML_template
$(1)-regs.h: $$($(1)_xml)
	$$(HEADERHOOK)
	tclsh RegOffsetGen.tcl $(1) $$($(1)_xml) $(1)-regs.h
endef

$(foreach target,$(XMLTARGETS),$(eval $(call XML_template,$(target))))

# GNU Make older than 3.80
# $(ALLHEADERS): $(foreach target,$(XMLTARGETS),$($(target)_xml))
#	@#Now if any XML file changes all headers will be rebuilt
#	@#Bit of a pain but with Make 3.80 or older there isn't much else we can do
#	$(HEADERHOOK)
#	@#The next line has the 3 make vars as PREFIX PREFIX_xml TARGET
#	tclsh RegOffsetGen.tcl $(@:-regs.h=) $($(@:-regs.h=_xml)) $@
##############################
#Get dependency files to depend on these 
$(OBJS:.o=.d)::  $(ALLHEADERS)


# Generate dependency files
# Script to help transform compiler output in cygwin


# dependency fixup:
# fix C:\blahblah paths generated by GHS compiler dependency files
# Remove standard compiler headers from dependencies.
ifeq ($(TOOLS),ghs)
ifeq ($(OSTYPE),cygwin)
# There are 2 dependency file manipulations here:
# 1. for cygwin, all DOS style paths c:\blah are converted to cygwin friendly unix paths
# 2. To avoid make errors when a dependency described in a dependency file has been removed, all dependencies in the header file 
#    are added as an empty target.  source: http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
define GENERATE_DEPENDENCY
  $(V)dos2unix -q $(@:.o=.d)
  $(V)sed -e '/[a-zA-Z]:/ {' \
      -e 's/\([a-zA-Z]\):[\\\/]/\/cygdrive\/\L\1\//g' \
      -e 's/\(.\)\\\(.\)/\1\/\2/g' \
      -e '}' < $(@:.o=.d)  > $(@:.o=.dep)
  $(V)cp $(@:.o=.dep) $(@:.o=.dp2)
  $(V)sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.dp2) >> $(@:.o=.dep)
  $(V)rm $(@:.o=.dp2)

endef
else
define GENERATE_DEPENDENCY
  $(V)cp $(@:.o=.d) $(@:.o=.dep)
  cp $(@:.o=.dep) $(@:.o=.dp2)
  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.dp2) >> $(@:.o=.dep)
  rm $(@:.o=.dp2)
endef

endif # ifeq ($(OSTYPE),cygwin)
endif # ifeq ($(TOOLS),ghs)

# fixpath will create more easily findable filenames passed to compiler, makes 
# compilation error messages easier to parse to locate files.

$(OBJOUTPUTDIR) :
	$(V)mkdir -p $@

$(B2COUTPUTDIR) :
	$(V)mkdir -p $@

$(LIBOUTPUTDIR) :
	$(V)mkdir -p $@
$(LOGDEFDIR) :
	$(V)mkdir -p $@
HP_EXTRA_INCLUDE :=  $(SDK_INCLUDEDIRS)

$(BIN2C_CFILES) : $(B2COUTPUTDIR)/%.c : %.bin $(EVERYTHING_DEPENDS_ON) | $(B2COUTPUTDIR)
	$(BIN2CHOOK)
	$(AT)$(BIN2C) -crc -size -o $@ $< -array_name $(*F)_array -unsigned -volatile

$(BIN2ZC_CFILES) : $(B2COUTPUTDIR)/%.c : %.bin $(EVERYTHING_DEPENDS_ON) | $(B2COUTPUTDIR)
	$(BIN2ZCHOOK)
	$(AT)gzip -9 -c $< > $(@:.c=.bin.gz)
	$(AT)$(BIN2C) -crc -size -o $@ $(@:.c=.bin.gz) -array_name $(*F)_array -unsigned -volatile

$(ASMOBJS) : $(OBJOUTPUTDIR)/%.o : %.mip $(EVERYTHING_DEPENDS_ON) | $(OBJOUTPUTDIR)
	$(ASSEMBLEHOOK)
	$(V)$(AS) $(LISTINGFLAG)=$(@:.o=.lst) -c $(ASMWITHCPPFLAG) $(ASFLAGS) $(ASMMAKEOBJFLAG) $(CPPINC) $(CPPFLAGS) -MD $< -o $@
	$(GENERATE_DEPENDENCY)


$(COBJS) : $(OBJOUTPUTDIR)/%.o : %.c $(EVERYTHING_DEPENDS_ON) | $(OBJOUTPUTDIR)
	$(COMPILEHOOK)
	$(V)$(CC) $(LISTINGFLAG)=$(@:.o=.lst)  -c $(CPPINC) $(CPPFLAGS)  -MD $(call fixpath,$<) -o $@
	$(GENERATE_DEPENDENCY)

$(COBJS_NOOPT) : $(OBJOUTPUTDIR)/%.o : %.c $(EVERYTHING_DEPENDS_ON) | $(OBJOUTPUTDIR)
	$(COMPILEHOOK) ":: Note: optimization disabled!"
	$(V)$(CC) $(LISTINGFLAG)=$(@:.o=.lst)  -c $(CPPINC) $(CPPFLAGS_NOOPT) -G -MD $(call fixpath,$<) -o $@
	$(GENERATE_DEPENDENCY)

$(COBJS_VENDOR) : $(OBJOUTPUTDIR)/%.o : %.c  $(EVERYTHING_DEPENDS_ON) | $(OBJOUTPUTDIR)
	$(COMPILEHOOK) 
	$(V)$(CC) $(LISTINGFLAG)=$(@:.o=.lst)  -c $(CPPINC) $(CPPFLAGS_VENDOR) -G -MD $(call fixpath,$<) -o $@
	$(GENERATE_DEPENDENCY)

# How to generate the library
$(LIBOUTPUTDIR)/lib$(LIBNAME).a: $(OBJS) | $(LIBOUTPUTDIR)
	$(LIBHOOK)
	@rm -f $@
	$(V)$(AR) $(ARFLAGS) $(AROUTPUTFLAG) $@ $^
	@echo "lib$(LIBNAME).a has been successfully created";

lib: $(LIBOUTPUTDIR)/lib$(LIBNAME).a

ifneq ($(MAKECMDGOALS),lint)
$(MODULES):
	@echo "Processing module $@" >&2; cd $(SRCTL)/$@/build && $(MAKE) -s || exit 1
endif # ifneq ($(MAKECMDGOALS),lint)

# How to generate the program
LINK_DEPS = $(LIBOUTPUTDIR)/lib$(LIBNAME).a $(LOCAL_OBJS)
EXTRA_LIBS = $(foreach mod,$(LIB_MODULES),$(SRCTL)/$(mod)/lib/lib$(notdir $(mod)).a )

ifeq ($(filter clean, $(MAKECMDGOALS)),clean)
FORCE_CLEAN = FORCE
LIB_TARGET = clean
ACTION = "Cleaning"
else
ACTION = "Building"
endif

.PHONY: FORCE

$(EXTRA_LIBS): %.a : FORCE $(FORCE_CLEAN)
	$(V)+make -s -C $(dir $(@D))build $(LIB_TARGET)

LIBFLAGS=$(addprefix -l,$(EXTRA_LIBS))

$(PROGRAM).elf:  $(LINK_DEPS) $(EXTRA_LIBS) $(LINKERFILES:.lds=.ld) | $(OBJOUTPUTDIR)
	$(V)$(LD) $(LDFLAGS) -Ml -dist_inf_local -o $@ $(LIBFLAGS) -l$(LIBNAME) -L$(LIBOUTPUTDIR) $(LOCAL_OBJS)
	$(V)@$(GDUMP) -ysec $(PROGRAM).elf > $(PROGRAM).dasm
	$(ENDHOOK)

$(FW_VERSION).bin:
	$(V)$(CC) $(FW_VERSION_FILE) -c $(CPPINC) $(CPPFLAGS) -o $(OBJOUTPUTDIR)/version.o
	$(V)$(GSREC) -S7 $(OBJOUTPUTDIR)/version.o -o version.srec
	$(V)$(MOT2BIN) version.srec $@ 
	$(AT)rm -f version.srec
	


	
# How to generate Linux application program
linux:  
	$(CC) $(addprefix -L, $(LIBDIRS)) $(OPTIMIZATIONFLAG) $(WARNINGFLAGS) $(CPPINC) $(CSOURCES) -o $(PROGRAM) $(LIBFLAGS) 

# clean the project (project and all libraries)
clean:  $(CLEAN_PROGRAM) $(EXTRA_LIBS)
	@echo "  (rules.mak) Cleaning $(PROGRAM)"
	$(AT)rm -Rf $(OBJOUTPUTDIR)
	$(AT)rm -Rf $(B2COUTPUTDIR)
	$(AT)rm -Rf $(LIBOUTPUTDIR)
	$(AT)rm -f  $(PROGRAM).elf $(PROGRAM).dasm $(PROGRAM).dla $(PROGRAM).dnm $(PROGRAM).map
	$(AT)rm -f $(patsubst %.lds, %.ld, $(filter %.lds,$(LINKERFILES)))
	$(AT)rm -f $(PROGRAM).mem

# clean for release (all except libraries, .elf file and .mem files)
clean-release:
	@echo "Release-cleaning $(PROGRAM)"
	$(AT)rm -rf $(OBJOUTPUTDIR)
	$(AT)rm -rf $(LIBOUTPUTDIR)
	$(AT)rm -f *.dla *.dle *.dnm $(patsubst %.lds, %.ld, $(filter %.lds,$(LINKERFILES)))
	@dirs=$(subst /src/,/$(OBJDIR)/, $(dir $(CSOURCES) $(ASMSOURCES))); \
	if [ $$dirs ]; \
	then \
	    for dir in $$dirs; do \
		    (cd `echo "$$dir" | $(SED) -r 's/\/$(OBJDIR)\/.*/\//g'` ; rm -rf $(OBJDIR) ; rm -f *.i *.s *.o *.dbo *.d); \
	     done; \
	fi

# Generate header from XML
# this rule is not recursive and must be called in the module/build location.
# H_DEST, XML_SOURCE and XML_PREFIX must be defined in the module makefile
xml:
	@echo; echo "Generating the header file: ../inc/$(H_DEST_$(XML_NAME)) from the xml file: ../etc/$(XML_SOURCE_$(XML_NAME))"; echo
	@tclsh $(SRCTL)/shared/tools/xml_regs_parser/RegOffsetGen.tcl $(XML_PREFIX_$(XML_NAME)) ../etc/$(XML_SOURCE_$(XML_NAME)) ../inc/$(H_DEST_$(XML_NAME))


# How to generate the log
$(LOGDEFDIR)/$(LOGDEFFILE): $(LOGSOURCES) | $(LOGDEFDIR)
	@echo "Generating Log definition data $(LOGSOURCES)"
	$(AT)$(SRCTL)/tools/bin/gen_log_def.sh $(SRCTL)/tools/bin $(SRCTL)/_rocw/inc/pmcfw_mid.h $@ "$(LOGSOURCES)"
	@echo "Log definition file generation done"

ifneq ($(LOG_PATH),)
$(LOG_PATH)/$(LOGDEFFILE): $(LOGDEFDIR)/$(LOGDEFFILE)
	$(AT)cp $(LOGDEFDIR)/$(LOGDEFFILE) $(TARGET_LOG_PATH)/$(LOGDEFFILE)


log: $(TARGET_LOG_PATH)/$(LOGDEFFILE)

else
log: $(LOGDEFDIR)/$(LOGDEFFILE)

endif

# Include all dependency files (don't include them if we want to clean)
CMD_GOALS_LIST := lint log clean clean-release clean-release-lib clean-lib
ifneq ($(MAKECMDGOALS:xml%=xml),xml)
  ifeq (,$(findstring $(MAKECMDGOALS),$(CMD_GOALS_LIST)))
	-include $(OBJS:.o=.dep)
  endif
endif
