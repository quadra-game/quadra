/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_CONFIG
#define _HEADER_CONFIG

#include "types.h"

class Config {
	int version;
public:
	const static int game_version;
	static int net_version;
	const static int major, minor, patchlevel;
	static bool registered;
	static bool xtreme;
	static char user_name[];
	int warning;
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
		int some_stuff[16];
		char yet_more_stuff[64];
	} info2;
	char fname[1024];
	Config();
	virtual ~Config();
	void default_config();
	void read();
	void write();
	void check_register();
};

extern Config config;

#endif
