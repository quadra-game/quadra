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

#ifndef _HEADER_PCX
#define _HEADER_PCX

#include <stdint.h>

#include "res.h"
#include "image.h"

#pragma pack(1)
class Pcx: public Image {
	struct Head {
		uint8_t manufacturer;
		uint8_t version;
		uint8_t encoding;
		uint8_t bpp;
		uint16_t x1,y1,x2,y2;
		uint16_t hdpi, vdpi;
		uint8_t colormap[48];
		uint8_t reserved;
		uint8_t nplane;
		uint16_t byteperline;
		uint16_t paletteinfo;
		uint16_t screensizeh, screensizev;
		uint8_t filler[54];
	} h;
	int width_, height_;
	uint8_t* pic_;
	uint8_t* pal_;
 public:
	Pcx(Res& res);
	virtual ~Pcx();
	int width() const { return width_; }
	int height() const { return height_; }
	uint8_t* pic() const { return pic_; }
	uint8_t* pal() const { return pal_; }
	int palettesize() const { return 256; }
};
#pragma pack()

#endif
