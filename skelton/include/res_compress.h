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

#ifndef _HEADER_RES_COMPRESS
#define _HEADER_RES_COMPRESS

#include "res.h"

class Res_compress: public Res_mem {
	Res_mode mode;
	Res *res;
	Res_dos *res_dos;
	mutable Dword ressize;
	mutable Dword write_pos;
	void read_uncompress();
public:
	void write_compress();
	bool exist;
	Res_compress(const char *fil, Res_mode pmode=RES_READ, bool res_doze=false);
	virtual ~Res_compress();
	virtual void write(const void *b, int nb);
	virtual Dword size();
};

#endif
