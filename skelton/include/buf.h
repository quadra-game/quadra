/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_BUF
#define _HEADER_BUF

#include "types.h"

class Buf {
	Byte* data;
	Dword size_;
	Dword capacity;
	Dword inc;
public:
	Buf(Dword size=0, Dword in=16);
	virtual ~Buf();
	Byte* get() const {
		return data;
	}
	Byte& operator[](int offset) const {
		return data[offset];
	}
	void remove_from_start(Dword s);
	void append(const Byte* d, Dword s);
	void append(const char* d);
	Dword size() const {
		return size_;
	}
	void resize(Dword s);
	void reserve(Dword s);
};

class Textbuf {
	char* data;
	Dword capacity;
public:
	Textbuf(Dword size=0);
	virtual ~Textbuf();
	int len() const;
	char* get() const;
	void append(const char* s, ...);
	void reserve(Dword s);
};

#endif
