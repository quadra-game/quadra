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

#ifndef _HEADER_INPUT
#define _HEADER_INPUT

#include "autoconf.h"
#if defined(HAVE_SDL_H)
#include "SDL.h"
#elif defined(HAVE_SDL_SDL_H)
#include "SDL/SDL.h"
#endif

#include "types.h"
#include "input_keys.h"

#define SHIFT 1
#define ALT 2
#define CONTROL 4

#define MAXKEY 32

class Input {
public:
  struct {
    Byte button[4];
    int quel;
  } mouse;
  char key_buf[MAXKEY];
  unsigned int key_pending;
  Byte keys[SDLK_LAST];
  bool pause;
  SDL_keysym last_key;
  static Input* New(bool dumb = false);
  virtual ~Input() { };
  virtual void clear_key() = 0;
  virtual void check() = 0;
};

extern Input* input;

#endif
