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

#ifndef _HEADER_BITMAP
#define _HEADER_BITMAP

#include "types.h"
#include "utils.h"
#include "error.h"
#include "raw.h"
#include "clipable.h"

#define COPY 1

class Video_bitmap;
class Bitmap;

class Bitmap: public Clipable {
	friend class Video;
	friend class Video_bitmap;
	friend class DirectX_Video;
	friend class DirectX_Video_bitmap;
	typedef Byte T;
	void initlines();
 public:
	int const realwidth;
 protected:
	int* zlines;
	T** const lines;
	Dword const size;
	T* mem;
	Byte const fmem;
  /* FIXME: this is awful. */
	bool directx;
 public:
	static Bitmap* loadPcx(const char* n);
	static Bitmap* loadRaw(const char* n);
	Bitmap(int w, int h, int rw); // Bitmap vide
	Bitmap(void* m, int w, int h, int rw); // Bitmap pointant sur mem existante
	Bitmap(void* m, int w, int h, int rw, int bob); // Copie mem dans bitmap
	Bitmap(const Image& raw, bool dx=false);
	virtual ~Bitmap();
	void reload(const Image& raw);
	void setmem(const void* m) {
		mem=(T*)m;
		for(int i(0); i<height; i++)
			lines[i]=(T*) (((Byte *)mem)+zlines[i]);
	}
	T* operator[](const int y) const {
		return lines[y];
	}
	void draw(const Bitmap& d, const int dx, const int dy) const;
	void draw(const Video_bitmap* d, const int dx, const int dy) const;
	void hline(const int y, const int x, const int w, const Byte color) const;
	void vline(const int x, const int y, const int h, const T color) const;
	void line(const int x1, const int y1, const int x2, const int y2,
		const Byte color) const;
	void put_pel(const int x, const int y, const Byte color) const;
	void fast_pel(const int x, const int y, const Byte color) const {
		*(operator[](y)+x) = color;
	}
	void clear(const T color) const;
};

#endif
