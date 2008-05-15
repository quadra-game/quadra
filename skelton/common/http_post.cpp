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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "error.h"
#include "version.h"
#include "net.h"
#include "http_post.h"

RCSID("$Id$")

Http_post::Http_post(const char* aHost, int port, const char *path): Http_request(aHost, port), data(0, 1024) {
	init(path);
}

Http_post::Http_post(const char* aHost, Dword hostaddr, int port, const char *path): Http_request(aHost, hostaddr, port), data(0, 1024) {
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

void Http_post::add_data_raw(const Buf &m) {
	data.append(m.get(), m.size());
}

void Http_post::add_data_raw(const char* m) {
	data.append((const Byte*)m, strlen(m));
}

void Http_post::send() {
	char st[256];
	url.resize(0);
	url.append("POST ");
	url.append(cgi);
	url.append(" HTTP/1.0\r\n");
	if(host) {
		url.append("Host: ");
		url.append(host);
		url.append("\r\n");
	}
	sprintf(st, "User-Agent: Quadra/%s\r\n", VERSION_STRING);
  	url.append(st);
	//Try to make those idiot proxies behave. Long life e2e!!! :)
	url.append("Pragma: no-cache\r\n");
	url.append("Cache-Control: no-cache\r\n");
	url.append("Content-type: application/x-www-form-urlencoded\r\nContent-length: ");
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
