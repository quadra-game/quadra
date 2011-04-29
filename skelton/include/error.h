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

#ifndef _HEADER_ERROR
#define _HEADER_ERROR

#include "config.h"

extern bool _debug;
extern bool skelton_debug;

#ifdef UGS_DIRECTX
	#ifndef NDEBUG
		extern int copper;
		void COPPER(int a, int b, int c);
		void debug_point();
	#else
		#define COPPER(a,b,c) ;
	#endif
	void calldx(long hr);
#endif

extern void delete_obj();
void msgbox(const char* m, ...);
void skelton_msgbox(const char* m, ...);
void lock_msgbox(const char* m, ...);
void user_output(const char* title, const char *msg);

class Error {
public:
	Error(const char* m, ...);
};

#endif
