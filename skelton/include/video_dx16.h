/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_VIDEO_DX16
#define _HEADER_VIDEO_DX16

#include "video.h"

class Surface_basedx;
class Surface;

class Video16: public Video {
public:
  LPDIRECTDRAW lpdd;
  LPDIRECTDRAWSURFACE lpddsprimary, lpddsback;
  LPDIRECTDRAWPALETTE lpddpal;
  DDSURFACEDESC ddsdlock;
	Surface_basedx *back;
	Array<Surface *> surfaces;
  static int screen_shot;
	int green_high;
  Video16(int w, int h, int b, const char *wname);
  virtual ~Video16();
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
	void add_surface(Surface *s);
	void remove_surface(Surface *s);
};

#endif /* _HEADER_VIDEO_DX16 */
