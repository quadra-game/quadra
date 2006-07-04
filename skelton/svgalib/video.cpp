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

#include "autoconf.h"
#include "video.h"
#include "video_dumb.h"
#include "video_x11.h"

RCSID("$Id$")

Video* video = NULL;

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h, const int rw) {
  Video_bitmap* obj;

#ifndef X_DISPLAY_MISSING
  if((obj = Video_bitmap_X11::New(px, py, w, h, rw)))
    return obj;
  else
#endif /* X_DISPLAY_MISSING */
    return NULL;
}

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h) {
  Video_bitmap* obj;

#ifndef X_DISPLAY_MISSING
  if((obj = Video_bitmap_X11::New(px, py, w, h)))
    return obj;
  else
#endif /* X_DISPLAY_MISSING */
    return NULL;
}

Video* Video::New(int w, int h, int b, const char *wname, bool dumb) {
	if(dumb)
		return Video_Dumb::New(w, h, b, wname);
  Video* obj;
#ifndef X_DISPLAY_MISSING
  if((obj = Video_X11::New(w, h, b, wname))) {
    return obj;
  } else
#endif /* X_DISPLAY_MISSING */
    return NULL;
}

