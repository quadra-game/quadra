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
#include "res.h"
#include "resfile.h"

#define stricmp strcasecmp

Resdata::Resdata(char *resname, int ressize, Byte *resdata, Resdata *list) {
	name = resname;
	size = ressize;
	data = resdata;
	next = list;
}

Resdata::~Resdata() {
	delete name;
	delete data;
	if(next)
		delete next;
}

Resfile::Resfile(const char *fname, bool ro) {
	list = NULL;

	if(ro)
		res = new Res_dos(fname, RES_TRY);
	else
		res = new Res_dos(fname, RES_CREATE);

	if(res->exist) {
	  skelton_msgbox("Resfile %s is fine\n", fname);
	  thaw();
	} else
	  skelton_msgbox("Resfile %s does not exist\n", fname);
}

Resfile::~Resfile() {
	clear();
	if(res)
		delete res;
}

void Resfile::thaw() {
	char sig[sizeof(signature)];
	int resnamelen;
	char *resname;
	Byte *resdata;
	int ressize;

	res->position(0);

	res->read(sig, sizeof(signature));
	if(strncmp(sig, signature, sizeof(signature)) != 0) {
		msgbox("Resfile::thaw(): invalid signature\n");
		return;
	}

	do {
		res->read(&resnamelen, sizeof(resnamelen));
		if(resnamelen == 0)
			break;
		resname = new char[resnamelen];
		res->read(resname, resnamelen);
		res->read(&ressize, sizeof(ressize));
		resdata = new Byte[ressize];
		res->read(resdata, ressize);

		list = new Resdata(resname, ressize, resdata, list);
	} while(resnamelen > 0);

}

void Resfile::clear() {
	if(list)
		delete list;

	list = NULL;
}

int Resfile::get(const char *resname, Byte **resdata) {
	Resdata *ptr;

	ptr = list;

	while(ptr != NULL) {
		if(stricmp(ptr->name, resname) == 0)
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
		if(stricmp(ptr->name, resname) == 0)
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
	//Can somebody tell me why Resfile::list isn't an Array<Resdata>?
	//  We all know linked lists suck, don't we? Whatever...
}
