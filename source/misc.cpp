/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "input.h"
#include "random.h"
#include "sons.h"
#include "main.h"
#include "global.h"
#include "misc.h"

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
	if(input->quel_key != -1 || (ecran && ecran->clicked)) {
		input->quel_key = -1;
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
	time_control = TIME_FREEZE; // fade toujours sync avec le display
	if(fad->step())
		ret();
	fad->set();
}

void Fade_in::init() {
	Fade_to::init();
	Sfx stmp(sons.fadein, 0, -400, 0, 11000+ugs_random.rnd(511));
}

void Fade_out::init() {
	Fade_to::init();
	Sfx stmp(sons.fadeout, 0, -400, 0, 22000+ugs_random.rnd(511));
}

Setpalette::Setpalette(const Palette& p): pal(p) {
}

void Setpalette::init() {
	Module::init();
	video->setpal(pal);
	ret();
}
