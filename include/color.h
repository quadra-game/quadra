/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_COLOR
#define _HEADER_COLOR

#include "palette.h"

class Color {
	Byte base;
	Palette& pal;
public:
	Color(Byte q, Palette& p);
	void set(int r, int g, int b, int r2, int g2, int b2);
	Byte shade(Byte i) const {
		return base+i;
	}
};

#endif
