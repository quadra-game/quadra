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

#include <assert.h>
#include "sprite.h"

#include "types.h"
#include "bitmap.h"
#include "video.h"
#include "SDL_endian.h"

using std::max;

#define FONT_SIZE (141-32)

Fontdata::Fontdata(Res &res, int s) {
	shrink = s;
	int w, h, rw;
  memset(spr, 0, sizeof(spr));
	for(int i=0; i<FONT_SIZE; i++) {
		res.read(&w, sizeof(int));
                w = SDL_SwapLE32(w);
		if(w != 0) {
			res.read(&h, sizeof(int));
                        h = SDL_SwapLE32(h);
			res.read(&rw, sizeof(int));
                        rw = SDL_SwapLE32(rw);

      char* buf = new char[h * rw];
      res.read(buf, h * rw);

      spr[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 0, 0, 0, 0);
      assert(spr[i]);

      int retval = SDL_LockSurface(spr[i]);
      assert(retval == 0);

      for (int row = 0; row < h; ++row)
        memcpy(static_cast<char*>(spr[i]->pixels) + (row * spr[i]->pitch),
               buf + (row * rw), w);

      SDL_UnlockSurface(spr[i]);
      SDL_SetColorKey(spr[i], SDL_SRCCOLORKEY, 0);

      delete[] buf;

			pre_width[i] = max(spr[i]->w - shrink, 3);
		} else {
			spr[i] = NULL;
			pre_width[i] = 0;
		}
	}
}

Fontdata::Fontdata(const Fontdata &o) {
  memset(spr, 0, sizeof(spr));

	for(int i=0; i<FONT_SIZE; i++) {
		if(o.spr[i]) {
		  spr[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, o.spr[i]->w, o.spr[i]->h,
		                                8, 0, 0, 0, 0);
      SDL_SetColorKey(spr[i], SDL_SRCCOLORKEY, 0);
      SDL_BlitSurface(o.spr[i], NULL, spr[i], NULL);
		} else
			spr[i] = NULL;
		pre_width[i] = o.pre_width[i];
	}
	shrink = o.shrink;
}

Fontdata::~Fontdata() {
	for(int i=0; i<FONT_SIZE; i++)
		if(spr[i])
			SDL_FreeSurface(spr[i]);
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
		SDL_Surface *spr = fdata_original.spr[i];
		if(spr) {
		  int retval = SDL_LockSurface(spr);
		  assert(retval == 0);
		  retval = SDL_LockSurface(fdata.spr[i]);
		  assert(retval == 0);

      Byte* srcpixels = static_cast<Byte*>(spr->pixels);
      Byte* dstpixels = static_cast<Byte*>(fdata.spr[i]->pixels);
      for (int row = 0; row < spr->h; ++row)
        for (int x = 0; x < spr->w; ++x) {
          Byte pixel = srcpixels[row * spr->pitch + x];
          if (pixel != 0)
            dstpixels[row * fdata.spr[i]->pitch + x] = map->map[pixel];
        }

      SDL_UnlockSurface(fdata.spr[i]);
      SDL_UnlockSurface(spr);
		}
	}
}

void Font::draw(const char *m, const Video_bitmap& b, int x, int y) const {
	int c;
	if(x == CENTER) {
		x = (b.width - fdata.width(m)) >> 1;
	}

  for (int i = 0; i < 256; ++i)
    if (fdata.spr[i])
		video->clone_palette(fdata.spr[i]);

	while(*m) {
		c = fdata.translate(&m);
		if(c < 0) {
			c = 'i'-33;
		} else {
			if(fdata.spr[c])
				b.put_surface(fdata.spr[c], x, y);
		}
		x += fdata.pre_width[c];
	}
}
