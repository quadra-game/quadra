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

#include "random.h"
#include "main.h"
#include "input.h"
#include "surface.h"
#include "video_dx16.h"

extern LRESULT CALLBACK windowproc(HWND hwnd, UINT msg,
				   WPARAM wparam, LPARAM lparam);

/* singleton interne */
static Video16 *video16 = NULL;

Surface *screen; // pour permettre a l'applic de connaitre le back buffer

class Surface_basedx: public Surface {
public:
	LPDIRECTDRAWSURFACE surf;
	Surface_basedx(int w, int h);
	virtual ~Surface_basedx();
	virtual void upload(void *src, int w, int h) { }
	virtual void blit(Surface *dest, int x, int y);
	virtual void load24rgb(Res &res, Dither_mode dither=NONE) { }
	virtual void load24rgb(void *mem, Dither_mode dither=NONE) { }
	virtual void upload() { }
	virtual void restore();
	virtual void start_fade() { }
	virtual void end_fade() { }
	virtual void fade_down(int c) { }
	virtual void set_mask_color(int color) { }
	virtual void rect(int x, int y, int w, int h, int color);
	virtual bool clip(int *x, int *y, int *w, int *h, int *start_x=NULL, int *start_y=NULL);
};

Video16::Video16(int w, int h, int b, const char *wname) {
	xwindow=false;
	video16 = this;
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
	DDSURFACEDESC ddsd;
	memset(&ddsd, 0, sizeof(ddsd));
	DDSCAPS ddscaps;
	memset(&ddscaps, 0, sizeof(ddscaps));
	calldx(DirectDrawCreate(NULL, &lpdd, NULL));
	calldx(lpdd->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT|DDSCL_ALLOWMODEX));
	calldx(lpdd->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT|DDSCL_ALLOWMODEX));
	calldx(lpdd->SetDisplayMode(width, height, bit));
  
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE|DDSCAPS_FLIP|DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount=1;
	calldx(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL));
	ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
	calldx(lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback));

	back = new Surface_basedx(width, height);
	back->surf = lpddsback;
	screen = back;

	DDPIXELFORMAT pel_format;
	pel_format.dwSize = sizeof(pel_format);
	lpddsprimary->GetPixelFormat(&pel_format);
	msgbox("Pixel format info: RGB Bit count=%i\n", pel_format.dwRGBBitCount);
	msgbox("  Bits:  Red=%x  Green=%x   Blue=%x\n", pel_format.dwRBitMask, pel_format.dwGBitMask, pel_format.dwBBitMask);
	if(pel_format.dwGBitMask >> 5 == 63) {
		green_high = 1;
		msgbox("  Hicolor 565 mode detected\n");
	} else {
		green_high = 0; 
		msgbox("  Hicolor 555 mode detected\n");
	}

	framecount = 0;
	need_paint=2;

	lock();
	pitch = ddsdlock.lPitch;
	unlock();
	back->rect(0, 0, width, height, 0);
	flip();
	back->rect(0, 0, width, height, 0);
}

Video16::~Video16() {
	delete back;
	if(lpdd) {
		if(lpddsprimary) {
			unlock();
			lpddsprimary->Release();
			lpddsprimary=NULL;
		}
		lpdd->Release();
		lpdd=NULL;
	}
	DestroyWindow(hwnd);
	UnregisterClass("SkeltonClass", hinst);
	ShowCursor(TRUE);
	video16 = NULL;
}

void Video16::lock() {
	if(ddsdlock.lpSurface)
		return;
	ddsdlock.dwSize=sizeof(ddsdlock);
	ddsdlock.dwFlags=DDSD_ALL;
	calldx(lpddsback->Lock(NULL, &ddsdlock, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL));
}

void Video16::unlock() {
	if(ddsdlock.lpSurface) {
		lpddsback->Unlock(NULL);
		ddsdlock.lpSurface = NULL;
	}
}

void Video16::clean_up() {
	if(!alt_tab) {
		//vb->rect(0,0,width,height,0);
		flip();
		//vb->rect(0,0,width,height,0);
		flip();
	}
}

void Video16::flip() {
	calldx(lpddsprimary->Flip(lpddsback, DDFLIP_WAIT));
	framecount++;
}

void Video16::setpal(const Palette& p) {
}

void Video16::dosetpal(PALETTEENTRY pal[256], int size) {
}

int Video16::screen_shot = 0;

void Video16::end_frame() {
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

void Video16::snap_shot(int x, int y, int w, int h) {
/*	Raw raw(w, h, 256);
	char st[32];
	sprintf(st,"shot%02i.raw",screen_shot++);
	skelton_msgbox("Video::snap_shot: file [%s], x=%i, y=%i, w=%i, h=%i...", st, x,y,w,h);
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

	skelton_msgbox("ok\n");*/
}

void Video16::start_frame() {
}

void Video16::restore() {
	need_paint=2;
	calldx(lpddsprimary->Restore());
	for(int i=0; i<surfaces.size(); i++)
		surfaces[i]->restore();
}

void Video16::add_surface(Surface *s) {
	surfaces.add(s);
}

void Video16::remove_surface(Surface *s) {
	surfaces.remove_item(s);
}

Surface_basedx::Surface_basedx(int w, int h) {
	width = w;
	height = h;
	surf = NULL;
	transparent = false;
}

Surface_basedx::~Surface_basedx() {
}

void Surface_basedx::rect(int x, int y, int w, int h, int color) {
	if(!clip(&x, &y, &w, &h))
		return;
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = w+x;
	rect.bottom = y+h;
	DDBLTFX ddbltfx;
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = color;
	calldx(surf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx));
}

bool Surface_basedx::clip(int *x, int *y, int *w, int *h, int *start_x, int *start_y) {
	if(*x >= width || *y >= height || *x+*w<=0 || *y+*h<=0)
		return false;
	if(*x < 0) {
		*w += *x;
		if(start_x)
			*start_x -= *x;
		*x = 0;
	}
	if(*y < 0) {
		*h += *y;
		if(start_y)
			*start_y -= *y;
		*y = 0;
	}
	if(*x+*w > width)
		*w = width - *x;
	if(*y+*h > height)
		*h = height - *y;
	if(*w < 0 || *h < 0)
		return false;

	return true;
}

void Surface_basedx::blit(Surface *dest, int x, int y) {
	int w = width;
	int h = height;
	int start_x = 0, start_y = 0;
	if(!dest->clip(&x, &y, &w, &h, &start_x, &start_y))
		return;
	RECT rect;
	rect.left = start_x;
	rect.top = start_y;
	rect.right = start_x+w;
	rect.bottom = start_y+h;
	DDBLTFX ddbltfx;
	ddbltfx.dwSize = sizeof(ddbltfx);

/*	ddbltfx.dwAlphaSrcConst = ugs_random.rnd2(255);
	ddbltfx.dwAlphaSrcConstBitDepth = ugs_random.rnd2(7);
	RECT rect2;
	rect2.left = x;
	rect2.top = y;
	rect2.right = x+w;
	rect2.bottom = y+h;*/

	Surface_basedx *d = (Surface_basedx *) dest;
	if(transparent) {
//		calldx(d->surf->Blt(&rect2, surf, &rect, DDBLT_ALPHASRCCONSTOVERRIDE | DDBLT_KEYSRC | DDBLT_WAIT, &ddbltfx));
		calldx(d->surf->BltFast(x, y, surf, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT));
	} else {
//		calldx(d->surf->Blt(&rect2, surf, &rect, DDBLT_WAIT, &ddbltfx));
		calldx(d->surf->BltFast(x, y, surf, &rect, DDBLTFAST_WAIT));
	}
}

void Surface_basedx::restore() {
	if(surf) {
		if(surf->IsLost() == DDERR_SURFACELOST)
			surf->Restore();
	}
}

class Surface_dx_sys: public Surface_basedx {
public:
	Word *buffer;
	bool fading;
	int fade_value;
	Surface_dx_sys(int w, int h);
	virtual ~Surface_dx_sys();
	virtual void create_surface();
	virtual void upload(void *src, int w, int h);
	virtual void load24rgb(Res &res, Dither_mode dither=NONE);
	virtual void load24rgb(void *mem, Dither_mode dither=NONE);
	virtual void upload();
	virtual void restore();
	virtual void start_fade();
	virtual void end_fade();
	virtual void fade_down(int c);
	virtual void set_mask_color(int color);
};

class Surface_dx_vid: public Surface_basedx {
public:
	Surface *sys_surf;
	Surface_dx_vid(int w, int h);
	virtual ~Surface_dx_vid();
	virtual void create_surface();
	virtual void upload(void *src, int w, int h);
	virtual void load24rgb(Res &res, Dither_mode dither=NONE);
	virtual void load24rgb(void *mem, Dither_mode dither=NONE);
	virtual void upload();
	virtual void restore();
	virtual void start_fade();
	virtual void end_fade();
	virtual void fade_down(int c);
	virtual void set_mask_color(int color);
};

Surface *Surface::New_video(int w, int h) {
	Surface_dx_vid *s = new Surface_dx_vid(w, h);
	if(!s->surf) {
		delete s;
		msgbox("Surface::New_video: Allocating a system memory surface instead of video.\n");
		return new Surface_dx_sys(w, h);
	} else
		return s;
}

Surface *Surface::New(int w, int h) {
	return new Surface_dx_sys(w, h);
}

Surface_dx_sys::Surface_dx_sys(int w, int h): Surface_basedx(w,h) {
	buffer = NULL;
	fading = false;
	video16->add_surface(this);
	create_surface();
}

Surface_dx_sys::~Surface_dx_sys() {
	video16->remove_surface(this);
	if(surf)
		surf->Release();
	if(buffer)
		free(buffer);
}

void Surface_dx_sys::create_surface() {
	DDSURFACEDESC	ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	calldx(video16->lpdd->CreateSurface(&ddsd, &surf, NULL));
}

void Surface_dx_sys::upload(void *src, int w, int h) {
	DDSURFACEDESC lock;
	lock.dwSize=sizeof(lock);
	lock.dwFlags=DDSD_ALL;
	calldx(surf->Lock(NULL, &lock, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL));
	int pit_s = lock.lPitch;
	int pitch = w*2;
	h = min(h, lock.dwHeight);
	w = min(w, lock.dwWidth);
	for(int i=0; i<h; i++)
		memcpy((Byte *) lock.lpSurface + i*pit_s, (Byte *) src+i*pitch, w*2);
	calldx(surf->Unlock(NULL));
}

void Surface_dx_sys::upload() {
	if(buffer)
		upload(buffer, width, height);
}

void Surface_dx_sys::load24rgb(Res &res, Dither_mode dither) {
	load24rgb((void *) res.buf(), dither);
}

void Surface_dx_sys::load24rgb(void *mem, Dither_mode dither) {
	Byte *p = (Byte *) mem;
	if(!buffer)
		buffer = (Word *) malloc(width*height*2);
	Word *p2 = buffer;
	int i,j;
	if(dither == Dither_mode::NONE) {
		for(j=0; j<height; j++) {
			for(i=0; i<width; i++) {
				int r,g,b;
				r = (*p++>>3);
				g = (*p++>>(3 - video16->green_high));
				b = (*p++>>3);

				*p2++ = (b) + ((g)<<5) + ((r)<<(10 + video16->green_high));
			}
		}
	}
	if(dither == Dither_mode::RANDOM) {
		for(j=0; j<height; j++) {
			for(i=0; i<width; i++) {
				int r,g,b,r2,g2,b2;
				r2 = *p++;
				g2 = *p++;
				b2 = *p++;
				r = r2>>3;
				g = g2>>(3 - video16->green_high);
				b = b2>>3;
				r2 &= 7;
				b2 &= 7;
				int rnd = ugs_random.rnd(7);
				if(video16->green_high && g < 63) {
					g2 &= 3;
					if((rnd>>1) < g2)
						g++;
				} else if(g < 31) {
					g2 &= 7;
					if(rnd < g2)
						g++;
				}
				if(r < 31 && rnd < r2)
					r++;
				if(b < 31 && rnd < b2)
					b++;
				*p2++ = (b) + ((g)<<5) + ((r)<<(10 + video16->green_high));
			}
		}
	}
	if(dither == Dither_mode::ORDERED) {
		for(j=0; j<height; j++) {
			for(i=0; i<width; i++) {
				int r,g,b,r2,g2,b2;
				r2 = *p++;
				g2 = *p++;
				b2 = *p++;
				r = r2>>3;
				g = g2>>(3 - video16->green_high);
				b = b2>>3;
				r2 &= 7;
				b2 &= 7;
				if(video16->green_high && g < 63) {
					g2 &= 3;
					if((i+j)%(g2+1) > 0)
						g++;
				} else if(g < 31) {
					g2 &= 7;
					if((i+j)%(g2+1) > 0)
						g++;
				}
				if(r < 31 && (i+j)%(r2+1) > 0)
					r++;
				if(b < 31 && (i+j)%(b2+1) > 0)
					b++;
				*p2++ = (b) + ((g)<<5) + ((r)<<(10 + video16->green_high));
			}
		}
	}
}

void Surface_dx_sys::restore() {
	if(surf->IsLost() == DDERR_SURFACELOST) {
		(void) new Error("Surface_dx_sys:restore: System memory surface was lost (It should NOT happen!)");
	}
}

void Surface_dx_sys::start_fade() {
	fading = true;
	fade_value = 0;
}

void Surface_dx_sys::end_fade() {
	fading = false;
	fade_value = 0;
	upload();
}

void Surface_dx_sys::fade_down(int c) {
	fade_value = c;
	Word *in = buffer;
	Word *out;

	DDSURFACEDESC lock;
	lock.dwSize=sizeof(lock);
	lock.dwFlags=DDSD_ALL;
	calldx(surf->Lock(NULL, &lock, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL));
	if(video16->green_high) {
		for(int j=0; j<height; j++) {
			out = (Word *) ((Byte *) lock.lpSurface + j*lock.lPitch);
			for(int i=0; i<width; i++) {
				int r,g,b;
				r = ((*in>>11)) - c;
				g = ((*in>>5)&63) - (c<<1);
				b = (*in&31) - c;
				*in++;
				if(r<0)
					r=0;
				if(g<0)
					g=0;
				if(b<0)
					b=0;
				*out++ = (b) + ((g)<<5) + ((r)<<11);
			}
		}
	} else {
		for(int j=0; j<height; j++) {
			out = (Word *) ((Byte *) lock.lpSurface + j*lock.lPitch);
			for(int i=0; i<width; i++) {
				int r,g,b;
				r = ((*in>>10)) - c;
				g = ((*in>>5)&31) - c;
				b = (*in&31) - c;
				*in++;
				if(r<0)
					r=0;
				if(g<0)
					g=0;
				if(b<0)
					b=0;
				*out++ = (b) + ((g)<<5) + ((r)<<10);
			}
		}
	}
	calldx(surf->Unlock(NULL));
}

void Surface_dx_sys::set_mask_color(int color) {
	trans_color = color;
	transparent = true;
	DDCOLORKEY dcol;
	dcol.dwColorSpaceLowValue  = trans_color;
	dcol.dwColorSpaceHighValue = trans_color;
	calldx(surf->SetColorKey(DDCKEY_SRCBLT, &dcol));
}

Surface_dx_vid::Surface_dx_vid(int w, int h): Surface_basedx(w,h) {
	create_surface();
	if(surf) {
		video16->add_surface(this);
		sys_surf = Surface::New(width, height);
	}
}

Surface_dx_vid::~Surface_dx_vid() {
	if(surf) {
		video16->remove_surface(this);
		surf->Release();
		delete sys_surf;
	}
}

void Surface_dx_vid::create_surface() {
	DDSURFACEDESC	ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	HRESULT hr = video16->lpdd->CreateSurface(&ddsd, &surf, NULL);
	if(hr != DD_OK) {
		msgbox("Surface_dx_vid::create_surface: Unable to create surface in video memory.\n");
		surf = NULL;
	}
}

void Surface_dx_vid::upload(void *src, int w, int h) {
	sys_surf->upload(src, w, h);
	sys_surf->blit(this, 0, 0);
}

void Surface_dx_vid::upload() {
	sys_surf->upload();
	sys_surf->blit(this, 0, 0);
}

void Surface_dx_vid::load24rgb(Res &res, Dither_mode dither) {
	sys_surf->load24rgb((void *) res.buf(), dither);
}

void Surface_dx_vid::load24rgb(void *mem, Dither_mode dither) {
	sys_surf->load24rgb(mem, dither);
}

void Surface_dx_vid::restore() {
	if(surf->IsLost() == DDERR_SURFACELOST) {
		Surface_basedx::restore();
		sys_surf->blit(this, 0, 0);
	}
}

void Surface_dx_vid::start_fade() {
	sys_surf->start_fade();
}

void Surface_dx_vid::end_fade() {
	sys_surf->end_fade();
	sys_surf->blit(this, 0, 0);
}

void Surface_dx_vid::fade_down(int c) {
	sys_surf->fade_down(c);
	sys_surf->blit(this, 0, 0);
}

void Surface_dx_vid::set_mask_color(int color) {
	trans_color = color;
	transparent = true;
	DDCOLORKEY dcol;
	dcol.dwColorSpaceLowValue  = trans_color;
	dcol.dwColorSpaceHighValue = trans_color;
	calldx(surf->SetColorKey(DDCKEY_SRCBLT, &dcol));
}
