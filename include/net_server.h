/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_NET_SERVER
#define _HEADER_NET_SERVER

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

class Net_server: public Net_callable {
	Array<Executor *> pendings;
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
