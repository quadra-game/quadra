/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "stringtable.h"

Stringtable *stringtable = NULL;

Stringtable::Stringtable(const char *nam) {
	mustfree=true;
	Res_doze res(nam);
	buf = new char[res.size()+1];
	memcpy(buf, res.buf(), res.size());
	parse(buf, res.size());
}

Stringtable::Stringtable(Byte *b, Dword size) {
	mustfree=false;
	parse((char *)b, size);
}

Stringtable& Stringtable::operator=(const Stringtable& src) {
	delete table;
	if(mustfree)
		delete buf;
	mustfree=true;
	num = src.size();
	int i;
	//calculate space needed in buf
	Dword siz=0;
	for(i=0; i<num; i++)
		siz+=strlen(src.get(i))+1;
	buf = new char[siz];
	table = new char*[num];
	//fill-in buf and table
	siz=0;
	for(i=0; i<num; i++) {
		memcpy(buf+siz, src.get(i), strlen(src.get(i))+1);
		table[i] = buf+siz;
		siz+=strlen(src.get(i))+1;
	}
	return *this;
}

void Stringtable::parse(char *buf, Dword size) {
	Dword ptr;
	int index;

	ptr = 0;
	num = 0;

	while(ptr < size) {
		if(buf[ptr] == 10)
			num++;
		ptr++;
	}

	/* le == 13 && != 10, c'est pour pas qu'une ligne de 1 char soit perdue */
	if((buf[size-1] != 10) ||
		 (buf[size-1] == 13 &&
			buf[size-2] != 10))
		num++;

	/* avec une clôture, pas de danger de se segmenter la face */
	buf[size] = 0;

	table = new char*[num];

	ptr = 0;
	index = 0;

	while(index < num) {
		table[index] = &buf[ptr];

		while(ptr < size) {
			if(buf[ptr] == 10) { // pour UNIX (juste un '10')
				buf[ptr] = 0;
				ptr++;
				break;
			}
			if(buf[ptr] == 13) { // pour DOZE (un '13', PUIS un '10')
				buf[ptr] = 0;
				ptr++;
				if((ptr < size) && (buf[ptr] == 10)) {
					buf[ptr] = 0;
					ptr++;
				}
				break;
			}
			ptr++;
		}
		index++;
	}
}

Stringtable::~Stringtable() {
	delete table;
	if(mustfree)
		delete buf;
}
