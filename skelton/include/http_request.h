/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
