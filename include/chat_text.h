/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_CHAT_TEXT
#define _HEADER_CHAT_TEXT

#include "net_stuff.h"
#include "inter.h"

#define CHAT_NBLINE 22

class Chat_text: public Net_callable {
	friend class Chat_interface;
	struct {
		char text[256];
		int team;
		int color_cut;
	}	list[CHAT_NBLINE];
	Fontdata *font;
	void scroll_up();
	bool new_text;
	int w;
public:
	static int quel_player;
	int to_player;
	Chat_text(Fontdata *f, int wid);
	void add_text(int team, const char *text, bool sound=true);
	void removewatch();
	void addwatch();
	void net_call(Packet *p2);
	void clear();
};

extern Chat_text *chat_text;

void message(int color, const char *text, bool sound=true, bool in_packet=true, bool trusted=false, Net_connection *but=NULL);

#endif
