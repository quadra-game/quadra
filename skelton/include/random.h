/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_RANDOM
#define _HEADER_RANDOM

#include "types.h"

class Random {
	int seed;
public:
	Random();
	Random(int p);
	int get_seed() const;
	void set_seed(int p);
	Word rnd(int and=0xFFFF);
	Word crap_rnd(int and=0xFFFF);
};

extern Random ugs_random;

#endif
