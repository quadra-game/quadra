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

#ifndef _HEADER_SVGALIB
#define _HEADER_SVGALIB

extern bool svgalib_ok;

void load_svgalib();

/* types */

typedef void (*__mouse_handler) (int button, int dx, int dy, int dz,
				 int drx, int dry, int drz);

typedef void (*__keyboard_handler) (int, int);

typedef struct {
  struct {
    unsigned char red;  /* 6-bit values */
    unsigned char green;
    unsigned char blue;
  } color[256];
} gl_palette;

/* functions */

extern void (*mouse_seteventhandler)(__mouse_handler);
extern int (*mouse_update)(void);
extern void (*mouse_close)(void);

extern int (*vga_init)(void);
extern void (*vga_waitretrace)(void);
extern void (*vga_setmousesupport)(int);
extern int (*vga_getkey)(void);
extern int (*vga_setmode)(int);

extern int (*gl_setcontextvga)(int);
extern void (*gl_setdisplaystart)(int, int);
extern void (*gl_setpalettecolors)(int, int, void*);
extern void (*gl_setpixel)(int, int, int);
extern void (*gl_hline)(int, int, int, int);
extern void (*gl_line)(int, int, int, int, int);
extern void (*gl_putboxmask)(int, int, int, int, void*);
extern void (*gl_putboxpart)(int, int, int, int, int, int, void*, int, int);
extern void (*gl_putbox)(int, int, int, int, void*);
extern void (*gl_fillbox)(int, int, int, int, int);
extern void (*gl_getbox)(int, int, int, int, void*);

extern int (*keyboard_init)(void);
extern void (*keyboard_close)(void);
extern void (*keyboard_clearstate)(void);
extern void (*keyboard_seteventhandler)(__keyboard_handler);
extern void (*keyboard_translatekeys)(int);
extern int (*keyboard_update)(void);

#endif /* _HEADER_SVGALIB */
