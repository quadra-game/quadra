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

CLEAN+=config/depends.mk
DISTCLEAN+=config/config.mk

CXXFLAGS+=-Wall -pedantic -pipe -Iinclude -Iskelton/include -Iimages

ARFLAGS=rcs

# FIXME: this should be the reverse, config.cpp should get values from
# this makefile using some -D options to the compiler.
MAJOR:=$(shell grep 'Config::major' source/config.cpp | cut -d= -f2 | bc)
MINOR:=$(shell grep 'Config::minor' source/config.cpp | cut -d= -f2 | bc)
PATCHLEVEL:=$(shell grep 'Config::patchlevel' source/config.cpp | cut -d= -f2 | bc)
VERSION:=$(MAJOR).$(MINOR).$(PATCHLEVEL)

# FIXME: temporary hack, until we properly detect stuff
CXXFLAGS+=-DUGS_LINUX -DUGS_LINUX_X11 -DUGS_LINUX_SVGA

