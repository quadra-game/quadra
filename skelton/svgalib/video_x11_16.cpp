/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <signal.h>
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

Video_X11_16::Video_X11_16(int w, int h, int b,
			   const char *wname,
			   Display* dpy,
			   Visual* vis,
			   int dp):
  Video_X11(w, h, b, wname, dpy, vis, dp) {
  vfb = (unsigned char*)malloc(w * h);

  if(!vfb)
    (void)new Error("Could not allocate virtual frame buffer.");
}

Video_X11_16::~Video_X11_16() {
  if(vfb)
    free(vfb);
}

void Video_X11_16::flip() {
  unsigned short* buf;
  int x, y;

  if(newpal) {
    pal.set();
    newpal = false;
  }

  if(max_x > min_x) {
    // Conversion du buffer 8 bpp vers un buffer 16 bpp.
    buf = (unsigned short*) image->data;
    for(y = 0; y < 480; y++)
      for(x = min_x[y]; x <= max_x[y]; x++) {
        buf[(y * width) + x] = colors[vfb[(y * width) + x]];
      }
    
    /* reset le dirty rect */
    for(y = 0; y < 480; y++) {
      min_x[y] = vb->width;
      max_x[y] = 0;
    }
  }

  Video_X11::flip();

  framecount++;
}

void Video_X11_16::dosetpal(PALETTEENTRY pal[256], int size) {
  XColor color;
  int i;

  color.flags = DoRed|DoGreen|DoBlue;

  for(i = 0; i < size; i++) {
    color.red = (pal[i].peRed << 8) + pal[i].peRed;
    color.green = (pal[i].peGreen << 8) + pal[i].peGreen;
    color.blue = (pal[i].peBlue << 8) + pal[i].peBlue;

    XAllocColor(display,
		DefaultColormap(display, DefaultScreen(display)),
		&color);

    colors[i] = color.pixel;
  }

  dirty(0, 0, width-1, height-1);
}
