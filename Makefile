#RenDev#2616 File Selector Tool
#    Copyright (C) 2021 RenDev

#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#Contact me at vgngamingnetwork@gmail.com if you need to contact me

CC = g++
UNAME = $(shell uname)
CC_ARGS = -Wno-unused-result -Ofast -lX11
DEFINES_DEF = DATE="\"`date`\"" VER=\"$(VERSION)\" PROG_NAME=\"$(BUILD)\" OS_CLEAR=\"$(OS_CLEAR)\" FORCE_EXPERIMENTAL_FS

ODIR := build
ROOT_DIRECTORY=.
SOURCES := ${shell find ${ROOT_DIRECTORY} -type d -print}
INCLUDE := ./include

C_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.c) ) $(wildcard *.c)
CPP_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.cpp) ) #$(wildcard *.cpp)
H_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.h) ) $(wildcard *.h)

ASM_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.asm) ) $(wildcard *.asm)
S_FILES := $(foreach dir,$(SOURCES),  $(wildcard $(dir)/*.s) ) $(wildcard *.s)

O_FILES = $(abspath $(addprefix $(ODIR)/, $(CPP_FILES:.cpp=.o)))


VERSION := 0.0
BUILD := $(shell basename $(CURDIR)).linux
DEFINE_PREFIX = -
OS_CLEAR = clear


FORCE_EXPERIMENTAL_FS=true

ifndef $(DEBUG)
DEBUG = false
endif

ifeq ($(DEBUG), true)
DEFINES_DEF += DEBUG=$(DEBUG)
CC_ARGS += -g
endif

DEFINES = $(foreach def,$(DEFINES_DEF), $(DEFINE_PREFIX)D$(def))


$(ODIR)/./%.o : %.cpp
	@echo $(notdir $(basename $*)).cpp
	@mkdir -p $(dir $@)
	@$(CC) -c $*.cpp -o $@ -I $(INCLUDE) $(CC_ARGS) $(DEFINES)


.PHONY: all
.PHONY: message
.PHONY: compile
.PHONY: wine_release
.PHONY: wine_deubg

all: compile run
	

compile: message $(addprefix $(ODIR)/, $(CPP_FILES:.cpp=.o)) | $(ODIR)
	@$(CC) $(O_FILES) -o $(BUILD) -I $(INCLUDE) $(CC_ARGS) $(DEFINES) 

message:
	@echo Building ...
	@echo "\tDebug Build = $(DEBUG)"
	@echo "\tTarget = $(UNAME)"
	@echo "\tVersion = $(VERSION)"
	@echo "\tC++ Files to Compile = $(words $(CPP_FILES))"
	
$(ODIR):
	@mkdir -p $@

link:
	@echo $(CC) $(O_FILES) -o $(BUILD) -I $(INCLUDE) $(CC_ARGS) $(DEFINES)

compile1:
	@$(foreach cc,$(CPP_FILES),  $(shell $(CC) -I $(INCLUDE) $(CC_ARGS) $(DEFINES) -c $(cc) -o $(ODIR)/$(notdir $(basename $(cc)).o)))
	@$(CC) -o $(BUILD) -I $(INCLUDE) $(CC_ARGS) $(DEFINES) $(O_FILES)
compile2:
#@$(CC) -o $(BUILD) -I $(INCLUDE) $(CC_ARGS) $(DEFINES) $(CPP_FILES)
	@$(CC) $(CPP_FILES) -o $(BUILD) -I $(INCLUDE) $(CC_ARGS) $(DEFINES) 
run:
	@./$(BUILD)

clean:
	@rm -rf $(ODIR)
	@rm -rf x64
	@rm -rf Debug
	@rm -rf Release
	@rm -rf .vs
	@rm -rf $(BUILD)


debug:
	@$(MAKE) DEBUG=true --no-print-directory
release:
	@$(MAKE) --no-print-directory
