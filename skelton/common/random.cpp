/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <time.h>
#include "types.h"
#include "random.h"

Random ugs_random;

Random::Random() {
	set_seed(time(NULL));
}

Random::Random(int p) {
	set_seed(p);
}

Word Random::rnd(int and) { // better proc
	int tmp;
	seed = seed * 0x41c64e6d + 0x00003039;
	tmp = seed >> 10;

	return (Word) (tmp & and);
};

Word Random::crap_rnd(int and) { // crappy proc
	int tmp;
	tmp = seed * 0x41c64e6d + 0x00003039;
	seed = tmp >> 10;

	return (Word) (tmp & and);
};

int Random::get_seed() const {
	return seed;
}

void Random::set_seed(int p) {
	seed = p;
}
