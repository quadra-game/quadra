/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <stdio.h>
#include "res.h"
#include "resfile.h"

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
