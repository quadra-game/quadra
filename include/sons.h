/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_SONS
#define _HEADER_SONS

#include "sound.h"

typedef struct {
	Sample *fadein, *fadeout, *point, *click, *drip, *glass, *depose;
	Sample *depose2, *depose3, *depose4, *flash, *levelup, *bonus1, *pause;
	Sample *enter, *start, *msg, *potato_get, *potato_rid;
} Samples;

extern Samples sons;

#endif
