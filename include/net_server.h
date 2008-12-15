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
#ifndef _HEADER_NET_SERVER
#define _HEADER_NET_SERVER

#include <vector>

#include "types.h"
#include "net_stuff.h"
#include "overmind.h"

class Net_client: public Net_callable {
	void pause(Packet *p2);
public:
	Net_client();
	virtual ~Net_client();
	virtual void net_call(Packet *p2);
};

class Recording;
class Packet_wantjoin;
class Packet_gameserver;

class Net_server: public Net_callable {
	std::vector<Executor*> pendings;
	void playerwantjoin(Packet *p2);
	void findgame(Packet *p2);
	void wantjoin(Packet *p2);
	void clientstartwatch(Packet *p2);
	void clientmoves(Packet *p2);
	void clientchat(Packet *p2);
public:
	void clientpause(Packet *p2);
	bool allow_start;
	bool allow_pause;
	void record_packet(Packet *p2);
	void stop_multi_recording();
	Net_server();
	virtual ~Net_server();
	virtual void net_call(Packet *p2);
};

class Net_pendingjoin: public Module, public Notifyable {
	Packet_wantjoin *pac;
	bool cancel;
public:
	Net_pendingjoin(Packet_wantjoin *p);
	virtual ~Net_pendingjoin();
	virtual void step();
	virtual void notify();
	static void load_packet_gameserver(Packet_gameserver* resp);
};

#endif
