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

#include "clipable.h"
#include "palette.h"

class Bitmap;
class Sprite;

class Video_bitmap: public Clipable {
protected:
  Video_bitmap(): Clipable(0, 0) { };
public:
  int pos_x, pos_y;
  static Video_bitmap* New(const int px, const int py,
			   const int w, const int h, const int rw);
  static Video_bitmap* New(const int px, const int py,
			   const int w, const int h);
  virtual ~Video_bitmap() { };

  /* remplis un rectangle a la position x,y de largeur w et hauteur h
     avec la couleur color. */
  virtual void rect(const int x,const int y,const int w,const int h,
		    const int color) const = 0;

  /* rectangle vide (contour) a la position x,y de largeur w et hauteur h
     avec la couleur color. */
  virtual void box(const int x,const int y,const int w,const int h,
		   const int color) const = 0;

  /* get un bout de bitmap à partir du Video_bitmap */
  virtual void get_bitmap(const Bitmap* bit, const int x, const int y,
			  const int w, const int h) const = 0;

  /* mets le pel à la position x,y a la couleur c */
  virtual void put_pel(const int x, const int y, const Byte c) const = 0;

  /* ligne horizontale partant de x,y et mettant w pels de couleur c
     vers la droite (?) */
  virtual void hline(const int y, const int x,
		     const int w, const Byte c) const = 0;

  /* ligne horizontale partant de x,y et mettant h pels de couleur c
     vers le bas (?) */
  virtual void vline(const int x, const int y,
		     const int h, const Byte c) const = 0;

  /* ligne quelquonque de <x1,y1> à <x2,y2> de couleur c */
  virtual void line(const int x1, const int y1, const int x2, const int y2,
		    const Byte c) const = 0;

  /* blit un Bitmap d a la position dx,dy */
  virtual void put_bitmap(const Bitmap& d,
			  const int dx, const int dy) const = 0;

  /* blit un Sprite (mask) d a la position dx,dy */
  virtual void put_sprite(const Sprite& d,
			  const int dx, const int dy) const = 0;

  /* ajuste la mem vidéo pointé par cette video_bitmap
     dans la page vidéo en background */
  virtual void setmem() = 0;
};

class Video {
public:
  bool xwindow;
  Video_bitmap *vb;
  Byte newpal;
  Palette pal;
  int width, height, bit;
  int need_paint;
  int pitch;
  Dword framecount;
  static Video* New(int w, int h, int b, const char *wname, bool dumb=false);
  virtual ~Video() { };
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual void flip() = 0;
  virtual void start_frame() = 0;
  virtual void end_frame() = 0;
  virtual void setpal(const Palette& p) = 0;
  virtual void dosetpal(PALETTEENTRY pal[256], int size) = 0;
  virtual void restore() = 0;
  virtual void clean_up() = 0;
  virtual void snap_shot(int x, int y, int w, int h) = 0;
};

extern Video* video;
extern bool video_is_dumb;

#endif
