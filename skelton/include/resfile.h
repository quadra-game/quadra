/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_RESFILE
#define _HEADER_RESFILE

#include "types.h"

const char signature[4] = "UGS";

class Resdata {
private:
	friend class Resfile;
	int size;
	Byte *data;
public:
	char *name;
	Resdata *next;
	Resdata(char *resname, int ressize, Byte *resdata, Resdata *list);
	virtual ~Resdata();
};

class Res_dos;

class Resfile {
private:
	Res_dos *res;
public:
	Resdata *list;
	Resfile(const char *fname, bool ro=true);
	virtual ~Resfile();
	virtual void freeze();
	virtual void thaw();
	virtual void clear();
	virtual void add(const char *resname, int size, const char *resdata);
	virtual int get(const char *resname, Byte **resdata);
	virtual void remove(const char* resname);
};

#endif
