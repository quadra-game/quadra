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

#ifndef _HEADER_MAIN
#define _HEADER_MAIN

#include "types.h"

#ifdef UGS_DIRECTX
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <windowsx.h>
	extern HINSTANCE hinst;
	extern HWND hwnd;
#endif

enum Time_mode {
	TIME_NORMAL,
	TIME_FREEZE,
	TIME_SLOW,
	TIME_FAST
};

extern Time_mode time_control;

extern bool alt_tab;

extern char exe_directory[];

void start_game();
void quit_game();
void start_frame();
void end_frame();
Dword getmsec();

#endif
