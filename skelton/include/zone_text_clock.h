/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_ZONE_TEXT_CLOCK
#define _HEADER_ZONE_TEXT_CLOCK

#include "inter.h"

class Zone_text_clock: public Zone_text_field {
protected:
  char timebuf[256];
public:
	Zone_text_clock(Inter* in, Dword *s, int px, int py, int pw, bool frame, Font *f2=NULL);
	virtual ~Zone_text_clock();
	virtual void draw();
};

#endif
