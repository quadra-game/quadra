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

#ifndef _HEADER_SPRITE
#define _HEADER_SPRITE
#include "types.h"
#include "utils.h"
#include "error.h"
#include "raw.h"
#include "bitmap.h"
#include "palette.h"

#define CENTER (-123456)
#define CORNER (-123457)

class Sprite: public Bitmap {
	typedef Byte T;
public:
	int hot_x;
	int hot_y;
	void set_hotspot(const int hx, const int hy);
	Sprite(const Bitmap& b, const int hx=CENTER, const int hy=CENTER, const bool dx=false);
	void draw(const Bitmap& d, const int dx, const int dy) const;
	void draw(const Video_bitmap* d, const int dx, const int dy) const;
	//void color_draw(const Remap& remap, const Bitmap& d, int dx, int dy) const;
};

class Font;

class Fontdata {
	friend class Font;
	Sprite* spr[256]; // attention: il y a du lousse car ils ne servent pas tous
	int shrink; // indique de combien 'overlapper' cette font
	int pre_width[256]; // 'width' pre-calculer des symboles
public:
	Fontdata(Res &res, int s=0);
	Fontdata(const Fontdata &o);
	virtual ~Fontdata();
	int width(const char *m) const;
	int width(const char *m, int num) const;
	int height() const {
		return spr[1]->height;
	}
	int translate(const char **m) const;
};

class Font {
public:
	const Fontdata& fdata_original;
private:
	Fontdata fdata;
public:
	Font(const Fontdata& f);
	Font(const Fontdata& f, const Palette& dst, int r, int g, int b, int r2=0, int g2=0, int b2=0);
	void colorize(const Palette& dst, int r, int g, int b, int r2=0, int g2=0, int b2=0);
	void remap(const Remap *map);
	void draw(const char *m, const Bitmap& b, int x, int y) const;
	void draw(const char *m, const Video_bitmap* b, int x, int y) const;
	int width(const char *m) const {
		return fdata_original.width(m);
	}
	int width(const char *m, int num) const {
		return fdata_original.width(m, num);
	}
	int height() const {
		return fdata_original.height();
	}
};

#endif
