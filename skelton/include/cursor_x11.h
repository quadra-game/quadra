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

#ifndef _HEADER_CURSOR_X11
#define _HEADER_CURSOR_X11

#ifdef UGS_LINUX_X11

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

#endif /* UGS_LINUX_X11 */

#endif /* _HEADER_CURSOR_X11 */
