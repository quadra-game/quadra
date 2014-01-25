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
#include <mmsystem.h>

#undef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION 0x0300
#include <ddraw.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>
#include "error.h"
#include "main.h"
#include "res.h"
#include "sound.h"
#include "input.h"
#include "video.h"

#ifndef NDEBUG
int copper=0;
#endif

#ifndef NDEBUG
	bool _debug = true;
#else
	bool _debug = false;
#endif

bool skelton_debug = true;

#ifndef NDEBUG
void COPPER(int a, int b, int c) {
	if(copper) {
		_outp(0x3c8,0);
		_outp(0x3c9,a);
		_outp(0x3c9,b);
		_outp(0x3c9,c);
	}
}
#endif

static void output_msg(char *m) {
	OutputDebugString(m);
	int siz=strlen(m);
	if(m[siz-1] ==10) {
		m[siz-1] = 13;
		m[siz] = 10;
		siz++;
	}
	static Res_dos out("output.txt", RES_CREATE);
	if(out.exist) // so we don't crash if creating 'output.txt' didn't work!
		out.write(m, siz);
}

void lock_msgbox(const char* m, ...) {
	if(_debug) {
		video->unlock();
		char st[1024];
		va_list marker;
		va_start(marker, m);
		vsprintf(st, m, marker);
		va_end(marker);
		output_msg(st);
		video->lock();
	}
}

void msgbox(const char* m, ...) {
	if(_debug) {
		char st[1024];
		va_list marker;
		va_start(marker, m);
		vsprintf(st, m, marker);
		va_end(marker);
		output_msg(st);
	}
}

void skelton_msgbox(const char* m, ...) {
	if(_debug && skelton_debug) {
		char st[1024];
		va_list marker;
		va_start(marker, m);
		vsprintf(st, m, marker);
		va_end(marker);
		output_msg(st);
	}
}

#ifndef NDEBUG
void debug_point() {
	uint32_t tim = getmsec();
	while(getmsec() - tim < 5000) {
		start_frame();
		end_frame();
		if(input && input->quel_key == DIK_F2) { // skip break point
			input->quel_key = -1;
			break;
		}
	}
}
#endif

void user_output(const char* title, const char *msg) {
	ShowCursor(TRUE);
	MessageBox(NULL, msg, title, MB_ICONINFORMATION);
	ShowCursor(FALSE);
}

void fatal_msgbox(const char* m, ...) {
	delete_obj();
	ShowCursor(TRUE);
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	char st[1024];
	va_list marker;
	va_start(marker, m);
	vsprintf(st, m, marker);
	va_end(marker);
	msgbox("fatal error: %s\n", st);
	MessageBox(NULL, st, "Error", MB_ICONEXCLAMATION);
	exit(1);
}

void calldx(HRESULT hr) {
	if(SUCCEEDED(hr))
		return;
	switch(hr) {
		case DDERR_INVALIDMODE:
			fatal_msgbox("Invalid mode");
		case DDERR_INVALIDPARAMS:
			fatal_msgbox("Invalid parameters");
		case DDERR_OUTOFMEMORY:
			fatal_msgbox("DirectDraw out of system memory");
		case DDERR_GENERIC:
			fatal_msgbox("Generic DirectX error");
		case DDERR_INVALIDRECT:
			fatal_msgbox("Invalid rectangle specified");
		case DDERR_SURFACEBUSY:
			fatal_msgbox("DirectDraw surface is busy");
		case DDERR_SURFACELOST:
			fatal_msgbox("DirectDraw surface was lost");
		case DDERR_TOOBIGHEIGHT:
			fatal_msgbox("DirectDraw Object too large");
		case DDERR_OUTOFVIDEOMEMORY:
			fatal_msgbox("Out of video memory");
		case DDERR_INCOMPATIBLEPRIMARY:
			fatal_msgbox("Incompatible primary");
		case DDERR_INVALIDCAPS:
			fatal_msgbox("Invalid caps");
		case DDERR_INVALIDOBJECT:
			fatal_msgbox("Invalid object");
		case DDERR_INVALIDPIXELFORMAT:
			fatal_msgbox("Invalid pixel format");
		case DDERR_NOALPHAHW:
			fatal_msgbox("No alpha HW");
		case DDERR_NOCOOPERATIVELEVELSET:
			fatal_msgbox("No cooperative level set");
		case DDERR_NODIRECTDRAWHW:
			fatal_msgbox("No directdraw HW");
		case DDERR_NOEMULATION:
			fatal_msgbox("No emulation");
		case DDERR_NOEXCLUSIVEMODE:
			fatal_msgbox("No exclusive mode");
		case DDERR_NOFLIPHW:
			fatal_msgbox("No flip hw");
		case DDERR_NOMIPMAPHW:
			fatal_msgbox("No mipmap hw");
		case DDERR_NOZBUFFERHW:
			fatal_msgbox("No zbuffer Hw");
		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			fatal_msgbox("Primary surface already exist");
		case DDERR_UNSUPPORTEDMODE:
			fatal_msgbox("Unsupported mode");
		case DDERR_IMPLICITLYCREATED:
			fatal_msgbox("Unable to perform (implicitly created)");
		case DDERR_UNSUPPORTED:
			fatal_msgbox("Unsupported operation");
		case DDERR_WRONGMODE:
			fatal_msgbox("Wrong video mode");
		case DDERR_WASSTILLDRAWING:
			fatal_msgbox("Directdraw still drawing");
		case DDERR_NOTAOVERLAYSURFACE:
			fatal_msgbox("An overlay component is called for a non-overlay surface");
		case DDERR_NOOVERLAYHW:
			fatal_msgbox("The operation cannot be carried out because no overlay hardware is present or available");
		case DDERR_INVALIDSURFACETYPE:
			fatal_msgbox("The requested operation could not be performed because the surface was of the wrong type");
		case DDERR_NOCOLORKEYHW:
			fatal_msgbox("Operation could not be carried out because there is no hardware support of the dest color key");
		case DI_BUFFEROVERFLOW:
			fatal_msgbox("Directinput buffer overflow");
		case DIERR_NOTACQUIRED:
			fatal_msgbox("Input not acquired");
		case DIERR_INPUTLOST:
			fatal_msgbox("Input lost");
		case DSERR_ALLOCATED:
			fatal_msgbox("The call failed because resources (such as a priority level) were already being used by another caller.");
		case DSERR_CONTROLUNAVAIL:
			fatal_msgbox("The control (vol,pan,etc.) requested by the caller is not available.");
		case DSERR_INVALIDCALL:
			fatal_msgbox("This call is not valid for the current state of this object");
		case DSERR_PRIOLEVELNEEDED:
			fatal_msgbox("The caller does not have the priority level required for the function to succeed.");
		case DSERR_BADFORMAT:
			fatal_msgbox("The specified WAVE format is not supported");
		case DSERR_NODRIVER:
			fatal_msgbox("No sound driver is available for use");
		case DSERR_ALREADYINITIALIZED:
			fatal_msgbox("This object is already initialized");
		case DSERR_NOAGGREGATION:
			fatal_msgbox("This object does not support aggregation");
		case DSERR_BUFFERLOST:
			fatal_msgbox("The buffer memory has been lost, and must be restored.");
		case DSERR_OTHERAPPHASPRIO:
			fatal_msgbox("Another app has a higher priority level, preventing this call from succeeding.");
		case DSERR_UNINITIALIZED:
			fatal_msgbox("This object has not been initialized");
		case DSERR_NOINTERFACE:
			fatal_msgbox("The requested COM interface is not available");

		default:
			fatal_msgbox("Unknown error #%i.", hr);
	}
}
