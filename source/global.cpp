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

#include "types.h"
#include "sound.h"
#include "config.h"
#include "nglog.h"
#include "global.h"

RCSID("$Id$")

char st[4096] = {""};

char quadradir[1024] = {"."};

const char *english_teams[MAXTEAMS] = {
	"Orange",
	"Cyan",
	"Red",
	"Purple",
	"Yellow",
	"Green",
	"Blue",
	"Gray"
};

const char *french_teams[MAXTEAMS] = {
	"Orange",
	"Turquoise",
	"Rouge",
	"Pourpre",
	"Jaune",
	"Vert",
	"Bleu",
	"Gris"
};

char team_name[MAXTEAMS][40] = {
	{"Orange"},
	{"Cyan"},
	{"Red"},
	{"Purple"},
	{"Yellow"},
	{"Green"},
	{"Blue"},
	{"Gray"}
};

bool named_team[MAXTEAMS] = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false
};

void set_team_name(Byte team, const char *name) {
	if(!name || !name[0]) {
		if(config.info.language==1)
			strcpy(team_name[team], french_teams[team]);
		else
			strcpy(team_name[team], english_teams[team]);
		named_team[team]=false;
	}
	else {
		strcpy(team_name[team], name);
		named_team[team]=true;
	}
	const char *da_name;
	if(named_team[team])
		da_name=team_name[team];
	else
		da_name=english_teams[team];
	log_step("team_name\t%s\t%s", log_team(team), da_name);
}

bool quitting=false;

void quit_fast() {
	quitting=true;
	if(sound)
		sound->active=false;
}

const char built[]=
"Built on " \
__DATE__ \
" " \
__TIME__;
