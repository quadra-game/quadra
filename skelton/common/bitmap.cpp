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

#include <string.h>
#include "types.h"
#include "video.h"
#include "res.h"
#include "image_png.h"

Bitmap* Bitmap::loadPng(const char* n) {
	Res_doze res(n);
	Png png(res);
	return new Bitmap(png);
}

Bitmap::Bitmap(int w, int h, int rw):
		Clipable(w, h),
		realwidth(rw*sizeof(Byte)),
		zlines(new int[height]),
		lines(new Byte*[height]),
		size(h*rw),
		fmem(1) {
	directx = false;
	initlines();
	setmem((void*)new Byte[size]);
	clear(0);
}

Bitmap::Bitmap(void* m, int w, int h, int rw):
		Clipable(w, h),
		realwidth(rw),
		zlines(new int[height]),
		lines(new Byte*[height]),
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
		lines(new Byte*[height]),
		size(h*rw),
		fmem(1) {
	directx = false;
	initlines();
	setmem((void*)new Byte[size]);
	memcpy(mem, m, size);
}

Bitmap::Bitmap(const Image& raw, bool dx):
	  Clipable(raw.width(), raw.height()),
	  realwidth(width*sizeof(Byte)),
	  zlines(new int[height]),
	  lines(new Byte*[height]),
	  size(height*realwidth),
	  fmem(1) {
	directx = false;
	initlines();
	setmem((void*)new Byte[size]);
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

void Bitmap::clear(const Byte color) const {
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

void Bitmap::hline(const int y, const int x, const int w, const Byte color) const {
	if(clip(x, y, w, 1))
		return;
	memset(operator[](y)+clip_x1, color, clip_w);
}

void Bitmap::vline(const int x, const int y, const int h, const Byte color) const {
	if(clip(x, y, 1, h))
		return;
	for(int i=clip_y1; i<=clip_y2; i++)
		operator[](i)[x]=color;
}

void Bitmap::line(const int x1, const int y1, const int x2, const int y2,
		  const Byte color) const
{
	/* This function use the Bresenham's line algorithm to draw a line. */

	int i, deltax, deltay, numpixels;
	int d, dinc1, dinc2;
	int x, xinc1, xinc2;
	int y, yinc1, yinc2;

	// Calculate deltax and deltay for initialisation.
	deltax = abs(x2 - x1);
	deltay = abs(y2 - y1);

	// Special cases: vertical and horizontal lines.
	if(y1 == y2)	{
		hline(y1, min(x1, x2), deltax, color);
		return;
	}
	if(x1 == x2)	{
		vline(x1, min(y1, y2), deltay, color);
		return;
	}

	// Initialize all vars based on which is the independent variable.
	if (deltax >= deltay)
	{
		// x is independent variable.
		numpixels = deltax + 1;
		d = (2 * deltay) - deltax;
		dinc1 = deltay << 1;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = 1;
		xinc2 = 1;
		yinc1 = 0;
		yinc2 = 1;
	}
	else
	{
		// y is independent variable.
		numpixels = deltay + 1;
		d = (2 * deltax) - deltay;
		dinc1 = deltax << 1;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = 1;
		yinc1 = 1;
		yinc2 = 1;
	}

	// Make sure x and y move in the right directions.
	if (x1 > x2)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (y1 > y2)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	// Start drawing at <x1, y1>.
	x = x1;
	y = y1;

	// Draw the pixels.
	for (i = 1; i <= numpixels; i++)
	{
		// Draw pixel.
		if(clip(x, y, 1, 1))
			return;
		operator[](y)[x] = color;

		// Calculate next step.
		if (d < 0)
		{
			d = d + dinc1;
			x = x + xinc1;
			y = y + yinc1;
		}
		else
		{
			d = d + dinc2;
			x = x + xinc2;
			y = y + yinc2;
		}
	}
}

void Bitmap::put_pel(const int x, const int y, const Byte color) const {
	if(clip(x, y, 1, 1))
		return;
	fast_pel(x, y, color);
}
