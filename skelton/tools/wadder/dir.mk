# Makefile pour le Universal Game Skelton
# $Id$

ifndef MAKEFILE_INCLUDED

.PHONY: local

local:
	$(MAKE) -C ../..

else
ifndef RULES

TARGETS+=tools/wadder/wadder
CLEANS+=tools/wadder/wadder

else

tools/wadder/wadder: tools/wadder/wadder.o lib/libugs_s.a
	$(LINK.cc) -o $@ $^

endif
endif

