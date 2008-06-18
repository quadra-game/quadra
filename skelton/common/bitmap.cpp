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

#include "bitmap.h"

#include <assert.h>
#include <string.h>
#include "types.h"
#include "video.h"
#include "res.h"
#include "image_png.h"

using std::min;

Bitmap* Bitmap::loadPng(const char* n) {
	Res_doze res(n);
	Png png(res);
	return new Bitmap(png);
}

Bitmap::Bitmap(void* m, int w, int h, int rw):
		Clipable(w, h),
	  surface(SDL_CreateRGBSurfaceFrom(m, w, h, 8, rw, 0, 0, 0, 0)) {
}

Bitmap::Bitmap(const Image& raw):
	  Clipable(raw.width(), raw.height()),
	  surface(raw.get_surface()) {
}

Bitmap::~Bitmap() {
  SDL_FreeSurface(surface);
}

void Bitmap::reload(const Image& raw) {
  SDL_Surface* tmp = raw.get_surface();

  SDL_SetColors(surface, tmp->format->palette->colors, 0, tmp->format->palette->ncolors);
  SDL_BlitSurface(tmp, NULL, surface, NULL);

  SDL_FreeSurface(tmp);
}

void Bitmap::draw(const Bitmap& d, const int dx, const int dy) const {
	if(d.clip(dx, dy, this))
		return;
	for(int y=clip_y1; y<=clip_y2; y++) {
		memcpy(d[y]+clip_x1, operator[](y-dy)+(clip_x1-dx), clip_w);
	}
}
