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

#ifndef _HEADER_SCORE
#define _HEADER_SCORE

#include <stdint.h>

#include "global.h"
#include "stats.h"

class Dict;

class Score {
	CS::Stat_type current_sort;
	void update_team();
public:
	uint8_t player_team[MAXPLAYERS];
	uint8_t player_count[MAXTEAMS];
	uint8_t team_order[MAXTEAMS], order[MAXPLAYERS];
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
