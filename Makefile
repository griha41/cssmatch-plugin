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
# along with CSSMatch; if not, see <http://www.gnu.org/licenses>.
# 
# Additional permission under GNU GPL version 3 section 7
# 
# If you modify CSSMatch, or any covered work, by linking or combining
# it with "Source SDK" (or a modified version of that SDK), containing
# parts covered by the terms of Source SDK licence, the licensors of 
# CSSMatch grant you additional permission to convey the resulting work.
#

# Dossier de travail
BASE_DIR = ../hl2sdk-ob

# Compilateur
#CXX = g++-3.4 # maximise la compatibilité
CXX = g++

# Nom du fichier binaire de sortie
BINARY_NAME = cssmatch.so

# Dossier de sortie du fichier binaire
BINARY_DIR = zip/addons

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
SRCDS_A_DIR = $(SDK_SRC_DIR)/lib/linux


# Paramètres du compilateur
ARCH_CFLAGS = -mtune=i686 -march=pentium  -mmmx
USER_CFLAGS = -DTIXML_USE_TICPP -pthread
BASE_CFLAGS = 	-msse \
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
				-m32 
OPT_FLAGS = -O3 -funroll-loops -s -pipe
DEBUG_FLAGS = -g -ggdb3 -O0 -D_DEBUG				

# Fichiers à compiler
SRC= $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp) 			

# Fichiers à lier
LINK_SO =	$(SRCDS_BIN_DIR)/libtier0_srv.so			
LINK_A = 	$(SRCDS_A_DIR)/tier1_i486.a

LINK = -lm -ldl $(LINK_A) $(LINK_SO)

# Dossiers des fichiers inclus
INCLUDE = 	-I. \
			-I$(SDK_PUBLIC_DIR) \
			-I$(SDK_PUBLIC_DIR)/engine \
			-I$(SDK_PUBLIC_DIR)/tier0 \
			-I$(SDK_PUBLIC_DIR)/tier1 \
			-I$(SDK_PUBLIC_DIR)/vstdlib \
			-I$(SDK_PUBLIC_DIR)/game/server \
			-I$(SDK_SRC_DIR)/tier1 \
			-I$(SDK_SRC_DIR)/game \
			-I$(SDK_SRC_DIR)/game/server \
			-I$(SDK_SRC_DIR)/game/shared
			

# Règles de compilation

ifeq "$(DEBUG)" "false"
	BIN_DIR = $(RELEASE_DIR)
	CFLAGS = $(OPT_FLAGS)
else
	BIN_DIR = $(DEBUG_DIR)
	CFLAGS = $(DEBUG_FLAGS)
endif
CFLAGS += $(USER_CFLAGS) $(BASE_CFLAGS) $(ARCH_CFLAGS)

OBJECTS := $(SRC:%.cpp=$(BIN_DIR)/%.o)

compile_object = \
	@mkdir -p $(2); \
	echo "$(1) => $(3)"; \
	$(CXX) $(INCLUDE) $(CFLAGS) -o $(3) -c $(1);

$(BIN_DIR)/%.o: %.cpp %.h
	$(call compile_object, $<, $(@D), $@)

$(BIN_DIR)/%.o: %.cpp
	$(call compile_object, $<, $(@D), $@)

all: $(OBJECTS)
	@$(CXX) $(INCLUDE) $(CFLAGS) $(OBJECTS) $(LINK) -shared -o $(BINARY_DIR)/$(BINARY_NAME)
	
release:
	@$(MAKE) all DEBUG=false

clean:
	@rm -rf $(RELEASE_DIR)
	@rm -rf $(DEBUG_DIR)
	@rm -rf $(BINARY_DIR)/$(BINARY_NAME)
	
.PHONY: clean

