# Makefile pour le Universal Game Skelton
# $Id$

.PHONY: skelton clean niceclean deps

TARGETS:=
CLEANS:=
DEPENDS:=
OBJECTS=

-include config/local.mk

include config/compiler.mk

include $(wildcard */dir.mk)

all: skelton $(TARGETS)

skelton:
	$(MAKE) -C skelton

CLEANS+=$(shell find . -name '*.o' -print)

clean: niceclean
	rm -rf .depends $(CLEANS)

niceclean:
	rm -rf $(shell find . -name 'core' -print) $(shell find . -name '*~' -print)

.depends:
	@$(foreach DEP,$(DEPENDS),$(COMPILE.cc) -M $(DEP) | sed -e 's|^.*:|$(dir $(DEP))&|' >> $@ ;)

ifneq ($(MAKECMDGOALS),clean)
include .depends
endif

