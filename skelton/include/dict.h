/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
	Dict *find_sub(const char *s) const;
	Dict *get_sub(const int i) const;
};

#endif
