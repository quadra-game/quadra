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

.PHONY: clean distclean

clean:
	rm -f $(wildcard $(CLEAN) $(TARGETS) $(shell find . -name 'core' -print) $(shell find . -name '*~' -print))

distclean: clean
	rm -f $(wildcard $(DISTCLEAN))

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)

config/config.mk: config/config.mk.in configure
	@echo "Please run './configure'."
	@exit 1

configure: configure.in
	@echo "Please run 'autoconf'."
	@exit 1

config/depends.mk: config/config.mk
	@echo "Building dependencies file ($@)"
	@$(foreach DEP,$(CXXDEPS),$(COMPILE.cc) -M $(DEP) | sed -e 's|^.*:|$(dir $(DEP))&|' >> $@;)

-include config/depends.mk

endif
endif

