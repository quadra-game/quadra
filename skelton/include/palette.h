/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * 
 * Quadra, an action puzzle game
 * Copyright (C) 1998-2000  Ludus Design
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _HEADER_PALETTE
#define _HEADER_PALETTE

#ifdef UGS_DIRECTX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdio.h>
#include "types.h"
#include "error.h"
#include "utils.h"
#include "raw.h"

#ifndef UGS_DIRECTX
typedef struct {
  Byte peRed, peGreen, peBlue, peFlags;
} PALETTEENTRY;
#endif

class Palette {
private:
  friend class Fade;
  friend class Remap;
  PALETTEENTRY pal[256];
public:
  int size;
  Palette() {
    mset(pal, 0, sizeof(pal));
    size=256;
  }
  Palette(const Image& raw) {
    mset(pal, 0, sizeof(pal));
    load(raw);
  }
  void set_size(int s) {
    size=s;
  }
  void load(const Image& raw);
  void set();
  Byte r(Byte c) {
    return pal[c].peRed;
  }
  Byte g(Byte c) {
    return pal[c].peGreen;
  }
  Byte b(Byte c) {
    return pal[c].peBlue;
  }
  void setcolor(Byte c, Byte r, Byte g, Byte b) {
    pal[c].peRed=r;
    pal[c].peGreen=g;
    pal[c].peBlue=b;
  }
};

extern Palette noir;

class Remap {
  const Palette& dst;
public:
  Byte map[256];
  Remap(const Palette& d, Palette* src=NULL);
  void findrgb(const Byte m, const Byte r, const Byte g, const Byte b);
};

class Fade {
  Palette dest;
  short delta[768];
  short current[768];
  int currentframe;
  int destframe;
public:
  Fade(const Palette& dst=Palette(), const Palette& src=Palette(), int frame=70);
  void newdest(const Palette& dst, int frame=70);
  void setdest(const Palette& dst);
  bool done() const {
    return currentframe==destframe;
  }
  int step();
  void set();
};

#endif
