/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_MULTI_PROVIDER
#define _HEADER_MULTI_PROVIDER
#include "overmind.h"
#include "inter.h"
#include "bitmap.h"

class Menu_multi_provider: public Menu {
	Bitmap *bit;
	Font *font, *font2;
	const Palette &pal2;
	int quel;
public:
	Menu_multi_provider(int q, Bitmap *b, Font *f, Font *f2, const Palette &p);
	virtual void step();
};

#endif
