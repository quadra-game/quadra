/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <stdio.h>
#include "stringtable.h"
#include "res.h"

char *usage = "insufficient number of parameters\n";
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

	if(ARGC < 3) {
		fprintf(stderr, "%s: %s", ARGV[0], usage);
		exit(1);
	}

	wad = new Resfile(ARGV[1], false);

	wad->clear();

	res = new Res_dos(ARGV[2], RES_READ);
	data = new Byte[res->size()+1];

	memcpy(data, res->buf(), res->size());

	Stringtable st(data, res->size());

	for(int i=0; i<st.size(); i++)
		addfile(st.get(i));

	delete data;
	delete res;

	wad->freeze();

	delete wad;

	return 0;
}

void start_game(void) {
}
