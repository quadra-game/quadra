/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
