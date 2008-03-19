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

#ifndef _HEADER_DICT
#define _HEADER_DICT

#include "types.h"
#include "buf.h"
#include "array.h"

class Dict {
	Array<Dict *> sub;
	char key[32];
	Textbuf value;
public:
	Dict(const char *k=NULL, const char *v=NULL);
	virtual ~Dict();
	void add(const char *s);
	void dump() const;
	Dword size() const;
	const char *get_key() const;
	const char *find(const char *s) const;
	Dict *find_sub(const char *s);
	Dict *get_sub(const int i);
	const Dict *find_sub(const char *s) const;
	const Dict *get_sub(const int i) const;
};

#endif
