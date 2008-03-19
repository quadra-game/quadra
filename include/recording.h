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

#ifndef _HEADER_RECORDING
#define _HEADER_RECORDING

#include "types.h"
#include "net_stuff.h"
#include "buf.h"

class Res_compress;
class Res;
class Canvas;

class Recording {
	char playername[40];
	int score, lines, level;
	/*
	Voici les hunks:
		0: start_for_single: un seed pis 3 repeatspeeds
		1: single_start: un numero de player
		2: game_begin: les touches d'une game single
		3: game_stop: les infos d'une game single
	 11: write_packet: un numero de frame d'overmind (Dword), la size d'un packet (Word) pis le packet
	 13: summary: size (Dword) et du texte (à la Qserv)
	*/
	void write_hunk(Byte h);
	void end_single(Canvas *c);
public:
	Dword frame;
	void step();
	Res_compress *res;
	Recording();
	virtual ~Recording();
	void start_for_multi(Packet* p);
	void write_packet(Packet* p);
	bool create(const char *n);
	void end_multi();
	void write_summary();
};

class Demo_packet {
public:
	Dword frame;
	Packet* p;
	Demo_packet(const Demo_packet& dp);
	Demo_packet(Dword pframe, Packet* pp);
	virtual ~Demo_packet();
};

class Dict;

class Playback {
	Res *res;
	bool *verification_flag;
	Byte read_hunk();
	void read_all(); //Sole caller of next 6
	void read_seed();
	void read_single();
	void read_data();
	void read_info();
	void read_packet();
	void read_summary();
	Buf data;
	Dword nextByte;
	Array<Demo_packet *> packets;
public:
	Byte single_player;
	Packet_gameserver *packet_gameserver;
	Dict *sum;
	bool single();
	bool old_mode;
	bool valid;
	bool completed;
	bool auto_demo;
	void create_game();
	void shit_skipper2000(bool remove_chat);
	int seed;
	struct {
		char name[40];
		int color, shadow, smooth, repeat;
	} player[3];
	int score, lines, level, multi_level;
	Playback(Res* r);
	virtual ~Playback();
	void set_verification_flag(bool *p);
	bool verify_summary(const Game *game);
	Byte get_byte();
	bool check_scores(Canvas* c);
	Demo_packet next_packet();
	void remove_packet();
};

extern Recording *recording;
extern Playback *playback;

#endif
