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
#include "stringtable.h"
#include "../include/texte.h"

RCSID("$Id$")

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

