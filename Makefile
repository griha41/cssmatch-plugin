# 
# Copyright 2007, 2008 Nicolas Maingot
# 
# This file is part of CSSMatch.
# 
# CSSMatch is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# CSSMatch is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with CSSMatch; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# Portions of this code are also Copyright © 2004-2006 SourceMM Development Team, All rights reserved
#

# Dossier de travail
BASE_DIR = ../hl2sdk

# Compilateur
#CXX = g++-3.4
CXX = g++-4.1

# Code source du SDK de VALVE
SDK_SRC_DIR = $(BASE_DIR)
SDK_PUBLIC_DIR = $(SDK_SRC_DIR)/public
SDK_TIER0_DIR = $(SDK_SRC_DIR)/public/tier0
SDK_TIER1_DIR = $(SDK_SRC_DIR)/tier1

# Dossiers de sortie
RELEASE_DIR = Release/linux
DEBUG_DIR = Debug/linux

# Dossier contenant les librairies dynamiques
SRCDS_BIN_DIR = bin

# Dossier contenant les librairies statiques
SRCDS_A_DIR = $(SDK_SRC_DIR)/linux_sdk

###############
# Options de compilation
###############

# Architecture du processeur
ARCH_CFLAGS = 	-mtune=i686 \
				-march=pentium \
				-mmmx
ARCH = i486
ARCH_BIN = .so

# Options du compilateur
USER_CFLAGS =
BASE_CFLAGS = 	-fno-rtti \
				-msse \
				-fpermissive \
				-D_LINUX \
				-DNDEBUG \
				-Dstricmp=strcasecmp \
				-D_stricmp=strcasecmp \
				-D_strnicmp=strncasecmp \
				-Dstrnicmp=strncasecmp \
				-D_snprintf=snprintf \
				-D_vsnprintf=vsnprintf \
				-D_alloca=alloca \
				-Dstrcmpi=strcasecmp \
				-fPIC \
				-Wno-deprecated \
				-msse 
OPT_FLAGS = -O3 \
			-fno-rtti \
			-funroll-loops \
			-s \
			-pipe
DEBUG_FLAGS = 	-g \
				-ggdb3 \
				-O0 \
				-D_DeBuG


############
# Objets à compiler
############

SRC= $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp) 			

###########
# Librairies à lier			
###########

LINK_SO =	$(SRCDS_BIN_DIR)/tier0_i486.so \
			$(SRCDS_BIN_DIR)/vstdlib_i486.so
LINK_A = 	$(SRCDS_A_DIR)/tier1_i486.a \
			$(SRCDS_A_DIR)/mathlib_i486.a \
			$(SRCDS_A_DIR)/choreoobjects_i486.a
			

LINK = -lm -ldl $(LINK_A) $(LINK_SO)

# #############
# Bibliothèques à inclure 
##############

INCLUDE = 	-I. \
			-I$(SDK_PUBLIC_DIR) \
			-I$(SDK_PUBLIC_DIR)/dlls \
			-I$(SDK_PUBLIC_DIR)/engine \
			-I$(SDK_PUBLIC_DIR)tier0 \
			-I$(SDK_PUBLIC_DIR)/tier1 \
			-I$(SDK_PUBLIC_DIR)/vstdlib \
			-I$(SDK_SRC_DIR)/tier1 \
			-I$(SDK_SRC_DIR)/game_shared \
			-I$(SDK_SRC_DIR)/dlls

###################
# Cibles et règles de compilation
###################

# Nom du fichier binaire de sortie
BINARY_NAME = cssmatch_$(ARCH)$(ARCH_BIN)

# Dossier de sortie du fichier binaire
BINARY_DIR = zip/addons

# Détermination du dossier de sortie et rassemblement des options de compilation
ifeq "$(DEBUG)" "true"
	BIN_DIR = $(DEBUG_DIR)
	CFLAGS = $(DEBUG_FLAGS)
else
	BIN_DIR = $(RELEASE_DIR)
	CFLAGS = $(OPT_FLAGS)
endif
CFLAGS += $(USER_CFLAGS) $(BASE_CFLAGS) $(ARCH_CFLAGS)

# Règles de compilation des fichiers .cpp
OBJ_LINUX := $(SRC:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "$< => $@"
	@$(CXX) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	@mkdir -p $(BIN_DIR)
	
	@$(MAKE) release

release: $(OBJ_LINUX)
	@$(CXX) $(INCLUDE) $(CFLAGS) $(OBJ_LINUX) $(LINK) -shared -o $(BINARY_DIR)/$(BINARY_NAME)
	
debug:
	@$(MAKE) all DEBUG=true

clean:
	@rm -rf $(RELEASE_DIR)
	@rm -rf $(DEBUG_DIR)	
