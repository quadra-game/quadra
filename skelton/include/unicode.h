/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_UNICODE
#define _HEADER_UNICODE

#include "types.h"
#include "buf.h"

class Unicode {
	Buf the_string;
public:
	Unicode(char *s);
	Unicode &cat(char *s);
	operator Byte *();
	Dword size();
};

#endif
