/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_CURSOR_X11
#define _HEADER_CURSOR_X11

#include "sprite.h"
#include "cursor.h"

class Cursor_X11: public Cursor {
protected:
public:
  Cursor_X11();
  virtual void set_pos(int px, int py);
  virtual void set_limit(int x1, int y1, int x2, int y2);
  virtual void restore_back(bool r);
  virtual void change_cursor(Sprite* s);
  virtual void draw() const;
  virtual void move();
  virtual void get_back();
  virtual void put_back();
  virtual void set_speed(const Byte s);
};

#endif /* _HEADER_CURSOR_X11 */
