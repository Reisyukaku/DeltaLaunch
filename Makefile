#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# EXEFS_SRC is the optional input directory containing data copied into exefs, if anything this normally should only contain "main.npdm".
#---------------------------------------------------------------------------------
TITLE       :=  Delta Launch
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source source/UI source/Services source/UI/Menus source/UI/Popup source/Core source/Core/GL
DATA		:=	data
INCLUDES	:=	include
ROMFS	    :=	romfs
DEFINES	    :=	
EXEFS_SRC   := exefs

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -ffunction-sections $(ARCH) $(DEFINES) $(INCLUDE) -DTITLE='"$(TITLE)"' `freetype-config --cflags` -D__SWITCH__

ifeq ($(BUILD),debug)
    CFLAGS += -D__DEBUG__
endif

CXXFLAGS	:= $(CFLAGS) -fno-rtti -O2 -s -fno-exceptions -std=gnu++17 -lstdc++fs

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lglfw3 -lEGL -lglapi -ldrm_nouveau -lfreetype -lSDL2 -lSDL2_mixer -lopusfile -lopus -lmodplug -lmpg123 -lvorbisidec -logg `freetype-config --libs` -lcurl -lmbedtls -lmbedcrypto -lmbedx509 -lz -lnx -lstdc++fs


#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@mkdir -p ./0100000000001000	
ifeq ($(BUILD),debug)
	@mkdir -p D:/switch/$(TARGET)/
	@build_romfs romfs D:/switch/$(TARGET)/romfs.bin
	@mv $(OUTPUT).nro D:/switch/$(TARGET)
else
	@build_romfs romfs ./0100000000001000/romfs.bin
	@mkdir -p ./exefs
	@mv $(OUTPUT).nso ./exefs/main
	@mv $(OUTPUT).npdm ./exefs/main.npdm
	@build_pfs0 exefs ./0100000000001000/exefs.nsp
	@rm -rf ./exefs
endif
	@rm -rf D:/ReiNX/contents/0100000000001000
	@mv $(CURDIR)/0100000000001000 D:/ReiNX/contents/
    
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr build debug 0100000000001000 main $(TARGET).elf $(TARGET).nro $(TARGET).nso


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all	:	$(OUTPUT).nso $(OUTPUT).npdm $(OUTPUT).nro
$(OUTPUT).npdm	:	$(OUTPUT).json
$(OUTPUT).nro	:	$(OUTPUT).elf
$(OUTPUT).nso	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
