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

#ifndef _HEADER_MISC
#define _HEADER_MISC

#include "overmind.h"
#include "palette.h"

class Fade_to: public Module {
	Fade* fad;
public:
	Fade_to(const Palette& dst, const Palette& src, int nframe=16);
	virtual ~Fade_to();
	virtual void step();
};

class Fade_in: public Fade_to {
public:
	Fade_in(const Palette& dst): Fade_to(dst, noir) { }
	virtual void init();
};

class Fade_out: public Fade_to {
public:
	Fade_out(const Palette& src): Fade_to(noir, src) { }
	virtual void init();
};

class Setpalette: public Module {
	const Palette& pal;
public:
	Setpalette(const Palette& p);
	virtual void init();
};

class Wait_event: public Module {
public:
	virtual void step();
};

class Wait_time: public Wait_event {
	int delay;
	bool force_wait;
public:
	Wait_time(int del, bool force=false);
	virtual void step();
};

#endif
