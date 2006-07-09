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

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "autoconf.h"
#include "config.h"
#include "url.h"
#include "http_post.h"
#include "dict.h"
#include "stringtable.h"
#include "video.h"
#include "qserv.h"

Dword Qserv::http_addr=0;
int Qserv::http_port=0;

Qserv::Qserv() {
	req=NULL;
	status[0]=0;
	reply=NULL;

	create_req();

	req->add_data_raw("data=");
}

Qserv::~Qserv() {
	if(req)
		delete req;
	if(reply)
		delete reply;
}

bool Qserv::done() {
	if(!req)
		return true;
	if(!req->done())
		return false;

	//Save ip info for future requests
	Qserv::http_addr = req->gethostaddr();
	Qserv::http_port = req->gethostport();

	//Parse reply
	reply=new Dict();
	if(!req->getsize()) {
		strcpy(status, "");
	}
	else {
		Stringtable st(req->getbuf(), req->getsize());
		int i=0;
		for(i=0; i<st.size(); i++)
			if(strlen(st.get(i))==0) { //end of HTTP header
				i++; // Skip blank line
				if(i>=st.size())
					strcpy(status, "");
				else {
					strncpy(status, st.get(i), sizeof(status)-1);
					status[sizeof(status)-1]=0;
					i++; // Skip status line
				}
				break;
			}
		while(i<st.size()) {
			//msgbox("Qserv::done: adding [%-60.60s]\n", st.get(i));
			reply->add(st.get(i));
			i++;
		}
	}
	if(!strcmp(status, "Redirect permanent") && reply->find("location")) {
		//we are being redirected to another qserv server
		//update the config
		strncpy(config.info.game_server_address, reply->find("location"), sizeof(config.info.game_server_address)-1);
		config.info.game_server_address[sizeof(config.info.game_server_address)-1]=0;
		//clear cache of ip address unless we use a proxy
		Url proxy(config.info2.proxy_address);
		if(!strlen(proxy.getHost())) {
			Qserv::http_addr = 0;
			Qserv::http_port = 0;
		}
		//and retry the query with the same data
		Buf data = req->get_data();
		delete req;
		delete reply;
		status[0] = 0;
		create_req();
		req->add_data_raw(data);
		req->send();
		return false;
	}
	else {
		delete req;
		req=NULL;
	}
	msgbox("Qserv::done: done\n");
	return true;
}

void Qserv::add_data(const char *s, ...) {
	char st[32768];
	Textbuf buf;
	va_list marker;
	va_start(marker, s);
	vsprintf(st, s, marker);
	va_end(marker);
	Http_request::url_encode(st, buf);
	req->add_data_raw(buf.get());
}

void Qserv::add_data_large(const Textbuf &buf) {
	req->add_data_raw(buf.get());
}

void Qserv::send() {
	req->add_data_encode("info/language %i\n", config.info.language);
	req->add_data_encode("info/platform/os %s\n",
		#if defined(UGS_DIRECTX)
			"Windows"
		#elif defined(UGS_LINUX)
			"Linux i386"
		#else
			#error "What platform???"
		#endif
	);
	if(video_is_dumb)
		req->add_data_encode("info/platform/display None\n");
	else {
		#if defined(UGS_LINUX)
		req->add_data_encode("info/platform/display Xlib\n");
		#endif
		#if defined(UGS_DIRECTX)
		req->add_data_encode("info/platform/display DirectX\n");
		#endif
	}
	req->send();
}

const char *Qserv::get_status() {
	if(status[0])
		return status;
	else
		return NULL;
}

Dict *Qserv::get_reply() {
	return reply;
}

bool Qserv::isconnected() const {
	if(req && req->isconnected())
		return true;
	return false;
}

Dword Qserv::getnbrecv() const {
	int val = 0;
	if(req) {
		val = req->getsize();
		if(val < 0)
			val = 0;
	}
	return val;
}

void Qserv::create_req()
{
	const char* host;
	int port;
	char path[256];

	Url url(config.info.game_server_address);
	if(!url.getPort())
		url.setPort(80);
	if(!strcmp(url.getHost(), ""))
		url.setHost("quadra.sourceforge.net:80");
	if(!strcmp(url.getPath(), "/"))
		url.setPath("/cgi-bin/qserv.pl");

	Url proxy(config.info2.proxy_address);
	if(!proxy.getPort())
		proxy.setPort(80);

	if(strlen(proxy.getHost())) {
		//Use proxy info for host and port, and full game server address for path
		host = proxy.getHost();
		port = proxy.getPort();
		url.getFull(path);
	}
	else {
		//No proxy configuration, use game server address for everything
		host = url.getHost();
		port = url.getPort();
		strcpy(path, url.getPath());
	}

	//Use IP cache if set
	if(http_addr)
		req=new Http_post(host, http_addr, http_port, path);
	else
		req=new Http_post(host, port, path);
}
