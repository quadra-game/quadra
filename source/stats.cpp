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

#include "stats.h"

RCSID("$Id$")

void CS::add(const CS& other) {
	for(int i=0; i<LAST; i++)
		stats[i].add(other.stats[i].get_value());
}

void CS::clear() {
	for(int i=0; i<LAST; i++)
		stats[i].set_value(0);
}

void GS::clear() {
	for(int i=0; i<LAST; i++)
		stats[i].set_value(0);
}

bool CS::better(const CS& o, Stat_type type) {
	if(type<LAST) {
		if(stats[type]>o.stats[type])
			return type!=DEATH? true:false;
		if(stats[type]<o.stats[type])
			return type!=DEATH? false:true;
	}
	if(stats[FRAG]>o.stats[FRAG])
		return true;
	if(stats[FRAG]<o.stats[FRAG])
		return false;
	if(stats[DEATH]<o.stats[DEATH])
		return true;
	if(stats[DEATH]>o.stats[DEATH])
		return false;
	if(stats[SCORE]>o.stats[SCORE])
		return true;
	if(stats[SCORE]<o.stats[SCORE])
		return false;
	if(stats[LINESTOT]>o.stats[LINESTOT])
		return true;
	if(stats[LINESTOT]<o.stats[LINESTOT])
		return false;
	return false;
}
