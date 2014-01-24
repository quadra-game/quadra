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

#ifndef _HEADER_RAW
#define _HEADER_RAW

#include <stdint.h>

#include "types.h"
#include "res.h"
#include "image.h"

#pragma pack(1)
class Raw: public Image {
	struct Head {
		static char signature[6];
		char sig[6];
		uint16_t version;
		uint16_t width;
		uint16_t height;
		uint16_t palettesize;
		uint16_t HDPI;
		uint16_t VDPI;
		uint16_t gamma;
		char reserved[12];
		void xlat();
	} h;
	uint8_t* pic_;
	uint8_t* pal_;
 public:
	Raw(Res& res);
	Raw(int w, int h, int ps);
	virtual ~Raw();
	int width() const { return h.width; }
	int height() const { return h.height; }
	uint8_t* pic() const { return pic_; }
	uint8_t* pal() const { return pal_; }
	int palettesize() const { return h.palettesize; }
	void write(Res_dos& r);
};
#pragma pack()

#endif
