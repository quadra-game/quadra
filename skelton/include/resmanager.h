/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_RESMANAGER
#define _HEADER_RESMANAGER

#include "types.h"
#include "array.h"

class Resfile;

class Resmanager {
private:
	Array<Resfile*> files;
public:
	Resmanager();
	virtual ~Resmanager();
	virtual void loadresfile(const char *fname);
	virtual int get(const char *resname, Byte **resdata);
};

extern Resmanager *resmanager;

#endif
