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

#include "raw.h"

#include <string.h>
#include <stdio.h>
#include "error.h"
#include "res.h"

char Raw::Head::signature[]={0x6d, 0x68, 0x77, 0x61, 0x6e, 0x68};

#define swap(A) ((Word) (((((Word)A)>>8)   )| ((((Word)A)  )<<8)))
void Raw::Head::xlat() {
	version=swap(version);
	width=swap(width);
	height=swap(height);
	palettesize=swap(palettesize);
	HDPI=swap(HDPI);
	VDPI=swap(VDPI);
	gamma=swap(gamma);
}
#undef swap

Raw::Raw(Res& res) {
	res.read(&h, sizeof(h));
	h.xlat();
	if(strncmp(h.sig, Head::signature, sizeof(h.sig)))
		fatal_msgbox("Invalid raw image file");
	if(h.palettesize<1) {
		pal_=NULL;
		pic_=new Byte[h.width*h.height*2];
		Byte *temp=new Byte[h.width*h.height*3];
		if(pic_==NULL || temp==NULL)
			fatal_msgbox("Not enough memory to load image");
		res.read(temp, h.width*h.height*3);
		Byte r,g,b;
		for(int y=0; y<h.height; y++)
			for(int x=0; x<h.width; x++) {
				b = (Byte) (temp[(x+y*h.width)*3]>>3);
				g = (Byte) (temp[(x+y*h.width)*3+1]>>2);
				r = (Byte) (temp[(x+y*h.width)*3+2]>>3);
				*(((Word *)pic_)+(x+y*h.width)) = (Word) (r+(g<<5)+(b<<11));
			}
		delete(temp);
	} else {
		pal_=new Byte[h.palettesize*3];
		if(pal_==NULL)
			fatal_msgbox("Not enough memory to load image");
		res.read(pal_, h.palettesize*3);
		pic_=new Byte[h.width*h.height];
		if(pic_==NULL)
			fatal_msgbox("Not enough memory to load image");
		res.read(pic_, h.width*h.height);
	}
}

Raw::Raw(int w, int h2, int ps) {
	memcpy(h.sig, Head::signature, sizeof(Head::signature));
	h.width = w;
	h.height = h2;
	h.palettesize = ps;
	h.version = 4;
	h.HDPI = h.VDPI = 300;
	h.gamma = 0;
	memset(h.reserved, 0, sizeof(h.reserved));
	pal_=NULL;
	pic_=NULL;
}

Raw::~Raw() {
	if(pic_)
		delete[] pic_;
	if(pal_)
		delete[] pal_;
}

void Raw::write(Res_dos& res) {
	h.xlat();
	res.write(&h, sizeof(h));
}
