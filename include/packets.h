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

#ifndef _HEADER_PACKETS
#define _HEADER_PACKETS

#include "packet.h"
#include "array.h"
#include "net_stuff.h"
#include "game.h"
#include "config.h"
#include "track.h"

enum Packet_type {
	P_CHAT,
	P_FINDGAME,
	P_GAMESERVER,
	P_WANTJOIN,
	P_PLAYERWANTJOIN,
	P_CLIENTCHAT,
	P_PLAYERACCEPTED,
	P_PLAYER,
	P_CLIENTPAUSE,
	P_PAUSE,
	P_STAT,
	P_GAMEINFO,
	P_DROPPLAYER,
	P_CLIENTDROPPLAYER,
	P_SERVERDROPPLAYER,
	P_STAMPBLOCK,
	P_CLIENTSTAMPBLOCK,
	P_DEAD,
	P_CLIENTDEAD,
	P_RESPAWN,
	P_CLIENTRESPAWN,
	DEPRECATED_P_WATCH,
	P_STARTWATCH,
	P_CLIENTSTARTWATCH,
	P_DOWNLOAD,
	P_CLIENTLINES,
	P_LINES,
	P_CLIENTTESTPING,
	P_TESTPING,
	P_FIRST_FRAG,
	P_CLIENTFIRST_FRAG,
	P_GONE,
	P_CLIENTGONE,
	P_ENDGAME,
	P_REJOIN,
	P_MOVES,
	P_CLIENTMOVES,
	P_STATE,
	P_CLIENTSTATE,
	P_SERVERSTATE,
	P_SERVERRANDOM,
	P_SERVERPOTATO,
	DEPRECATED_P_SERVERLINES,
	P_SERVERTESTPING,
	P_BYE,
	P_REMOVEBONUS,
	P_CLIENTREMOVEBONUS,
	P_SERVERNAMETEAM,
	P_GAMESTAT
};

class Packet_findgame: public Packet_udp {
public:
	Packet_findgame() {
		packet_id = P_FINDGAME;
	}
};

class Packet_wantjoin: public Packet_ping {
public:
	Byte net_version;
	Byte language;
	Byte os;
	Packet_wantjoin() {
		packet_id = P_WANTJOIN;
		net_version=Config::net_version;
		language=config.info.language;
		os=
		#if defined(UGS_DIRECTX)
			1
		#elif defined(UGS_LINUX)
			2
		#else
			#error "What platform???"
		#endif
		;
	}
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
};

class Net_player {
public:
	Byte quel; //Not sent over the net in Packet_gameinfo,
	           //  sent only with Packet_gameserver
	Byte team;
	char name[40];
	Dword player_id;
	Byte idle; //This is not sent thru the net, it's only there if
	           //  someone set it from a Qserv query
	int handicap; //May not be there, but default of zero is ok in
	              //  those cases
	Net_player(Byte q, Byte t, const char *s, Dword pid, int status, int phandicap) {
		quel=q;
		team=t;
		strcpy(name, s);
		player_id=pid;
		if(status!=-1)
			idle=status;
		else
			idle=255;
		handicap=phandicap;
	}
};

class Packet_gameinfo: public Packet_udp {
public:
	Array <Net_player *> players;
	char name[32];
	Byte version;
	int port, game_end_value;
	bool nolevel_up, delay_start, terminated;
	bool survivor, hot_potato;
	Attack normal_attack, clean_attack, potato_normal_attack, potato_clean_attack;
	Byte level_start, combo_min, game_end;
	bool allow_handicap;
	Packet_gameinfo();
	virtual ~Packet_gameinfo();
	void add_player(Byte q, Byte t, const char *s, int status, int handicap) {
		players.add(new Net_player(q, t, s, 0, status, handicap));
	}
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
};

class Packet_gameserver: public Packet_ping {
public:
	Byte version;
	Array <Net_player *> players;
	char name[32];
	bool accepted;
	int game_seed, game_end_value;
	bool paused, nolevel_up;
	Byte level_start, combo_min;
	bool allow_handicap;
	bool survivor, hot_potato;
	Attack normal_attack, clean_attack, potato_normal_attack, potato_clean_attack;
	Byte game_end;
	Word delay_start;
	bool wants_moves;
	Byte syncpoint;
	Byte potato_team;
	bool single;
	bool terminated;
	Packet_gameserver() {
		packet_id = P_GAMESERVER;
		name[0] = 0;
	}
	virtual ~Packet_gameserver();
	void add_player(Byte q, Byte t, const char *s, Dword pid, int handicap) {
		players.add(new Net_player(q, t, s, pid, -1, handicap));
	}
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
	bool any_attack();
};

class Packet_chat: public Packet_tcp {
public:
	signed char team;
	signed char to_team;
	char text[256];
	Packet_chat() {
		packet_id = P_CHAT;
		text[0] = 0;
		team = -1;
	}
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
};

class Packet_clientchat: public Packet_chat {
public:
	Packet_clientchat() {
		packet_id = P_CLIENTCHAT;
	}
};

class Packet_playerwantjoin: public Packet_ping {
public:
	Byte team;
	char name[40];
	Byte player;
	int h_repeat, v_repeat, smooth, shadow, handicap;
	Byte player_hash[16];
	char team_name[40];
	Byte team_hash[16];
	Packet_playerwantjoin() {
		packet_id=P_PLAYERWANTJOIN;
		name[0]=0;
		team_name[0]=0;
	}
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
};

class Packet_player: public Packet_ping {
public:
	Byte team;
	char name[40];
	Dword player_id;
	Byte player;
	int h_repeat, v_repeat, smooth, shadow;
	Byte pos;
	int handicap;
	Packet_player() {
		packet_id = P_PLAYER;
		name[0]=0;
		player_id=0;
		pos=0;
	}
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
};

class Packet_playeraccepted: public Packet_ping {
public:
	Byte pos;
	Byte accepted;
	Packet_playeraccepted() {
		packet_id = P_PLAYERACCEPTED;
		pos=0;
		accepted = 0;
	}
	virtual void write(Net_buf *p);
	virtual bool read(Net_buf *p);
};

class Packet_clientpause: public Packet_tcp {
public:
	Packet_clientpause() {
		packet_id = P_CLIENTPAUSE;
	}
};

class Packet_pause: public Packet_tcp {
public:
	signed char player;
	Packet_pause() {
		packet_id = P_PAUSE;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Net_stat {
public:
	Byte st;
	int value;
	Net_stat(Byte s, int v) {
		st=s;
		value=v;
	}
};

class Packet_playerbase: public Packet_tcp {
	TRACKED;
public:
	Byte player;
	Packet_playerbase() {
		player = 255;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_stat: public Packet_playerbase {
public:
	Array<Net_stat *> net_stats;
	Byte num_stat;
	Packet_stat() {
		packet_id = P_STAT;
	}
	virtual ~Packet_stat();
	void add_stat(Byte s, int v);
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_gamestat: public Packet_tcp {
public:
	Array<Net_stat *> net_stats;
	Byte num_stat;
	Packet_gamestat() {
		packet_id = P_GAMESTAT;
	}
	virtual ~Packet_gamestat();
	void add_stat(Byte s, int v);
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_dropplayer: public Packet_playerbase {
public:
	Drop_reason reason;
	Packet_dropplayer() {
		packet_id = P_DROPPLAYER;
		reason=DROP_LAST;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientdropplayer: public Packet_dropplayer {
public:
	Packet_clientdropplayer() {
		packet_id = P_CLIENTDROPPLAYER;
	}
};

class Packet_serverdropplayer: public Packet_dropplayer {
public:
	Packet_serverdropplayer() {
		packet_id = P_SERVERDROPPLAYER;
	}
};

class Packet_stampblock: public Packet_playerbase {
public:
	Byte x, y;
	Byte rotate;
	Byte score;
	Word date;
	Byte block_rotated;
	Word time_held;
	Packet_stampblock() {
		packet_id = P_STAMPBLOCK;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientstampblock: public Packet_stampblock {
public:
	Packet_clientstampblock() {
		packet_id = P_CLIENTSTAMPBLOCK;
	}
};

class Packet_dead: public Packet_playerbase {
public:
	bool then_gone;
	Packet_dead() {
		packet_id = P_DEAD;
		then_gone=false;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientdead: public Packet_dead {
public:
	Packet_clientdead() {
		packet_id = P_CLIENTDEAD;
	}
};

class Packet_respawn: public Packet_playerbase {
public:
	Packet_respawn() {
		packet_id = P_RESPAWN;
	}
};

class Packet_clientrespawn: public Packet_respawn {
public:
	Packet_clientrespawn() {
		packet_id = P_CLIENTRESPAWN;
	}
};

class Packet_startwatch: public Packet_playerbase {
public:
	Byte update;
	Dword address;
	bool stop;
	Packet_startwatch() {
		packet_id = P_STARTWATCH;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientstartwatch: public Packet_startwatch {
public:
	Packet_clientstartwatch() {
		packet_id = P_CLIENTSTARTWATCH;
	}
};

class Packet_download: public Packet_playerbase {
public:
	int seed;
	Byte bloc, next, next2, next3, bonus, idle, state;
  struct {
    Byte x;   //position du 'trou'
    Byte color;
		Byte blind_time;
		Word hole_pos; //Hole positions
		bool final;
  } bon[20];  //les lignes chiantes en attente
	Byte can[32][10];
	bool occ[32][10];
	Byte blinded[32][10];
	Byte attacks[MAXPLAYERS];
	Byte last_attacker;

	Packet_download() {
		packet_id = P_DOWNLOAD;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_lines: public Packet_playerbase {
public:
	Byte nb;
	Byte nc;
	Byte lx;
	Byte sender;
	Attack attack;
	Word hole_pos[36];
	Packet_lines() {
		packet_id = P_LINES;
		for(int i=0; i<36; i++)
			hole_pos[i]=0;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientlines: public Packet_lines {
public:
	Packet_clientlines() {
		packet_id = P_CLIENTLINES;
	}
};

class Packet_testping: public Packet_tcp {
public:
	Dword frame;
	Packet_testping() {
		packet_id = P_TESTPING;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clienttestping: public Packet_testping {
public:
	Packet_clienttestping() {
		packet_id = P_CLIENTTESTPING;
	}
};

class Packet_servertestping: public Packet_testping {
public:
	Packet_servertestping() {
		packet_id = P_SERVERTESTPING;
	}
};

class Packet_first_frag: public Packet_playerbase {
public:
	Packet_first_frag() {
		packet_id = P_FIRST_FRAG;
	}
};

class Packet_clientfirst_frag: public Packet_first_frag {
public:
	Packet_clientfirst_frag() {
		packet_id = P_CLIENTFIRST_FRAG;
	}
};

class Packet_gone: public Packet_playerbase {
public:
	bool chat_msg; //Inverted on the net
	Packet_gone() {
		packet_id = P_GONE;
		chat_msg = true;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientgone: public Packet_gone {
public:
	Packet_clientgone() {
		packet_id = P_CLIENTGONE;
	}
};

class Packet_endgame: public Packet_tcp {
public:
	bool auto_end;
	Packet_endgame() {
		packet_id = P_ENDGAME;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_rejoin: public Packet_playerbase {
public:
	int h_repeat, v_repeat, smooth, shadow, handicap;
	Packet_rejoin() {
		packet_id = P_REJOIN;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_moves: public Packet_playerbase {
public:
	Byte size;
	Byte moves[256];
	Packet_moves() {
		packet_id = P_MOVES;
		size=0;
	}
	void start_byte();
	void set_bit(int v);
	void write_byte();
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientmoves: public Packet_moves {
public:
	Packet_clientmoves() {
		packet_id = P_CLIENTMOVES;
	}
};

class Packet_state: public Packet_playerbase {
public:
	Byte state;
	Packet_state() {
		packet_id = P_STATE;
		state=255;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_clientstate: public Packet_state {
public:
	Packet_clientstate() {
		packet_id = P_CLIENTSTATE;
	}
};

class Packet_serverstate: public Packet_state {
public:
	Packet_serverstate() {
		packet_id = P_SERVERSTATE;
		//We got a Byte player from Packet_state but we don't need it
		//  so we'll always set it to 0. Yeah it sucks, sue me.
		player = 0;
	}
};

//This should be sent in the dead space between two syncpoints
//  because there's no Packet_clientrandom
class Packet_serverrandom: public Packet_tcp {
public:
	Dword seed;
	Packet_serverrandom() {
		packet_id = P_SERVERRANDOM;
		seed=0;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_serverpotato: public Packet_tcp {
public:
	Byte team; //The team that will now get the potato
	Dword potato_lines; //Number of lines to clear
	Packet_serverpotato() {
		packet_id = P_SERVERPOTATO;
		team=255;
		potato_lines=0;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

class Packet_bye: public Packet_tcp {
public:
	Packet_bye() {
		packet_id = P_BYE;
	}
};

class Packet_removebonus: public Packet_playerbase {
public:
	Packet_removebonus() {
		packet_id = P_REMOVEBONUS;
	}
};

class Packet_clientremovebonus: public Packet_removebonus {
public:
	Packet_clientremovebonus() {
		packet_id = P_CLIENTREMOVEBONUS;
	}
};

class Packet_servernameteam: public Packet_tcp {
public:
	Byte team;
	char name[40];
	Packet_servernameteam() {
		packet_id = P_SERVERNAMETEAM;
		name[0]=0;
	}
	virtual bool read(Net_buf *p);
	virtual void write(Net_buf *p);
};

#endif
