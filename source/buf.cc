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

#include "buf.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "error.h"

Buf::Buf(const Buf &buf) {
	data=NULL;
	size_=0;
	capacity=0;
	inc=buf.inc;
	reserve(buf.capacity);
	append(buf.data, buf.size());
}

Buf::Buf(uint32_t size, uint32_t in) {
	data=NULL;
	size_=0;
	capacity=0;
	inc=in;
	resize(size);
}

Buf::~Buf() {
	if(data)
		free(data);
}

void Buf::remove_from_start(uint32_t s) {
	uint32_t current_size=size();
	if(s<current_size) {
		memmove(get(), get()+s, current_size-s);
		resize(current_size-s);
	}
	else {
		resize(0);
	}
}

void Buf::append(const uint8_t* d, uint32_t s) {
	uint32_t end = size();
	resize(size()+s);
	memcpy(get()+end, d, s);
}

void Buf::append(const char* d) {
	append((const uint8_t*)d, strlen(d));
}

void Buf::resize(uint32_t s) {
	if(s>size()) {
		uint32_t end = size();
		uint32_t endsize = s-size();
		reserve(s);
		memset(get()+end, 0, endsize);
	}
	size_=s;
}

void Buf::reserve(uint32_t s) {
	//Round up to next inc
	s=((s+inc-1)/inc)*inc;
	if(s>capacity) {
		data = (uint8_t*)realloc(data, s);
		if(!data)
			fatal_msgbox("Out of memory!");
		capacity=s;
	}
}

Textbuf::Textbuf() {
	data=NULL;
	capacity=0;
	reserve(0);
}

Textbuf::~Textbuf() {
	if(data)
		free(data);
}

int Textbuf::len() const {
	if(data)
		return strlen(data);
	else
		return 0;
}

char* Textbuf::get() const {
	static char st=0;
	if(data)
		return data;
	else
		return &st;
}

void Textbuf::append(const char* s, ...) {
	char st[32768];
	va_list marker;
	va_start(marker, s);
	if (vsnprintf(st, sizeof(st), s, marker) >= static_cast<int>(sizeof(st)))
		fatal_msgbox("Textbuf::append overflow");
	va_end(marker);
	appendraw(st);
}

void Textbuf::appendraw(const char* s) {
	if(data) {
		reserve(strlen(data)+strlen(s)+1);
	} else {
		reserve(strlen(s)+1);
	}
	if(data)
		strcat(data, s);
}

void Textbuf::reserve(uint32_t size) {
	uint32_t wanted=(size+15)/16*16;
	if(wanted<=capacity)
		return;
	bool init=data? false:true;
	data=(char*)realloc(data, wanted);
	if(!data)
		fatal_msgbox("Out of memory!");
	capacity=wanted;
	if(init)
		data[0]=0;
}
