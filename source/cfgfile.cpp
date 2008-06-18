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

#include "cfgfile.h"

#include <string.h>
#include <stdio.h>
#include "input.h"
#include "res.h"
#include "global.h"
#include "crypt.h"
#include "video.h"
#include "version.h"
#include "unicode.h"
#include "quadra.h"

const int Config::game_version = 19;
int Config::net_version = 24;

const int Config::major = VERSION_MAJOR;
const int Config::minor = VERSION_MINOR;
const int Config::patchlevel = VERSION_PATCHLEVEL;

char Config::user_name[64] = {""};

Config::Config() {
	fname[0]=0;
}

Config::~Config() {
}

void Config::default_config() {
	memset(&version, 0, sizeof(version));
	memset(&info, 0, sizeof(info));
	memset(player, 0, sizeof(player));
	memset(player2, 0, sizeof(player2));
	memset(&info2, 0, sizeof(info2));
	memset(&info3, 0, sizeof(info3));
	info.xlanguage = 0;
	info.setup_player = 0;
	info.cdmusic = 0; // 0=no music  1=auto-change  2=loop all
	info.multi_level = 1;
	info.unlock_theme = 0;
	info.port_number = 3456;
	info.mouse_speed = 100;
	info.pane[0] = 2;
	info.pane[1] = 0;
	info.pane[2] = 3;
	info.update_rate = 10;
	memset(info.book, sizeof(info.book), 0);
	info.game_name[0] = 0;
	info.game_type = info.level_up = info.level_start = info.combo_min = info.game_end = 0;
	info.game_public = 1;
	info.game_end_value = 1;

	for(int i=0; i<3; i++) {
		sprintf(st,"#%i", i+1);
		strcpy(player[i].name, st);
		player[i].color = i;
		player[i].shadow = 0;
		player[i].smooth = 1;
		player[i].repeat = -1;
		player2[i].h_repeat = 2;
		player2[i].v_repeat = 2;
		player2[i].continuous = 1;
		player[i].key[0] = SDLK_LEFT;
		player[i].key[1] = SDLK_RIGHT;
		player[i].key[2] = SDLK_UP;
		player[i].key[3] = SDLK_DOWN;
		player[i].key[4] = SDLK_UP;
		player2[i].key[0] = SDLK_RSHIFT;
		player2[i].key[1] = SDLK_SPACE;
	}
}

void Config::read() {
	if(!fname[0])
		snprintf(fname, sizeof(fname) - 1, "%s/%s", quadradir, "quadra.cfg");

	int i;

	Res_dos res(fname, RES_TRY);
	memset(&version, 0, sizeof(version));
	memset(&info, 0, sizeof(info));
	memset(player, 0, sizeof(player));
	memset(player2, 0, sizeof(player2));
	memset(&info2, 0, sizeof(info2));
	memset(&info3, 0, sizeof(info3));
	warning = 0;
	if(!res.exist) {
		default_config();
		warning = 1;
	} else {
		if(res.size() != (sizeof(version) + sizeof(info) + sizeof(player)) &&
		   res.size() != (sizeof(version) + sizeof(info) + sizeof(player) + sizeof(player2) + sizeof(info2)) &&
       res.size() != (sizeof(version) + sizeof(info) + sizeof(player) + sizeof(player2) + sizeof(info2) + sizeof(info3))) {
			default_config();
			warning = 2;
		} else {
			version = -1;
			res.read(&version, sizeof(version));
			if(version != game_version) {
				default_config();
				warning = 2;
			} else {
				res.read(&info, sizeof(info));
				res.read(player, sizeof(player));
				//Those may not be present, but the default is all-zero anyway
				res.read(player2, sizeof(player2));
				res.read(&info2, sizeof(info2));
				res.read(&info3, sizeof(info3));
			}
		}
	}
	
	if(info3.latest_version[0] == 0)
	{
		strcpy(info3.latest_version, VERSION_STRING);
	}

	for(i=0; i<3; i++) {
		player[i].name[39] = 0;
		if(player[i].color<0 || player[i].color>=MAXTEAMS)
			player[i].color=i;
		if(player[i].shadow<0 || player[i].shadow>1)
			player[i].shadow=1;
		if(player[i].smooth<0 || player[i].smooth>1)
			player[i].smooth=1;
		if(player[i].repeat<-1 || player[i].repeat>3)
			player2[i].h_repeat=player2[i].v_repeat=2;
		else
			if(player[i].repeat>=0) {
				player2[i].h_repeat=player[i].repeat;
				player2[i].v_repeat=player[i].repeat;
			}
		if(player2[i].h_repeat<0 || player2[i].h_repeat>3) {
			player2[i].h_repeat=2;
		}
		if(player2[i].v_repeat<0 || player2[i].v_repeat>3) {
			player2[i].v_repeat=2;
		}
		player[i].repeat = -1;
		if(player2[i].continuous<0 || player2[i].continuous>1)
			player2[i].continuous=1;
		if(player2[i].handicap<0 || player2[i].handicap>4)
			player2[i].handicap=0;
		player2[i].ngPasswd[63]=0;
		player2[i].ngTeam[39]=0;
		player2[i].ngTeamPasswd[63]=0;
	}
	for(i=0; i<10; i++) {
		info.book[i][255] = 0;
	}
	info2.proxy_address[127] = 0;
	info3.last_modified[63] = 0;
	info3.default_game_server_address[255] = 0;
	info3.latest_version[255] = 0;
}

void fix_str(char *st, Dword len) {
	bool in_str(true);

	for(Dword i = 0; i < len; ++i)
		if(!in_str)
			st[i] = 0;
		else
			if(!st[i])
				in_str = false;
	st[len - 1] = 0;
}

void Config::write() {
	int i;
	if(!fname[0])
		snprintf(fname, sizeof(fname) - 1, "%s/%s", quadradir, "quadra.cfg");

	if(!video_is_dumb) {
		Res_dos res(fname, RES_CREATE);
		if(res.exist) {
			version = game_version;
			res.write(&version, sizeof(version));
			for(i=0; i<10; i++)
				fix_str(info.book[i], 256);
			fix_str(info.game_name, 32);
			fix_str(info.game_server_address, 256);
			res.write(&info, sizeof(info));
			for(i=0; i<3; i++) {
				fix_str(player[i].name, 40);
				fix_str(player2[i].ngPasswd, 64);
				fix_str(player2[i].ngTeam, 40);
				fix_str(player2[i].ngTeamPasswd, 64);
			}
			res.write(player, sizeof(player));
			res.write(player2, sizeof(player2));
			fix_str(info2.proxy_address, 128);
			res.write(&info2, sizeof(info2));
			fix_str(info3.last_modified, 64);
			fix_str(info3.default_game_server_address, 256);
			fix_str(info3.latest_version, 256);
			res.write(&info3, sizeof(info3));
		}
	}
}

void Config::get_player_hash(Byte* buf, unsigned qplayer) {
	if(player2[qplayer].ngPasswd[0]) {
		Unicode uni_p(player[qplayer].name);
		uni_p.cat(player2[qplayer].ngPasswd);
		Crypt name_crypt;
		name_crypt.step(uni_p, uni_p.size());
		name_crypt.finalize(false);
		memcpy(buf, name_crypt.get_digest(), 16);
	}
	else
		memset(buf, 0, 16);
}

void Config::get_team_hash(Byte* buf, unsigned qplayer) {
	if(player2[qplayer].ngTeam[0] && player2[qplayer].ngTeamPasswd[0]) {
		Unicode uni_t(player2[qplayer].ngTeam);
		uni_t.cat(player2[qplayer].ngTeamPasswd);
		Crypt team_crypt;
		team_crypt.step(uni_t, uni_t.size());
		memcpy(buf, team_crypt.get_digest(), 16);
	}
	else
		memset(buf, 0, 16);
}

Config config;
