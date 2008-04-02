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

#include "video_dumb.h"

#include "utils.h"
#include "error.h"
#include "input.h"
#include "main.h"
#include "net.h"
#include "sprite.h"

bool video_is_dumb=false; //Defaults to false

Dumb_Video_bitmap* Dumb_Video_bitmap::New(const int px, const int py,
						const int w, const int h,
						const int rw) {
  return new Dumb_Video_bitmap(px, py, w, h, rw);
}

Dumb_Video_bitmap* Dumb_Video_bitmap::New(const int px, const int py,
						const int w, const int h) {
  return new Dumb_Video_bitmap(px, py, w, h);
}

Dumb_Video_bitmap::Dumb_Video_bitmap(const int px, const int py,
					   const int w, const int h,
					   const int rw) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
}

Dumb_Video_bitmap::Dumb_Video_bitmap(const int px, const int py,
					   const int w, const int h) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
}

void Dumb_Video_bitmap::rect(const int x, const int y,
				const int w, const int h,
				const int color) const {
  clip(x, y, w, h);
}

void Dumb_Video_bitmap::box(const int x, const int y,
			       const int w, const int h,
			       const int color) const {
}

void Dumb_Video_bitmap::put_pel(const int x, const int y, const Byte c) const {
  clip(x, y, 1, 1);
}

void Dumb_Video_bitmap::hline(const int y, const int x,
			 const int w, const Byte c) const {
  clip(x, y, w, 1);
}

void Dumb_Video_bitmap::vline(const int x, const int y, const int h,
			 const Byte c) const {
  clip(x, y, 1, h);
}

void Dumb_Video_bitmap::put_bitmap(const Bitmap& d, const int dx,
			      const int dy) const {
  clip(dx, dy, d.width, d.height);
}

void Dumb_Video_bitmap::put_sprite(const Sprite& d, const int dx,
			      const int dy) const {
  int dx2 = dx - d.hot_x;
  int dy2 = dy - d.hot_y;

  clip(dx2, dy2, d.width, d.height);
}

void Dumb_Video_bitmap::setmem() {
}

Video_Dumb* Video_Dumb::New(int w, int h, int b, const char *wname) {
  return new Video_Dumb(w, h, b, wname);
}

Video_Dumb::Video_Dumb(int w, int h, int b, const char *wname) {
	video_is_dumb=true;

  width = w;
  height = h;
  framecount = 0;
  newpal = true;
  need_paint = 2;

  vb = Dumb_Video_bitmap::New(0, 0, w, h);
}

Video_Dumb::~Video_Dumb() {
  if(vb)
    delete vb;
}

void Video_Dumb::setpal(const Palette& p) {
  pal = p;
  newpal=true;
}

void Video_Dumb::dosetpal(SPalette pal[256], int size) {
}

void Video_Dumb::start_frame() {
}

void Video_Dumb::end_frame() {
  if(newpal) {
    pal.set();
    newpal = false;
  }
  framecount++;
}

void Video_Dumb::snap_shot(int x, int y, int w, int h) {
}

void Video_Dumb::toggle_fullscreen() {
}

