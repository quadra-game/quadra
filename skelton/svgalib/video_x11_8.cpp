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

#include <signal.h>
#include "autoconf.h"
#ifndef X_DISPLAY_MISSING
#define Font XFont
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#undef Font
#include "debug.h"
#include "main.h"
#include "bitmap.h"
#include "sprite.h"
#include "video_x11.h"

Video_X11_8::Video_X11_8(int w, int h, int b,
			 const char *wname,
			 Display* dpy,
			 Visual* vis):
  Video_X11(w, h, b, wname, dpy, vis, 8) {
  int i;

  vfb = (unsigned char*) image->data;

  cmap = XCreateColormap(display,
			 window,
			 visual,
			 AllocAll);

  for(i = 0; i < 256; i++) {
    colors[i].pixel = i;
    colors[i].flags = DoRed | DoGreen | DoBlue;
  }

  XSetWindowColormap(display, window, cmap);
}

Video_X11_8::~Video_X11_8() {
  if(cmap)
    XFreeColormap(display, cmap);
}

void Video_X11_8::flip() {
  short y;

  if(newpal) {
    pal.set();
    newpal = false;
  }

  /* reset the dirty rect */
  for(y = 0; y < 480; y++) {
    min_x[y] = vb->width;
    max_x[y] = 0;
  }

  Video_X11::flip();

  framecount++;
}

void Video_X11_8::dosetpal(PALETTEENTRY pal[256], int size) {
  int i;

  for(i = 0; i < size; i++) {
    colors[i].red = (pal[i].peRed << 8) + pal[i].peRed;
    colors[i].green = (pal[i].peGreen << 8) + pal[i].peGreen;
    colors[i].blue = (pal[i].peBlue << 8) + pal[i].peBlue;
  }

  XStoreColors(display, cmap, colors, size);
}

#endif /* X_DISPLAY_MISSING */
