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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "SDL.h"

#include "error.h"
#include "image.h"
#include "types.h"

class Palette {
private:
  SDL_Color pal[256];
public:
  int size;
  Palette()
    : size(256) {
    memset(pal, 0, sizeof(pal));
  }
  Palette(const Image& raw)
    : size(256) {
    memset(pal, 0, sizeof(pal));
    load(raw);
  }
  void load(const Image& raw);
  void set() const;
  uint8_t r(uint8_t c) const {
    return pal[c].r;
  }
  uint8_t g(uint8_t c) const {
    return pal[c].g;
  }
  uint8_t b(uint8_t c) const {
    return pal[c].b;
  }
  void setcolor(uint8_t c, uint8_t r, uint8_t g, uint8_t b) {
    pal[c].r = r;
    pal[c].g = g;
    pal[c].b = b;
    pal[c].a = SDL_ALPHA_OPAQUE;
  }
};

extern Palette noir;

class Remap {
  const Palette& dst;
public:
  uint8_t map[256];
  Remap(const Palette& d, Palette* src=NULL);
  void findrgb(const uint8_t m, const uint8_t r, const uint8_t g, const uint8_t b);
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
