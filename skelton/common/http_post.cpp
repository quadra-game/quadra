/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "error.h"
#include "http_post.h"

Http_post::Http_post(const char *host, int port, const char *path): Http_request(host, port), data(0, 1024) {
	init(path);
}

Http_post::Http_post(Dword hostaddr, int port, const char *path): Http_request(hostaddr, port), data(0, 1024) {
	init(path);
}

Http_post::~Http_post() {
}

void Http_post::init(const char *path) {
	strcpy(cgi, path);
}

void Http_post::add_data_encode(const char* m, ...) {
	char st[32768];
	Textbuf buf;
	va_list marker;
	va_start(marker, m);
	vsprintf(st, m, marker);
	va_end(marker);
	Http_request::url_encode(st, buf);
	add_data_raw(buf.get());
}

void Http_post::add_data_raw(const char* m) {
	data.append((const Byte*)m, strlen(m));
}

void Http_post::send() {
	url.resize(0);
	url.append("POST ");
	url.append(cgi);
	url.append(" HTTP/1.0\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: ");
	char st[16];
	sprintf(st, "%i\r\n\r\n", data.size());
	url.append(st);
	url.append(data.get(), data.size());
	request = (Byte*)url.get();
	size = url.size();
	/*
	st[0]=0;
	url.append((Byte*)st, 1); //So the following msgbox won't crash
	msgbox("Http_post::send: data: \n{\n%s\n} size=%i\n", request, size);
	*/
}
