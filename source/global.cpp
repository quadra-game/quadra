/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "types.h"
#include "sound.h"
#include "config.h"
#include "nglog.h"
#include "global.h"

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
