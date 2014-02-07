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

#ifdef HAVE_CONFIG_H
#include "autoconf.h"
#endif

#include "video.h"
#include "video_dumb.h"
#include "video_x11.h"

Video* video = NULL;

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h, const int rw) {
  return Video_bitmap_X11::New(px, py, w, h, rw);
}

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h) {
  return Video_bitmap_X11::New(px, py, w, h);
}

void Video_bitmap::box(const int x, const int y, const int w, const int h,
                       const int color) const {
  hline(y, x, w, color);
  hline(y + h - 1, x, w, color);
  vline(x, y, h, color);
  vline(x + w - 1, y, h, color);
}

Video* Video::New(int w, int h, const char *wname, bool dumb) {
  Video* obj;

	if (dumb)
		obj = new Video_Dumb(w, h, wname);
  else
    obj = Video_X11::New(w, h, wname);

  return obj;
}

