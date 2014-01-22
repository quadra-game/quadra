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
