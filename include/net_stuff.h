/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_NET_STUFF
#define _HEADER_NET_STUFF

enum Drop_reason {
	DROP_AUTO,
	DROP_MANUAL,
	DROP_INVALID_BLOCK,
	DROP_LAST
};

#include "types.h"
#include "array.h"
#include "global.h"
#include "packets.h"
#include "net.h"
#include "net_call.h"
#include "overmind.h"

class Net_starter {
	class Net_module: public Module {
		Dword last_video_frame;
	public:
		Net_module();
		virtual void step();
	};
	Executor *net_exec;
public:
	Net_starter();
	virtual ~Net_starter();
};

extern Net_starter* net_starter;

class Quadra_param: public Net_param {
public:
	virtual int tcpport();
	virtual void print_packet(Packet *p2, char *st);
	virtual Packet *alloc_packet(Word pt);
	virtual bool is_dispatchable(Net_connection *nc, Packet *p);
	virtual Dword magic() {
		return ('R'<<24) | ('M'<<16) | ('T'<<8) | ('3');
	}
	virtual void server_deconnect();
	virtual void client_connect(Net_connection *adr);
	virtual void client_deconnect(Net_connection *adr);
	virtual bool accept_connection(Net_connection *nc);
	virtual char *get_motd();
};

extern void send_msg(Net_connection *nc, char *msg, ...);

#endif
