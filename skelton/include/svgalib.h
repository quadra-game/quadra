/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
