/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_CURSOR_SELF
#define _HEADER_CURSOR_SELF

#include "bitmap.h"
#include "sprite.h"
#include "cursor.h"

class Cursor_Self: public Cursor {
private:
  Sprite *spr;
  int sx, sy;
  int lx1, ly1, lx2, ly2;
  int pool_x, pool_y;
  Byte speed;
  struct Back {
    int x, y;
    Bitmap *buf;
    int dirty;
  } back[2];
  int page, mouse_rate;
  bool _restore_back, smooth_mode;
public:
  Cursor_Self(Sprite* s);
  virtual ~Cursor_Self();
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

#endif /* _HEADER_CURSOR_SELF */
