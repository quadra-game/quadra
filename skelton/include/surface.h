/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
