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

#include "error.h"
#include "main.h"
#include "input_dumb.h"

Input_Dumb::Input_Dumb() {
	mouse.dx = mouse.dy = mouse.dz = 0;
	mouse.quel = -1;
	int i;
	for(i=0; i<4; i++)
		mouse.button[i] = 0;

	quel_key = -1;
	pause = false;
	for(i=0; i<256; i++)
		keys[i] = 0;
	clear_key();
}

Input_Dumb::~Input_Dumb() {
}

void Input_Dumb::clear_key() {
	process_key(); // empties the key queue
	shift_key = 0;
	quel_key = -1;
	key_pending = 0;
	for(int i=0; i<256; i++)
		keys[i] = 0;
}

void Input_Dumb::process_key() {
}

void Input_Dumb::process_mouse() {
	mouse.dx = mouse.dy = mouse.dz = 0;
}

void Input_Dumb::add_key_buf(char c, bool special) {
  if(key_pending < MAXKEY) {
    key_buf[key_pending].c = c;
    key_buf[key_pending].special = special;
    key_pending++;
  }
}

void Input_Dumb::restore() {
  clear_key();
}
