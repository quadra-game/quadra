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

#ifndef _HEADER_VIDEO
#define _HEADER_VIDEO

#include <stdint.h>

#include "clipable.h"
#include "palette.h"

class Bitmap;
class Sprite;

class Video_bitmap: public Clipable {
protected:
  Video_bitmap(int _pos_x, int _pos_y, int w, int h)
    : Clipable(w, h),
      pos_x(_pos_x),
      pos_y(_pos_y) {
    }
public:
  const int pos_x, pos_y;
  virtual ~Video_bitmap() { };

  /* fills a rectangle at position 'x','y' of width 'w', height 'h'
     and color 'color'. */
  virtual void rect(const int x,const int y,const int w,const int h,
		    const int color) const = 0;

  /* empty rectangle at position 'x','y' of width 'w', height 'h' and
     color 'color'. */
  virtual void box(const int x,const int y,const int w,const int h,
		   const int color) const;

  /* puts a pixel at position 'x','y' with color 'c' */
  virtual void put_pel(const int x, const int y, const uint8_t c) const = 0;

  /* horizontal line starting from 'x','y', width 'w' and color 'c' */
  virtual void hline(const int y, const int x,
		     const int w, const uint8_t c) const = 0;

  /* vertical line starting from 'x','y', height 'h' and color 'c' */
  virtual void vline(const int x, const int y,
		     const int h, const uint8_t c) const = 0;

  /* blits a Bitmap to position 'dx','dy' */
  virtual void put_bitmap(const Bitmap& d,
			  const int dx, const int dy) const = 0;

  /* blits a Sprite (mask) 'd' to position 'dx','dy' */
  virtual void put_sprite(const Sprite& d,
			  const int dx, const int dy) const = 0;
};

class Video {
public:
  Video_bitmap* const vb;
  bool newpal;
  Palette pal;
  const int width, height;
  int need_paint;
  const int pitch;
  uint32_t framecount;
  static Video* New(int w, int h, const char *wname, bool dumb=false);
  Video(Video_bitmap* _vb, int _width, int _height, int _pitch)
    : vb(_vb),
      newpal(true),
      width(_width),
      height(_height),
      need_paint(2),
      pitch(_pitch),
      framecount(0) {
  }
  virtual ~Video() {
    delete vb;
  }
  virtual void end_frame() = 0;
  virtual void setpal(const Palette& p) = 0;
  virtual void dosetpal(const PALETTEENTRY pal[256], int size) = 0;
  virtual void restore() = 0;
  virtual void snap_shot(int x, int y, int w, int h) = 0;
  virtual void toggle_fullscreen() = 0;
  virtual Video_bitmap* new_bitmap(int px, int py, int w, int h) = 0;
};

extern Video* video;
extern bool video_is_dumb;

#endif
