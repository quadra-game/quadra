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

using std::string;
using std::vector;

void Resmanager::loadresfile(const char *fname) {
	Resfile* rf=new Resfile(fname);
	files.push_back(rf);
	Resdata* rd = rf->list;
	while(rd) {
		char *name=rd->name;
		int i;
		for(i=files.size()-2; i>=0; i--)
			files[i]->remove(name);
		rd = rd->next;
	}
}

int Resmanager::get(const ResName& resname, uint8_t **resdata) const {
	int i=files.size();
	while(i--) {
		int ret=files[i]->get(resname, resdata);
		if(*resdata)
			return ret;
	}
	return 0;
}

std::string Resmanager::get(const ResName& resname) const {
  uint8_t* buf;
  const int size(get(resname, &buf));

  if (size == 0)
    return string();
  else
    return string(reinterpret_cast<char*>(buf), size);
}

Resmanager::~Resmanager() {
	vector<Resfile*>::const_iterator it;
	for (it = files.begin(); it != files.end(); ++it)
		delete *it;
}

Resmanager *resmanager;
