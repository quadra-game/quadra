/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_GLOBAL
#define _HEADER_GLOBAL

#define MAXPLAYERS 8
#define MAXTEAMS 8

extern char st[];
extern char quadradir[];
extern char team_name[MAXTEAMS][40];
extern bool named_team[MAXTEAMS];
extern const char *english_teams[];
extern const char *french_teams[];

void set_team_name(Byte team, const char *name);

extern bool quitting;
extern const char built[];

void quit_fast();

#endif
