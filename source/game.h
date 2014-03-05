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

#ifndef _HEADER_GAME
#define _HEADER_GAME

#include <stdint.h>
#include <vector>

#include "attack.h"
#include "global.h"
#include "net_list.h"
#include "stats.h"

class Game_params;
class Net_client;
class Net_server;
class Qserv;

enum End_type {
	END_NEVER,
	END_FRAG,
	END_TIME,
	END_POINTS,
	END_LINES,
	END_LAST
};

//Gotchas:
//Constructing a Game sets ::game to this
//Destroying a Game sets ::game to NULL
class Game: public GS {
	friend class Net_list;
	std::vector<Packet*> stack;
	Qserv *gameinfo;
	bool http_failed;
	void buildgameinfo(const char* cmd);
	char record_filename[1024];
	char slog_filename[1024];
	bool is_recording, is_slogging;
	uint32_t last_given_potato;
	uint8_t the_net_version;
public:
	int net_version(); //Required net_version to play this game
	bool auto_restart;
	void stop_stuff();
	void restart();
	void count_playing_time();
	void addgameinfo(Textbuf *tb);
	bool verifygameinfo(const Dict *sum) const;
	uint32_t frame_start;
	bool wants_moves;
	Net_server *net_server;
	Net_client *net_client;
	bool single;
	bool network;
	bool survivor;
	bool hot_potato;
	// Potato stuff (server only)
	int potato_lines[MAXTEAMS];
	uint8_t potato_order[MAXTEAMS];
	uint8_t potato_team;
	uint8_t previous_potato_team;
	void reset_potato();
	void new_potato_order();
	void got_potato(uint8_t team, int lines);
	void done_potato(uint8_t team);
	void check_potato();
	uint8_t next_potato_team();
	int server_accept_player, server_accept_connection;
	int server_max_players, server_max_teams;
	int server_min_players, server_min_teams;
	Net_connection *loopback_connection;
	Net_list net_list;
	bool valid_frag; //Survivor only
	bool server, abort, level_up, terminated, game_public;
	uint8_t level_start, combo_min;
	End_type game_end;
	bool allow_handicap;
	char name[32];
	int seed, game_end_value;
	Attack normal_attack, potato_normal_attack;
	Attack clean_attack, potato_clean_attack;
	bool boring_rules;
	bool any_attack();
	bool paused;
	uint16_t delay_start;
	explicit Game(const Packet_gameserver *p);
	explicit Game(const Game_params* p);
	virtual ~Game();
	void clientpause();
	void stackpacket(Packet *p);
	Packet *peekpacket(uint8_t type);
	void removepacket();
	void sendgameinfo(bool quit);
	void stepgameinfo();
	bool gameinfo_completed() const;
	void endgame();
	void prepare_recording(const char *fn);
	void prepare_logging();
	int get_multi_level();
	void set_seed(Packet_serverrandom *p);
	char *get_motd();
};

enum Game_preset {
	PRESET_FFA,
	PRESET_SURVIVOR,
	PRESET_PEACE,
	PRESET_BLIND,
	PRESET_FULLBLIND,
	PRESET_HOT_POTATO,
	PRESET_SINGLE,
	PRESET_SINGLE_SPRINT,
	PRESET_LAST
};

class Game_params {
public:
	std::string name;
	bool single;
	bool survivor;
	bool hot_potato;
	Attack normal_attack, potato_normal_attack;
	Attack clean_attack, potato_clean_attack;
	bool level_up;
	int level_start;
	bool allow_handicap;
	End_type game_end;
	int game_end_value;
	bool game_public;
	bool network;
	bool boring_rules;
	void set_preset(Game_preset preset);
	Game_params();
};

extern Game *game;

#endif
