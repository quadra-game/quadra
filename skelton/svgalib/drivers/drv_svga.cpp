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

#include "types.h"
#include "wraplib.h"

RCSID("$Id$")

class SvgalibImpl: public Svgalib {
public:
  virtual int vga_init() {
    return ::vga_init();
  };
  virtual int vga_setmode(int p1) {
    return ::vga_setmode(p1);
  };
  virtual void vga_waitretrace() {
    ::vga_waitretrace();
  };
  virtual void vga_setmousesupport(int p1) {
    ::vga_setmousesupport(p1);
  };
  virtual int vga_getkey() {
    return ::vga_getkey();
  };
  virtual int keyboard_init() {
    return ::keyboard_init();
  };
  virtual void keyboard_close() {
    ::keyboard_close();
  };
  virtual void keyboard_clearstate() {
    ::keyboard_clearstate();
  };
  virtual void keyboard_seteventhandler(__keyboard_handler p1) {
    ::keyboard_seteventhandler(p1);
  };
  virtual void keyboard_translatekeys(int p1) {
    ::keyboard_translatekeys(p1);
  };
  virtual int keyboard_update() {
    return ::keyboard_update();
  };
  virtual void mouse_close() {
    ::mouse_close();
  };
  virtual int mouse_update() {
    return ::mouse_update();
  };
  virtual void mouse_seteventhandler(__mouse_handler p1) {
    ::mouse_seteventhandler(p1);
  };
  virtual void gl_getbox(int p1, int p2, int p3, int p4, void* p5) {
    ::gl_getbox(p1, p2, p3, p4, p5);
  };
  virtual void gl_putboxpart(int p1, int p2, int p3, int p4, int p5,
                             int p6, void* p7, int p8, int p9) {
    ::gl_putboxpart(p1, p2, p3, p4, p5, p6, p7, p8, p9);
  };
  virtual void gl_putbox(int p1, int p2, int p3, int p4, void* p5) {
    ::gl_putbox(p1, p2, p3, p4, p5);
  };
  virtual void gl_putboxmask(int p1, int p2, int p3, int p4, void* p5) {
    ::gl_putboxmask(p1, p2, p3, p4, p5);
  };
  virtual int gl_setcontextvga(int p1) {
    return ::gl_setcontextvga(p1);
  };
  virtual void gl_setdisplaystart(int p1, int p2) {
    ::gl_setdisplaystart(p1, p2);
  };
  virtual void gl_setpalettecolors(int p1, int p2, void* p3) {
    ::gl_setpalettecolors(p1, p2, p3);
  };
  virtual void gl_fillbox(int p1, int p2, int p3, int p4, int p5) {
    ::gl_fillbox(p1, p2, p3, p4, p5);
  };
  virtual void gl_setpixel(int p1, int p2, int p3) {
    ::gl_setpixel(p1, p2, p3);
  };
  virtual void gl_hline(int p1, int p2, int p3, int p4) {
    ::gl_hline(p1, p2, p3, p4);
  };
  virtual void gl_line(int p1, int p2, int p3, int p4, int p5) {
    ::gl_line(p1, p2, p3, p4, p5);
  };
};

extern "C" Svgalib* createSvgalib() {
  return new SvgalibImpl;
}
