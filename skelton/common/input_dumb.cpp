/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
	process_key(); // vide la queue de touche 
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
