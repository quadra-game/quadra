/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_STRINGTABLE
#define _HEADER_STRINGTABLE
#include "res.h"

class Stringtable {
	char **table;
	int num;
	char *buf;
	bool mustfree;
	void parse(char *buf, Dword size);
public:
	Stringtable(const char *nam);
	Stringtable(Byte *buf, Dword size);
	virtual ~Stringtable();
	Stringtable& operator=(const Stringtable&);
	int size() const {
		return num;
	}
	const char *get(const int quel) const {
		if(quel<num)
			return table[quel];
		else
			return "###";
	}
};

extern Stringtable *stringtable;

#endif
