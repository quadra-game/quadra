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

#ifndef _HEADER_RES
#define _HEADER_RES

#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "resfile.h"
#include "config.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include "error.h"
#include "res_name.h"
#include "types.h"

class Res {
public:
	Res() {
	}
	virtual ~Res() {
	}
	virtual int read(void *b, int nb)=0;
	virtual uint32_t size()=0;
	virtual void position(uint32_t po)=0;
	virtual const void *buf()=0;
	virtual bool eof()=0;
	virtual uint32_t get_position()=0;

private:
	Res(const Res&);
	Res& operator=(const Res&);
};

class Res_mem: public Res {
protected:
	uint8_t *_buf;
	uint32_t pos;
	uint32_t ressize;
public:
	Res_mem()
		: _buf(NULL),
			pos(0),
			ressize(0) {
	}
	virtual int read(void *b, int nb) {
		if(pos+nb>size()) {
			memset(b, 0, nb);
			nb=size()-pos;
		}
		memcpy((uint8_t *) b, _buf + pos, nb);
		pos += nb;
		return nb;
	}
	virtual void position(uint32_t po) {
		pos = po;
	}
	virtual const void *buf() {
		return(_buf + pos);
	}
	virtual bool eof() {
		return (pos >= size());
	}
	virtual uint32_t get_position() {
		return pos;
	}
	virtual uint32_t size() {
		return ressize;
	}
};

class Res_doze: public Res_mem {
public:
	Res_doze(const ResName& res);
};

enum Res_mode {
	RES_READ,
	RES_WRITE,
	RES_CREATE,
	RES_TRY
};

class Res_dos: public Res {
	int handle;
	void *_buf;
public:
	bool exist;
	Res_dos(const char *fil, Res_mode mode=RES_READ);
	virtual ~Res_dos();
	virtual void position(uint32_t po);
	virtual int read(void *b, int nb);
	virtual void write(const void *b, int nb);
	virtual uint32_t size();
	virtual const void* buf();
	virtual bool eof();
	virtual uint32_t get_position();
};

#endif
