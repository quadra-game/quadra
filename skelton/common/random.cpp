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

#include "random.h"

#include <time.h>
#include "types.h"

Random ugs_random;

Random::Random() {
	set_seed(time(NULL));
}

Random::Random(time_t p) {
	set_seed(p);
}

Word Random::rnd(int _and) { // better proc
	int tmp;
	seed = seed * 0x41c64e6d + 0x00003039;
	tmp = seed >> 10;

	return (Word) (tmp & _and);
};

Word Random::crap_rnd(int _and) { // crappy proc
	int tmp;
	tmp = seed * 0x41c64e6d + 0x00003039;
	seed = tmp >> 10;

	return (Word) (tmp & _and);
};

time_t Random::get_seed() const {
	return seed;
}

void Random::set_seed(time_t p) {
	seed = p;
}
