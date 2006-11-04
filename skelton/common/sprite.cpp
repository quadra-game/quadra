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

#include "sprite.h"

#include "types.h"
#include "bitmap.h"
#include "video.h"
#include "SDL.h"

#define FONT_SIZE (141-32)

void Sprite::set_hotspot(const int hx, const int hy) {
	if(hx == CENTER)
		hot_x = width>>1;
	else
		if(hx == CORNER)
			hot_x = width-1;
		else
			hot_x = hx;
	if(hy == CENTER)
		hot_y = height>>1;
	else
		if(hy == CORNER)
			hot_y = height-1;
		else
			hot_y = hy;
}

Sprite::Sprite(const Bitmap& b, const int hx, const int hy, const bool dx):
	Bitmap(b[0], b.width, b.height, b.realwidth, COPY)
{
	set_hotspot(hx, hy);
}

void Sprite::draw(const Bitmap& d, const int dx, const int dy) const {
	int tx, ty;
	tx = dx-hot_x;
	ty = dy-hot_y;
	if(d.clip(tx, ty, this))
		return;
	for(int y=clip_y1; y<=clip_y2; y++) {
		for(int i=clip_x1; i<=clip_x2; i++) {
			Byte pel = *(operator[](y-ty)+(i-tx));
			// optimization since the mask is always 0
			// because of Svgalib
			if(pel)
			  d.fast_pel(i, y, pel);
		}
	}
}

void Sprite::draw(const Video_bitmap* d, const int dx, const int dy) const {
	d->put_sprite(*this, dx, dy);
}

/*void Sprite::color_draw(const Remap& remap, const Bitmap& d, int dx, int dy) const {
	int x1, y1, x2, y2;
	dx -= hot_x;
	dy -= hot_y;
	x1=max(0, dx);
	x2=min(d.width-1, dx+width-1);
	y1=max(0, dy);
	y2=min(d.height-1, dy+height-1);
	if(x1>=d.width || x2<0 || y1>=d.height || y2<0)
		return;
	for(int y(y1); y<=y2; y++)
		for(int i=x1; i<=x2; i++) {
			Byte pel = *(operator[](y-dy)+(i-dx));
			if(pel != mask)
				d.fast_pel(i, y, remap.map[pel]);
		}
}*/

Fontdata::Fontdata(Res &res, int s) {
	Bitmap *tmp;
	shrink = s;
	int w, h, rw;
	for(int i=0; i<FONT_SIZE; i++) {
		res.read(&w, sizeof(int));
                w = SDL_SwapLE32(w);
		if(w != 0) {
			res.read(&h, sizeof(int));
                        h = SDL_SwapLE32(h);
			res.read(&rw, sizeof(int));
                        rw = SDL_SwapLE32(rw);
			tmp = new Bitmap(w, h, rw);
			res.read((*tmp)[0], rw*h);
			spr[i] = new Sprite(*tmp, 0, 0, 0);
			pre_width[i] = max(spr[i]->width - shrink, 3);
			delete tmp;
		} else {
			spr[i] = NULL;
			pre_width[i] = 0;
		}
	}
}

Fontdata::Fontdata(const Fontdata &o) {
	for(int i=0; i<FONT_SIZE; i++) {
		if(o.spr[i]) 
			spr[i] = new Sprite(*o.spr[i], o.spr[i]->hot_x, o.spr[i]->hot_y);
		else
			spr[i] = NULL;
		pre_width[i] = o.pre_width[i];
	}
	shrink = o.shrink;
}

Fontdata::~Fontdata() {
	for(int i=0; i<FONT_SIZE; i++)
		if(spr[i])
			delete spr[i];
}

int Fontdata::width(const char *m) const {
	int c;
	int wid = 0;
	while(*m) {
		c = translate(&m);
		if(c < 0)
			c = 'i'-33;
		wid += pre_width[c];
	}
	return wid+shrink;
}

int Fontdata::width(const char *m, int num) const {
	int c;
	int wid = 0;
	while(*m && num--) {
		c = translate(&m);
		if(c < 0)
			c = 'i'-33;
		wid += pre_width[c];
	}
	return wid+shrink;
}


int Fontdata::translate(const char **m) const {
	int ret;
	char c;
	c = *(*m)++;
	if(c == 32)
		return -1;  // special code for space
	if(c > 32 && c < 127)
		return c-33; // standard value
	switch(c) {
		// ascii 183 followed with a number to do a glyph
		case '·':
			ret=(int) (*(*m)++)-48 + 133;
			if(ret<133 || ret>137)
				return -2;
			break;
		case 'Ç': ret='C'; break;
		case 'ù': ret=139; break;
		case 'û': ret=140; break;
		case 'ü': ret='u'; break;
		case 'é': ret=127; break;
		case 'è': ret=128; break;
		case 'ê': ret=129; break;
		case 'ë': ret=130; break;
		case 'à': ret=131; break;
		case 'â':
		case 'ä': ret='a'; break;
		case 'ç': ret=132; break;
		case 'Ù':
		case 'Û':
		case 'Ü': ret='U'; break;
		case 'À':
		case 'Â':
		case 'Ä': ret='A'; break;
		case 'É':
		case 'È':
		case 'Ê':
		case 'Ë': ret='E'; break;
		case 'ô': ret=138; break;
		case 'ò':
		case 'ö': ret='o'; break;
		case 'ÿ': ret='y'; break;
		case 'Ò':
		case 'Ô':
		case 'Ö': ret='O'; break;
		case 'î': ret=141; break;
		case 'ì':
		case 'ï': ret='i'; break;
		case 'Ì':
		case 'Î':
		case 'Ï': ret='I'; break;
		default: return -2;
	}
	return ret-33;
}

Font::Font(const Fontdata& f):
	fdata_original(f),
	fdata(f) {
}

Font::Font(const Fontdata& f, const Palette& dst, int r, int g, int b, int r2, int g2, int b2):
	fdata_original(f),
	fdata(f) {
	colorize(dst, r,g,b,r2,g2,b2);
}

void Font::colorize(const Palette& dst, int r, int g, int b, int r2, int g2, int b2) {
	Byte i;
	Remap *map = new Remap(dst);
	for(i=0; i<8; i++)
		map->findrgb(i, (Byte) ((r2*(7-i)+r*i)/7), (Byte) ((g2*(7-i)+g*i)/7), (Byte) ((b2*(7-i)+b*i)/7));
	remap(map);
	delete map;
}

void Font::remap(const Remap *map) {
	for(int i=0; i<FONT_SIZE; i++) {
		Sprite *spr = fdata_original.spr[i];
		if(spr) {
			for(int y=0; y<spr->height; y++)
				for(int x=0; x<spr->width; x++) {
					Byte pel = *((*spr)[y]+x);
					if(pel != 0)
						fdata.spr[i]->fast_pel(x, y, map->map[pel]);
				}
		}
	}
}

void Font::draw(const char *m, const Bitmap& b, int x, int y) const {
	int c;
	if(x == CENTER) {
		x = (b.width - fdata.width(m)) >> 1;
	}
	while(*m) {
		c = fdata.translate(&m);
		if(c < 0) {
			c = 'i'-33;
		} else {
			if(fdata.spr[c])
				fdata.spr[c]->draw(b, x, y);
		}
		x += fdata.pre_width[c];
	}
}

void Font::draw(const char *m, const Video_bitmap* b, int x, int y) const {
	int c;
	if(x == CENTER) {
		x = (b->width - fdata.width(m)) >> 1;
	}
	while(*m) {
		c = fdata.translate(&m);
		if(c < 0) {
			c = 'i'-33;
		} else {
			if(fdata.spr[c])
				fdata.spr[c]->draw(b, x, y);
		}
		x += fdata.pre_width[c];
	}
}
