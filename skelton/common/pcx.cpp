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

#include "pcx.h"

RCSID("$Id$")

Pcx::Pcx(Res& res) {
	Error *error;
	res.read(&h, sizeof(h));
	if(h.nplane != 1)
		error = new Error("This PCX file has more than 1 plane!");
	if(h.version != 5)
		error = new Error("PCX file must be version 5");
	width_ = h.x2-h.x1+1;
	height_ = h.y2-h.y1+1;
	pic_=new Byte[width_*height_];
	if(pic_==NULL)
		error = new Error("Not enough memory to load PCX");
	pal_=new Byte[256*3];
	if(pal_==NULL)
		error = new Error("Not enough memory to load PCX");
	Byte* buf = (Byte *) res.buf();
	Byte c, num;
	Byte* out = pic_;
	int x,i;
	if(width_ == h.byteperline) { // unpack faster if it is the right width
		for(i=0; i < height_; i++) {
			x = 0;
			while(x < width_) {
				c = *buf++;
				if(0xC0 == (0xC0 & c)) {
					num = (Byte) (0x3F & c);
					c = *buf++;
					x += num;
					while(num--)
						*out++ = c;
				} else {
					*out++ = c;
					x++;
				}
			}
		}
	} else { // damn shitty PCX with bad bytes at the end of the lines!
		for(i=0; i < height_; i++) {
			x = 0;
			while(x < h.byteperline) {
				c = *buf++;
				if(0xC0 == (0xC0 & c)) {
					num = (Byte) (0x3F & c);
					c = *buf++;
					x += num;
					if(x > width_) {
						num = (Byte) (num - (x-width_));
					}
					while(num--)
						*out++ = c;
				} else {
					if(x < width_)
						*out++ = c;
					x++;
				}
			}
		}
	}
	if(*buf++ != 12)
		error = new Error("Can't find palette in PCX file");
	cpy(pal_, buf, 768);
}

Pcx::~Pcx() {
	if(pic_)
		delete[] pic_;
	if(pal_)
		delete[] pal_;
}
