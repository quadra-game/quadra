/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
