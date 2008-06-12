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

#ifndef _HEADER_CLIPABLE
#define _HEADER_CLIPABLE

class Bitmap;

class Clipable {
public:
  int width, height;
  /* return values variables */
  static int clip_x1, clip_x2, clip_y1, clip_y2, clip_w;
  Clipable(int w, int h):
    width(w), height(h) {
  }
  /* 'true' if out of range */
  bool clip(const int x, const int y, const int w, const int h) const;
  bool clip(const int x, const int y, const Bitmap *b) const;
};

#endif /* _HEADER_CLIPABLE */
