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

#ifndef _HEADER_GAME_MENU
#define _HEADER_GAME_MENU

#include "net_stuff.h"
#include "net_call.h"

class Game;
class Stringtable;

class Create_game: public Menu, public Notifyable {
	static int game_end_y;
	Bitmap *bit_;
	Zone *start, *cancel, *save;
	Zone *z_record_name, *z_slog_name;
	char name[32], record_name[32], slog_name[32];
	Observable *game_type, *game_end_watch;
	Observable *record_watch, *slog_watch;
	Zone *record_zone, *slog_zone;
	int selected, level_up, game_end, game_end_value, game_public;
	int record_game, slog;
	Zone_text *game_end_text;
	Zone_text *game_desc[10];
	Stringtable *game_descriptions;
	Zone *game_end_selector;
	Zone_input_numeric *game_end_num;
	void save_setting();
	bool net_game, local_net;
	void recreate_game_end();
public:
	Create_game(Bitmap *bit, Font *font, Font *font2, const Palette& p, bool pnet_game, bool plocal_net);
	virtual ~Create_game();
	virtual void step();
	virtual void notify();
};

class Create_game_start: public Module {
	Bitmap *bit_;
	Font *font_;
	const Palette &pal;
public:
	Create_game_start(const Palette &pal_, Bitmap *bit, Font *font);
	virtual ~Create_game_start();
	virtual void init();
};

class Create_game_end: public Menu {
	Zone *cancel;
	const Palette &pal;
public:
	Create_game_end(const Palette &pal_, Bitmap *bit, Font *font);
	virtual ~Create_game_end();
	virtual void step();
	virtual void init();
};

class Join_game: public Menu, public Net_callable {
	Zone *cancel;
	Zone_text *status;
	Packet_wantjoin pac;
	Exec_ping *eping;
	int delay;
	Bitmap *bit_;
	Font *font2_;
	Dword address;
	int port;
	bool rejoin;
public:
	Join_game(Bitmap *bit, Font *font, Font *font2, const Palette& p, const char *n, Dword sa, int sport, bool prejoin);
	virtual ~Join_game();
	virtual void init();
	virtual void step();
	virtual void net_call(Packet *p2);
};

class Join_download: public Menu {
	Bitmap *bit_;
	Font *font2_;
	int nb_total, nb_current, nb_percent;
	bool rejoin;
public:
	Join_download(Bitmap *bit, Font *font, Font *font2, const Palette& p, bool prejoin);
	virtual ~Join_download();
	virtual void step();
};

#endif
