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

#include "zone_text_clock.h"

RCSID("$Id$")

Zone_text_clock::Zone_text_clock(Inter* in, Dword *s, int px, int py, int pw, bool frame, Font *f2):
	Zone_text_field(in, (int *)s, px, py, pw, f2) {
	draw_frame = frame;
}

Zone_text_clock::~Zone_text_clock() {
}

void Zone_text_clock::draw() {
  Zone_panel::draw();
	if(!draw_frame) {
		video->vb->hline(y, x, w, 255);
		video->vb->hline(y+h-1, x, w, 0);
		video->vb->vline(x, y, h, 255);
		video->vb->vline(x+w-1, y, h, 0);
		video->vb->rect(x+1, y+1, w-2, h-2, 210);
	}

	int seconde = *var % 60;
	int minute = (*var / 60) % 60;
	int heure = *var / 60 / 60;
	sprintf(timebuf, "%02i:%02i:%02i", heure, minute, seconde);
	font->draw(timebuf, pan, w - font->width(timebuf) - 3, 0);  // alignement du texte a droite
}
