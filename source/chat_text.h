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
