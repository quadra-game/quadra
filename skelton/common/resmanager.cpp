/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "resfile.h"
#include "resmanager.h"

Resmanager::Resmanager() {
}

void Resmanager::loadresfile(const char *fname) {
	Resfile* rf=new Resfile(fname);
	files.add(rf);
	Resdata* rd = rf->list;
	while(rd) {
		char *name=rd->name;
		int i;
		for(i=files.size()-2; i>=0; i--)
			files[i]->remove(name);
		rd = rd->next;
	}
}

int Resmanager::get(const char *resname, Byte **resdata) {
	int i=files.size();
	while(i--) {
		int ret=files[i]->get(resname, resdata);
		if(*resdata)
			return ret;
	}
	return 0;
}

Resmanager::~Resmanager() {
	files.deleteall();
}

Resmanager *resmanager;
