/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_FONTS
#define _HEADER_FONTS

class Fontdata;

class Fonts {
public:
	Fontdata *normal, *courrier;
	void init();
	void deinit();
};

extern Fonts fonts;

#endif
