/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "res.h"
#include "pcx24.h"

Pcx24::Pcx24(Res& res) {
	Error *error;
	res.read(&h, sizeof(h));
	if(h.nplane != 3)
		error = new Error("PCX24 need a 3 plane true-color file!");
	if(h.version != 5)
		error = new Error("PCX file must be version 5");
	width_ = h.x2-h.x1+1;
	height_ = h.y2-h.y1+1;
	pic_=new Byte[width_*height_*3];
	Byte *temp=new Byte[h.byteperline*height_*3];
	if(pic_==NULL || temp==NULL)
		error = new Error("Not enough memory to load PCX");
	Byte* buf = (Byte *) res.buf();
	Byte c, num;
	int x,i;
	int total_byte = h.byteperline*3;
	for(i=0; i < height_; i++) {
		x = 0;
		Byte *out = temp + i*total_byte;
		while(x < total_byte) {
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
	for(i=0; i < height_; i++) {
		for(int plane=0; plane<3; plane++) {
			Byte *out = pic_ + i*width_*3 + plane;
			Byte *in = temp + (i*3+plane)*h.byteperline;
			for(x=0; x < width_; x++) {
				*out = *in++;
				out += 3;
			}
		}
	}
	delete[] temp;
}

Pcx24::~Pcx24() {
	if(pic_)
		delete[] pic_;
}
