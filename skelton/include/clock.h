/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_CLOCK
#define _HEADER_CLOCK

#include "types.h"

class Clock {
	Clock();
public:
	static int get_time();
	static char *time2char(int time);
	static char *absolute_time();
};

#endif
