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

#ifndef _HEADER_NET_BUF
#define _HEADER_NET_BUF

#include "types.h"
#include "net.h"

class Net_buf {
public:
	Byte *point;
	Net_connection *from;
	Dword from_addr;
	Byte buf[1024];
	void write_dword(Dword v) {
		*(Dword *) point = htonl(v);
		point += sizeof(Dword);
	}
	void write_word(Word v) {
		*(Word *) point = htons(v);
		point += sizeof(Word);
	}
	void write_byte(Byte v) {
		*(Byte *) point = v;
		point += sizeof(Byte);
	}
	void write_bool(bool b) {
		write_byte(b? 1:0);
	}
	void write_mem(void *v, int num) {
		memcpy(point, v, num);
		point += num;
	}
	void write_string(char *v) {
		write_mem(v, strlen(v)+1); // ecrit un string avec son '0'
	}
	Dword read_dword() {
		if(((unsigned int)len())<=1024-sizeof(Dword)) {
			Dword ret = ntohl(*(Dword *) point);
			point += sizeof(Dword);
			return ret;
		}
		else
			return 0;
	}
	Word read_word() {
		if(((unsigned int)len())<=1024-sizeof(Word)) {
			Word ret = ntohs(*(Word *) point);
			point += sizeof(Word);
			return ret;
		}
		else
			return 0;
	}
	Byte read_byte() {
		if(((unsigned int)len())<=1024-sizeof(Byte)) {
			Byte ret = *(Byte *) point;
			point += sizeof(Byte);
			return ret;
		}
		else
			return 0;
	}
	bool read_bool() {
		if(read_byte())
			return true;
		else
			return false;
	}
	void read_mem(void *v, int num) {
		if(len()<=1024-num) {
			memcpy(v, point, num);
			point += num;
		}
		else
			memset(v, 0, num);
	}
	bool read_string(char *v, int size) { // lit un string avec son '0'
		do {
			char c=(char)read_byte();
			if(c>0 && c<' ')
				c=' ';
			*v=c;
			if(!*v)
				return true;
			v++;
			size--;
		} while(size);
		if(v[-1]) {
			v[-1]=0;
			return false;
		}
		return true;
	}
	Net_buf() {
		reset();
		from=NULL;
		memset(buf, 0, sizeof(buf));
	}
	void reset() {
		point = buf;
	}
	const int len() const {
		return point - buf;
	}
};

#endif
