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

#include "types.h"
#include "input_keys.h"

#define SHIFT 1
#define ALT 2
#define CONTROL 4

#define MAXKEY 32

extern const char *keynames[256];

class Input {
public:
  struct {
    int dx,dy,dz;
    Byte button[4];
    int quel;
  } mouse;
  struct {
    bool special;
    char c;
  } key_buf[MAXKEY];
  Byte keys[256];
  bool pause;
  int quel_key;
  int shift_key;
  int key_pending;
  static Input* New(bool dumb=false);
  virtual ~Input() { };
  virtual void clear_key() = 0;
  virtual void check() = 0;
  virtual void deraw() = 0;
  virtual void reraw() = 0;
};

extern Input* input;

#endif
