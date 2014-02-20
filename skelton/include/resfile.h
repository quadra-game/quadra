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

#ifndef _HEADER_RESFILE
#define _HEADER_RESFILE

#include <stdint.h>

class Res;
class ResName;
class Res_dos;

const char signature[4] = "UGS";

class Resdata {
private:
	friend class Resfile;
	int size;
	uint8_t *data;
public:
	char *name;
	Resdata *next;
	Resdata(char *resname, int ressize, uint8_t *resdata, Resdata *list);
	virtual ~Resdata();
};

class Res_dos;

class Resfile {
	void thaw(Res& res);
	void clear();
public:
	Resdata *list;
	Resfile();
	Resfile(const char *fname);
	~Resfile();
	void freeze(Res_dos& res) const;
	void add(const char *resname, int size, const char *resdata);
	int get(const ResName& resname, uint8_t **resdata) const;
	void remove(const char* resname);
};

#endif
