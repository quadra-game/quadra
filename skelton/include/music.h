/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_MUSIC
#define _HEADER_MUSIC

class Music {
public:
	bool active;
	static Music* alloc();
	virtual ~Music() { };
	virtual void open() = 0;
	virtual void close() = 0;
	virtual void play(int quel, bool loop=false) = 0;
	virtual void replay() = 0;
	virtual void stop() = 0;
};

extern Music *music;

#endif
