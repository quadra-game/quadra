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

#ifndef _HEADER_VIDEO_X11
#define _HEADER_VIDEO_X11

#define Font XFont
#define Cursor XCursor
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/xf86vmode.h>
#undef Font
#undef Cursor

#include "video.h"

class Video_bitmap_X11: public Video_bitmap {
private:
  Bitmap* fb;
public:
  static Video_bitmap* New(const int px, const int py,
			   const int w, const int h, const int rw);
  static Video_bitmap* New(const int px, const int py,
			   const int w, const int h);
  Video_bitmap_X11(const int px, const int py,
		   const int w, const int h, const int rw);
  Video_bitmap_X11(const int px, const int py,
		   const int w, const int h);
  virtual ~Video_bitmap_X11();
  virtual void rect(const int x,const int y,const int w,const int h,
		    const int color) const;
  virtual void box(const int x,const int y,const int w,const int h,
		   const int color) const;
  virtual void get_bitmap(const Bitmap* bit, const int x, const int y,
			  const int w, const int h) const;
  virtual void put_pel(const int x, const int y, const Byte c) const;
  virtual void hline(const int y, const int x,
		     const int w, const Byte c) const;
  virtual void vline(const int x, const int y,
		     const int h, const Byte c) const;
  virtual void line(const int x1, const int y1, const int x2, const int y2,
		    const Byte c) const;
  virtual void put_bitmap(const Bitmap& d,
			  const int dx, const int dy) const;
  virtual void put_sprite(const Sprite& d,
			  const int dx, const int dy) const;
  virtual void setmem();
};

class Video_X11: public Video {
public:
  Display *display;
  XImage *image;
  unsigned char* vfb;
  short min_x[480], max_x[480];
  int min_x2, max_x2, min_y2, max_y2;
  Window window;
  Window fullscreen_window;
  int modecount;
  XF86VidModeModeInfo** modes;
  XF86VidModeModeInfo* fullscreen_mode;
  bool allow_fullscreen;
  bool fullscreen;
  bool restore_fullscreen;
  GC gc;
  Visual *visual;
  Atom delete_win;
  int depth;
  XShmSegmentInfo shminfo;
  bool do_shm;
  static Video* New(int w, int h, int b, const char *wname);
  Video_X11(int w, int h, int b, const char *wname, Display*, Visual*, int d);
  virtual ~Video_X11();
  virtual void dirty(int x1, int y1, int x2, int y2);
  virtual void dirty2(int x1, int y1, int x2, int y2);
  virtual void lock();
  virtual void unlock();
  virtual void start_frame();
  virtual void flip();
  virtual void end_frame();
  virtual void setpal(const Palette& p) ;
  virtual void restore();
  virtual void snap_shot(int x, int y, int w, int h);
  void focus_in(Window w);
  void focus_out(Window w);
  virtual void toggle_fullscreen();
};

class Video_X11_8: public Video_X11 {
public:
  Colormap cmap;
  XColor colors[256];
  Video_X11_8(int w, int h, int b, const char *wname, Display*, Visual*);
  virtual ~Video_X11_8();
  virtual void flip();
  virtual void dosetpal(PALETTEENTRY pal[256], int size);
};

class Video_X11_16: public Video_X11 {
public:
  unsigned short colors[256];
  Video_X11_16(int w, int h, int b, const char *wname, Display*, Visual*, int);
  virtual ~Video_X11_16();
  virtual void flip();
  virtual void dosetpal(PALETTEENTRY pal[256], int size);
};

class Video_X11_24: public Video_X11 {
public:
 struct Color {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
  };
  unsigned long colors[256];
  Video_X11_24(int w, int h, int b, const char *wname, Display*, Visual*);
  virtual ~Video_X11_24();
  virtual void flip();
  virtual void dosetpal(PALETTEENTRY pal[256], int size);
};

#endif /* _HEADER_VIDEO_X11 */

