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

#include <stdio.h>
#include <string.h>
#include "error.h"
#include "dict.h"

RCSID("$Id$")

Dict::Dict(const char *k, const char *v) {
	if(k) {
		strncpy(key, k, sizeof(key)-1);
		key[sizeof(key)-1] = 0;
	}
	else
		key[0] = 0;

	if(v)
		value.append("%s", v);
}

Dict::~Dict() {
	sub.deleteall();
}

void Dict::add(const char *s) {
	Textbuf buf;
	buf.append("%s", s);
	char *st=buf.get();
	char *val = strchr(st, ' ');
	char *rep = strchr(st, '/');
	if(rep && val && rep>val)
		rep=NULL;

	if(!rep) {
		if(val) {
			*val = 0;
			sub.add(new Dict(st, val+1));
		} else {
			sub.add(new Dict(st));
		}
	} else {
		*rep = 0;

		Dict *d = find_sub(st);
		if(!d) {
			d = new Dict(st);
			sub.add(d);
		}
		d->add(rep+1);
	}
}

void Dict::dump() const {
	if(sub.size()) {
		for(int i=0; i<sub.size(); i++) {
			msgbox("%s / ", key);
			sub[i]->dump();
		}
	} else {
		msgbox("key=[%s], value=[%-20.20s]\n", key, value.get());
	}
}

Dword Dict::size() const {
	return sub.size();
} 

const char *Dict::get_key() const {
	return key;
}

const char *Dict::find(const char *s) const {
	const Dict *d = find_sub(s);
	if(d)
		return d->value.get();
	else
		return NULL;
}

Dict *Dict::find_sub(const char *s) {
	for(int i=0; i<sub.size(); i++) {
		if(strcmp(sub[i]->key, s) == 0) {
			return sub[i];
		}
	}
	return NULL;
}

Dict *Dict::get_sub(const int i) {
	if(i<sub.size())
		return sub[i];
	else
		return NULL;
}

const Dict *Dict::find_sub(const char *s) const {
	for(int i=0; i<sub.size(); i++) {
		if(strcmp(sub[i]->key, s) == 0) {
			return sub[i];
		}
	}
	return NULL;
}

const Dict *Dict::get_sub(const int i) const {
	if(i<sub.size())
		return sub[i];
	else
		return NULL;
}
