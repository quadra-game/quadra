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
#include <stdlib.h>
#include <stdio.h>
// for isalpha:
#include <ctype.h>
#include "url.h"

Url::Url(const char* u) {
	setFull(u);
}

const char* Url::getScheme() const {
	return scheme;
}

const char* Url::getHost() const {
	return host;
}

Word Url::getPort() const {
	return port;
}

const char* Url::getPath() const {
	return path;
}

void Url::getFull(char* buf) const {
	buf[0]=0;
	if(strlen(getScheme())) {
		strcat(buf, getScheme());
		strcat(buf, "://");
	}
	strcat(buf, getHost());
	if(getPort()) {
		char n[16];
		sprintf(n, ":%i", getPort());
		strcat(buf, n);
	}
	strcat(buf, getPath());
}

void Url::setScheme(const char* s) {
	if(strlen(s) < sizeof(scheme)) {
		strcpy(scheme, s);
		if(!port) {
			if(!strcasecmp(scheme, "http"))
				port=80;
			if(!strcasecmp(scheme, "ftp"))
				port=21;
			//Add more schemes with default ports here (or don't)
		}
	}
}

void Url::setHost(const char* h) {
	unsigned int len;
	char* sep=strchr(h, ':');
	if(sep) {
		port=atoi(sep+1);
		len=sep-h;
	}
	else
		len=strlen(h);
	if(len<sizeof(host)) {
		memcpy(host, h, len);
		host[len]=0;
	}
}

void Url::setPort(const Word p) {
	port=p;
}

void Url::setPath(const char* p) {
	if(strlen(p)<sizeof(path)-1) {
		if(p[0] != '/')
			strcpy(path, "/");
		else
			path[0]=0;
		strcat(path, p);
	}
}

//Parse a URL, roughly following standard RFC 1808
void Url::setFull(const char* u) {
	port=0;
	int len;
	char buf[sizeof(path)];
	char fragment[sizeof(path)];
	char rest[sizeof(path)];
	if(strlen(u)>sizeof(rest)) {
		//Url too long, fuck it
		strcpy(scheme, "");
		strcpy(host, "");
		strcpy(path, "");
	}
	else
		strcpy(rest, u);
	char* sep;
	//Find fragment
	sep=strchr(rest, '#');
	if(sep) {
		//Keep the frament for later (when we set the path)
		strcpy(fragment, sep);
		*sep=0; //Remove fragment
	}
	else {
		//No fragment
		fragment[0]=0;
	}
	//Look for scheme
	//NB: As opposed to RFC 1808, we don't consider numbers, '+',
	//    '-' or '.' to be legal in scheme names. It just wouldn't
	//    feel right (if everybody followed standards, life would
	//    be boring anyway)
	sep=strstr(rest, "://");
	if(sep) {
		bool legalscheme=true;
		char* p;
		for(p=rest; p<sep; p++) {
			if(!isalpha(*p)) {
				legalscheme=false;
				break;
			}
		}
		if(legalscheme) {
			len=sep-rest;
			memcpy(buf, rest, len);
			buf[len]=0;
			setScheme(buf);
			strcpy(rest, sep+3);
		}
		else
			setScheme("");
	}
	else
		setScheme("");
	//The next slash is supposed to be our path separator
	sep=strchr(rest, '/');
	if(sep) {
		len=sep-rest;
		memcpy(buf, rest, len);
		buf[len]=0;
		//setHost will parse the port if present
		setHost(buf);
		//We keep the '/' in path
		strcpy(rest, sep);
	}
	else {
		//We have no path at all
		setHost(rest);
		rest[0]=0;
	}
	//Add (possibly empty) fragment
	strcat(rest, fragment);
	setPath(rest);
}
