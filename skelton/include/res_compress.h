/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_RES_COMPRESS
#define _HEADER_RES_COMPRESS

#include "res.h"

class Res_compress: public Res_mem {
	Res_mode mode;
	Res *res;
	Res_dos *res_dos;
	mutable Dword ressize;
	mutable Dword write_pos;
	void read_uncompress();
public:
	Byte *write_compress(Dword *size);
	bool exist;
	Res_compress(const char *fil, Res_mode pmode=RES_READ, bool res_doze=false);
	virtual ~Res_compress();
	virtual void write(const void *b, int nb);
	virtual Dword size();
};

#endif
