/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_QSERV
#define _HEADER_QSERV

#include "types.h"

class Http_post;
class Dict;

class Qserv {
	Http_post *req;
	char status[256];
	Dict *reply;
public:
	static Dword http_addr;
	static int http_port;
	Qserv();
	virtual ~Qserv();
	bool done();
	void add_data(const char *s, ...);
	void send();
	bool bad_reply();
	const char *get_status();
	Dict *get_reply();
	bool isconnected() const;
	Dword getnbrecv() const;
};

#endif
