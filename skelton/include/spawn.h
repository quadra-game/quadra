/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_SPAWN
#define _HEADER_SPAWN

class Process {
public:
	static Process *alloc();
	virtual ~Process();
	virtual void init(const char *fn, int argc, char *argv[])=0;
	virtual bool done()=0;
	virtual int get_exit_code()=0;
};

#endif
