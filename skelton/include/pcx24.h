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

#ifndef _HEADER_PCX24
#define _HEADER_PCX24

#include "types.h"
class Res;

#pragma pack(1)
class Pcx24 {
	struct Head {
		Byte manufacturer;
		Byte version;
		Byte encoding;
		Byte bpp;
		Word x1,y1,x2,y2;
		Word hdpi, vdpi;
		Byte colormap[48];
		Byte reserved;
		Byte nplane;
		Word byteperline;
		Word paletteinfo;
		Word screensizeh, screensizev;
		Byte filler[54];
	} h;
	int width_, height_;
	Byte* pic_;
 public:
	Pcx24(Res& res);
	virtual ~Pcx24();
	int width() const { return width_; }
	int height() const { return height_; }
	Byte* pic() const { return pic_; }
};
#pragma pack()

#endif
