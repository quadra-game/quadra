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

#ifndef _HEADER_BUF
#define _HEADER_BUF

#include <stdint.h>

#include "types.h"

class Buf {
	uint8_t* data;
	uint32_t size_;
	uint32_t capacity;
	uint32_t inc;
public:
	Buf(const Buf &buf);
	Buf(uint32_t size=0, uint32_t in=16);
	virtual ~Buf();
	uint8_t* get() const {
		return data;
	}
	uint8_t& operator[](int offset) const {
		return data[offset];
	}
	void remove_from_start(uint32_t s);
	void append(const uint8_t* d, uint32_t s);
	void append(const char* d);
	uint32_t size() const {
		return size_;
	}
	void resize(uint32_t s);
	void reserve(uint32_t s);
};

class Textbuf {
	char* data;
	uint32_t capacity;
public:
	Textbuf();
	virtual ~Textbuf();
	int len() const;
	char* get() const;
	void append(const char* s, ...);
	void appendraw(const char* s);
	void reserve(uint32_t s);
};

#endif
