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

#ifndef __HEADER_TRACK_H
#define __HEADER_TRACK_H

#if defined(_DEBUG) && defined(UGS_DIRECTX) 

void * __cdecl operator new(unsigned int, int, const char *, int);

#define TRACKED public: \
                  void *operator new(size_t s) { \
										return ::operator new(s, 1, __FILE__, __LINE__); \
									} \
                private:

#else /* !_DEBUG || !UGS_DIRECTX*/

#define TRACKED

#endif

#endif
