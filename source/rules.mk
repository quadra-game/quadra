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

quadra: $(QUADRA_OBJECTS) skelton/lib/libugs_s.a
	$(LINK.cc) -Lskelton/lib $(X_CFLAGS) $(X_LIBS) -o $@ $^ -lX11 -lXext -lXxf86vm -lXpm -lpng -ldl -lz

quadra.res: $(shell cat resources.txt) resources.txt skelton/tools/wadder/wadder
	skelton/tools/wadder/wadder ./ $@ resources.txt

ifdef UGS_LINUX_SVGA
quadra-svga.so: $(SVGADRV_OBJECTS)
	$(LINK.cc) -shared -lvga -lvgagl $^ -o $@
endif

