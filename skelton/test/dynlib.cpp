/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "dynlib.h"
#include <stdio.h>

void start_game() {
	DynLib *dlobj;
	void *test;

	dlobj = new DynLib("libz.so");

	if(!dlobj->openLibrary())
		printf("openLibrary() error: %s\n", dlobj->getError());

	test = dlobj->getSymbol("foo");
	if(!test) {
		printf("getSymbol() error: %s\n", dlobj->getError());
	}

	test = dlobj->getSymbol("zlibVersion");
	if(!test) {
		printf("getSymbol() error: %s\n", dlobj->getError());
	}

	/* this step is optional, the destructor calls closeLibrary */
	dlobj->closeLibrary();

	delete dlobj;
}
