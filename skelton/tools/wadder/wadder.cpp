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
#include "stringtable.h"
#include "res.h"

RCSID("$Id$")

char *usage = "usage: wadder <working directory> <output res> <input text>\n";
Resfile *wad;

char *basename(const char* f) {
	char* p=(char*)(f+strlen(f));
	while(*p != '/' && *p != '\\' && p>=f)
		p--;
	return p+1;
}

void addfile(const char* fname) {
	Res_dos *res;
	int resnamelen;
	char *data;

	printf("%s: ", fname);

	res = new Res_dos(fname, RES_TRY);
	data = new char[res->size()];
	res->read(data, res->size());

	resnamelen = strlen(basename(fname))+1;

	wad->add(basename(fname), res->size(), data);

	delete res;
	delete data;

	printf("done\n");
}

int main(int ARGC, char **ARGV, char **ENV) {
	Res_dos *res;
	Byte* data;

	if(ARGC < 4) {
		fprintf(stderr, "%s: %s", ARGV[0], usage);
		exit(1);
	}

	char wad_file[256];
	sprintf(wad_file, "%s%s", ARGV[1], ARGV[2]);

	wad = new Resfile(wad_file, false);

	wad->clear();

	char res_file[256];
	sprintf(res_file, "%s%s", ARGV[1], ARGV[3]);

	res = new Res_dos(res_file, RES_READ);
	data = new Byte[res->size()+1];

	memcpy(data, res->buf(), res->size());

	Stringtable st(data, res->size());

	for(int i=0; i<st.size(); i++)
	{
		char temp[256];
		sprintf(temp, "%s%s", ARGV[1], st.get(i));
		addfile(temp);
	}

	delete data;
	delete res;

	wad->freeze();

	delete wad;

	return 0;
}

void start_game(void) {
}
