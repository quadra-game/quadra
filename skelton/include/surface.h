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

#ifndef _HEADER_SURFACE
#define _HEADER_SURFACE

class Res;

class Surface {
public:
	enum Dither_mode {
		NONE,
		RANDOM,
		ORDERED
	};
	int width, height;
	bool transparent;
	int trans_color;
	static Surface *New_video(int w, int h);
	static Surface *New(int w, int h);
	virtual ~Surface() { };
	virtual void upload(void *src, int w, int h)=0;
	virtual void upload()=0;
	virtual void rect(int x, int y, int w, int h, int color)=0;
	virtual bool clip(int *x, int *y, int *w, int *h, int *start_x=NULL, int *start_y=NULL)=0;
	virtual void blit(Surface *dest, int x, int y)=0;
	virtual void load24rgb(Res &res, Dither_mode dither=NONE)=0;
	virtual void load24rgb(void *mem, Dither_mode dither=NONE)=0;
	virtual void restore()=0;
	virtual void start_fade()=0;
	virtual void fade_down(int c)=0;
	virtual void end_fade()=0;
	virtual void set_mask_color(int color)=0;
};

extern Surface *screen;

#endif
