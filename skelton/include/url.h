/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_URL
#define _HEADER_URL

#include "types.h"

//NB: This is not a complete parser. URL fragments, parameters and
//    query information will remain in the path unparsed
class Url {
	//sizeof(path) should remain >= than all others
	char scheme[16];
	char host[256];
	Word port;
	char path[256];
public:
	Url(const char* u="");
	const char* getScheme() const;
	const char* getHost() const;
	Word getPort() const;
	const char* getPath() const;
	void getFull(char* buf) const;
	void setScheme(const char* s);
	void setHost(const char* h);
	void setPort(const Word p);
	void setPath(const char* p);
	void setFull(const char* u);
};

#endif
