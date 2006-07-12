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

#include "http_request.h"

#include <stdio.h>
#include "error.h"
#include "net.h"

char Http_request::base64table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
};

Byte Http_request::reversebase64table[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62,
	0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
	45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void Http_request::base64encode(const Byte *in, Textbuf& out, Dword size) {
	char* table = base64table;
	Byte a, b, c;
	Dword i;
	char o[5];
	o[4]=0;
	for(i=0; i<(size+2)/3; i++) {
    a = i*3+0<size? in[i*3+0]:0;
    b = i*3+1<size? in[i*3+1]:0;
    c = i*3+2<size? in[i*3+2]:0;
    o[0] = table[a>>2];
    o[1] = table[((a&3)<<4) | (b>>4)];
    o[2] = table[((b&15)<<2) | (c>>6)];
    o[3] = table[c&63];
		out.append("%s", o);
  }
	if(size%3) {
		char *end=out.get()+strlen(out.get())-1;
		switch(size%3) {
			case 1:
				*end--='=';
				// no break, it's normal!
			case 2:
				*end--='=';
		}
	}
}

void Http_request::base64decode(const char *in, Buf& out, Dword size) {
	Byte* table = reversebase64table;
	if(size&3) {
		skelton_msgbox("Http_request::base64decode: incorrect size\n");
		return;
	}
	Byte a, b, c, d;
	Dword i;
	for(i=0; i<size/4; i++) {
		a = in[i*4+0];
		b = in[i*4+1];
		c = in[i*4+2];
		d = in[i*4+3];
		Byte o[3];
		o[0] = (table[a]<<2) | (table[b]>>4);
		o[1] = (table[b]<<4) | (table[c]>>2);
		o[2] = (table[c]<<6) | (table[d]);
		int n=3;
		if(d=='=')
			n--;
		if(c=='=')
			n--;
		out.append(o, n);
	}
}

void Http_request::url_encode(const char *src, Textbuf& dest) {
	char tmp[4];
	while(*src) {
		tmp[0] = *src++;
		tmp[1] = 0;
		if(tmp[0] < 48 || tmp[0] > 122 || (tmp[0] >= 58 && tmp[0] <= 64))
			sprintf(tmp, "%c%02X", '%', (Byte)tmp[0]); // converted to '%FF' url
		dest.append("%s", tmp);
	}
}

Http_request::Http_request(const char *aHost, int port, const Byte *request, int size) {
	if(request) {
		this->request=request;
		this->size=size? size:strlen((const char *)request);
	}
	if(aHost) {
		host = strdup(aHost);
	} else {
		host = NULL;
	}
	nc=net->start_other(aHost, port); // nc could be NULL in case of error!
	sent=false;
}

Http_request::Http_request(const char* aHost, Dword hostaddr, int port, const Byte *request, int size) {
	if(request) {
		this->request=request;
		this->size=size? size:strlen((const char *)request);
	}
	if(aHost) {
		host = strdup(aHost);
	} else {
		host = NULL;
	}
	nc=net->start_other(hostaddr, port); // nc could be NULL in case of error!
	sent=false;
}

Http_request::~Http_request() {
	if(host)
		free(host);
	if(nc)
		delete nc;
}

void Http_request::sendrequest() {
	if(nc) {
		nc->sendtcp(request, size);
		nc->commit();
	}
	sent=true;
}

Byte *Http_request::getbuf() const {
	return buf.get();
}

Dword Http_request::getsize() const {
	return buf.size()? buf.size()-1:0; //Don't count nul byte added in 'done'
}

bool Http_request::isconnected() const { // indicate if the connection has been established
	if(nc && nc->state() == Net_connection::connected)
		return true;
	return false;
}

bool Http_request::done() {
	Net_connection::Netstate state;
	if(nc)
		state = nc->state();
	else
		state = Net_connection::invalid;
	if(state==Net_connection::invalid || state==Net_connection::disconnected) {
		Byte st=0;
		buf.append(&st, 1);
		return true;
	}
	if(state<Net_connection::connected)
		return false;
	if(!sent)
		sendrequest();
	Byte tmp[4096];
	Dword tube=nc->receivetcp(tmp, 4096);
	if(tube)
		buf.append(tmp, tube);
	if(!tube && nc->state()==Net_connection::disconnected) {
		Byte st=0;
		buf.append(&st, 1);
		return true;
	}
	return false;
}

Dword Http_request::gethostaddr() const {
	if(nc)
		return nc->getdestaddr();
	else
		return 0;
}

int Http_request::gethostport() const {
	if(nc)
		return nc->getdestport();
	else
		return 0;
}
