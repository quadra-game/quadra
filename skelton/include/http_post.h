/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * 
 * Quadra, an action puzzle game
 * Copyright (C) 1998-2000  Ludus Design
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
