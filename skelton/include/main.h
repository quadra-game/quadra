/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
