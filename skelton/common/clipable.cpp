/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "array.h"
#include "bitmap.h"
#include "clipable.h"

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

bool Clipable::clip(const int x, const int y, const Bitmap &b) const {
	return clip(x, y, b.width, b.height);
}

bool Clipable::clip(const int x, const int y, const Bitmap *b) const {
	return clip(x, y, b->width, b->height);
}
