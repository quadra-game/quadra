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

Video_X11_24::Video_X11_24(int w, int h, int b,
			   const char *wname,
			   Display* dpy,
			   Visual* vis):
  Video_X11(w, h, b, wname, dpy, vis, 24) {
  vfb = (unsigned char*)malloc(w * h);

  if(!vfb)
    (void)new Error("Could not allocate virtual frame buffer.");
}

Video_X11_24::~Video_X11_24() {
  if(vfb)
    free(vfb);
}

void Video_X11_24::flip() {

#pragma pack(1)
  struct Pixel3 {
    unsigned char red, green, blue;
  };

  struct PixelValue {
    Pixel3 pixel;
    unsigned char pad;
  };
#pragma pack()

  unsigned long* buf32;
  Pixel3* buf24;
  int x, y;

  if(newpal) {
    pal.set();
    newpal = false;
  }
  
  if(max_x > min_x) {
    if(image->bits_per_pixel == 24) {
      /* Conversion 8 bpp to 24bpp. */
      buf24 = (Pixel3*) image->data;
      for(y = 0; y < 480; y++)
        for(x = min_x[y]; x <= max_x[y]; x++)
          buf24[(y * width) + x] =
	    ((PixelValue*)(&colors[vfb[(y * width) +x]]))->pixel;
    } else {
      /* Conversion du buffer 8 bpp vers un buffer 32 bpp. */
      buf32 = (unsigned long*) image->data;
      for(y = 0; y < 480; y++)
        for(x = min_x[y]; x <= max_x[y]; x++)
          buf32[(y * width) + x] =
	    colors[vfb[(y * width) + x]];
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

void Video_X11_24::dosetpal(PALETTEENTRY pal[256], int size) {
  int i;

  for(i = 0; i < size; i++) {
    colors[i] = pal[i].peRed << 16;
    colors[i] += pal[i].peGreen << 8;
    colors[i] += pal[i].peBlue;
  }

  dirty(0, 0, width-1, height-1);
}
