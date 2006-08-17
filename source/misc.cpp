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

#include "misc.h"

#include "input.h"
#include "random.h"
#include "sons.h"
#include "main.h"
#include "global.h"

Wait_time::Wait_time(int del, bool force) {
	force_wait = force;
	delay = del;
}

void Wait_time::step() {
	if(!force_wait)
		Wait_event::step();
	delay--;
	if(delay < 0 || quitting)
		ret();
}

void Wait_event::step() {
	if(ecran)
		ecran->do_frame();
	if(input->last_key.sym != SDLK_UNKNOWN || (ecran && ecran->clicked)) {
		input->last_key.sym = SDLK_UNKNOWN;
		ret();
	}
}

Fade_to::Fade_to(const Palette& dst, const Palette& src, int nframe) {
	fad = new Fade(dst, src, nframe);
}

Fade_to::~Fade_to() {
	delete fad;
}

void Fade_to::step() {
	if(quitting) {
		ret();
		return;
	}
	time_control = TIME_FREEZE; // always fade the sync with the display
	if(fad->step())
		ret();
	fad->set();
}

void Fade_in::init() {
	Fade_to::init();
  Sound::play(sons.fadein, -400, 0, 11000 + ugs_random.rnd(511));
}

void Fade_out::init() {
	Fade_to::init();
  Sound::play(sons.fadeout, -400, 0, 22000 + ugs_random.rnd(511));
}

Setpalette::Setpalette(const Palette& p): pal(p) {
}

void Setpalette::init() {
	Module::init();
	video->setpal(pal);
	ret();
}
