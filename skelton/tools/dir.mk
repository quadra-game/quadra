# Makefile pour le Universal Game Skelton
# $Id$

ifndef MAKEFILE_INCLUDED

.PHONY: local

local:
	$(MAKE) -C ..

else
ifndef RULES

TARGETS+=tools/dumpwad
CLEANS+=tools/dumpwad

else

tools/dumpwad: tools/dumpwad.o lib/libugs_s.a
	$(LINK.cc) -o $@ $^

endif
include tools/wadder/dir.mk
endif
