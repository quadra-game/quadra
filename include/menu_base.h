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

#ifndef _HEADER_MENU_BASE
#define _HEADER_MENU_BASE

#include "overmind.h"
#include "misc.h"
class Inter;
class Zone;
class Bitmap;
class Font;

class Menu_quit: public Menu {
protected:
	bool quit;
public:
	Menu_quit(Inter *base=NULL): Menu(base) {
		quit = false;
	}
	virtual void step();
};

class Menu_standard: public Menu_quit {
public:
	Menu_standard(Inter *base=NULL): Menu_quit(base) { }
	virtual void init() {
		Menu_quit::init();
		call(new Fade_in(pal));
	}
};

class Menu_fadein: public Menu {
public:
	Menu_fadein(Inter *base=NULL): Menu(base) { }
	virtual void init() {
		Menu::init();
		call(new Fade_in(pal));
	}
};

class Menu_net_problem: public Menu {
	Zone *cancel;
public:
	Menu_net_problem(const char *s, const char *context, Bitmap *bit, Font *font);
	virtual void step();
};

class Call_setfont: public Module {
	Module *module;
	const Palette &pal;
public:
	Call_setfont(const Palette &p, Module *m);
	virtual void init();
	virtual void step();
};

#endif
