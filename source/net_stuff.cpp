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

#include <stdarg.h>
#include "error.h"
#include "packets.h"
#include "config.h"
#include "game.h"
#include "net_server.h"
#include "canvas.h"
#include "net_stuff.h"
#include "chat_text.h"
#include "texte.h"
#include "video.h"
#include "nglog.h"

RCSID("$Id$")

Net_starter::Net_module::Net_module() {
	last_video_frame=video->framecount;
}

void Net_starter::Net_module::step() {
	bool loop_only=video_is_dumb;
	if(last_video_frame!=video->framecount) {
		loop_only=false;
		last_video_frame=video->framecount;
	}
	net->step(loop_only);
}

char *packet_name[] = {
	"P_CHAT",
	"P_FINDGAME",
	"P_GAMESERVER",
	"P_WANTJOIN",
	"P_PLAYERWANTJOIN",
	"P_CLIENTCHAT",
	"P_PLAYERACCEPTED",
	"P_PLAYER",
	"P_CLIENTPAUSE",
	"P_PAUSE",
	"P_STAT",
	"P_GAMEINFO",
	"P_DROPPLAYER",
	"P_CLIENTDROPPLAYER",
	"P_SERVERDROPPLAYER",
	"P_STAMPBLOCK",
	"P_CLIENTSTAMPBLOCK",
	"P_DEAD",
	"P_CLIENTDEAD",
	"P_RESPAWN",
	"P_CLIENTRESPAWN",
	"DEPRECATED_P_WATCH",
	"P_STARTWATCH",
	"P_CLIENTSTARTWATCH",
	"P_DOWNLOAD",
	"P_CLIENTLINES",
	"P_LINES",
	"P_CLIENTTESTPING",
	"P_TESTPING",
	"P_FIRST_FRAG",
	"P_CLIENTFIRST_FRAG",
	"P_GONE",
	"P_CLIENTGONE",
	"P_ENDGAME",
	"P_REJOIN",
	"P_MOVES",
	"P_CLIENTMOVES",
	"P_STATE",
	"P_CLIENTSTATE",
	"P_SERVERSTATE",
	"P_SERVERRANDOM",
	"P_SERVERPOTATO",
	"DEPRECATED_P_SERVERLINES",
	"P_SERVERTESTPING",
	"P_BYE",
	"P_REMOVEBONUS",
	"P_CLIENTREMOVEBONUS",
	"P_SERVERNAMETEAM",
	"P_GAMESTAT",
	"P_SERVERLOG",
};

int Quadra_param::tcpport() {
	return config.info.port_number;
}

void Quadra_param::print_packet(Packet *p2, char *st) {
	if(!st)
		return;
	if(!p2) {
		st[0]=0;
		return;
	}
	if(p2->packet_id==P_STATE) {
		Packet_state *p=(Packet_state *) p2;
		sprintf(st, "P_STATE: player==%i state==%i", p->player, p->state);
		return;
	}
	if(p2->packet_id==P_CLIENTSTATE) {
		Packet_clientstate *p=(Packet_clientstate *) p2;
		sprintf(st, "P_CLIENTSTATE: player==%i state==%i", p->player, p->state);
		return;
	}
	if(p2->packet_id==P_SERVERSTATE) {
		Packet_serverstate *p=(Packet_serverstate *) p2;
		sprintf(st, "P_SERVERSTATE: state==%i", p->state);
		return;
	}
	if(p2->packet_id==P_CHAT) {
		Packet_chat *p=(Packet_chat *) p2;
		sprintf(st, "P_CHAT: %s", p->text);
		return;
	}
	if(p2->packet_id==P_CLIENTCHAT) {
		Packet_clientchat *p=(Packet_clientchat *) p2;
		sprintf(st, "P_CLIENTCHAT: %s", p->text);
		return;
	}
	if(p2->packet_id==P_PLAYER) {
		Packet_player *p=(Packet_player *) p2;
		sprintf(st, "P_PLAYER: %s", p->name);
		return;
	}
	if(p2->packet_id==P_MOVES) {
		Packet_moves *p=(Packet_moves *) p2;
		sprintf(st, "P_MOVES: %i", p->player);
		return;
	}
	if(p2->packet_id==P_STAMPBLOCK) {
		Packet_stampblock *p=(Packet_stampblock *) p2;
		sprintf(st, "P_STAMPBLOCK: %i", p->player);
		return;
	}
	sprintf(st, "%s", packet_name[p2->packet_id]);
}

Packet *Quadra_param::alloc_packet(Word pt) {
	switch(pt) {
		case P_CHAT: return new Packet_chat();
		case P_FINDGAME: return new Packet_findgame();
		case P_GAMESERVER: return new Packet_gameserver();
		case P_WANTJOIN: return new Packet_wantjoin();
		case P_PLAYERWANTJOIN: return new Packet_playerwantjoin();
		case P_CLIENTCHAT: return new Packet_clientchat();
		case P_PLAYERACCEPTED: return new Packet_playeraccepted();
		case P_PLAYER: return new Packet_player();
		case P_CLIENTPAUSE: return new Packet_clientpause();
		case P_PAUSE: return new Packet_pause();
		case P_STAT: return new Packet_stat();
		case P_GAMEINFO: return new Packet_gameinfo();
		case P_DROPPLAYER: return new Packet_dropplayer();
		case P_CLIENTDROPPLAYER: return new Packet_clientdropplayer();
		case P_SERVERDROPPLAYER: return new Packet_serverdropplayer();
		case P_STAMPBLOCK: return new Packet_stampblock();
		case P_CLIENTSTAMPBLOCK: return new Packet_clientstampblock();
		case P_DEAD: return new Packet_dead();
		case P_CLIENTDEAD: return new Packet_clientdead();
		case P_RESPAWN: return new Packet_respawn();
		case P_CLIENTRESPAWN: return new Packet_clientrespawn();
		case P_STARTWATCH: return new Packet_startwatch();
		case P_CLIENTSTARTWATCH: return new Packet_clientstartwatch();
		case P_DOWNLOAD: return new Packet_download();
		case P_CLIENTLINES: return new Packet_clientlines();
		case P_LINES: return new Packet_lines();
		case P_TESTPING: return new Packet_testping();
		case P_CLIENTTESTPING: return new Packet_clienttestping();
		case P_FIRST_FRAG: return new Packet_first_frag();
		case P_CLIENTFIRST_FRAG: return new Packet_clientfirst_frag();
		case P_GONE: return new Packet_gone();
		case P_CLIENTGONE: return new Packet_clientgone();
		case P_ENDGAME: return new Packet_endgame();
		case P_REJOIN: return new Packet_rejoin();
		case P_MOVES: return new Packet_moves();
		case P_CLIENTMOVES: return new Packet_clientmoves();
		case P_STATE: return new Packet_state();
		case P_CLIENTSTATE: return new Packet_clientstate();
		case P_SERVERSTATE: return new Packet_serverstate();
		case P_SERVERRANDOM: return new Packet_serverrandom();
		case P_SERVERPOTATO: return new Packet_serverpotato();
		case P_SERVERTESTPING: return new Packet_servertestping();
		case P_BYE: return new Packet_bye();
		case P_REMOVEBONUS: return new Packet_removebonus();
		case P_CLIENTREMOVEBONUS: return new Packet_clientremovebonus();
		case P_SERVERNAMETEAM: return new Packet_servernameteam();
		case P_GAMESTAT: return new Packet_gamestat();
		case P_SERVERLOG: return new Packet_serverlog();
		default: return NULL;
	}
}

bool Quadra_param::is_dispatchable(Net_connection *nc, Packet *p) {
	if(!p)
		return false;
	if(!nc)
		return false;
	if(p->packet_id==P_CHAT)
		return true;
	if(nc->joined)
		return true;
	return false;
}

void Quadra_param::server_deconnect() {
	msgbox("Quadra_param::server_deconnect: Game terminated.\n");
	if(game) {
		game->abort=true;
		if(!game->server)
			message(-1, ST_SERVERDECONNECT);
	}
}

bool Quadra_param::accept_connection(Net_connection *nc) {
	if(!Net_param::accept_connection(nc))
		return false;
	if(!game)
		return false;
	if(game->server_accept_connection)
		return false;
	if(!game->network)
		return false;
	if(!game->net_list.accept_connection(nc))
		return false;
	return true;
}

void Quadra_param::client_connect(Net_connection *adr) {
	if(!adr)
		return;
	char st[64], st1[256];
	Net::stringaddress(st, adr->address(), adr->getdestport());
	Packet_serverlog log("connect");
	log.add(Packet_serverlog::Var("id", adr->id()));
	log.add(Packet_serverlog::Var("address", st));
	if(game && game->net_server)
		game->net_server->record_packet(&log);
	sprintf(st1, ST_CONNECTFROMBOB, st);
	message(-1, st1, true, false, true);
}

void Quadra_param::client_deconnect(Net_connection *adr) {
	if(!adr)
		return;
	msgbox("Quadra_param::client_deconnect(%x)\n", adr);
	if(game)
		game->net_list.client_deconnect(adr);
	char st[64], st1[256];
	Net::stringaddress(st, adr->address(), adr->getdestport());

	Packet_serverlog log("disconnect");
	log.add(Packet_serverlog::Var("id", adr->id()));
	if(game && game->net_server)
		game->net_server->record_packet(&log);

	sprintf(st1, ST_DISCONNECTFROMBOB, st);
	message(-1, st1, true, false, true);
	if(!game)
		return;
	for(int j=0; j<MAXPLAYERS; j++) {
		Canvas *c = game->net_list.get(j);
		if(c && c->remote_adr == adr) {
			c->remote_adr = NULL; // this net_connection is now destroyed
			if(c->idle != 3) { 
				/* sends an automatic 'P_GONE' for every active players of the client that has been disconnected.  */
				if(c->idle != 2 && !c->dying) {
					Packet_dead d;
					d.player = j;
					d.then_gone = true;
					net->dispatch(&d, P_DEAD);
					if(game->net_server)
						game->net_server->record_packet(&d);
				}
				else {
					Packet_gone p;
					p.player = j;
					net->dispatch(&p, P_GONE);
					if(game->net_server)
						game->net_server->record_packet(&p);
				}
			}
		}
	}
}

char *Quadra_param::get_motd() {
	if(game && game->get_motd())
		return game->get_motd();
	else
		return Net_param::get_motd();
}

void send_msg(Net_connection *nc, char *msg, ...) {
	if(!nc || !msg)
		return;
	char st[4096];
	va_list marker;
	va_start(marker, msg);
	vsnprintf(st, sizeof(st) - 1, msg, marker);
	va_end(marker);
	if(nc->packet_based) {
		Packet_chat p;
		p.team = -1;
		p.to_team = -1;
		strncpy(p.text, st, 255);
		p.text[255]=0;
		if(game && game->loopback_connection==nc)
			chat_text->add_text(p.team, p.text, true);
		else
			nc->sendtcp(&p);
	}
	else {
		char st2[4120];
		//Output on a separate line if user is typing a line
		if(nc->incoming->size())
			sprintf(st2, "\r\n%s\r\n", st);
		else
			sprintf(st2, "%s\r\n", st);
		nc->sendtcp((Byte *) st2, strlen(st2));
		//Re-echo user command to be less annoying
		if(nc->incoming->size())
			nc->sendtcp(nc->incoming->get(), nc->incoming->size());
	}
}
