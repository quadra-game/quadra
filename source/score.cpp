/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "dict.h"
#include "game.h"
#include "canvas.h"
#include "score.h"

Score::Score() {
	int i;
	for(i=0; i<MAXPLAYERS; i++)
		player_team[i] = 255;
	for(i=0; i<MAXTEAMS; i++) {
		player_count[i] = 0;
		team_goals_changed[i] = false;
	}
	reset_order();
	team_order_changed=false;
	order_changed=false;
	current_sort=CS::LAST;
	if(game)
		sort(game->net_list.goal_stat);
}

void Score::updateFromGame() {
	if(!game)
		return;
	Byte team;
	int i;
	for(team=0; team<MAXTEAMS; team++) {
		int count=0;
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=game->net_list.get(i);
			if(c && c->color==team)
				count++;
		}
		team_goals_changed[team]=!(count==player_count[team]);
	}
	if(game) {
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=game->net_list.get(i);
			if(c) {
				player_team[i]=c->color;
				if(current_sort<CS::LAST)
					if(stats[i].stats[current_sort]!=c->stats[current_sort])
						team_goals_changed[c->color] = true;
				stats[i]=*c;
			}
			else {
				player_team[i]=255;
				stats[i].clear();
			}
		}
		update_team();
	}
	sort(current_sort);
}

void Score::updateFromDict(Dict *d) {
	int i, tmp;
	for(i=0; i<MAXTEAMS; i++)
		team_goals_changed[i]=true;
	const char *temp;
	for(i = 0; i < (int)d->size(); i++) {
		const Dict *d2 = d->get_sub(i);
		temp = d2->find("frags");
		if(temp)
			tmp=atoi(temp);
		else
			tmp=0;
		stats[i].stats[CS::FRAG].set_value(tmp);
		temp = d2->find("deaths");
		if(temp)
			tmp=atoi(temp);
		else
			tmp=0;
		stats[i].stats[CS::DEATH].set_value(tmp);
		temp = d2->find("lines");
		if(temp)
			tmp=atoi(temp);
		else
			tmp=0;
		stats[i].stats[CS::LINESTOT].set_value(tmp);
		temp = d2->find("score");
		if(temp)
			tmp=atoi(temp);
		else
			tmp=0;
		stats[i].stats[CS::SCORE].set_value(tmp);
		temp = d2->find("team");
		if(temp)
			tmp=atoi(temp);
		else
			tmp=255;
		player_team[i] = tmp;
	}
	update_team();
	sort(current_sort);
}

void Score::update_team() {
	Byte team;
	for(team=0; team<MAXTEAMS; team++) {
		player_count[team] = 0;
		team_stats[team].clear();
		for(int i=0; i<MAXPLAYERS; i++) {
			if(player_team[i]==team) {
				player_count[team]++;
				team_stats[team].add(stats[i]);
			}
		}
	}
}

void Score::sort(CS::Stat_type type) {
	team_order_changed=false;
	order_changed=false;
	int i;
	Byte team;
	Byte new_team_order[MAXTEAMS];
	for(team=0; team<MAXTEAMS; team++)
		new_team_order[team] = team_order[team];
	Byte new_order[MAXPLAYERS];
	for(i=0; i<MAXPLAYERS; i++)
		new_order[i] = order[i];
	bool done=false;
	while(!done) {
		done=true;
		for(i=0; i<MAXPLAYERS-1; i++) {
			bool xchg=false;
			if(player_team[new_order[i+1]]!=255) {
				if(player_team[new_order[i]]==255) {
					xchg=true;
				}
				else {
					if(stats[new_order[i+1]].better(stats[new_order[i]], type)) {
						xchg=true;
					}
				}
			}
			if(xchg) {
				Byte tmp=new_order[i];
				new_order[i]=new_order[i+1];
				new_order[i+1]=tmp;
				done=false;
				break;
			}
		}
	}
	done=false;
	while(!done) {
		done=true;
		for(team=0; team<MAXTEAMS-1; team++) {
			bool xchg=false;
			if(player_count[new_team_order[team+1]]) {
				if(!player_count[new_team_order[team]]) {
					xchg=true;
				}
				else {
					if(team_stats[new_team_order[team+1]].better(team_stats[new_team_order[team]], type)) {
						xchg=true;
					}
				}
			}
			if(xchg) {
				Byte tmp=new_team_order[team];
				new_team_order[team]=new_team_order[team+1];
				new_team_order[team+1]=tmp;
				done=false;
				break;
			}
		}
	}
	for(team=0; team<MAXTEAMS; team++) {
		if(new_team_order[team]!=team_order[team])
			team_order_changed=true;
		team_order[team]=new_team_order[team];
	}
	for(i=0; i<MAXPLAYERS; i++) {
		if(new_order[i]!=order[i])
			order_changed=true;
		order[i]=new_order[i];
	}
	current_sort=type;
}

void Score::reset_order() {
	int i;
	for(i=0; i<MAXPLAYERS; i++)
		order[i] = i;
	for(i=0; i<MAXTEAMS; i++)
		team_order[i] = i;
}
