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

#ifndef _HEADER_UTILS
#define _HEADER_UTILS

#include <stdint.h>

#include "types.h"
#include "error.h"

inline void mset(void* p, uint8_t c, uint32_t l) {
	for(uint32_t i(0); i<l; i++)
		((uint8_t*)p)[i]=c;
}

inline void cpy(void* d, void* s, uint32_t l) {
	for(uint32_t i(0); i<l; i++)
		((uint8_t*)d)[i]=((uint8_t*)s)[i];
}

#endif
