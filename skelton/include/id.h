/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_ID
#define _HEADER_ID

#include "types.h"

class Identifyable {
	static Dword next_id;
protected:
	Dword the_id;
public:
	Identifyable() {
		the_id=next_id++;
	}
	virtual void set_id(Dword id) {
		the_id=id;
	}
	virtual Dword id() {
		return the_id;
	}
};

#endif
