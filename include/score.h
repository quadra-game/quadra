/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_SCORE
#define _HEADER_SCORE

#include "types.h"
#include "stats.h"
#include "net_stuff.h"

class Dict;

class Score {
	CS::Stat_type current_sort;
	void update_team();
public:
	Byte player_team[MAXPLAYERS];
	Byte player_count[MAXTEAMS];
	Byte team_order[MAXTEAMS], order[MAXPLAYERS];
	CS team_stats[MAXTEAMS], stats[MAXPLAYERS];
	bool team_order_changed, order_changed;
	bool team_goals_changed[MAXTEAMS];
	Score();
	void reset_order();
	void updateFromGame();
	void updateFromDict(Dict *d);
	void sort(CS::Stat_type type);
};

#endif
