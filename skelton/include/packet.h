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

#ifndef _HEADER_PACKET
#define _HEADER_PACKET

#include "track.h"
#include "types.h"

class Net_buf;
class Net_connection;

class Packet {
	TRACKED;
public:
	bool istcp;
	Net_connection *from;
	Dword from_addr;
	Byte packet_id;
	Packet();
	virtual ~Packet() { };
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
};

class Packet_tcp: public Packet {
public:
	Packet_tcp();
	virtual bool read(Net_buf *p);
};

class Packet_udp: public Packet {
public:
	Dword magic;
	Packet_udp();
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_ping: public Packet_tcp {
public:
	Dword uid;
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
	void answer(Packet_ping *p2);
};

#endif
