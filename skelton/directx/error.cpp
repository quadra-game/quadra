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

RCSID("$Id$")

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
	Dword tim = getmsec();
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

Error::Error(const char* m, ...) {
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
	msgbox("Error::Error: %s\n", st);
	MessageBox(NULL, st, "Error", MB_ICONEXCLAMATION);
	exit(1);
}

void calldx(HRESULT hr) {
	if(SUCCEEDED(hr))
		return;
	switch(hr) {
		case DDERR_INVALIDMODE:
			new Error("Invalid mode");
		case DDERR_INVALIDPARAMS:
			new Error("Invalid parameters");
		case DDERR_OUTOFMEMORY:
			new Error("DirectDraw out of system memory");
		case DDERR_GENERIC:
			new Error("Generic DirectX error");
		case DDERR_INVALIDRECT:
			new Error("Invalid rectangle specified");
		case DDERR_SURFACEBUSY:
			new Error("DirectDraw surface is busy");
		case DDERR_SURFACELOST:
			new Error("DirectDraw surface was lost");
		case DDERR_TOOBIGHEIGHT:
			new Error("DirectDraw Object too large");
		case DDERR_OUTOFVIDEOMEMORY:
			new Error("Out of video memory");
		case DDERR_INCOMPATIBLEPRIMARY:
			new Error("Incompatible primary");
		case DDERR_INVALIDCAPS:
			new Error("Invalid caps");
		case DDERR_INVALIDOBJECT:
			new Error("Invalid object");
		case DDERR_INVALIDPIXELFORMAT:
			new Error("Invalid pixel format");
		case DDERR_NOALPHAHW:
			new Error("No alpha HW");
		case DDERR_NOCOOPERATIVELEVELSET:
			new Error("No cooperative level set");
		case DDERR_NODIRECTDRAWHW:
			new Error("No directdraw HW");
		case DDERR_NOEMULATION:
			new Error("No emulation");
		case DDERR_NOEXCLUSIVEMODE:
			new Error("No exclusive mode");
		case DDERR_NOFLIPHW:
			new Error("No flip hw");
		case DDERR_NOMIPMAPHW:
			new Error("No mipmap hw");
		case DDERR_NOZBUFFERHW:
			new Error("No zbuffer Hw");
		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			new Error("Primary surface already exist");
		case DDERR_UNSUPPORTEDMODE:
			new Error("Unsupported mode");
		case DDERR_IMPLICITLYCREATED:
			new Error("Unable to perform (implicitly created)");
		case DDERR_UNSUPPORTED:
			new Error("Unsupported operation");
		case DDERR_WRONGMODE:
			new Error("Wrong video mode");
		case DDERR_WASSTILLDRAWING:
			new Error("Directdraw still drawing");
		case DDERR_NOTAOVERLAYSURFACE:
			new Error("An overlay component is called for a non-overlay surface");
		case DDERR_NOOVERLAYHW:
			new Error("The operation cannot be carried out because no overlay hardware is present or available");
		case DDERR_INVALIDSURFACETYPE:
			new Error("The requested operation could not be performed because the surface was of the wrong type");
		case DDERR_NOCOLORKEYHW:
			new Error("Operation could not be carried out because there is no hardware support of the dest color key");
		case DI_BUFFEROVERFLOW:
			new Error("Directinput buffer overflow");
		case DIERR_NOTACQUIRED:
			new Error("Input not acquired");
		case DIERR_INPUTLOST:
			new Error("Input lost");
		case DSERR_ALLOCATED:
			new Error("The call failed because resources (such as a priority level) were already being used by another caller.");
		case DSERR_CONTROLUNAVAIL:
			new Error("The control (vol,pan,etc.) requested by the caller is not available.");
		case DSERR_INVALIDCALL:
			new Error("This call is not valid for the current state of this object");
		case DSERR_PRIOLEVELNEEDED:
			new Error("The caller does not have the priority level required for the function to succeed.");
		case DSERR_BADFORMAT:
			new Error("The specified WAVE format is not supported");
		case DSERR_NODRIVER:
			new Error("No sound driver is available for use");
		case DSERR_ALREADYINITIALIZED:
			new Error("This object is already initialized");
		case DSERR_NOAGGREGATION:
			new Error("This object does not support aggregation");
		case DSERR_BUFFERLOST:
			new Error("The buffer memory has been lost, and must be restored.");
		case DSERR_OTHERAPPHASPRIO:
			new Error("Another app has a higher priority level, preventing this call from succeeding.");
		case DSERR_UNINITIALIZED:
			new Error("This object has not been initialized");
		case DSERR_NOINTERFACE:
			new Error("The requested COM interface is not available");

		default:
			new Error("Unknown error #%i.", hr);
	}
}
