/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <string.h>
#include "config.h"
#include "net_stuff.h"
#include "sons.h"
#include "canvas.h"
#include "game.h"
#include "nglog.h"
#include "chat_text.h"

Chat_text *chat_text=NULL;
int Chat_text::quel_player=0;

Chat_text::Chat_text(Fontdata *f, int wid) {
	font = f;
	clear();
	new_text = false;
	w = wid;
}

void Chat_text::add_text(int team, const char *text, bool sound) {
	char tmp[256];
	strncpy(tmp, text, 256);
	tmp[255]=0;
	int color_cut=-1;
	if(team&16 && team>0) {
		team-=16;
		char *col=strchr(tmp, ':');
		if(col)
			color_cut=col-tmp;
	}
	int wid, last_s, i;
	char *tm = tmp;
	do {
		scroll_up();
		list[CHAT_NBLINE-1].team = team;
		if(color_cut!=-1)
			team=-1;
		list[CHAT_NBLINE-1].color_cut = color_cut;
		color_cut=-1;
		last_s = -1;
		for(i=0; (unsigned int)i < strlen(tm); i++) {
			wid = font->width(tm, i+1);
			if(wid > w-2) {
				int skip_space=1;
				if((last_s != -1 && font->width(tm, last_s) < w/2) || (last_s == -1)) {
					last_s = i-1;
					skip_space=0;
				}
				strncpy(list[CHAT_NBLINE-1].text, tm, last_s);
				list[CHAT_NBLINE-1].text[last_s] = 0;
				tm += last_s+skip_space;
				i = -1;
				break;
			} else
				if(tm[i] == 32)
					last_s = i;
		}
		if(i != -1) {
			strcpy(list[CHAT_NBLINE-1].text, tm);
		}
	} while(i == -1);
	new_text = true;

	if(game && !game->single && team != -1 && sound)
		Sfx stmp(sons.fadeout, 0, -200, 0, 28000);
}

void Chat_text::scroll_up() {
	for(int i=1; i<CHAT_NBLINE; i++) {
		list[i-1].team = list[i].team;
		strcpy(list[i-1].text, list[i].text);
		list[i-1].color_cut = list[i].color_cut;
	}
}

void Chat_text::addwatch() {
	net->addwatch(P_CHAT, this);
}

void Chat_text::removewatch() {
	net->removewatch(P_CHAT, this);
}

void Chat_text::net_call(Packet *p2) {
	Packet_chat *p=(Packet_chat *)p2;
	static Dword last_sound=0;
	bool ok = false;
	if(p->to_team != -1 && game) {
		for(int i=0; i<MAXPLAYERS; i++) {
			Canvas *c = game->net_list.get(i);
			if(c && c->color == p->to_team && c->islocal()) {
				ok = true;
				break;
			}
		}
	} else {
		ok = true;
	}
	if(ok || (game && game->server)) {
		if(last_sound-overmind.framecount>=4) {
			last_sound=overmind.framecount;
			Sfx stmp(sons.msg, 0, 0, 0, 11025);
		}
		message(p->team|16, p->text, false, true, !ok);
	}
	delete p2;
}

void Chat_text::clear() {
	to_player=0;
	for(int i=0; i<CHAT_NBLINE; i++) {
		list[i].text[0] = 0;
		list[i].team = -1;
		list[i].color_cut = -1;
	}
	new_text = true;
}

void message(int color, const char *text, bool sound, bool in_packet, bool trusted, Net_connection *but) {
	chat_text->add_text(color, text, sound);
	if(!game || !game->server)
		return;
	int co;
	for(co=0; co<net->connections.size(); co++) {
		Net_connection *nc=net->connections[co];
		if(nc && (nc->trusted || !trusted) && nc!=but && nc!=game->loopback_connection)
			if(!in_packet || !nc->packet_based)
				send_msg(nc, "%s", text);
	}
}
