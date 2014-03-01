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

#include "resfile.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "error.h"
#include "res.h"

Resdata::Resdata(char *resname, int ressize, uint8_t *resdata, Resdata *list) {
	name = resname;
	size = ressize;
	data = resdata;
	next = list;
}

Resdata::~Resdata() {
	delete[] name;
	delete[] data;
	if(next)
		delete next;
}

Resfile::Resfile()
	: list(NULL) {
}

Resfile::Resfile(const char *fname)
	: list(NULL) {
	Res_dos res(fname, RES_TRY);

	if(res.exist) {
		skelton_msgbox("Resfile %s is fine\n", fname);
		thaw(res);
	} else
		skelton_msgbox("Resfile %s does not exist\n", fname);
}

Resfile::~Resfile() {
	clear();
}

void Resfile::freeze(Res_dos& res) const {
	int resnamelen;
	Resdata *ptr;
	uint32_t d;

	res.write(&signature, sizeof(signature));

	ptr = list;

	while(ptr != NULL) {
		resnamelen = strlen(ptr->name)+1;
		d = SDL_SwapLE32(resnamelen);
		res.write(&d, sizeof(d));
		res.write(ptr->name, resnamelen);
		d = SDL_SwapLE32(ptr->size);
		res.write(&d, sizeof(d));
		res.write(ptr->data, ptr->size);
		ptr = ptr->next;
	}

	resnamelen = 0;
	res.write(&resnamelen, sizeof(resnamelen));
}

void Resfile::thaw(Res& res) {
	char sig[sizeof(signature)];
	int resnamelen;
	char *resname;
	uint8_t *resdata;
	int ressize;

	res.position(0);

	res.read(sig, sizeof(signature));
	if(strncmp(sig, signature, sizeof(signature)) != 0) {
		msgbox("Resfile::thaw(): invalid signature\n");
		return;
	}

	do {
		res.read(&resnamelen, sizeof(resnamelen));
		resnamelen = SDL_SwapLE32(resnamelen);
		if(resnamelen == 0)
			break;
		resname = new char[resnamelen];
		res.read(resname, resnamelen);
		res.read(&ressize, sizeof(ressize));
		ressize = SDL_SwapLE32(ressize);
		resdata = new uint8_t[ressize];
		res.read(resdata, ressize);

		list = new Resdata(resname, ressize, resdata, list);
	} while(resnamelen > 0);

}

void Resfile::clear() {
	if(list)
		delete list;

	list = NULL;
}

void Resfile::add(const char *resname, const int size, const char *resdata) {
	char *myname;
	uint8_t *mydata;

	myname = new char[strlen(resname)+1];
	memcpy(myname, resname, strlen(resname)+1);
	mydata = new uint8_t[size];
	memcpy(mydata, resdata, size);

	list = new Resdata(myname, size, mydata, list);
}

int Resfile::get(const ResName& resname, uint8_t **resdata) const {
	Resdata *ptr;

	ptr = list;

	while(ptr != NULL) {
		if(strcasecmp(ptr->name, resname.name_.c_str()) == 0)
			break;
		ptr = ptr->next;
	}

	if(ptr) {
		*resdata = ptr->data;
		return ptr->size;
	} else {
		*resdata = NULL;
		return 0;
	}
}

void Resfile::remove(const char* resname) {
	Resdata *ptr, *prev;

	prev = NULL;
	ptr = list;

	while(ptr != NULL) {
		if(strcasecmp(ptr->name, resname) == 0)
			break;
		prev = ptr;
		ptr = ptr->next;
	}

	if(ptr) {
		if(prev)
			prev->next = ptr->next;
		else
			list = ptr->next;
		ptr->next = NULL;
		delete ptr;
	}
	// Can somebody tell me why Resfile::list isn't an std::vector<Resdata>?
	// We all know linked lists suck, don't we? Whatever...
}
