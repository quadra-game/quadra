/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_NET_CALL
#define _HEADER_NET_CALL

class Packet;

class Net_callable {
public:
	virtual void net_call(Packet *p)=0;
};

#endif
