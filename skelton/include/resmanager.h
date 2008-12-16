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
#ifndef _HEADER_RESMANAGER
#define _HEADER_RESMANAGER

// See wadder.cpp for information on the .res file format, some tidbits of infos on making
//   a Quadra theme pack and some 100% pure Dada ranting.

#include <vector>

#include "types.h"

class Resfile;

class Resmanager {
private:
	std::vector<Resfile*> files;
public:
	Resmanager();
	virtual ~Resmanager();
	virtual void loadresfile(const char *fname);
	virtual int get(const char *resname, Byte **resdata);
};

extern Resmanager *resmanager;

#endif
