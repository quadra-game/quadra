/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_IMAGE
#define _HEADER_IMAGE

#include "types.h"

class Image {
 public:
	virtual int width() const=0;
	virtual int height() const=0;
	virtual Byte* pic() const=0;
	virtual Byte* pal() const=0;
	virtual int palettesize() const=0;
};

#endif
