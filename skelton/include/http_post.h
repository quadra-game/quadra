/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_HTTP_POST
#define _HEADER_HTTP_POST

#include "types.h"
#include "http_request.h"

class Http_post: public Http_request {
	Buf data;
	Buf url;
	char cgi[256];
	void init(const char *path);
public:
	Http_post(const char *host, int port, const char *path);
	Http_post(Dword hostaddr, int port, const char *path);
	virtual ~Http_post();
	void add_data_raw(const char* m);
	void add_data_encode(const char* m, ...);
	void send();
};

#endif
