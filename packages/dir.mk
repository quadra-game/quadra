# Makefile pour Quadra
# $Id$

# FIXME: this file is obsolete

ifndef MAKEFILE_INCLUDED

.PHONY: dist

dist:
	$(MAKE) -C .. dist

else

MAJOR:=$(shell grep 'Config::major' source/config.cpp | cut -d= -f2 | bc)
MINOR:=$(shell grep 'Config::minor' source/config.cpp | cut -d= -f2 | bc)
PATCHLEVEL:=$(shell grep 'Config::patchlevel' source/config.cpp | cut -d= -f2 | bc)
VERSION:=$(MAJOR).$(MINOR).$(PATCHLEVEL)
QUADRADIR:=quadra-$(VERSION)
QUADRAPKG:=$(QUADRADIR).tar.gz

PACKAGE_DEPS:=source/quadra source/quadra.res packages/install.sh NEWS LICENSE packages/README packages/quadra.spec images/quadra.xpm

TARGETS+=
CLEANS+=packages/README packages/install.sh packages/quadra.spec $(QUADRADIR) $(QUADRAPKG)

dist:
	@$(MAKE) $(QUADRAPKG) RELEASE=yes

$(QUADRAPKG): $(QUADRADIR) $(PACKAGES_DEPS)
	tar cf - $< | gzip -9 > $@

$(QUADRADIR): $(PACKAGE_DEPS)
	rm -rf $@
	mkdir $@
	cp $^ $@
	chmod 755 $@/install.sh

packages/README: packages/readme.txt
	sed $^ -e 's/@VERSION@/$(VERSION)/g' > $@

packages/quadra.spec: packages/quadra.spec.in
	sed $^ -e 's/@VERSION@/$(VERSION)/g' > $@

packages/install.sh: packages/install.sh.in
	sed $^ -e 's/@VERSION@/$(VERSION)/g' > $@

endif

#dist: ../source/quadra ../source/quadra.res install.sh readme.txt
#
#cleanhere:
#
#ROOTDIR=..
#include ../Config.make
#include ../Rules.make
#
