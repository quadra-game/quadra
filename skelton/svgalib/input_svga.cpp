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

const char *keynames[256] = {
  "", "Escape", "1", "2", "3", "4", "5", "6",
  "7", "8", "9", "0", "-", "=", "Backspace", "Tab",
  "Q", "W", "E", "R", "T", "Y", "U", "I",
  "O", "P", "[", "]", "Enter", "Ctrl", "A", "S",
  "D", "F", "G", "H", "J", "K", "L", ";",
  "'", "`", "Left shift", "\\", "Z", "X", "C", "V",
  "B", "N", "M", ",", ".", "/", "Right shift", "Pad *",
  "Alt", "Space", "Caps lock", "F1", "F2", "F3", "F4", "F5",
  "F6", "F7", "F8", "F9", "F10", "Num lock", "Scrl lock", "Pad 7",
  "Pad 8", "Pad 9", "Pad -", "Pad 4", "Pad 5", "Pad 6", "Pad +",
  "Pad 1", "Pad 2", "Pad 3", "Pad 0", "Pad .", "Print scrn", "",
  "<", "F11", "F12", "", "", "", "", "", "", "","Pad Enter",
  "Right Ctrl", "Pad /", "PrintScrn", "Alt Char", "Pause",
  "Home", "Up", "Page Up", "Left", "Right", "End", "Down",
  "Page Down", "Insert", "Delete",
  "", "", "", "", "", "", "", "Pause",
  "", "", "", "", "", "Win left", "Win right", "Win popup",
  "", "Pause", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "Pad Enter", "2nd Ctrl", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "Pad /", "", "",
  "2nd Alt", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "Home",
  "Up", "Page up", "", "Left", "", "Right", "", "End",
  "Down", "Page down", "Insert", "Del", "", "", "", "",
  "", "", "", "Win left", "Win right", "Win popup", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "Macro",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", ""
};

#ifdef UGS_LINUX_SVGA

RCSID("$Id$")

#include "wraplib.h"
#include "video.h"
#include "input_svga.h"

static Svgalib* lib;

Input_Svgalib::Input_Svgalib() {
  struct sigaction newsignals;

  lib = getSvgalib();

  tty_fd = 0;

  pause = false;

  clear_key();

  mouse.dx = mouse.dy = mouse.dz = 0;
  restore_mouse();

  tcgetattr(0, &termattr);

  israw = false;
  reraw();

  newsignals.sa_handler = Input_Svgalib::signal_handler;
  sigemptyset(&newsignals.sa_mask);
  newsignals.sa_flags = 0;
  newsignals.sa_restorer = NULL;
  sigaction(SIGUSR2, &newsignals, &oldsignals);
}

Input_Svgalib::~Input_Svgalib() {
  sigaction(SIGUSR2, &oldsignals, NULL);
  lib->mouse_close();
  deraw();

  tcsetattr(0, TCSANOW, &termattr);
}

void Input_Svgalib::clear_key() {
  shift_key = 0;
  quel_key = -1;
  key_pending = 0;
  for(int i=0; i<256; i++)
    keys[i] = 0;
  if(israw)
    lib->keyboard_clearstate();
}

void Input_Svgalib::check() {
  if(video) {
    if(mouse_reinit) {
      video->restore();
      restore_mouse();
    }
  }

  process_key();
  process_mouse();
}

void Input_Svgalib::deraw() {
  if(israw) {
    lib->keyboard_close();
    israw=false;
  }
}

void Input_Svgalib::reraw() {
  if(!israw) {
    lib->keyboard_init();
    lib->keyboard_seteventhandler(Input_Svgalib::keyboard_handler);
    lib->keyboard_translatekeys(8 /* DONT_CATCH_CTRLC */);
    israw=true;
  }
}

void Input_Svgalib::process_key() {
  int thekey;

  if(israw)
    lib->keyboard_update();
  else {
    fflush(stdin);
    thekey = lib->vga_getkey();
    if(thekey) {
      switch (thekey) {
      case 27:
	quel_key = KEY_ESCAPE;
	break;
      case 13:
      case 10:
	quel_key = KEY_ENTER;
	break;
      default:
	if(key_pending < MAXKEY) {
	  key_buf[key_pending].c = thekey;
	  key_buf[key_pending].special = false;
	  key_pending++;
	}
      }
    }
  }
}

void Input_Svgalib::process_mouse() {
  mouse.dx = mouse.dy = mouse.dz = 0;
  lib->mouse_update();
}

void Input_Svgalib::restore_mouse() {
  mouse_reinit = false;
  lib->mouse_seteventhandler((void*) Input_Svgalib::mouse_handler);
  mouse.quel = -1;
  for(int i=0; i<4; i++)
    mouse.button[i] = RELEASED;
}

void Input_Svgalib::mouse_handler(int button,
				  int dx, int dy, int dz,
				  int drx, int dry, int drz) {
  input->mouse.dx += dx;
  input->mouse.dy += dy;
  input->mouse.dz += dz;

  if(input->mouse.button[0] == RELEASED && button & 4)
    input->mouse.quel = 0;
  if(input->mouse.button[1] == RELEASED && button & 2)
    input->mouse.quel = 1;
  if(input->mouse.button[2] == RELEASED && button & 1)
    input->mouse.quel = 2;

  input->mouse.button[0] = (Byte) ((button & 4) ? PRESSED:RELEASED);
  input->mouse.button[1] = (Byte) ((button & 2) ? PRESSED:RELEASED);
  input->mouse.button[2] = (Byte) ((button & 1) ? PRESSED:RELEASED);
}

void Input_Svgalib::keyboard_handler(int scancode, int press) {
  if(press) {
    input->keys[scancode] |= PRESSED;
    switch(scancode) {
    case KEY_RSHIFT:
    case KEY_LSHIFT:
      input->shift_key |= SHIFT;
      break;
    case KEY_RALT:
    case KEY_LALT:
      input->shift_key |= ALT;
      break;
    case KEY_RCTRL:
    case KEY_LCTRL:
      input->shift_key |= CONTROL;
      break;
    case 101:
    case 119:
      input->pause = true;
      break;
    default:
      input->quel_key = scancode;
    }
  } else {
    input->keys[scancode] = RELEASED;
    switch(scancode) {
    case KEY_RSHIFT:
    case KEY_LSHIFT:
      input->shift_key &= ~SHIFT;
      break;
    case KEY_RALT:
    case KEY_LALT:
      input->shift_key &= ~ALT;
      break;
    case KEY_RCTRL:
    case KEY_LCTRL:
      input->shift_key &= ~CONTROL;
      break;
    }
  }
}

void Input_Svgalib::signal_handler(int signal) {
  /* indique que la souris devra etre re-initer au prochain refresh */
  ((Input_Svgalib*)input)->mouse_reinit = true;
	((Input_Svgalib*)input)->oldsignals.sa_handler(signal);
}

#endif /* UGS_LINUX_SVGA */

