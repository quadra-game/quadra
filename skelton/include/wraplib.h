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

#ifndef _HEADER_WRAPLIB
#define _HEADER_WRAPLIB

#ifdef UGS_LINUX_SVGA

#define Palette gl_palette
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>
#include <vgakeyboard.h>
#undef Palette

class Svgalib {
public:
  virtual int vga_init() = 0;
  virtual int vga_setmode(int) = 0;
  virtual void vga_waitretrace() = 0;
  virtual void vga_setmousesupport(int) = 0;
  virtual int vga_getkey() = 0;
  virtual int keyboard_init() = 0;
  virtual void keyboard_close() = 0;
  virtual void keyboard_clearstate() = 0;
  virtual void keyboard_seteventhandler(__keyboard_handler) = 0;
  virtual void keyboard_translatekeys(int) = 0;
  virtual int keyboard_update() = 0;
  virtual void mouse_close() = 0;
  virtual int mouse_update() = 0;
  virtual void mouse_seteventhandler(__mouse_handler) = 0;
  virtual void gl_getbox(int, int, int, int, void*) = 0;
  virtual void gl_putboxpart(int, int, int, int, int,
                             int, void*, int, int) = 0;
  virtual void gl_putbox(int, int, int, int, void*) = 0;
  virtual void gl_putboxmask(int, int, int, int, void*) = 0;
  virtual int gl_setcontextvga(int) = 0;
  virtual void gl_setdisplaystart(int, int) = 0;
  virtual void gl_setpalettecolors(int, int, void*) = 0;
  virtual void gl_fillbox(int, int, int, int, int) = 0;
  virtual void gl_setpixel(int, int, int) = 0;
  virtual void gl_hline(int, int, int, int) = 0;
  virtual void gl_line(int, int, int, int, int) = 0;
};

Svgalib* getSvgalib();

#endif /* UGS_LINUX_SVGA */

#endif /* _HEADER_WRAPLIB */
