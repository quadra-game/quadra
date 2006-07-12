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

#ifndef _HEADER_GLOBAL
#define _HEADER_GLOBAL

#include "types.h"

#define MAXPLAYERS 8
#define MAXTEAMS 8

extern char st[4096];
extern char quadradir[1024];
extern char team_name[MAXTEAMS][40];
extern bool named_team[MAXTEAMS];
extern const char *english_teams[MAXTEAMS];
extern const char *french_teams[MAXTEAMS];

void set_team_name(Byte team, const char *name);

extern bool quitting;
extern const char built[];

enum Drop_reason {
	DROP_AUTO,
	DROP_MANUAL,
	DROP_INVALID_BLOCK,
	DROP_LAST
};

void quit_fast();

#endif
