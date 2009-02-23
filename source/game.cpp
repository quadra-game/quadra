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

#include <string.h>
#include "input.h"
#include "net.h"
#include "qserv.h"
#include "overmind.h"
#include "random.h"
#include "packets.h"
#include "net_stuff.h"
#include "net_server.h"
#include "cfgfile.h"
#include "canvas.h"
#include "chat_text.h"
#include "texte.h"
#include "recording.h"
#include "dict.h"
#include "global.h"
#include "sons.h"
#include "nglog.h"
#include "clock.h"
#include "http_request.h"
#include "game.h"
#include "config.h"

RCSID("$Id$")

Game *game=NULL;

Game_params::Game_params() {
	name="[No name]";
	single=false;
	level_up=false;
	level_start=1;
	allow_handicap=true;
	game_end=END_NEVER;
	game_end_value=0;
	game_public=0;
	network=true;
	survivor=false;
	hot_potato=false;
	boring_rules = false;
	set_preset(PRESET_FFA);
}

void Game_params::set_preset(Game_preset preset) {
	switch(preset) {
		case PRESET_FFA:
			survivor=false;
			break;
		case PRESET_SURVIVOR:
			survivor=true;
			break;
		case PRESET_PEACE:
			normal_attack.type=ATTACK_NONE;
			clean_attack.type=ATTACK_NONE;
			break;
		case PRESET_BLIND:
			normal_attack.type=ATTACK_BLIND;
			normal_attack.param=30;
			clean_attack.type=ATTACK_FULLBLIND;
			clean_attack.param=12;
			break;
		case PRESET_HOT_POTATO:
			hot_potato=true;
			potato_normal_attack.type=ATTACK_NONE;
			potato_clean_attack.type=ATTACK_NONE;
			break;
		case PRESET_FULLBLIND:
			normal_attack.type=ATTACK_FULLBLIND;
			normal_attack.param=12;
			clean_attack.type=ATTACK_FULLBLIND;
			clean_attack.param=12;
			break;
		case PRESET_SINGLE:
			single=true;
			network=false;
			level_up=true;
			break;
		case PRESET_SINGLE_SPRINT:
			single=true;
			network=false;
			game_end=END_TIME;
			game_end_value=5;
			level_up=true;
			break;
		default:
			break;
	}
}

Game::Game(Packet_gameserver *p) {
	//Ok, we all know this sucks, but there supposedly are references
	//  to ::game in some of the things we call here and I don't
	//  feel like tracking them down
	game=this;
	the_net_version=p->version;
	gameinfo=NULL;
	http_failed=false;
	server=false;
	abort=false;
	terminated=p->terminated;
	net_server=NULL;
	net_client = new Net_client();
	loopback_connection = NULL;
	strcpy(name, p->name);
	seed=p->game_seed;
	paused=p->paused;
	survivor = p->survivor;
	hot_potato = p->hot_potato;
	normal_attack = p->normal_attack;
	clean_attack = p->clean_attack;
	if(hot_potato) {
		potato_normal_attack = p->potato_normal_attack;
		potato_clean_attack = p->potato_clean_attack;
	}
	boring_rules = p->boring_rules;
	level_up = !p->nolevel_up;
	level_start = p->level_start;
	combo_min = p->combo_min;
	delay_start = p->delay_start;
	game_end = (End_type) p->game_end;
	game_end_value = p->game_end_value;
	game_public = false; // inutile pour les clients (info pour serveur seulement)
	for(int i=0; i<p->players.size(); i++) {
		Canvas *canvas=new Canvas(seed, p->players[i]->team, p->players[i]->name, 2, 2, true, true, p->players[i]->handicap, net->server_addr(), 0, true); //On connait pas repeat, smooth, shadow mais on s'en tappe parce qu'on est pas en playback
		canvas->set_id(p->players[i]->player_id);
		net_list.set_player(canvas, p->players[i]->quel, false);
	}
	wants_moves=p->wants_moves;
	net_list.syncpoint=p->syncpoint;
	reset_potato();
	potato_team=p->potato_team;
	single=p->single;
	network=true;
	frame_start = overmind.framecount;
	valid_frag=false;
	auto_restart=false;
	record_filename[0] = 0;
	slog_filename[0] = 0;
	is_recording=is_slogging=false;
	allow_handicap=p->allow_handicap;
	net_list.reset_objectives();
}

Game::Game(Game_params* p) {
	//Ok, we all know this sucks, but there supposedly are references
	//  to ::game in some of the things we call here and I don't
	//  feel like tracking them down
	game=this;
	the_net_version = Config::net_version;
	if(the_net_version==20) {
		if(p->hot_potato)
			the_net_version = 22;
		if(p->normal_attack.type!=ATTACK_LINES)
			the_net_version = 22;
		if(p->clean_attack.type!=ATTACK_LINES)
			the_net_version = 22;
		if(p->game_end>=END_POINTS)
			the_net_version = 22;
	}
	gameinfo=NULL;
	http_failed=false;
	server = true;
	abort=false;
	terminated=false;
	server_accept_player = server_accept_connection = 0;
	server_max_players = server_min_players = 0;
	server_max_teams = server_min_teams = 0;
	loopback_connection = NULL;
	single=p->single;
	network=p->network;
	net_server = new Net_server();
	net_client = new Net_client();
	strncpy(name, p->name, sizeof(name));
	name[sizeof(name)-1]=0;
	seed = (int)ugs_random.get_seed();
	paused = false;
	delay_start = 0;
	survivor = p->survivor;
	hot_potato = p->hot_potato;
	normal_attack = p->normal_attack;
	clean_attack = p->clean_attack;
	if(hot_potato) {
		potato_normal_attack = p->potato_normal_attack;
		potato_clean_attack = p->potato_clean_attack;
	}
	boring_rules = p->boring_rules;
	level_up = p->level_up;
	level_start = p->level_start;
	combo_min = 2;
	game_end = p->game_end;
	game_end_value = p->game_end_value;
	if(game_end==END_FRAG && !any_attack()) {
		game_end=END_NEVER;
		game_end_value=0;
	}
	if(game_end == END_TIME)
		game_end_value = game_end_value * 6000; //minutes->centiseconds
	if(game_end == END_POINTS)
		game_end_value = game_end_value * 1000; //Kpoints->points
	game_public = p->game_public;
	if(!single) {
		paused = true; // starts the game on pause
		delay_start=500;
	}
	frame_start = overmind.framecount;
	valid_frag=false;
	wants_moves=true;
	reset_potato();
	auto_restart=false;
	record_filename[0] = 0;
	slog_filename[0] = 0;
	is_recording=is_slogging=false;
	allow_handicap=p->allow_handicap;
	net_list.reset_objectives();
}

Game::~Game() {
	stop_stuff();
	if(gameinfo)
		delete gameinfo;
	if(net_client)
		delete net_client;
	if(net_server)
		delete net_server;
	if(stack.size())
		msgbox("Game::~Game: stack.size should be 0\n");
	stack.deleteall();
	//Oh well...
	game = NULL;
}

void Game::stop_stuff() {
	//Can be called repetitively for the same game
	//Stop recording
	if(net_server && recording) {
		//Write game summary
		recording->end_multi();
		net_server->stop_multi_recording();
	}
}

void Game::restart() {
	if(!server)
		return;
	if(!terminated)
		return;
	if(quitting)
		return;
	//Wait until everybody is gone
	if(!net_list.all_gone())
		return;

	msgbox("Game::restart: restarting game now.\n");
	//Make all currently joined connections not joined
	int i;
	if(net->active) {
		for(i=0; i<net->connections.size(); i++) {
			Net_connection *nc = net->connections[i];
			if(nc->joined && nc!=loopback_connection)
				nc->joined=false;
		}
	}
	//Drop all players
	for(i=0; i<MAXPLAYERS; i++) {
		if(net_list.get(i)) {
			Packet_dropplayer p;
			p.player=i;
			p.reason=DROP_AUTO;
			net_list.drop_player(&p, false);
		}
	}
	//Fix everything up
	abort=false;
	if(!single)
		delay_start = 500;
	frame_start = overmind.framecount;
	paused = !single;
	//Reset all game stats to zero
	GS *current=this;
	GS zeroes;
	*current=zeroes;
	reset_potato();
	Random r; seed=(int)r.get_seed();
	server_accept_connection=0;
	server_accept_player=0;
	terminated = false;
	valid_frag = false;
	net_list.restart();
	//Restart recording
	if(is_recording) {
		prepare_recording(NULL);
		prepare_logging();
	}
	//Add connect events to the log(s) for all active connections
	for(i=0; i<net->connections.size(); i++) {
		Net_connection *nc=net->connections[i];
		if(nc && nc!=loopback_connection) {
			char st[64];
			Net::stringaddress(st, nc->address(), nc->getdestport());
			Packet_serverlog log("connect");
			log.add(Packet_serverlog::Var("id", nc->id()));
			log.add(Packet_serverlog::Var("address", st));
			if(game->net_server)
				game->net_server->record_packet(&log);
		}
	}
}

void Game::reset_potato() {
	int i;
	potato_team = previous_potato_team = 255;
	for(i=0; i<MAXTEAMS; i++) {
		potato_lines[i] = 2;
		potato_order[i]=255;
	}
	last_given_potato=0;
}

void Game::new_potato_order() {
	int team;
	bool order_taken[MAXTEAMS];
	for(team=0; team<MAXTEAMS; team++)
		order_taken[team]=false;
	for(team=0; team<MAXTEAMS; team++) {
		Byte rnd=ugs_random.rnd()%MAXTEAMS;
		while(order_taken[rnd]) {
			rnd++;
			if(rnd>=MAXTEAMS)
				rnd=0;
		}
		order_taken[rnd]=true;
		potato_order[team]=rnd;
	}
}

Byte Game::next_potato_team() {
	Byte ret=255;
	int i, j;
	for(i=0; i<MAXTEAMS; i++)
		if(potato_order[i]!=255 && potato_order[i]!=previous_potato_team) {
			for(j=0; j<MAXPLAYERS; j++) {
				Canvas *c=net_list.get(j);
				if(c && c->idle<2 && c->color==potato_order[i]) {
					ret=potato_order[i];
					potato_order[i]=255;
					//Break out of both loops
					i=MAXTEAMS;
					j=MAXPLAYERS;
					break;
				}
			}
		}
	if(ret==255) {
		new_potato_order();
		for(i=0; i<MAXTEAMS; i++)
			if(potato_order[i]!=255 && potato_order[i]!=previous_potato_team) {
				for(j=0; j<MAXPLAYERS; j++) {
					Canvas *c=net_list.get(j);
					if(c && c->idle<2 && c->color==potato_order[i]) {
						ret=potato_order[i];
						potato_order[i]=255;
						//Break out of both loops
						i=MAXTEAMS;
						j=MAXPLAYERS;
						break;
					}
				}
			}
	}
	if(ret!=255)
		previous_potato_team=ret;
	return ret;
}

void Game::got_potato(Byte team, int lines) {
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=net_list.get(i);
		if(c && c->color==team) {
			if(c->islocal()) {
				c->add_text_scroller(ST_YOUGOTPOTATO1, 4, -20);
				c->add_text_scroller(ST_YOUGOTPOTATO2, 4);
				Sfx stmp(sons.potato_get, 0, -1200, 0, 44100);
			}
			c->potato_lines=0;
			c->team_potato_lines=0;
			c->team_potato_linestot=lines;
			if(c->inter && !c->small_watch) {
				c->z_lines->disable();
				c->z_potatolines->enable();
				c->z_linestot->disable();
				c->z_potatolinestot->enable();
			}
		}
	}
	char st[1024];
	if(chat_text) {
		net_list.team2name(team, st);
		strcat(st, ST_GETSPOTATO);
		message(-1, st);
		sprintf(st, ST_CLEARBOBLINES, lines);
		message(-1, st);
	}
}

void Game::done_potato(Byte team) {
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=net_list.get(i);
		if(c && c->color==team) {
			if(c->islocal() && !(c->dying || c->idle>=2)) {
				c->add_text_scroller(ST_YOUGOTRIDOFPOTATO1, 4, -40);
				c->add_text_scroller(ST_YOUGOTRIDOFPOTATO2, 4, -20);
				Sfx stmp(sons.potato_rid, 0, -300, 0, 11025);
			}
			int x, y;
			for(y = 0; y < 36; y++)
				for(x = 0; x < 18; x++) {
					if(c->occupied[y][x]) {
						if(c->blinded[y][x]) {
							c->dirted[y][x] = 2;
							c->bflash[y][x] = 24;
						}
						c->blinded[y][x] = 0;
					}
				}
			if(c->islocal())
				c->should_remove_bonus=true;
			if(c->inter && !c->small_watch) {
				c->z_lines->enable();
				c->z_potatolines->disable();
				c->z_linestot->enable();
				c->z_potatolinestot->disable();
			}
		}
	}
/*	char st[1024];
	if(chat_text) {
		net_list.team2name(team, st);
		strcat(st, ST_DONEPOTATO);
		message(-1, st);
	}*/
}

void Game::check_potato() {
	int i;
	Canvas *c;
	if(!game->hot_potato)
		return;
	//Clients only have to obey the server's Packet_serverpotato
	Packet_serverpotato *p=(Packet_serverpotato *) peekpacket(P_SERVERPOTATO);
	if(p) {
		if(potato_team!=255 && p->team==255)
			done_potato(potato_team);
		potato_team=p->team;
		if(potato_team!=255)
			potato_lines[potato_team]=p->potato_lines;
		removepacket();
		if(potato_team!=255) {
			got_potato(potato_team, potato_lines[potato_team]);
		}
	}
	if(!server)
		return;
	//Server stuff from this point on...
	if(potato_team<MAXTEAMS) {
		int nb_player_alive=0;
		int nb_player=0;
		int total=0;
		for(i=0; i<MAXPLAYERS; i++) {
			c=net_list.get(i);
			if(c && c->color==potato_team) {
				total+=c->potato_lines;
				if(c->idle<2)
					nb_player_alive++;
				if(c->idle<3)
					nb_player++;
			}
		}
		if(total>=potato_lines[potato_team] || nb_player_alive==0) {
			if(nb_player==0)
				potato_lines[potato_team]=2;
			if(nb_player_alive) {
				//At least one alive, next time it will be tougher :)
				potato_lines[potato_team]++;
			}
			//Potato team is done with the potato, signal it
			Packet_serverpotato p;
			p.team=255;
			p.potato_lines=0;
			net->dispatch(&p, P_SERVERPOTATO, loopback_connection);
			if(net_server)
				net_server->record_packet(&p);
			const char *reason="all_gone";
			if(nb_player)
				reason="all_died";
			if(nb_player_alive)
				reason="cleared_lines";
			Packet_serverlog log("potato_done");
			log.add(Packet_serverlog::Var("team", log_team(potato_team)));
			log.add(Packet_serverlog::Var("reason", reason));
			if(game->net_server)
				game->net_server->record_packet(&log);
			done_potato(potato_team);
			potato_team=255;
		}
	}
	//Don't pass potato if not started or not competitive
	if(game->delay_start)
		return;
	if(!net_list.competitive())
		return;
	//... or terminated!
	if(terminated || net_list.end_signaled)
		return;
	last_given_potato++;
	if(potato_team==255 && last_given_potato>=200) {
		//Should give the potato to somebody now
		Byte to_team=next_potato_team();
		if(to_team!=255) {
			last_given_potato=0;
			//Give the potato to the chosen team
			Packet_serverpotato p;
			p.team=to_team;
			p.potato_lines=potato_lines[to_team];
			net->dispatch(&p, P_SERVERPOTATO, loopback_connection);
			if(net_server)
				net_server->record_packet(&p);
			got_potato(to_team, potato_lines[to_team]);
			potato_team=to_team;
			Packet_serverlog log("potato_given");
			log.add(Packet_serverlog::Var("team", log_team(to_team)));
			log.add(Packet_serverlog::Var("lines", potato_lines[to_team]));
			if(game->net_server)
				game->net_server->record_packet(&log);
		}
	}
}

void Game::clientpause() {
	Packet_clientpause p;
	net->sendtcp(&p);
}

void Game::stackpacket(Packet *p) {
	stack.add(p);
}

Packet *Game::peekpacket(Byte type) {
	if(stack.size()) {
		if(stack[0]->packet_id == type || type==255)
			return stack[0];
		else
			return NULL;
	} else {
		return NULL;
	}
}

void Game::removepacket() {
	if(stack.size()) {
		delete stack[0];
		stack.remove(0);
	}
}

void Game::count_playing_time() {
	if(!paused)
		stats[GS::PLAYING_TIME].add(1);
}

int Game::net_version() {
	if(playback && playback->old_mode)
		return 20;
	else
		return the_net_version;
}

bool Game::any_attack() {
	if(normal_attack.type!=ATTACK_NONE || clean_attack.type!=ATTACK_NONE)
		return true;
	if(hot_potato && (potato_normal_attack.type!=ATTACK_NONE || potato_clean_attack.type!=ATTACK_NONE))
		return true;
	return false;
}

void Game::addgameinfo(Textbuf *tb) {
	tb->append("name %s\n", name);
	tb->append("version %i\n", net_version());
	tb->append("address");
	for(int a=0; a<net->host_adr_pub.size(); ++a) {
		Dword ip = net->host_adr_pub[a];
		tb->append(" %i.%i.%i.%i", ip>>24, (ip>>16)&255, (ip>>8)&255, ip&255);
	}
	tb->append("\n");
	tb->append("port %i\n", config.info.port_number);
	tb->append("status/started %i\n", !delay_start? 1:0);
	tb->append("status/terminated %i\n", terminated? 1:0);
	tb->append("status/autorestart %i\n", auto_restart? 1:0);
	tb->append("status/running_time %i\n", overmind.framecount);
	tb->append("stats/playing_time %i\n", stats[GS::PLAYING_TIME].get_value());
	tb->append("stats/round_number %i\n", stats[GS::ROUND_NUMBER].get_value());
	int type=0;
	if(survivor)
		type=1;
	if(normal_attack.type==ATTACK_NONE)
		type=2;
	if(normal_attack.type==ATTACK_BLIND || normal_attack.type==ATTACK_FULLBLIND)
		type=3;
	if(hot_potato)
		type=4;
	if(single)
		type=5;
	tb->append("rules/type %i\n", type);
	tb->append("rules/survivor %i\n", survivor? 1:0);
	tb->append("rules/hot_potato %i\n", hot_potato? 1:0);
	tb->append("rules/attacks/normal %i %i\n", normal_attack.type, normal_attack.param);
	tb->append("rules/attacks/clean %i %i\n", clean_attack.type, clean_attack.param);
	if(hot_potato) {
		tb->append("rules/attacks/potato_normal %i %i\n", potato_normal_attack.type, potato_normal_attack.param);
		tb->append("rules/attacks/potato_clean %i %i\n", potato_clean_attack.type, potato_clean_attack.param);
	}
	tb->append("rules/levelstart %i\n", level_start);
	tb->append("rules/levelup %i\n", level_up? 1:0);
	tb->append("rules/mincombo %i\n", combo_min);
	tb->append("rules/allowhandicap %i\n", allow_handicap? 1:0);
	tb->append("end/type %i\n", game_end);
	tb->append("end/value %i\n", game_end_value);
	int player = 0;
	for(int i=0; i<MAXPLAYERS; i++) {
		Canvas *c = net_list.get(i);
		if(c) {
			tb->append("players/%i/name %s\n", player, c->name);
			tb->append("players/%i/team %i\n", player, c->color);
			tb->append("players/%i/status %i\n", player, c->idle);
			tb->append("players/%i/handicap %i\n", player, c->handicap);
			int maxcombo=0;
			int combo_count=0;
			int combo;
			for(combo=CS::CLEAR00; combo<=CS::CLEAR13; combo++) {
				int cc=c->stats[combo].get_value();
				if(cc) {
					combo_count=cc;
					switch(combo) {
						case CS::CLEAR00: maxcombo=1; break;
						case CS::CLEAR01: maxcombo=2; break;
						case CS::CLEAR02: maxcombo=3; break;
						case CS::CLEAR03: maxcombo=4; break;
						case CS::CLEAR04: maxcombo=5; break;
						case CS::CLEAR05: maxcombo=6; break;
						case CS::CLEAR06: maxcombo=7; break;
						case CS::CLEAR07: maxcombo=8; break;
						case CS::CLEAR08: maxcombo=9; break;
						case CS::CLEAR09: maxcombo=10; break;
						case CS::CLEAR10: maxcombo=11; break;
						case CS::CLEAR11: maxcombo=12; break;
						case CS::CLEAR12: maxcombo=13; break;
						case CS::CLEAR13: maxcombo=14; break;
					}
				}
			}
			for(combo=CS::CLEAR14; combo<=CS::CLEAR20; combo++) {
				int cc=c->stats[combo].get_value();
				if(cc) {
					combo_count=cc;
					switch(combo) {
						case CS::CLEAR14: maxcombo=15; break;
						case CS::CLEAR15: maxcombo=16; break;
						case CS::CLEAR16: maxcombo=17; break;
						case CS::CLEAR17: maxcombo=18; break;
						case CS::CLEAR18: maxcombo=19; break;
						case CS::CLEAR19: maxcombo=20; break;
						case CS::CLEAR20: maxcombo=21; break;
					}
				}
			}
			if(maxcombo)
				tb->append("players/%i/maxcombo %i %i\n", player, maxcombo, combo_count);
			if(c->stats[CS::PLAYING_TIME].get_value())
				tb->append("players/%i/ppm %u\n", player, c->stats[CS::PPM].get_value());
			tb->append("players/%i/playing_time %u\n", player, c->stats[CS::PLAYING_TIME].get_value());
			if(!game->delay_start) {
				tb->append("players/%i/frags %i\n", player, c->stats[CS::FRAG].get_value());
				tb->append("players/%i/deaths %i\n", player, c->stats[CS::DEATH].get_value());
				tb->append("players/%i/lines %i\n", player, c->stats[CS::LINESTOT].get_value());
				tb->append("players/%i/score %i\n", player, c->stats[CS::SCORE].get_value());
			}
			player++;
		}
	}
}

bool Game::verifygameinfo(const Dict *sum) const
{
	//Only verify the validity of a single player game for now
	//Verifying multi-player games could be done by a hypothetical
	//  qserv.pl that gathers demos from disparate tournament servers
	//  but it's a lot simpler to set up trusted servers for such a
	//  purpose
	if(!single || !level_up || level_start != 1)
		return false;

	const Canvas *canvas = net_list.get(0);
	if(!canvas)
		return false;

	const Dict *players = sum->find_sub("players");
	if(!players)
		return false;
	const Dict *player = players->find_sub("0");
	if(!player)
		return false;
	const char *pscore = player->find("score");
	const char *plines = player->find("lines");
	if(!pscore || !plines)
		return false;

	const int score = atoi(pscore);
	const int lines = atoi(plines);
	if(score <= 0 || lines <= 0)
		return false;
	const int level = (lines / 15) + 1;

	if(score != canvas->stats[CS::SCORE].get_value())
		return false;
	if(lines != canvas->stats[CS::LINESTOT].get_value())
		return false;
	if(level != canvas->level)
		return false;

	return true;
}

void Game::buildgameinfo(const char* cmd) {
	gameinfo->add_data(cmd);
	Textbuf tb;
	addgameinfo(&tb);
	gameinfo->add_data(tb.get());
}

void Game::sendgameinfo(bool quit) {
	char st[1024];
	if(!network || !game_public || !server)
		return;
	if(gameinfo)
		delete gameinfo;
	gameinfo=new Qserv();
	const char *msg = net->failed();
	if(msg) {
		sprintf(st, ST_NETWORKERRORLOOKINGBOB, msg);
		message(-1, st, true, false, true);
		message(-1, ST_GAMENOTPUBLIC, true, false, true);
		http_failed = true;
		delete gameinfo;
		gameinfo = NULL;
	} else {
		if(quit)
			buildgameinfo("deletegame\n");
		else
			buildgameinfo("postgame\n");
		gameinfo->send();
	}
}

void Game::stepgameinfo() {
	if(gameinfo) {
		if(gameinfo->done()) {
			const char *status=gameinfo->get_status();
			if(status==NULL || (strcmp(status, "Game added") && strcmp(status, "Game updated"))) {
				message(-1, ST_INVALIDSERVERRESPONSE, true, false, true);
				/*message(-1, ST_GAMENOTPUBLIC, true, false, true);
				http_failed = true;
				game_public = false;*/
			}
			delete gameinfo;
			gameinfo=NULL;
		}
	}
}

bool Game::gameinfo_completed() const {
	return gameinfo ? false:true;
}

void Game::endgame() {
	terminated = true;
}

void Game::prepare_recording(const char *fn) {
	recording = new Recording();
	is_recording=true;
	char st[1024];
	char nom[1024];
	if(fn) {
		strncpy(record_filename, fn, 1023);
		record_filename[1023] = 0;
	}
	strcpy(nom, record_filename);
	//Remove .qrec if present
	int len = strlen(nom);
	if(len>=5)
		if(!strcasecmp(".qrec", &nom[len-5]))
			nom[len-5] = 0;
	//When restarting and recording, auto-increment file name
	static int record_num = 0;
	if(auto_restart || !fn) {
		sprintf(st, ".%04i", record_num++);
		strcat(nom, st);
	}
	strcat(nom, ".qrec"); // ajoute .qrec
	if(!recording->create(nom)) {
		sprintf(st, ST_GAMENOTRECORDEDAS, nom);
		message(-1, st, true, false, true);
		msgbox("Game::prepare_recording: Warning: could not create demo file\n");
		delete recording; // si la creation du fichier a pas marcher
		recording=NULL;
	}
	else {
		sprintf(st, ST_GAMERECORDINGAS, nom);
		message(-1, st, true, false, true);
		Packet_gameserver p;
		Net_pendingjoin::load_packet_gameserver(&p);
		recording->start_for_multi(&p);
	}
}

void Game::prepare_logging() {
	is_slogging=true;
	//Begin log output here
	const char *os;
	#ifdef UGS_DIRECTX
	os="Windows";
	#endif
	#ifdef UGS_LINUX
	os="Linux";
	#endif
	char game_version[32];
	sprintf(game_version, "%i.%i.%i", Config::major, Config::minor, Config::patchlevel);

	Packet_serverlog log("game_information");
	log.add(Packet_serverlog::Var("version", game_version));
	log.add(Packet_serverlog::Var("os", os));

	Dword addr = INADDR_LOOPBACK;
	if (net->host_adr_pub.size())
		addr = net->host_adr_pub[0];
	char st[64];
	Net::stringaddress(st, addr, config.info.port_number);

	log.add(Packet_serverlog::Var("server_address", st));

	char *abs_time=Clock::absolute_time();
	log.add(Packet_serverlog::Var("absolute_time", abs_time));

	log.add(Packet_serverlog::Var("game_name", name));

	const char *game_type="ffa";
	if(survivor)
		game_type="survivor";
	if(normal_attack.type==ATTACK_NONE && clean_attack.type==ATTACK_NONE)
		game_type="peace";
	if(normal_attack.type==ATTACK_BLIND || normal_attack.type==ATTACK_FULLBLIND)
		game_type="blind";
	if(hot_potato)
		game_type="hot_potato";
	if(single) {
		game_type="single";
		if(game_end==END_TIME)
			game_type="single_sprint";
	}
	log.add(Packet_serverlog::Var("game_type", game_type));

	log.add(Packet_serverlog::Var("survivor", survivor? "true":"false"));

	log.add(Packet_serverlog::Var("hot_potato", hot_potato? "true":"false"));

	log.add(Packet_serverlog::Var("normal_attack", normal_attack.log_type()));
	log.add(Packet_serverlog::Var("normal_attack_param", normal_attack.param));

	log.add(Packet_serverlog::Var("clean_attack", clean_attack.log_type()));
	log.add(Packet_serverlog::Var("clean_attack_param", clean_attack.param));

	if(hot_potato) {
		log.add(Packet_serverlog::Var("potato_normal_attack", potato_normal_attack.log_type()));
		log.add(Packet_serverlog::Var("potato_normal_attack_param", potato_normal_attack.param));

		log.add(Packet_serverlog::Var("potato_clean_attack", potato_clean_attack.log_type()));
		log.add(Packet_serverlog::Var("potato_clean_attack_param", potato_clean_attack.param));
	}
	log.add(Packet_serverlog::Var("level_up", level_up? "true":"false"));

	log.add(Packet_serverlog::Var("level_start", level_start));

	log.add(Packet_serverlog::Var("allow_handicap", allow_handicap? "true":"false"));

	const char *end_type="unknown";
	switch(game_end) {
		case END_NEVER: end_type="never"; break;
		case END_FRAG: end_type="frags"; break;
		case END_TIME: end_type="time"; break;
		case END_POINTS: end_type="points"; break;
		case END_LINES: end_type="lines"; break;
		default: break;
	}
	log.add(Packet_serverlog::Var("game_end", end_type));
	log.add(Packet_serverlog::Var("game_end_value", game_end_value));

	if(net_server)
		net_server->record_packet(&log);
}

int Game::get_multi_level() {
	if(playback)
		return playback->multi_level;
	else
		return config.info.multi_level;
}

void Game::set_seed(Packet_serverrandom *p) {
	msgbox("Game::set_seed\n");
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=net_list.get(i);
		if(c) {
			c->rnd.set_seed(p->seed);
		}
	}
	seed=p->seed;
}

char *Game::get_motd() {
	if(net_list.motd[0])
		return net_list.motd;
	else
		return NULL;
}
