# Makefile pour le Universal Game Skelton
# $Id$

.PHONY: default all clean niceclean deps

default: all

TARGETS:=
CLEANS:=
DEPENDS:=
OBJECTS=

-include config/local.mk

include config/compiler.mk

include $(wildcard */dir.mk)

RULES:=now

include $(wildcard */dir.mk)

all: $(TARGETS)

CLEANS+=$(shell find . -name '*.o' -print)

clean: niceclean
	rm -f .depends $(CLEANS)

niceclean:
	rm -f $(shell find . -name 'core' -print)
	rm -f $(shell find . -name '*~' -print)

.depends:
	@$(foreach DEP,$(DEPENDS),$(COMPILE.cc) -M $(DEP) | sed -e 's|^.*:|$(dir $(DEP))&|' >> $@ ;)

ifneq ($(MAKECMDGOALS),clean)
include .depends
endif

