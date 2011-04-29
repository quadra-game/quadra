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

#include "clipable.h"

#include "array.h"
#include "bitmap.h"

using std::max;
using std::min;

int Clipable::clip_x1;
int Clipable::clip_x2;
int Clipable::clip_y1;
int Clipable::clip_y2;
int Clipable::clip_w;

bool Clipable::clip(const int x, const int y, const int w, const int h) const {
	clip_x1=max(0, x);
	if(clip_x1>=width)
		return true;
	clip_x2=min(width-1, x+w-1);
	if(clip_x2<0)
		return true;
	clip_y1=max(0, y);
	if(clip_y1>=height)
		return true;
	clip_y2=min(height-1, y+h-1);
	if(clip_y2<0)
		return true;
	clip_w = clip_x2-clip_x1+1;
	return false;
}

bool Clipable::clip(const int x, const int y, const Bitmap *b) const {
	return clip(x, y, b->width, b->height);
}

