# Makefile pour le Universal Game Skelton
# $Id$

ifndef MAKEFILE_INCLUDED

.PHONY: local

local:
	$(MAKE) -C ..

else
ifndef RULES

DEPENDS+=$(wildcard svgalib/*.cpp)

ifeq "$(TARGET)" "linux"
OBJECTS+=$(patsubst %.cpp,%.o,$(wildcard svgalib/*.cpp))
endif

else
endif
endif
