/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
