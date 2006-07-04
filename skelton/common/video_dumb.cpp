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
#ifdef UGS_DIRECTX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "utils.h"
#include "error.h"
#include "input.h"
#include "main.h"
#include "net.h"
#include "video_dumb.h"
#include "sprite.h"

RCSID("$Id$")

bool video_is_dumb=false; //Defaults to false

Dumb_Video_bitmap* Dumb_Video_bitmap::New(const int px, const int py,
						const int w, const int h,
						const int rw) {
  return new Dumb_Video_bitmap(px, py, w, h, rw);
}

Dumb_Video_bitmap* Dumb_Video_bitmap::New(const int px, const int py,
						const int w, const int h) {
  return new Dumb_Video_bitmap(px, py, w, h);
}

Dumb_Video_bitmap::Dumb_Video_bitmap(const int px, const int py,
					   const int w, const int h,
					   const int rw) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
}

Dumb_Video_bitmap::Dumb_Video_bitmap(const int px, const int py,
					   const int w, const int h) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
}

void Dumb_Video_bitmap::rect(const int x, const int y,
				const int w, const int h,
				const int color) const {
  clip(x, y, w, h);
}

void Dumb_Video_bitmap::box(const int x, const int y,
			       const int w, const int h,
			       const int color) const {
}

void Dumb_Video_bitmap::get_bitmap(const Bitmap* bit, const int x, const int y,
			      const int w, const int h) const {
  clip(x, y, w, h);
}

void Dumb_Video_bitmap::put_pel(const int x, const int y, const Byte c) const {
  clip(x, y, 1, 1);
}

void Dumb_Video_bitmap::hline(const int y, const int x,
			 const int w, const Byte c) const {
  clip(x, y, w, 1);
}

void Dumb_Video_bitmap::vline(const int x, const int y, const int h,
			 const Byte c) const {
  clip(x, y, 1, h);
}

void Dumb_Video_bitmap::line(const int x1, const int y1,
const int x2, const int y2, const Byte c) const {
	clip(x1, y1, x2, y2);
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

void Dumb_Video_bitmap::setmem() {
}

#ifdef UGS_DIRECTX
LRESULT CALLBACK dumbwindowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
/*		case WM_CHAR:
			if(input)
				((Input_DX *) input)->add_key_buf((char) wparam);
			return 0;*/
/*		case WM_KEYDOWN:
			if(input) {
				if(wparam == 19) // touche 'pause'
					input->pause = true;
				if(wparam >= 16 && wparam <= 46)
					((Input_DX *) input)->add_key_buf((char) wparam, true);
			}
			return 0;*/
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

Video_Dumb* Video_Dumb::New(int w, int h, int b, const char *wname) {
  return new Video_Dumb(w, h, b, wname);
}

Video_Dumb::Video_Dumb(int w, int h, int b, const char *wname) {
	video_is_dumb=true;

#ifdef UGS_LINUX
  setuid(getuid());
  setgid(getgid());
  seteuid(getuid());
  setegid(getgid());
#endif

  xwindow = false;
  width = w;
  height = h;
  bit = b;
  framecount = 0;
  newpal = true;
  need_paint = 2;

#ifdef UGS_DIRECTX
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
		new Error("Can't register class");
	hwnd = CreateWindowEx(0, "SkeltonClass", wname, WS_MINIMIZE|WS_POPUPWINDOW|WS_CAPTION|WS_SYSMENU, 100, 100, 200, 50, NULL, NULL, hinst, NULL);
	if(hwnd == NULL)
		new Error("Can't create window");
#endif

  vb = Dumb_Video_bitmap::New(0, 0, w, h);
}

Video_Dumb::~Video_Dumb() {
  if(vb)
    delete vb;
#ifdef UGS_DIRECTX
	ShowWindow(hwnd, SW_HIDE);
	DestroyWindow(hwnd);
	UnregisterClass("SkeltonClass", hinst);
#endif
}

void Video_Dumb::lock() {
  vb->setmem();
}

void Video_Dumb::unlock() {
}

void Video_Dumb::flip() {
  if(newpal) {
    pal.set();
    newpal = false;
  }

#ifdef UGS_LINUX
  usleep(1);
#endif

  framecount++;
}

void Video_Dumb::setpal(const Palette& p) {
  pal = p;
  newpal=true;
}

void Video_Dumb::dosetpal(PALETTEENTRY pal[256], int size) {
}

void Video_Dumb::start_frame() {
  lock();
}

void Video_Dumb::end_frame() {
  flip();
}

void Video_Dumb::restore() {
  newpal = true;
  need_paint = 2;
}

void Video_Dumb::clean_up() {
}

void Video_Dumb::snap_shot(int x, int y, int w, int h) {
}

void Video_Dumb::toggle_fullscreen() {
}

