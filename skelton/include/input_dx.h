/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_INPUT_DX
#define _HEADER_INPUT_DX

#include "input.h"

class Input_DX: public Input {
public:
  LPDIRECTINPUT lpinput;
  LPDIRECTINPUTDEVICE lpinputdevice;
  LPDIRECTINPUTDEVICE lpinputmouse;
  HANDLE mouse_handle;
  Input_DX();
  virtual ~Input_DX();
  void process_mouse();
  void process_key();
	void add_key_buf(char c, bool special=false);
  virtual void clear_key();
  virtual void check() {
    process_key();
    process_mouse();
  }
  virtual void deraw() { };
  virtual void reraw() { };
  void restore();
};

#endif /* _HEADER_INPUT_DX */
