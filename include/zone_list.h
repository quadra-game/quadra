/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_ZONE_LIST
#define _HEADER_ZONE_LIST

#include "array.h"
#include "types.h"
#include "inter.h"

class Zone_list {
protected:
	Array<Zone *> zone;
	void deleteall();
	virtual ~Zone_list() {
		deleteall();
	}
};

#endif
