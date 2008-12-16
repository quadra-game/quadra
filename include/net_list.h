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

#ifndef _HEADER_NET_LIST
#define _HEADER_NET_LIST

#include <string.h>
#include <vector>

#include "notify.h"
#include "global.h"
#include "stats.h"
#include "score.h"
#include "attack.h"
#include "overmind.h"

class Canvas;
class IP_addr;
class Net_connection;

class Lastline {
public:
	Net_connection *nc;
	char cmd[256], params[256];
	void set(const char *pcmd, const char *pparams) {
		strncpy(cmd, pcmd, 255);
		cmd[255] = 0;
		strncpy(params, pparams, 255);
		params[255] = 0;
	}
	Lastline(Net_connection *pnc, const char *pcmd, const char *pparams) {
		nc = pnc;
		set(pcmd, pparams);
	}
};

class Net_list_stepper;
class Packet_dropplayer;
class Packet_lines;
class Packet_serverrandom;

class Net_list: public Observable {
	friend class Net_list_stepper;
	friend class Game;
	void step_all();
	Net_list_stepper *stepper; //Overmind thread that will call step_all
	Canvas *list[MAXPLAYERS];
	Dword last_use[MAXPLAYERS];
	bool end_signaled, demo_completed;
	bool winner_signaled;
	void check_drop();
	void check_gone();
	void check_potato();
	bool check_first_frag(); //Returns true if it's a good time to end the game
	void check_pause();
	void check_end_game(bool end_it);
	void check_player();
	void check_admin();
	void check_stat();
	std::vector<Lastline*> cmd_cache;
	std::vector<IP_addr*> deny_list;
	std::vector<IP_addr*> allow_list;
	Dword lastgameinfo;
	int *objectives;
	bool reached[10][MAXTEAMS]; //Max 10 objectives
	//Return which goal was last attained (if any) since last call,
	//  given the team number and goals remaining
	int check_goals(Byte team, int remain);
	Dword gone_time_limit;
	Dword ppm_limit;
	Dword lag_limit;
	char admin_password[64];
	char motd[256];
	Byte idle_on_last_notify[MAXPLAYERS];
public:
	virtual void notify_all();
	CS::Stat_type goal_stat;
	Score score;
	void reset_objectives(); //Game calls this upon construction
	void team2name(Byte team, char *st);
	void update_team_names();
	void send_end_signal(bool auto_end);
	void restart();
	void got_admin_line(const char *line, Net_connection *nc);
	bool accept_connection(Net_connection *nc);
	void client_deconnect(Net_connection *nc);
	Byte syncpoint;
	unsigned count_teams(bool include_gone=true) const;
	unsigned count_alive() const;
	unsigned size(bool include_gone=true) const;
	int add_player(Canvas *c);
	void set_player(Canvas *c, int pos, bool msg);
	//Should be called on server only: initiate drop player procedure
	void server_drop_player(Byte player, Drop_reason reason);
	//Should be called on client or server when receiving
	//  Packet_dropplayer, to finish drop player procedure
	void drop_player(Packet_dropplayer *p, bool chat);
	void remove_player(Canvas *c);
	void rejoin_player(Canvas *c);
	int canvas2player(Canvas *c);
	void sendlines(Packet_lines *p);
	void send(Canvas *c, Byte nb, Byte nc, Byte lx, Attack attack, bool clean);
	void pause_all();
	void unpause_all();
	Canvas *get(int i) const {
		return list[i];
	}
	Dword gettimer() const;
	bool competitive() const;
	bool would_be_competitive() const;
	bool all_dead_or_gone() const;
	bool all_gone() const;
	void syncto(Byte syncpoint);
	Net_list();
	virtual ~Net_list();
};

class Net_list_stepper: public Module_thread {
	Net_list *the_net_list;
public:
	Net_list_stepper(Net_list *nl);
	virtual void step();
};

#endif
