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

#include "video_dumb.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "error.h"
#include "input.h"
#include "main.h"
#include "cursor.h"
#include "net.h"
#include "sprite.h"

bool video_is_dumb=false; //Defaults to false

Dumb_Video_bitmap::Dumb_Video_bitmap(const int px, const int py,
					   const int w, const int h)
  : Video_bitmap(px, py, w, h) {
}

void Dumb_Video_bitmap::rect(const int x, const int y,
				const int w, const int h,
				const int color) const {
  clip(x, y, w, h);
}

void Dumb_Video_bitmap::put_pel(const int x, const int y, const uint8_t c) const {
  clip(x, y, 1, 1);
}

void Dumb_Video_bitmap::hline(const int y, const int x,
			 const int w, const uint8_t c) const {
  clip(x, y, w, 1);
}

void Dumb_Video_bitmap::vline(const int x, const int y, const int h,
			 const uint8_t c) const {
  clip(x, y, 1, h);
}

void Dumb_Video_bitmap::put_bitmap(const Bitmap& d, const int dx,
			      const int dy) const {
  clip(dx, dy, d.width, d.height);
}

void Dumb_Video_bitmap::put_sprite(const Sprite& d, const int dx,
			      const int dy) const {
  int dx2 = dx - d.hot_x;
  int dy2 = dy - d.hot_y;

  clip(dx2, dy2, d.width, d.height);
}

#ifdef WIN32
LRESULT CALLBACK dumbwindowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_USER:
			if(net) {
				int err = WSAGETASYNCERROR(lparam);
				net->gethostbyname_completed(err == 0);
			}
			return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
#endif

Video_Dumb::Video_Dumb(int w, int h, const char *wname)
  : Video(new Dumb_Video_bitmap(0, 0, w, h), w, h, w) {
	video_is_dumb = true;

#if 0
#ifdef WIN32
	BOOL rc;
 	WNDCLASS wc;
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = dumbwindowproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinst;
	wc.hIcon = LoadIcon(hinst, "window.ico");
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "SkeltonClass";
	rc = RegisterClass(&wc);
	if(!rc)
		fatal_msgbox("Can't register class");
	hwnd = CreateWindowEx(0, "SkeltonClass", wname, WS_MINIMIZE|WS_POPUPWINDOW|WS_CAPTION|WS_SYSMENU, 100, 100, 200, 50, NULL, NULL, hinst, NULL);
	if(hwnd == NULL)
		fatal_msgbox("Can't create window");
#endif
#endif
}

Video_Dumb::~Video_Dumb() {
#if 0
#ifdef WIN32
	ShowWindow(hwnd, SW_HIDE);
	DestroyWindow(hwnd);
	UnregisterClass("SkeltonClass", hinst);
#endif
#endif
}

void Video_Dumb::dosetpal(const SDL_Color pal[256], int size) {
}

void Video_Dumb::end_frame() {
  if(newpal)
    newpal = false;

#ifndef WIN32
	// Shouldn't we do something on Windows too?
  usleep(1);
#endif

  framecount++;
}

void Video_Dumb::snap_shot(int x, int y, int w, int h) {
}

Video_bitmap* Video_Dumb::new_bitmap(int px, int py, int w, int h) {
  return new Dumb_Video_bitmap(px, py, w, h);
}
