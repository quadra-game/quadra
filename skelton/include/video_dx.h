/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_VIDEO_DX
#define _HEADER_VIDEO_DX

#include "video.h"

class DirectX_Surface {
public:
	LPDIRECTDRAWSURFACE s;
	Bitmap *b;
	DirectX_Surface(LPDIRECTDRAWSURFACE s2, Bitmap *b2):
		s(s2), b(b2) {
	}
};

class DirectX_Video_bitmap: Video_bitmap {
public:
  Bitmap* currentpage;
  DirectX_Video_bitmap(const int px, const int py, const int w, const int h,
		       const int rw);
  DirectX_Video_bitmap(const int px, const int py, const int w, const int h);
  virtual ~DirectX_Video_bitmap();
  virtual void rect(const int x, const int y,
		    const int w, const int h, const int color) const;
  virtual void box(const int x, const int y,const int w, const int h, const int color) const;
  virtual void get_bitmap(const Bitmap *bit, const int x, const int y,
			  const int w, const int h) const;
  virtual void put_pel(const int x, const int y, const Byte c) const;
  virtual void hline(const int y, const int x,
		     const int w, const Byte c) const;
  virtual void vline(const int x, const int y,
		     const int w, const Byte c) const;
  virtual void line(const int x1, const int y1,
		    const int x2, const int y2, const Byte c) const;
  virtual void put_bitmap(const Bitmap& d, const int dx, const int dy) const;
  virtual void put_sprite(const Sprite& d, const int dx, const int dy) const;
  virtual void setmem();
};

class DirectX_Video: public Video {
	void create_palette();
public:
  LPDIRECTDRAW lpdd;
  LPDIRECTDRAWSURFACE lpddsprimary, lpddsback;
  LPDIRECTDRAWPALETTE lpddpal;
  DDSURFACEDESC ddsdlock;
  static int screen_shot;
  Array<DirectX_Surface *> surfaces;
  void add_surface(LPDIRECTDRAWSURFACE s, Bitmap *b);
  void remove_surface(LPDIRECTDRAWSURFACE s, Bitmap *b);
  DirectX_Video(int w, int h, int b, const char *wname);
  virtual ~DirectX_Video();
  void lock();
  void unlock();
  void flip();
  void start_frame();
  void end_frame();
  void setpal(const Palette& p);
  virtual void dosetpal(PALETTEENTRY pal[256], int size);
  void restore();
  void clean_up();
  void snap_shot(int x, int y, int w, int h);
};

#endif /* _HEADER_VIDEO_DX */
