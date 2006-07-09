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

#include "input.h"
#include "inter.h"
#include "texte.h"
#include "global.h"
#include "zone.h"
#include "quadra.h"
#include "menu_base.h"

void Menu_quit::step() {
	Menu::step();
	if(input->quel_key == 1 || quitting)
		quit = true;
	if(quit)
		exec(new Fade_out(pal));
}

Menu_net_problem::Menu_net_problem(const char *s, const char *context, Bitmap *bit, Font *font) {
	inter->set_font(font, false);
	(void)new Zone_bitmap(inter, bit, 0, 0);
	(void)new Zone_text(inter, ST_NETERROR, 120);
	if(!s)	
		s = "Generic network error";
	(void)new Zone_text(inter, s, 160);
	(void)new Zone_text(inter, ST_NETERROR2, 200);
	if(context)
		(void)new Zone_text(inter, context, 240);
	cancel = new Zone_text_button2(inter, bit, font, ST_BACK, 560, 450);
}

void Menu_net_problem::step() {
	Menu::step();
	if(input->quel_key == 1 || result==cancel) {
		input->quel_key = 0;
		ret();
	}
}

Call_setfont::Call_setfont(const Palette &p, Module *m): pal(p) {
	module = m;
}

void Call_setfont::init() {
	call(module);
}

void Call_setfont::step() {
	set_fteam_color(pal);
	video->need_paint=2;
	ret();
}
