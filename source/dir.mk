# Makefile pour Quadra
# $Id$

ifndef MAKEFILE_INCLUDED

.PHONY: local

local:
	$(MAKE) -C .. source/quadra source/quadra.res

else

DEPENDS+=$(wildcard source/*.cpp)
OBJECTS+=$(patsubst %.cpp,%.o,$(wildcard source/*.cpp))
TARGETS+=source/quadra source/quadra.res
CLEANS+=source/quadra source/quadra.res

ifdef RELEASE
STATIC:=yes
endif

ifdef EFENCE
STATIC:=yes
endif

ifdef STATIC
OBJECTS+=$(SKELTON)/lib/libugs.a
QUADRA_OPT:=-L/usr/X11R6/lib -lX11 -lXpm -lXext -lvga -lvgagl -lz
ifdef SOCKS
OBJECTS+=-lsocks5
endif
ifdef EFENCE
OBJECTS+=-lefence
endif
else
QUADRA_OPT:=-lugs
endif

QUADRA_RES:=$(shell cat resources.txt)

source/quadra: $(OBJECTS)
	$(LINK.cc) $(QUADRA_OPT) -o $@ $(OBJECTS)
ifdef RELEASE
	nm -C $@ | sort > $@.sym
endif

source/quadra.res: $(QUADRA_RES)
	$(SKELTON)/tools/wadder/wadder $@ resources.txt

endif
