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

#ifdef UGS_LINUX_SVGA

#include "wraplib.h"
#include "video_svga.h"
#include "utils.h"
#include "error.h"
#include "input.h"
#include "main.h"
#include "cursor.h"

RCSID("$Id$")

static Svgalib* lib;

Svgalib_Video_bitmap* Svgalib_Video_bitmap::New(const int px, const int py,
						const int w, const int h,
						const int rw) {
  if(getenv("DISPLAY"))
    return NULL;
  else
    return new Svgalib_Video_bitmap(px, py, w, h, rw);
}

Svgalib_Video_bitmap* Svgalib_Video_bitmap::New(const int px, const int py,
						const int w, const int h) {
  if(getenv("DISPLAY"))
    return NULL;
  else
    return new Svgalib_Video_bitmap(px, py, w, h);
}

Svgalib_Video_bitmap::Svgalib_Video_bitmap(const int px, const int py,
					   const int w, const int h,
					   const int rw) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
}

Svgalib_Video_bitmap::Svgalib_Video_bitmap(const int px, const int py,
					   const int w, const int h) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
}

void Svgalib_Video_bitmap::rect(const int x, const int y,
				const int w, const int h,
				const int color) const {
  if(clip(x, y, w, h))
    return;
  
  lib->gl_fillbox(clip_x1+pos_x, clip_y1+pos_y+((Svgalib_Video*)video)->displayoffset,
	     clip_w, clip_y2-clip_y1+1, color);
}

void Svgalib_Video_bitmap::box(const int x, const int y,
			       const int w, const int h,
			       const int color) const {
  hline(y, x, w, color);
  hline(y+h-1, x, w, color);
  vline(x, y, h, color);
  vline(x+w-1, y, h, color);
}

void Svgalib_Video_bitmap::get_bitmap(const Bitmap* bit, const int x, const int y,
			      const int w, const int h) const {
  if(clip(x, y, w, h))
    return;

  if(clip_x1==x && clip_y1==y &&
     clip_w == bit->width && clip_y2 == y+bit->height-1) {
    lib->gl_getbox(clip_x1+pos_x, clip_y1+pos_y+((Svgalib_Video*)video)->displayoffset, clip_w,
		  clip_y2-clip_y1+1, (*bit)[0]);
  } else {
    int i;
    for(i=clip_y1; i<=clip_y2; i++)
      lib->gl_getbox(clip_x1+pos_x, i+((Svgalib_Video*)video)->displayoffset+pos_y, clip_w, 1,
		    (*bit)[i-y]+clip_x1-x);
  }
}

void Svgalib_Video_bitmap::put_pel(const int x, const int y, const Byte c) const {
  if(clip(x, y, 1, 1))
    return;

  lib->gl_setpixel(clip_x1+pos_x, clip_y1+pos_y+((Svgalib_Video*)video)->displayoffset, c);
}

void Svgalib_Video_bitmap::hline(const int y, const int x,
			 const int w, const Byte c) const {
  if(clip(x, y, w, 1))
    return;

  lib->gl_hline(clip_x1+pos_x, clip_y1+pos_y+((Svgalib_Video*)video)->displayoffset,
	   clip_x2+pos_x, c);
}

void Svgalib_Video_bitmap::vline(const int x, const int y, const int h,
			 const Byte c) const {
  if(clip(x, y, 1, h))
    return;

  lib->gl_line(clip_x1+pos_x, clip_y1+pos_y+((Svgalib_Video*)video)->displayoffset,
	  clip_x1+pos_x, clip_y2+pos_y+((Svgalib_Video*)video)->displayoffset, c);
}

void Svgalib_Video_bitmap::line(const int x1, const int y1,
const int x2, const int y2, const Byte c) const
{
	if(clip(x1, y1, x2, y2))
	return;
  lib->gl_line(clip_x1, clip_y1+((Svgalib_Video*)video)->displayoffset, clip_x2, clip_y2+((Svgalib_Video*)video)->displayoffset, c);
}

void Svgalib_Video_bitmap::put_bitmap(const Bitmap& d, const int dx,
			      const int dy) const {
  if(clip(dx, dy, d.width, d.height))
    return;
  
  if(d.width == d.realwidth) {
    lib->gl_putboxpart(clip_x1+pos_x,
		  clip_y1+pos_y+((Svgalib_Video*)video)->displayoffset,
		  clip_w,
		  clip_y2-clip_y1+1,
		  d.width,
		  d.height,
		  d[clip_y1-dy]+clip_x1-dx,
		  0, 0);
  } else {
    int y;
    for(y=clip_y1; y<=clip_y2; y++)
      lib->gl_putbox(clip_x1+pos_x,
		y+((Svgalib_Video*)video)->displayoffset+pos_y,
		clip_w,
		1,
		d[y-dy]+clip_x1-dx);
  }
}

void Svgalib_Video_bitmap::put_sprite(const Sprite& d, const int dx,
			      const int dy) const {
  int dx2 = dx - d.hot_x;
  int dy2 = dy - d.hot_y;

	int phys_y;

  if(clip(dx2, dy2, d.width, d.height))
    return;

	phys_y=pos_y+((Svgalib_Video*)video)->displayoffset;

  if(clip_x1==dx2 && clip_y1==dy2 &&
     clip_w == d.width && clip_y2 == dy2+d.height-1) {
    lib->gl_putboxmask(clip_x1+pos_x, clip_y1+phys_y, clip_w,
		  clip_y2-clip_y1+1, d[0]);
  } else {
    int y;
    for(y=clip_y1; y<=clip_y2; y++)
      lib->gl_putboxmask(clip_x1+pos_x, y+phys_y, clip_w, 1,
		    d[y-dy2]+clip_x1-dx2);
  }
}

void Svgalib_Video_bitmap::setmem() {
}

Svgalib_Video* Svgalib_Video::New(int w, int h, int b, const char *wname) {
  if(getenv("DISPLAY"))
    return NULL;

  lib = getSvgalib();
  if(!lib)
    return NULL;

  return new Svgalib_Video(w, h, b, wname);
}

Svgalib_Video::Svgalib_Video(int w, int h, int b, const char *wname) {
  xwindow = false;
  width = w;
  height = h;
  bit = b;
  framecount = 0;
  newpal = true;
  need_paint = 2;

  lib->vga_setmousesupport(1);

  if(lib->vga_init() != 0)
    (void)new Error("Fatal: error initializing SVGALib!\n");
  
  if(lib->vga_setmode(10 /* G640x480x256 */) != 0)
    (void)new Error("Fatal: could not set 640x480x256 SVGA mode. Check SVGALib configuration\n");
  
  lib->gl_setcontextvga(10 /* G640x480x256 */);

  vb = Video_bitmap::New(0, 0, w, h);
  
  displayoffset = (vb->height);
}

Svgalib_Video::~Svgalib_Video() {
  if(vb)
    delete vb;

  lib->vga_setmode(0 /* TEXT */);
}

void Svgalib_Video::lock() {
  vb->setmem();
}

void Svgalib_Video::unlock() {
}

void Svgalib_Video::flip() {
  lib->gl_setdisplaystart(0, displayoffset);
  displayoffset = (vb->height) - displayoffset;

  lib->vga_waitretrace();

  if(newpal) {
    pal.set();
    newpal = false;
  }

  framecount++;
}

void Svgalib_Video::setpal(const Palette& p) {
  pal = p;
  newpal=true;
}

void Svgalib_Video::dosetpal(PALETTEENTRY pal[256], int size) {
  int i;
  gl_palette newpal;

  for(i = 0; i < size; i++) {
    newpal.color[i].red = pal[i].peRed >> 2;
    newpal.color[i].green = pal[i].peGreen >> 2;
    newpal.color[i].blue = pal[i].peBlue >> 2;
  }
  lib->gl_setpalettecolors(0, size, &newpal);
}

void Svgalib_Video::start_frame() {
  lock();
  if(cursor) {
    cursor->put_back();
    cursor->move();
  }
}

void Svgalib_Video::end_frame() {
  if(cursor) {
    cursor->get_back();
    cursor->draw();
  }
  flip();
}

void Svgalib_Video::restore() {
  newpal = true;
  need_paint = 2;
  /* FIXME: ne devrait pas être nécessaire, probablement lié au
     trouble de signals dans Input_Svgalib. */
  lib->vga_setmode(10 /* G640x480x256 */);
}

void Svgalib_Video::clean_up() {
}

void Svgalib_Video::snap_shot(int x, int y, int w, int h) {
}

#endif /* UGS_LINUX_SVGA */

