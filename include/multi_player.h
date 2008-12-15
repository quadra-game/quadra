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

#ifndef _HEADER_MULTI_PLAYER
#define _HEADER_MULTI_PLAYER
#include "overmind.h"
#include "inter.h"
#include "bitmap.h"
#include "misc.h"
#include "menu_base.h"
#include "zone_list.h"
#include "zone.h"
#include "game.h"

class Pane;
class Pane_info;
class Playback;

class Zone_slow_play: public Zone_text_button2 {
public:
	Zone_slow_play(Inter *in, Bitmap *bit, Font *f, const char *t, int px, int py);
	virtual void process();
	virtual void waiting();
	virtual void clicked(int quel) { }
};

class Zone_fast_play: public Zone_text_button2 {
public:
	Zone_fast_play(Inter *in, Bitmap *bit, Font *f, const char *t, int px, int py);
	virtual void process();
	virtual void waiting();
	virtual void clicked(int quel) { }
};

class Multi_player: public Menu_fadein {
	Zone_list list;
	int *got_highscore;
	Zone *b_quit;
	int last_countdown;
	Module **menu_stat;
public:
	bool stop;  // set to true to quit game
	Bitmap *bit;
	Font *font2, *courrier;
	Executor *pane_exec[3];
	Pane *pane[3];
	const Pane_info *pane_info[3];
	bool pause;
	Zone_sprite *zone_pause;
	int time_demo;
	void check_pause();
	Multi_player(int *got_high);
	virtual ~Multi_player();
	virtual void step();
	void set_menu_stat(Module **module);
};

class Demo_multi_player: public Module {
	void init_playback();
public:
	Demo_multi_player(Playback *p);
	Demo_multi_player(Res *r, bool auto_demo=false);
	virtual ~Demo_multi_player();
	virtual void init();
};

class Single_player: public Module {
	int play_again;
	const Game_preset gp;
public:
	Single_player(Game_preset pgp);
	virtual void step();
};

class Single_player_iterate: public Module {
	int *play_again, hscore;
	const Game_preset gp;
public:
	Single_player_iterate(int *play, Game_preset pgp);
	virtual void init();
	virtual void step();
};

class Multi_player_launcher: public Module {
	Module *menu;
public:
	virtual void init();
	virtual void step();
};

#endif
