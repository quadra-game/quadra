/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
