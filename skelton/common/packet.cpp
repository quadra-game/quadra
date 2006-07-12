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

#include "packet.h"

#include "error.h"
#include "net.h"
#include "net_buf.h"

Packet::Packet() {
	istcp=false;
	from=NULL;
	from_addr=0;
	packet_id=0xFF;
}

void Packet::write(Net_buf *p) {
	p->reset();
	p->write_byte((Byte) packet_id);
}

bool Packet::read(Net_buf *p) {
	p->reset();
	from = p->from;
	from_addr = p->from_addr;
	packet_id = p->read_byte();
	return true;
}

Packet_tcp::Packet_tcp() {
	istcp=true;
}

bool Packet_tcp::read(Net_buf *p) {
	if(!Packet::read(p))
		return false;
	if(!istcp)
		return false;
	return true;
}

Packet_udp::Packet_udp() {
	istcp=false;
	magic=net->net_param->magic();
}

void Packet_udp::write(Net_buf *p) {
	Packet::write(p);
	p->write_dword(magic);
}

bool Packet_udp::read(Net_buf *p) {
	if(!Packet::read(p))
		return false;
	magic = p->read_dword();
	if(magic!=net->net_param->magic())
		return false;
	if(istcp)
		return false;
	return true;
}

void Packet_ping::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_dword(uid);
}

bool Packet_ping::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	uid = p->read_dword();
	return true;
}

void Packet_ping::answer(Packet_ping *p2) {
	uid=p2->uid;
	net->sendtcp(p2->from, this);
}

Dword Exec_ping::next_uid=0;

Exec_ping::Exec_ping(Packet_ping *p, Word pt, Net_callable *netc, Net_connection *d) {
	dest = d;
	type = pt;
	uid = p->uid = next_uid++;
	net_callable = netc;
	if(net_callable)
		net->addwatch(type, this);
	if(d) {
		net->sendtcp(d, p);
	}
	else {
		net->sendtcp(p);
	}
}

Exec_ping::~Exec_ping() {
	if(net_callable)
		net->removewatch(type, this);
}

void Exec_ping::net_call(Packet *p2) {
	Packet_ping *p=(Packet_ping *) p2;
	if(p && validate(p) && net_callable) {
		net_callable->net_call(p);
		net_callable=NULL;
		net->removewatch(type, this);
	}
	delete p2;
}

bool Exec_ping::validate(Packet_ping *p) {
	if(dest && p->from!=dest)
		return false;
	return p->packet_id == type && p->uid == uid;
}
