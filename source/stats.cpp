/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "stats.h"

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
