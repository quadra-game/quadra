/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_CURSOR
#define _HEADER_CURSOR

#include "sprite.h"

class Cursor {
protected:
  Cursor() { };
public:
  bool visible;
  int x, y;
  static Cursor* New(Sprite* s);
  virtual ~Cursor() { };
  virtual void set_pos(int px, int py) = 0;
  virtual void set_limit(int x1, int y1, int x2, int y2) = 0;
  virtual void restore_back(bool r) = 0;
  virtual void change_cursor(Sprite* s) = 0;
  virtual void draw() const = 0;
  virtual void move() = 0;
  virtual void get_back() = 0;
  virtual void put_back() = 0;
  virtual void set_speed(const Byte s) = 0;
};

extern Cursor* cursor;

#endif /* _HEADER_CURSOR */
