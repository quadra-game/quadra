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

#ifndef _HEADER_INPUT_SVGA
#define _HEADER_INPUT_SVGA

#include <signal.h>
#include <termios.h>
#include "input.h"

class Input_Svgalib: public Input {
private:
  bool israw;
  bool mouse_reinit;
  struct sigaction oldsignals;
  struct termios termattr;
  int tty_fd;
  void process_key();
  void process_mouse();
  void restore_mouse();
  static void mouse_handler(int button,
			    int dx, int dy, int dz,
			    int drx, int dry, int drz);
  static void keyboard_handler(int scancode, int press);
  static void signal_handler(int signal);
public:
  Input_Svgalib();
  virtual ~Input_Svgalib();
  virtual void clear_key();
  virtual void check();
  virtual void deraw();
  virtual void reraw();
};

#endif /* _HEADER_INPUT_SVGA */
