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

#ifndef _HEADER_HTTP_REQUEST
#define _HEADER_HTTP_REQUEST

#include "types.h"
#include "buf.h"

class Net_connection_tcp;

class Http_request {
protected:
	static char base64table[64];
	static Byte reversebase64table[256];
	Net_connection_tcp *nc;
	Buf buf;
	const Byte *request;
	int size;
	void sendrequest();
	bool sent;
public:
	Http_request(const char *host, int port, const Byte *request=NULL, int size=0);
	Http_request(Dword hostaddr, int port, const Byte *request=NULL, int size=0);
	virtual ~Http_request();
	Byte *getbuf() const;
	Dword getsize() const;
	Dword getnbrecv() const;
	bool isconnected() const;
	bool done();

	Dword gethostaddr() const;
	int gethostport() const;

	static void base64encode(const Byte *in, Textbuf& out, Dword size);
	static void base64decode(const char *in, Buf& out, Dword size);

	static void url_encode(const char *src, Textbuf& dest);
};

#endif
