/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "res.h"
#include "sprite.h"
#include "fonts.h"

void Fonts::init() {
	{
		Res_doze res("font.fnt");
		normal = new Fontdata(res, 2);
	}
	{
		Res_doze res("courrier.fnt");
		courrier = new Fontdata(res, 1);
	}
}

void Fonts::deinit() {
	delete courrier;
	delete normal;
}

Fonts fonts;
