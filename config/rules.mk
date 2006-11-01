# Quadra - a multiplayer action puzzle game
# Copyright (C) 2000  Pierre Phaneuf
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#
# $Id$

.PHONY: clean distclean dustclean maintainerclean dist installdirs install ChangeLog

dustclean:
	rm -f $(wildcard $(shell find . -name 'core' -print) $(shell find . -name '*~' -print) $(shell find . -name '.#*' -print))

clean: dustclean
	rm -f $(wildcard $(CLEAN) $(TARGETS))

distclean: clean
	rm -f $(wildcard $(DISTCLEAN))

maintainerclean: distclean
	rm -f $(wildcard $(REALCLEAN))

dist: distclean quadra.spec configure ChangeLog manual-dist-stuff
	rm -rf autom4te.cache

ChangeLog:
	rm -f ChangeLog
	-svn log -v > $@

installdirs:
	mkdir -p $(bindir)
	mkdir -p $(libgamesdir)
	mkdir -p $(datagamesdir)
	mkdir -p $(datadir)/pixmaps

install: installdirs $(TARGETS)
	$(INSTALL_PROGRAM) quadra $(bindir)/quadra
	$(INSTALL_DATA) quadra.res $(datagamesdir)/quadra.res
	$(INSTALL_DATA) images/quadra.xpm $(datadir)/pixmaps/quadra.xpm
# FIXME: the Quadra.desktop file should go to these places:
# /etc/X11/applnk/Games/Quadra.desktop
# /usr/share/gnome/apps/Games/Quadra.desktop

quadra.spec: packages/quadra.spec.in include/version.h
	sed -e 's%@VERSION@%$(VERSION)%g' >$@ <$<

Quadra.desktop: packages/Quadra.desktop.in config/config.mk
	sed -e 's%@bindir@%$(bindir)%g' -e 's%@datadir@%$(datadir)%g' >$@ <$<

configure: configure.ac
	autoreconf

.PHONY: manual-dist-stuff
manual-dist-stuff:
	@echo "-----------------------------------------------------------"
	@echo "remember to edit the version number in the following files:"
	@echo "include/version.h"
	@echo "packages/quadra.nsi"
	@echo "packages/readme-win32.txt"

ifeq ($(MAKECMDGOALS),dustclean)
NODEPENDS:=1
endif
ifeq ($(MAKECMDGOALS),clean)
NODEPENDS:=1
endif
ifeq ($(MAKECMDGOALS),distclean)
NODEPENDS:=1
endif
ifeq ($(MAKECMDGOALS),maintainerclean)
NODEPENDS:=1
endif
ifeq ($(MAKECMDGOALS),dist)
NODEPENDS:=1
endif

ifndef NODEPENDS

config/config.mk: config/config.mk.in configure
	@echo "Please run './configure'."
	@exit 1

config/depends.mk: config/config.mk
	@echo "Building dependencies file ($@)"
	@$(foreach DEP,$(CXXDEPS),$(COMPILE.cc) -M $(DEP) | sed -e 's|^.*:|$(dir $(DEP))&|' >> $@;)

-include config/depends.mk

endif

