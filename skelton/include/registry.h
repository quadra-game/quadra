/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_REGISTRY
#define _HEADER_REGISTRY

class Registry {
public:
	static Registry *alloc();
	virtual ~Registry() { };
	virtual void open(const char *n, const char *dir)=0;
	virtual void write(const char *k, char *v)=0;
	virtual void read(const char *k, char *buffer, unsigned long size)=0;
	virtual void close()=0;
};

#endif
