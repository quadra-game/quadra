/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
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
	Url url(config.info.game_server_address);
	if(!url.getPort())
		url.setPort(80);
	if(!strcmp(url.getHost(), ""))
		url.setHost("ludusdesign.com:80");
	if(!strcmp(url.getPath(), ""))
		url.setPath("/cgibin/qserv.pl");
	if(http_addr)
		req=new Http_post(http_addr, http_port, url.getPath());
	else
		req=new Http_post(url.getHost(), url.getPort(), url.getPath());
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
	delete req;
	req=NULL;
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

void Qserv::send() {
	req->add_data_encode("info/language %i\n", config.info.language);
	if(config.registered)
		req->add_data_encode("info/registered %i\n", Config::registered? 1:0);
	req->add_data_encode("info/quadra_version %i.%i.%i\n", config.major, config.minor, config.patchlevel);
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
		req->add_data_encode("info/platform/display %s\n", video->xwindow ? "Xlib":"Svgalib");
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
