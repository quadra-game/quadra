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

#include "input.h"
#include "main.h"
#include "cursor.h"
#include "bitmap.h"
#include "palette.h"
#include "sprite.h"
#include "video_dx.h"
#include "raw.h"

extern LRESULT CALLBACK windowproc(HWND hwnd, UINT msg,
				   WPARAM wparam, LPARAM lparam);

/* singleton interne */
DirectX_Video *directx_video = NULL;

DirectX_Video::DirectX_Video(int w, int h, int b, const char *wname) {
	xwindow = false;
	directx_video = this;
	ShowCursor(FALSE);
	width=w;
	height=h;
	bit=b;
	lpddsprimary=NULL;
	lpddsback=NULL;
	lpdd=NULL;
	lpddpal=NULL;
	BOOL rc;
 	WNDCLASS wc;
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = windowproc;
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
	hwnd = CreateWindowEx(WS_EX_TOPMOST , "SkeltonClass", wname, WS_POPUP|WS_SYSMENU, 0, 0, 0, 0, NULL, NULL, hinst, NULL);
	if(hwnd == NULL)
		new Error("Can't create window");

	ddsdlock.lpSurface=NULL;
	calldx(DirectDrawCreate(NULL, &lpdd, NULL));
	calldx(lpdd->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT|DDSCL_ALLOWMODEX));
	calldx(lpdd->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT|DDSCL_ALLOWMODEX));
	calldx(lpdd->SetDisplayMode(width, height, bit));

	DDSCAPS ddscaps;
	memset(&ddscaps, 0, sizeof(ddscaps));
	DDSURFACEDESC ddsd;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE|DDSCAPS_FLIP|DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount=1;
	calldx(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL));
	ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
	calldx(lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback));
	add_surface(lpddsprimary, NULL);

	if(bit == 8) {
		create_palette();
		calldx(lpddsprimary->SetPalette(lpddpal));
		newpal=TRUE;
	}
	framecount = 0;
	need_paint=2;

	lock();
	pitch = ddsdlock.lPitch;
	unlock();
	vb = Video_bitmap::New(0, 0, width, height, pitch);
}

DirectX_Video::~DirectX_Video() {
	delete vb;
	surfaces.deleteall();
	if(lpdd) {
		if(lpddsprimary) {
			unlock();
			lpddsprimary->Release();
			lpddsprimary=NULL;
		}
		if(lpddpal) {
			lpddpal->Release();
			lpddpal=NULL;
		}
		lpdd->Release();
		lpdd=NULL;
	}
	ShowWindow(hwnd, SW_HIDE);
	DestroyWindow(hwnd);
	UnregisterClass("SkeltonClass", hinst);
	ShowCursor(TRUE);
	directx_video = NULL;
}

void DirectX_Video::create_palette() {
	PALETTEENTRY tmp[256];
	for(int i=0; i<256; i++) {
		tmp[i].peRed = 0;
		tmp[i].peGreen = 0;
		tmp[i].peBlue = 0;
		tmp[i].peFlags = NULL;
	}
	calldx(lpdd->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256, tmp, &lpddpal, NULL));
}

void DirectX_Video::lock() {
	if(ddsdlock.lpSurface)
		return;
	ddsdlock.dwSize=sizeof(ddsdlock);
	ddsdlock.dwFlags=DDSD_ALL;
	HRESULT hr;
	lpddsback->Unlock(NULL);
	hr = lpddsback->Lock(NULL, &ddsdlock, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
	if(hr == DDERR_SURFACELOST) {
		skelton_msgbox("Backbuffer is lost...");
		lpddsprimary->Restore();
		if(lpddsprimary->IsLost() == DDERR_SURFACELOST) {
			skelton_msgbox("and front buffer too! Aborting. This surface is not restorable\n");
			(void) new Error("Video surface is not restorable.\nIt seems to be a buggy video driver.\nPlease update DirectX!");
		} else {
			skelton_msgbox(" but front buffer is not! (Buggy DirectX driver) Reconstructing video surfaces...\n");

			remove_surface(lpddsprimary, NULL);
			lpddsprimary->Release();
			DDSURFACEDESC ddsd;
			memset(&ddsd, 0, sizeof(ddsd));
			ddsd.dwSize=sizeof(ddsd);
			ddsd.dwFlags=DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
			ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE|DDSCAPS_FLIP|DDSCAPS_COMPLEX;
			ddsd.dwBackBufferCount=1;
			calldx(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL));
			DDSCAPS ddscaps;
			memset(&ddscaps, 0, sizeof(ddscaps));
			ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
			calldx(lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback));
			add_surface(lpddsprimary, NULL);

			lpddpal->Release();
			create_palette();
			pal.set();
			calldx(lpddsprimary->SetPalette(lpddpal));


			if(lpddsback->IsLost() == DDERR_SURFACELOST) {
				skelton_msgbox("Unable to restore video back buffer. Aborting\n");
				(void) new Error("Video surface is not restorable.\nIt seems to be a buggy video driver.\nPlease update DirectX!");
			}
			hr = lpddsback->Lock(NULL, &ddsdlock, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
			if(hr == DDERR_SURFACELOST) {
				skelton_msgbox("Unable to lock video back buffer. Aborting\n");
				(void) new Error("Video surface is not restorable.\nIt seems to be a buggy video driver.\nPlease update DirectX!");
			}
		}
	}
	calldx(hr);
}

void DirectX_Video::unlock() {
	if(ddsdlock.lpSurface) {
		lpddsback->Unlock(NULL);
		ddsdlock.lpSurface = NULL;
	}
}

void DirectX_Video::clean_up() {
	if(!alt_tab) {
		vb->rect(0,0,width,height,0);
		flip();
		vb->rect(0,0,width,height,0);
		flip();
	}
}

void DirectX_Video::flip() {
	unlock();

	calldx(lpddsprimary->Flip(lpddsback, DDFLIP_WAIT));
	if(newpal) {
		// Nouvelle methode pour palette:
		int boo;
		lpdd->GetVerticalBlankStatus(&boo);
		if(!boo)
			lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		pal.set();
		newpal=FALSE;
	}
	framecount++;
}

void DirectX_Video::setpal(const Palette& p) {
	pal = p;
	newpal=TRUE;
}

void DirectX_Video::dosetpal(PALETTEENTRY pal[256], int size) {
	if(bit == 8) 
		calldx(lpddpal->SetEntries(0, 0, size, pal));
}

int DirectX_Video::screen_shot = 0;

void DirectX_Video::end_frame() {
	if(cursor) {
		cursor->get_back();
		cursor->draw();
	}
	if(_debug) { // screen shot
		if(input) {
			if(input->keys[DIK_F1] & RELEASED) {
				input->keys[DIK_F1] = 0;
				snap_shot(0,0, width, height);
			}
		}
	}

	flip();
}

void DirectX_Video::snap_shot(int x, int y, int w, int h) {
	Raw raw(w, h, 256);
	char st[32];
	sprintf(st,"shot%02i.raw",screen_shot++);
	skelton_msgbox("Video::snap_shot: file [%s], x=%i, y=%i, w=%i, h=%i...", st, x, y, w, h);
	Res_dos res(st, RES_CREATE);
	if(!res.exist) {
		skelton_msgbox("Can't create file!\n");
		return;
	}
	raw.write(res);
	int i;
	char pa[3];
	for(i=0; i<256; i++) {
		pa[0] = pal.r(i);
		pa[1] = pal.g(i);
		pa[2] = pal.b(i);
		res.write(pa, 3);
	}
	lock();
	for(i=y; i<y+h; i++)
		res.write((Byte *) ddsdlock.lpSurface + pitch*i + x, w);
	unlock();

	skelton_msgbox("ok\n");
}

void DirectX_Video::start_frame() {
	COPPER(0,0,0);
	lock();
	vb->setmem();

COPPER(30,0,30);
	if(cursor) {
		cursor->put_back();
		cursor->move();
	}
}

void DirectX_Video::add_surface(LPDIRECTDRAWSURFACE s, Bitmap *b) {
	surfaces.add(new DirectX_Surface(s, b));
}

void DirectX_Video::remove_surface(LPDIRECTDRAWSURFACE s, Bitmap *b) {
	for(int i=0; i<surfaces.size(); i++) {
		if(s == surfaces[i]->s && b == surfaces[i]->b) {
			delete surfaces[i];
			surfaces.remove(i);
			break;
		}
	}
}

void DirectX_Video::restore() {
	for(int i=0; i<surfaces.size(); i++) {
		LPDIRECTDRAWSURFACE s = surfaces[i]->s;
		if(s->IsLost() == DDERR_SURFACELOST) {
			calldx(s->Restore());
		}
	}
	newpal=TRUE;
	need_paint=2;
}

DirectX_Video_bitmap::DirectX_Video_bitmap(const int px, const int py, const int w, const int h, const int rw) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
  currentpage = new Bitmap(NULL, w, h, rw);
}

DirectX_Video_bitmap::DirectX_Video_bitmap(const int px, const int py, const int w, const int h) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
  currentpage = new Bitmap(NULL, w, h, video->pitch);
  setmem();
}

DirectX_Video_bitmap::~DirectX_Video_bitmap() {
  delete currentpage;
}

void DirectX_Video_bitmap::setmem() {
  currentpage->setmem((Byte *) directx_video->ddsdlock.lpSurface + pos_y*video->pitch + pos_x);
}

void DirectX_Video_bitmap::rect(const int x,const int y,const int w,const int h,const int color) const {
	if(clip(x, y, w, h))
		return;
	video->unlock();
	RECT rect;
	rect.top = clip_y1+pos_y;
	rect.left = clip_x1+pos_x;
	rect.right = clip_x2+pos_x+1;  // maudit que c'est poche
	rect.bottom = clip_y2+pos_y+1; // le dernier pixel est 'exclu' bordel
	DDBLTFX ddbltfx;
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = color;
	calldx(directx_video->lpddsback->Blt(&rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx));
	video->lock();
}

void DirectX_Video_bitmap::box(const int x,const int y,const int w,const int h,const int color) const {
	hline(y, x, w, color);
	hline(y+h-1, x, w, color);
	vline(x, y, h, color);
	vline(x+w-1, y, h, color);
}

void DirectX_Video_bitmap::get_bitmap(const Bitmap* bit, const int x, const int y, const int w, const int h) const {
	if(clip(x, y, w, h))
		return;
	Bitmap src((*currentpage)[clip_y1]+clip_x1, clip_w, clip_y2-clip_y1+1, currentpage->realwidth);
	src.draw(*bit, clip_x1-x, clip_y1-y);
}

void DirectX_Video_bitmap::put_pel(const int x, const int y, const Byte c) const {
	currentpage->put_pel(x, y, c);
}

void DirectX_Video_bitmap::hline(const int y, const int x, const int w, const Byte c) const {
	currentpage->hline(y, x, w, c);
}

void DirectX_Video_bitmap::vline(const int x, const int y, const int h, const Byte c) const {
	currentpage->vline(x, y, h, c);
}

void DirectX_Video_bitmap::line(const int x1, const int y1, const int x2, const int y2,
		  const Byte c) const
{
	currentpage->line(x1, y1, x2, y2, c);
}

void DirectX_Video_bitmap::put_bitmap(const Bitmap& d, const int dx, const int dy) const {
	d.draw(*currentpage, dx, dy);
}

void DirectX_Video_bitmap::put_sprite(const Sprite& d, const int dx, const int dy) const {
	d.draw(*currentpage, dx, dy);
}
