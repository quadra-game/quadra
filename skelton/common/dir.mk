# Makefile pour le Universal Game Skelton
# $Id$

ifndef MAKEFILE_INCLUDED

.PHONY: local

local:
	$(MAKE) -C ..

else
ifndef RULES

DEPENDS+=$(wildcard common/*.cpp)
OBJECTS+=$(patsubst %.cpp,%.o,$(wildcard common/*.cpp))

else
endif
endif

