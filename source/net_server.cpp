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

#include "game.h"
#include "canvas.h"
#include "chat_text.h"
#include "texte.h"
#include "global.h"
#include "config.h"
#include "sons.h"
#include "recording.h"
#include "nglog.h"
#include "net_server.h"

RCSID("$Id$")

Net_client::Net_client() {
	msgbox("Net_client::Net_client\n");
	net->addwatch(P_GAMESTAT, this);
	net->addwatch(P_SERVERNAMETEAM, this);
	net->addwatch(P_MOVES, this);
	net->addwatch(P_DROPPLAYER, this);
	net->addwatch(P_SERVERDROPPLAYER, this);
	net->addwatch(P_STAT, this);
	net->addwatch(P_PAUSE, this);
	net->addwatch(P_PLAYER, this);
	net->addwatch(P_STAMPBLOCK, this);
	net->addwatch(P_DEAD, this);
	net->addwatch(P_RESPAWN, this);
	net->addwatch(P_LINES, this);
	net->addwatch(P_FIRST_FRAG, this);
	net->addwatch(P_GONE, this);
	net->addwatch(P_ENDGAME, this);
	net->addwatch(P_REJOIN, this);
	net->addwatch(P_SERVERSTATE, this);
	net->addwatch(P_STATE, this);
	net->addwatch(P_SERVERRANDOM, this);
	net->addwatch(P_SERVERPOTATO, this);
	net->addwatch(P_SERVERTESTPING, this);
	net->addwatch(P_REMOVEBONUS, this);
}

Net_client::~Net_client() {
	net->removewatch(P_REMOVEBONUS, this);
	net->removewatch(P_SERVERTESTPING, this);
	net->removewatch(P_SERVERPOTATO, this);
	net->removewatch(P_SERVERRANDOM, this);
	net->removewatch(P_STATE, this);
	net->removewatch(P_SERVERSTATE, this);
	net->removewatch(P_REJOIN, this);
	net->removewatch(P_ENDGAME, this);
	net->removewatch(P_GONE, this);
	net->removewatch(P_FIRST_FRAG, this);
	net->removewatch(P_LINES, this);
	net->removewatch(P_RESPAWN, this);
	net->removewatch(P_DEAD, this);
	net->removewatch(P_STAMPBLOCK, this);
	net->removewatch(P_PLAYER, this);
	net->removewatch(P_PAUSE, this);
	net->removewatch(P_STAT, this);
	net->removewatch(P_SERVERDROPPLAYER, this);
	net->removewatch(P_DROPPLAYER, this);
	net->removewatch(P_MOVES, this);
	net->removewatch(P_SERVERNAMETEAM, this);
	net->removewatch(P_GAMESTAT, this);
	//Intricate, isn't it? :)
	if(game->single)
		net->stop_client();
}

void Net_client::net_call(Packet *p2) {
	switch(p2->packet_id) {
		case P_SERVERTESTPING:
			p2->packet_id=P_TESTPING;
			net->sendtcp(p2);
			break;
		case P_PAUSE:
			pause(p2);
			break;
		case P_STAT:
		case P_DROPPLAYER:
		case P_GONE:
		case P_DEAD:
		case P_MOVES:
		case P_REMOVEBONUS:
		case P_SERVERDROPPLAYER:
		case P_PLAYER:
		case P_STAMPBLOCK:
		case P_RESPAWN:
		case P_LINES:
		case P_FIRST_FRAG:
		case P_REJOIN:
		case P_SERVERSTATE:
		case P_STATE:
		case P_SERVERPOTATO:
		case P_ENDGAME:
		case P_SERVERRANDOM:
		case P_SERVERNAMETEAM:
		case P_GAMESTAT:
			game->stackpacket(p2);
			return;
	}
	delete p2;
}

void Net_client::pause(Packet *p2) {
	msgbox("Net_client::pause\n");
	if(game->paused) {
		if(game->delay_start==500) {
			message(-1, ST_GAMEWILLSTART);
			game->delay_start = 499; // starts the countdown
			msgbox("Net_client::pause: starting countdown...\n");
			Packet_serverlog log("game_start");
			if(game->net_server)
				game->net_server->record_packet(&log);
			return; // doesn't remove game->paused immediately
		}
		if(game->delay_start != 0) {
			game->delay_start = 0; // force the countdown to stop if it was active
			msgbox("Net_client::pause: stop countdown...\n");
			return; // and stay on pause
		}
		message(-1, "Game unpaused", true, true);
		if(p2 && p2->from) {
			Packet_serverlog log("unpause");
			log.add(Packet_serverlog::Var("id", p2->from->id()));
			if(game->net_server)
				game->net_server->record_packet(&log);
		}
	} else {
		Packet_pause *p=(Packet_pause *) p2;
		const char *pn;
		if(p->player == -1)
			pn = ST_SERVER;
		else {
			Canvas *c=game->net_list.get(p->player);
			if(c)
				pn = c->name;
			else
				pn = ST_SERVER;
		}
		sprintf(st, ST_PAUSEDBYBOB, pn);
		message(-1, st);
		if(p2 && p2->from) {
			Packet_serverlog log("pause");
			log.add(Packet_serverlog::Var("id", p2->from->id()));
			if(game->net_server)
				game->net_server->record_packet(&log);
		}
	}
	game->paused=!game->paused;
	msgbox("Net_client::pause done\n");
}

Net_server::Net_server() {
	allow_start=true;
	allow_pause=true;
	net->start_server(game->network);
	net->addwatch(P_CLIENTPAUSE, this);
	net->addwatch(P_WANTJOIN, this);
	net->addwatch(P_FINDGAME, this);
	net->addwatch(P_CLIENTCHAT, this);
	net->addwatch(P_PLAYERWANTJOIN, this);
	net->addwatch(P_CLIENTSTAMPBLOCK, this);
	net->addwatch(P_CLIENTDEAD, this);
	net->addwatch(P_CLIENTRESPAWN, this);
	net->addwatch(P_CLIENTSTARTWATCH, this);
	net->addwatch(P_CLIENTTESTPING, this);
	net->addwatch(P_CLIENTLINES, this);
	net->addwatch(P_CLIENTFIRST_FRAG, this);
	net->addwatch(P_CLIENTGONE, this);
	net->addwatch(P_CLIENTMOVES, this);
	net->addwatch(P_CLIENTSTATE, this);
	net->addwatch(P_CLIENTDROPPLAYER, this);
	net->addwatch(P_BYE, this);
	net->addwatch(P_CLIENTREMOVEBONUS, this);
}

Net_server::~Net_server() {
	net->removewatch(P_CLIENTREMOVEBONUS, this);
	net->removewatch(P_BYE, this);
	net->removewatch(P_CLIENTDROPPLAYER, this);
	net->removewatch(P_CLIENTSTATE, this);
	net->removewatch(P_CLIENTMOVES, this);
	net->removewatch(P_CLIENTGONE, this);
	net->removewatch(P_CLIENTFIRST_FRAG, this);
	net->removewatch(P_CLIENTLINES, this);
	net->removewatch(P_CLIENTTESTPING, this);
	net->removewatch(P_CLIENTSTARTWATCH, this);
	net->removewatch(P_CLIENTRESPAWN, this);
	net->removewatch(P_CLIENTDEAD, this);
	net->removewatch(P_CLIENTSTAMPBLOCK, this);
	net->removewatch(P_PLAYERWANTJOIN, this);
	net->removewatch(P_CLIENTCHAT, this);
	net->removewatch(P_FINDGAME, this);
	net->removewatch(P_WANTJOIN, this);
	net->removewatch(P_CLIENTPAUSE, this);
	net->stop_server();
}

void Net_server::record_packet(Packet *p2) {
	if(recording)
		recording->write_packet(p2);
}

void Net_server::stop_multi_recording() {
	if(recording) {
		delete recording;
		recording=NULL;
	}
}

void Net_server::net_call(Packet *p2) {
	Packet_playerbase *ppb=NULL;
	switch(p2->packet_id) {
		case P_CLIENTDROPPLAYER:
		case P_CLIENTSTAMPBLOCK:
		case P_CLIENTDEAD:
		case P_CLIENTRESPAWN:
		case P_CLIENTLINES:
		case P_CLIENTFIRST_FRAG:
		case P_CLIENTGONE:
		case P_CLIENTMOVES:
		case P_CLIENTSTATE:
		case P_CLIENTREMOVEBONUS:
			ppb=(Packet_playerbase *) p2;
	}
	if(ppb) {
		Canvas *c=game->net_list.get(ppb->player);
		if(!c) {
			if(ppb->packet_id==P_CLIENTDROPPLAYER && ppb->from==game->loopback_connection) {
				//We let through a P_CLIENTDROPPLAYER about a dropped
				//  player if it's coming from the loopback_connection
				//  cause the canvas is already deleted in that case.
			}
			else {
				msgbox("Net_server::net_call: got a Packet_playerbase but player has been dropped, ignoring.\n");
				delete p2;
				return;
			}
		}
		//Check whether the originating connection of
		//  the packet is consistent with the player for cheat
		//  protection. For some reason, the remote_adr of canvases
		//  on the loopback client is NULL so we'll automatically
		//  trust the loopback_connection
		if(c && c->remote_adr!=p2->from && p2->from!=game->loopback_connection) {
			msgbox("Net_server::net_call: got a Packet_playerbase from wrong connection, ignoring.\n");
			msgbox("  id==%i, player==%i, remote_adr==%s%p, from==%p.\n", p2->packet_id, ppb->player, c? "":"[c is NULL] ", c? c->remote_adr:NULL, p2->from);
			delete p2;
			return;
		}
	}
	if(p2->from && !p2->from->joined && p2->packet_id!=P_WANTJOIN && p2->packet_id!=P_CLIENTCHAT) {
		//Until joined, we accept only P_CLIENTCHAT or P_WANTJOIN
		//Anything else is ignored
		delete p2;
		return;
	}
	switch(p2->packet_id) {
		case P_PLAYERWANTJOIN:
			playerwantjoin(p2);
			break;
		case P_FINDGAME:
			findgame(p2);
			break;
		case P_CLIENTCHAT:
			clientchat(p2);
			break;
		case P_CLIENTPAUSE:
			clientpause(p2);
			break;
		case P_CLIENTDROPPLAYER:
			net->dispatch(p2, P_DROPPLAYER, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTSTAMPBLOCK:
			net->dispatch(p2, P_STAMPBLOCK, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTDEAD:
			net->dispatch(p2, P_DEAD, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTRESPAWN:
			net->dispatch(p2, P_RESPAWN, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTSTARTWATCH:
			clientstartwatch(p2);
			break;
		case P_WANTJOIN:
			wantjoin(p2);
			return;
		case P_CLIENTTESTPING:
			p2->packet_id = P_TESTPING;
			net->sendtcp(p2->from, p2);
			break;
		case P_CLIENTLINES:
			net->dispatch(p2, P_LINES, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTFIRST_FRAG:
			net->dispatch(p2, P_FIRST_FRAG, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTGONE:
			net->dispatch(p2, P_GONE, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTMOVES:
			p2->packet_id=P_MOVES;
			record_packet(p2);
			clientmoves(p2);
			break;
		case P_CLIENTSTATE:
			net->dispatch(p2, P_STATE, p2->from);
			record_packet(p2);
			break;
		case P_CLIENTREMOVEBONUS:
			net->dispatch(p2, P_REMOVEBONUS, p2->from);
			record_packet(p2);
			break;
		case P_BYE:
			p2->from->disconnect();
			break;
	}
	delete p2;
}

void Net_server::playerwantjoin(Packet *p2) {
	Packet_playerwantjoin *p=(Packet_playerwantjoin *) p2;
	msgbox("Net_server::playerwantjoin, name=%s\n", p->name);
	Packet_playeraccepted playeraccepted;
	playeraccepted.accepted = game->server_accept_player;
	playeraccepted.pos=0;
	if(game->terminated)
		playeraccepted.accepted = 3; // the game is finished, do not accept players
	if(playeraccepted.accepted == 0) {
		for(int i=0; i<MAXPLAYERS; i++) {
			Canvas *c = game->net_list.get(i);
			if(c) {
				if(!strcmp(c->name, p->name) && !memcmp(c->player_hash, p->player_hash, sizeof(c->player_hash))) { // if already someone with this name
					if(c->idle == 3) { // if player is 'gone'
						if(c->color != p->team) {
							// if joins with a different team, drop him and accept the new one
							Packet_dropplayer p3;
							p3.player = i;
							p3.reason = DROP_AUTO;
							net->dispatch(&p3, P_DROPPLAYER, game->loopback_connection);
							record_packet(&p3);
							game->net_list.drop_player(&p3, true); // immediately drop the player
						} else { // si meme team, conserve les stats
							playeraccepted.accepted = 4; // already someone, we're going to replace this guy!
							playeraccepted.pos = i; // indicate the number of the player to replace
							playeraccepted.answer(p);

							Packet_rejoin p_rejoin;
							p_rejoin.player = i;
							p_rejoin.h_repeat = p->h_repeat;
							p_rejoin.v_repeat = p->v_repeat;
							p_rejoin.smooth = p->smooth;
							p_rejoin.shadow = p->shadow;
							p_rejoin.handicap = p->handicap;
							net->dispatch(&p_rejoin, P_REJOIN);
							record_packet(&p_rejoin);
							c->remote_adr = p->from; // the server re-adjuste the remote_adr of the canvas
							Dword id=0;
							if(p->from!=game->loopback_connection)
								id=p->from->id();
							Packet_serverlog log("player_rejoin");
							log.add(Packet_serverlog::Var("id", c->id()));
							log.add(Packet_serverlog::Var("connection_id", id));
							log.add(Packet_serverlog::Var("handicap", log_handicap(p->handicap)));
							record_packet(&log);
							game->net_list.update_team_names();
							return;
						}
					} else {
						playeraccepted.accepted = 2; // already someone, refuse the player
					}
					break;
				}
			}
		}
		if(playeraccepted.accepted == 0) {
			if(game->net_list.size() == MAXPLAYERS)
				playeraccepted.accepted = 5; // game is full, can't join
			if(game->server_max_players && game->net_list.size() >= game->server_max_players)
				playeraccepted.accepted = 5; // game is full, can't join
			if(game->server_max_teams && game->net_list.count_teams() >= game->server_max_teams) {
				for(unsigned i=0; i<MAXPLAYERS; ++i) {
					Canvas* c=game->net_list.get(i);
					if(c && c->color==p->team)
						break;
				}
				if(i==MAXPLAYERS) {
					// if not joining an already existing team, we can't accept the new player
					playeraccepted.accepted = 5; // game is full, can't join
				}
			}
		}
	}

	if(playeraccepted.accepted == 0) { // si on accepte le joueur
		Packet_player player;  // dispatches P_PLAYER to everyone (except the asking host!)
		strcpy(player.name, p->name);
		player.player = p->player;
		player.team = p->team;
		Canvas *canvas=new Canvas(game->seed, p->team, p->name, p->h_repeat, p->v_repeat, p->smooth? true:false, p->shadow? true:false, p->handicap, p->from, p->player, false);
		memcpy(canvas->player_hash, p->player_hash, sizeof(canvas->player_hash));
		strncpy(canvas->team_name, p->team_name, 40);
		memcpy(canvas->team_hash, p->team_hash, sizeof(canvas->team_hash));
		canvas->team_name[39]=0;
		playeraccepted.pos=game->net_list.add_player(canvas); 
		player.pos=playeraccepted.pos;
		player.h_repeat = p->h_repeat;
		player.v_repeat = p->v_repeat;
		player.smooth = p->smooth;
		player.shadow = p->shadow;
		player.handicap = p->handicap;
		net->dispatch(&player, P_PLAYER, p->from);
		record_packet(&player);
		Dword id=0;
		if(p->from && p->from!=game->loopback_connection)
			id=p->from->id();
		Packet_serverlog log("player_join");
		log.add(Packet_serverlog::Var("id", canvas->id()));
		log.add(Packet_serverlog::Var("connection_id", id));
		log.add(Packet_serverlog::Var("team", log_team(p->team)));
		log.add(Packet_serverlog::Var("handicap", log_handicap(p->handicap)));
		log.add(Packet_serverlog::Var("name", canvas->name));
		log.add(Packet_serverlog::Var("team_name", canvas->team_name));
		record_packet(&log);
	}
	playeraccepted.answer(p);
	game->net_list.update_team_names();
}

void Net_server::findgame(Packet *p2) {
	Packet_gameinfo resp;
	strcpy(resp.name, game->name);
	resp.version = game->net_version();
	resp.port = config.info.port_number;
	for(int i=0; i<MAXPLAYERS; i++) {
		Canvas *c=game->net_list.get(i);
		if(c)
			resp.add_player(i, c->color, c->name, c->idle, c->handicap);
	}
	resp.game_end_value = game->game_end_value;
	resp.nolevel_up = !game->level_up;
	resp.delay_start = game->delay_start ? true:false;
	resp.terminated = game->terminated;
	resp.level_start = game->level_start;
	resp.combo_min = game->combo_min;
	resp.allow_handicap = game->allow_handicap;
	resp.survivor = game->survivor;
	resp.hot_potato = game->hot_potato;
	resp.normal_attack = game->normal_attack;
	resp.clean_attack = game->clean_attack;
	resp.potato_normal_attack = game->potato_normal_attack;
	resp.potato_clean_attack = game->potato_clean_attack;
	resp.game_end = game->game_end;

	//net->sendudp(INADDR_BROADCAST, &resp);
	net->sendudp(p2->from_addr, &resp);
}

void Net_server::wantjoin(Packet *p2) {
	Packet_wantjoin *p=(Packet_wantjoin *) p2;
	Executor *exec = new Executor(true);
	pendings.add(exec);
	exec->add(new Net_pendingjoin(p));
	overmind.start(exec);
}

void Net_server::clientpause(Packet *p2) {
	Packet_pause p;
	msgbox("Net_server::clientpause from %x\n", p2->from);
	if(!game) {
		// unlikely but what the hell...
		return;
	}
	if(game->delay_start && game->delay_start!=500) {
		//Don't ever interrupt countdown
		return;
	}

	bool allowed=false;
	// check all the starting conditions
	if(game->delay_start==500 && allow_start)
		allowed=true;
	// make sure there's enough players
	if(game->server_min_players && game->server_min_players > game->net_list.size(false))
		allowed=false;
	// make sure there's enough teams
	if(game->server_min_teams && game->server_min_teams > game->net_list.count_teams(false))
		allowed=false;

	// commandline options or trusted connections are always allowed to start/pause
	if(!p2->from || p2->from->trusted)
		allowed=true;

	// if game is already started, all of the above doesn't apply anyway
	if(game->delay_start==0 && allow_pause)
		allowed=true;

	if(!allowed)
		return;

	if(game->paused) {
		p.player = -1;
		net->dispatch(&p, P_PAUSE);
		record_packet(&p);
	}
	else {
		int i;
		for(i=0; i<MAXPLAYERS; i++) {
			if(game->net_list.get(i)) {
				if(game->net_list.get(i)->remote_adr == p2->from)
					break;
			}
		}
		if(i==MAXPLAYERS)
			i = -1;
		p.player=(signed char) i;
		net->dispatch(&p, P_PAUSE);
		record_packet(&p);
	}
}

void Net_server::clientstartwatch(Packet *p2) {
	Packet_clientstartwatch *p=(Packet_clientstartwatch *) p2;
	Canvas *c = game->net_list.get(p->player);
	if(c == NULL) {
		msgbox("Server: Packet_clientstartwatch by %x for player %i, but player is down. Ignoring\n", p->from, p->player);
		return;
	}
	if(p->stop) {
		msgbox("client: stop_watch for player %i\n", p->player);
		c->remove_watcher(p->from);
	}
	else {
		msgbox("client: start_watch for player %i\n", p->player);
		c->add_watcher(new Canvas::Watcher(p->from));
	}
}

void Net_server::clientmoves(Packet *p2) {
	Packet_moves *p=(Packet_moves *) p2;
	if(p->size<25)
		return;
	Canvas *c=game->net_list.get(p->player);
	if(!c)
		return;
	int i;
	for(i=0; i<net->connections.size(); i++) {
		Net_connection *nc=net->connections[i];
		if(nc)
			for(int j=0; j<c->watchers.size(); j++) {
				Canvas::Watcher *w=c->watchers[j];
				if(w && w->nc==nc)
					net->sendtcp(nc, p);
			}
	}
}

void Net_server::clientchat(Packet *p2) {
	Packet_clientchat *p=(Packet_clientchat *) p2;
	if(p->text[0]=='/' && p->team==-1) {
		if(game)
			game->net_list.got_admin_line(p->text, p->from);
	}
	else {
		net->dispatch(p, P_CHAT);
		record_packet(p);
		if(p && p->from) {
			Packet_serverlog log("chat");
			log.add(Packet_serverlog::Var("id", p->from!=game->loopback_connection? p->from->id():0));
			log.add(Packet_serverlog::Var("text", p->text));
			record_packet(&log);
		}
	}
}

Net_pendingjoin::Net_pendingjoin(Packet_wantjoin *p) {
	msgbox("Pending join for %x...\n", p->from);
	pac = p;
	net->add_watch(this);
	cancel=false;
}

Net_pendingjoin::~Net_pendingjoin() {
	delete pac;
	net->remove_watch(this);
}

void Net_pendingjoin::load_packet_gameserver(Packet_gameserver* resp) {
	resp->version = game->net_version();
	resp->accepted = 1; // 'accepted' is useless since Quadra_param->accept_connection()
	resp->game_seed = game->seed;
	resp->paused = game->paused;
	strcpy(resp->name, game->name);
	resp->nolevel_up = !game->level_up;
	resp->level_start = game->level_start;
	resp->survivor = game->survivor;
	resp->hot_potato = game->hot_potato;
	resp->normal_attack = game->normal_attack;
	resp->clean_attack = game->clean_attack;
	resp->potato_normal_attack = game->potato_normal_attack;
	resp->potato_clean_attack = game->potato_clean_attack;
	resp->combo_min = game->combo_min;
	resp->allow_handicap = game->allow_handicap;
	resp->delay_start = game->delay_start;
	resp->game_end = game->game_end;
	resp->game_end_value = game->game_end_value;
	if(game->game_end == 2) { // if game_end == time in minutes
		Dword timer = game->net_list.gettimer();
		if(timer < (Dword)resp->game_end_value)
			resp->game_end_value -= timer; // computes and gives the remaining time only
		else
			resp->game_end_value=0;
	}
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=game->net_list.get(i);
		if(c)
			resp->add_player(i, c->color, c->name, c->id(), c->handicap);
	}
	resp->wants_moves = game->wants_moves;
	resp->syncpoint = game->net_list.syncpoint;
	resp->potato_team = game->potato_team;
	resp->single = game->single;
	resp->terminated = game->terminated;
}

void Net_pendingjoin::step() {
	if((overmind.framecount+15)&127)
		return;
	int i, j, x, y;
	msgbox("Pending join being processed for %x...\n", pac->from);

	if(cancel) {
		msgbox("   Client disconnected, cancelling the whole deal\n");
		ret();
		return;
	}
	if(!game) {
		msgbox("   No game, cancelling the whole deal\n");
		ret();
		return;
	}
	if(game->peekpacket(255)) {
		msgbox("   Something on the stack!\n");
		return;
	}
	if(game->net_list.syncpoint!=Canvas::LAST) {
		msgbox("   Currently syncing!\n");
		return;
	}
	for(i=0; i<MAXPLAYERS; i++) {
 		Canvas *c = game->net_list.get(i);
		if(c && (!c->idle || c->dying)) {
			// if a canvas isn't 'idle' (process_key), abandon
			msgbox("   Canvas %x is not idle now!\n", c);
			return;
		}
	}
	msgbox("   Uploading canvas information!\n");

	Packet_gameserver resp;
	load_packet_gameserver(&resp);
	resp.answer(pac);
	pac->from->joined=true;
	char addr[64];
	Net::stringaddress(addr, pac->from->address(), pac->from->getdestport());

	Packet_serverlog log("connection_joined");
	log.add(Packet_serverlog::Var("id", pac->from->id()));
	if(game->net_server)
		game->net_server->record_packet(&log);

	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c = game->net_list.get(i);
		if(c) {
			Packet_download d;
			d.player = i;
			for(y=0; y<32; y++)
				for(x=0; x<10; x++) {
					d.can[y][x] = c->block[y][x+4];
					d.occ[y][x] = c->occupied[y][x+4];
					d.blinded[y][x] = c->blinded[y][x+4];
				}
			d.seed = c->rnd.get_seed();
			d.idle = c->idle;
			d.state = c->state;
			if(c->bloc) {
				d.bloc = c->bloc->quel;
				d.next = c->next->quel;
				d.next2 = c->next2->quel;
				d.next3 = c->next3->quel;
			} else {
				d.bloc = d.next = d.next2 = d.next3 = 255;
			}
			d.bonus = c->bonus;
			int ii;
			for(ii=0; ii<20; ii++) {
				if(ii<c->bonus) {
					d.bon[ii].x=c->bon[ii].x;
					d.bon[ii].color=c->bon[ii].color;
					d.bon[ii].blind_time=c->bon[ii].blind_time;
					d.bon[ii].hole_pos=c->bon[ii].hole_pos;
					d.bon[ii].final=c->bon[ii].final;
				}
				else {
					d.bon[ii].x=0;
					d.bon[ii].color=0;
					d.bon[ii].blind_time=0;
					d.bon[ii].hole_pos=0;
					d.bon[ii].final=false;
				}
			}
			for(ii=0; ii<MAXPLAYERS; ii++)
				d.attacks[ii]=c->attacks[ii];
			d.last_attacker=c->last_attacker;
			net->sendtcp(pac->from, &d);

			Packet_stat p;
			p.player=i;
			CS::Stat_type last;
			if(game->net_version()>=23)
				last=CS::LAST;
			else
				last=CS::LAST_BEFORE_120;
			for(j=0; j<last; j++)
				p.add_stat(j, c->stats[j].get_value());
			net->sendtcp(pac->from, &p);
		}
	}
	char *motd=game->get_motd();
	if(motd && motd[0]) {
		send_msg(pac->from, " ");
		send_msg(pac->from, "%s", motd);
	}
	send_msg(pac->from, " ");
	ret();
}

void Net_pendingjoin::notify() {
	//Check whether the connection got closed and cancel if applicable
	int co;
	for(co=0; co<net->connections.size(); co++)
		if(net->connections[co]==pac->from)
			return;
	cancel=true; //Will cancel on next step()
}
