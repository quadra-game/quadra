# Makefile pour le Universal Game Skelton
# $Id$

ifndef MAKEFILE_INCLUDED

.PHONY: local

local:
	$(MAKE) -C ..

else
ifndef RULES

ifeq "$(TARGET)" "win32"
DEPENDS+=$(wildcard directx/*.cpp)
OBJECTS+=$(patsubst %.cpp,%.o,$(wildcard directx/*.cpp))
endif

else
endif
endif
