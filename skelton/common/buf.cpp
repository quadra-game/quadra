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

Buf::Buf(Dword size, Dword in) {
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

void Buf::remove_from_start(Dword s) {
	Dword current_size=size();
	if(s<current_size) {
		memmove(get(), get()+s, current_size-s);
		resize(current_size-s);
	}
	else {
		resize(0);
	}
}

void Buf::append(const Byte* d, Dword s) {
	Dword end = size();
	resize(size()+s);
	memcpy(get()+end, d, s);
}

void Buf::append(const char* d) {
	append((const Byte*)d, strlen(d));
}

void Buf::resize(Dword s) {
	if(s>size()) {
		Dword end = size();
		Dword endsize = s-size();
		reserve(s);
		memset(get()+end, 0, endsize);
	}
	size_=s;
}

void Buf::reserve(Dword s) {
	//Round up to next inc
	s=((s+inc-1)/inc)*inc;
	if(s>capacity) {
		data = (Byte*)realloc(data, s);
		if(!data)
			fatal_msgbox("Out of memory!");
		capacity=s;
	}
}

Textbuf::Textbuf(Dword size) {
	data=NULL;
	capacity=0;
	reserve(size);
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
	vsprintf(st, s, marker);
	va_end(marker);
	if(data) {
		reserve(strlen(data)+strlen(st)+1);
	}
	else {
		reserve(strlen(st)+1);
	}
	if(data)
		strcat(data, st);
}

void Textbuf::reserve(Dword size) {
	Dword wanted=(size+15)/16*16;
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
