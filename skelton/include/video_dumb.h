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

#ifndef _HEADER_VIDEO_DUMB
#define _HEADER_VIDEO_DUMB

#include <stdint.h>

#include "video.h"

class Dumb_Video_bitmap: public Video_bitmap {
public:
  int opos_y;
  static Dumb_Video_bitmap* New(const int px, const int py,
				   const int w, const int h,
				   const int rw);
  static Dumb_Video_bitmap* New(const int px, const int py,
				   const int w, const int h);
  Dumb_Video_bitmap(const int px, const int py, const int w, const int h,
		       const int rw);
  Dumb_Video_bitmap(const int px, const int py, const int w, const int h);
  virtual void rect(const int x, const int y,
		    const int w, const int h, const int color) const;
  virtual void put_pel(const int x, const int y, const uint8_t c) const;
  virtual void hline(const int y, const int x,
		     const int w, const uint8_t c) const;
  virtual void vline(const int x, const int y,
		     const int w, const uint8_t c) const;
  virtual void put_bitmap(const Bitmap& d, const int dx, const int dy) const;
  virtual void put_sprite(const Sprite& d, const int dx, const int dy) const;
  virtual void setmem();
};

class Video_Dumb: public Video {
public:
  int displayoffset;
  Video_Dumb(int w, int h, const char *wname);
  virtual ~Video_Dumb();
  virtual void lock();
  virtual void unlock();
  virtual void flip();
  virtual void start_frame();
  virtual void end_frame();
  virtual void setpal(const Palette& p);
  virtual void dosetpal(const PALETTEENTRY pal[256], int size);
  virtual void restore();
  virtual void snap_shot(int x, int y, int w, int h);
  virtual void toggle_fullscreen();
  virtual Video_bitmap* new_bitmap(int px, int py, int w, int h);
};

#endif /* _HEADER_VIDEO_DUMB */
