/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <stdio.h>
#include "res.h"
#include "resfile.h"
#include "stringtable.h"
#include "../include/texte.h"

/* Ceci est un test pour Res_doze et stringtable */

void start_game() {
	int i;

#ifdef UGS_LINUX
	resfile = new Resfile("../source/quadra.res");
#endif
	stringtable = new Stringtable("texte.txt");

	i = 1;
	while(i <= 363) {
		printf("%3i - %s\n", i, ST_BASE(i));
		i++;
	}

}

