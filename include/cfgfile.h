/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * 
 * Quadra, an action puzzle game
 * Copyright (C) 1998-2000  Ludus Design
 * Copyright (C) 2006 Pierre Phaneuf <pphaneuf@users.sourceforge.net>
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

#ifndef _HEADER_CFGFILE
#define _HEADER_CFGFILE

#include <time.h>
#include "types.h"

class Config {
	int version;
public:
	const static int game_version;
	static int net_version;
	const static int major, minor, patchlevel;
	static bool xtreme;
	static char user_name[];
	int warning;
	/*
   * WARNING: Everything in the following structs is pretty much set
   *          in stone. A full understanding of cfgfile.cpp is needed
   *          to change anything.
   */
	struct {
		int language;
		int setup_player, cdmusic;
		Byte multi_level, unlock_theme;
		int port_number, mouse_speed;
		Byte pane[3];
		Byte update_rate;
		char book[10][256];
		char game_name[32];
		char game_server_address[256];
		int game_type, level_up, level_start, combo_min, game_end, game_end_value, game_public;
	} info;
	struct {
		char name[40];
		int color, shadow, smooth, repeat;
		int key[5];
	} player[3];
	//The 'stuff' things are for future expansion
	struct {
		int handicap;
		char ngPasswd[64];
		char ngTeam[40];
		char filler1[24];
		char ngTeamPasswd[64];
		int key[2];
		int h_repeat, v_repeat;
		int continuous;
		int whole_bunch_of_stuff[11];
	} player2[3];
	struct {
		char proxy_address[128];
	} info2;
  struct {
    time_t last_update;
    char last_modified[64];
    char default_game_server_address[256];
    char latest_version[256];
  } info3;
	char fname[1024];
	Config();
	virtual ~Config();
	void default_config();
	void read();
	void write();

	//Get the hash value for a player
	void get_player_hash(Byte* buf, unsigned qplayer);
	//Get the hash value for a player's team
	void get_team_hash(Byte* buf, unsigned qplayer);
};

extern Config config;

#endif
