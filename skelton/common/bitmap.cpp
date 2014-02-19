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

#include <string.h>
#include "types.h"
#include "video.h"
#include "res.h"
#include "image_png.h"
#include "bitmap.h"

using std::min;

Bitmap* Bitmap::loadPng(const ResName& n) {
	Res_doze res(n);
	Png png(res);
	return new Bitmap(png);
}

Bitmap::Bitmap(int w, int h, int rw):
		Clipable(w, h),
		realwidth(rw*sizeof(uint8_t)),
		zlines(new int[height]),
		lines(new uint8_t*[height]),
		size(h*rw),
		fmem(1) {
	directx = false;
	initlines();
	setmem((void*)new uint8_t[size]);
	clear(0);
}

Bitmap::Bitmap(void* m, int w, int h, int rw):
		Clipable(w, h),
		realwidth(rw),
		zlines(new int[height]),
		lines(new uint8_t*[height]),
		size(h*rw),
		fmem(0) {
	directx = false;
	initlines();
	setmem(m);
}

Bitmap::Bitmap(void* m, int w, int h, int rw, int bob):
	  Clipable(w, h),
		realwidth(rw),
		zlines(new int[height]),
		lines(new uint8_t*[height]),
		size(h*rw),
		fmem(1) {
	directx = false;
	initlines();
	setmem((void*)new uint8_t[size]);
	memcpy(mem, m, size);
}

Bitmap::Bitmap(const Image& raw, bool dx):
	  Clipable(raw.width(), raw.height()),
	  realwidth(width*sizeof(uint8_t)),
	  zlines(new int[height]),
	  lines(new uint8_t*[height]),
	  size(height*realwidth),
	  fmem(1) {
	directx = false;
	initlines();
	setmem((void*)new uint8_t[size]);
	reload(raw);
}

void Bitmap::reload(const Image& raw) {
	memcpy(mem, raw.pic(), size);
}

void Bitmap::initlines() {
	for(int y(0); y<height; y++)
		zlines[y]=y*realwidth;
}

Bitmap::~Bitmap() {
	delete[] zlines;
	delete[] lines;
	if(fmem)
		delete[] mem;
}

void Bitmap::clear(const uint8_t color) const {
	memset(mem, color, size);
}

void Bitmap::draw(const Bitmap& d, const int dx, const int dy) const {
	if(d.clip(dx, dy, this))
		return;
	for(int y=clip_y1; y<=clip_y2; y++) {
		memcpy(d[y]+clip_x1, operator[](y-dy)+(clip_x1-dx), clip_w);
	}
}

void Bitmap::draw(const Video_bitmap* d, const int dx, const int dy) const {
	d->put_bitmap(*this, dx, dy);
}

void Bitmap::hline(const int y, const int x, const int w, const uint8_t color) const {
	if(clip(x, y, w, 1))
		return;
	memset(operator[](y)+clip_x1, color, clip_w);
}

void Bitmap::vline(const int x, const int y, const int h, const uint8_t color) const {
	if(clip(x, y, 1, h))
		return;
	for(int i=clip_y1; i<=clip_y2; i++)
		operator[](i)[x]=color;
}

void Bitmap::put_pel(const int x, const int y, const uint8_t color) const {
	if(clip(x, y, 1, 1))
		return;
	fast_pel(x, y, color);
}
