/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION 0x0300
#include <ddraw.h>

#include "video_dumb.h"
#include "video_dx.h"
#include "video_dx16.h"

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
