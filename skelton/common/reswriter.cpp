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
#include "res.h"
#include "resfile.h"

RCSID("$Id$")

void Resfile::freeze() {
	int resnamelen;
	Resdata *ptr;

	res->write(&signature, sizeof(signature));

	ptr = list;

  while(ptr != NULL) {
		resnamelen = strlen(ptr->name)+1;
		res->write(&resnamelen, sizeof(resnamelen));
		res->write(ptr->name, resnamelen);
		res->write(&ptr->size, sizeof(ptr->size));
		res->write(ptr->data, ptr->size);
    ptr = ptr->next;
  }

	resnamelen = 0;
	res->write(&resnamelen, sizeof(resnamelen));
}

void Resfile::add(const char *resname, const int size, const char *resdata) {
	char *myname;
	Byte *mydata;

	myname = new char[strlen(resname)+1];
	memcpy(myname, resname, strlen(resname)+1);
	mydata = new Byte[size];
	memcpy(mydata, resdata, size);

	list = new Resdata(myname, size, mydata, list);
}
