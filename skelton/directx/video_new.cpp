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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION 0x0300
#include <ddraw.h>

#include "video_dumb.h"
#include "video_dx.h"
#include "video_dx16.h"

RCSID("$Id$")

Video* video = NULL;
static bool video_8bit = true;

Video_bitmap* Video_bitmap::New(const int px, const int py,
		  const int w, const int h, const int rw) {
	if(video_is_dumb)
		return Dumb_Video_bitmap::New(px, py, w, h, rw);
	if(video_8bit)
		return new DirectX_Video_bitmap(px, py, w, h, rw);
	else
		return NULL;
}

Video_bitmap* Video_bitmap::New(const int px, const int py,
		  const int w, const int h) {
	if(video_is_dumb)
		return Dumb_Video_bitmap::New(px, py, w, h);
	if(video_8bit)
		return new DirectX_Video_bitmap(px, py, w, h);
	else
		return NULL;
}

Video* Video::New(int w, int h, int b, const char *wname, bool dumb) {
	if(dumb)
		return Video_Dumb::New(w, h, b, wname);
	if(b == 8) {
		video_8bit = true;
		return new DirectX_Video(w, h, b, wname);
	}	else {
		video_8bit = false;
		return new Video16(w, h, b, wname);
	}
}
