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

#ifndef _HEADER_MENU
#define _HEADER_MENU

#include "sound.h"
#include "sprite.h"
#include "inter.h"
#include "listbox.h"
#include "zone_list.h"
#include "stats.h"
#include "net_stuff.h"
#include "menu_base.h"
#include "score.h"
#include "highscores.h"
#include "game.h"

class Pane_option;
class Pane_info;

class Menu_do_nothing: public Menu {
public:
	//It actually does something:
	//  wait for quitting==true and then ret()
	virtual void step();
};

class Menu_intro: public Menu {
	int warning;
	Font *font2;
	bool once;
public:
	Menu_intro();
	virtual ~Menu_intro();
	virtual void init();
	virtual void step();
};

class Menu_guy: public Menu {
	Sample *son;
public:
	Menu_guy();
	virtual ~Menu_guy();
	virtual void init();
	virtual void step();
};

class Menu_ugs: public Menu {
	Sample *son;
public:
	Menu_ugs();
	virtual ~Menu_ugs();
	virtual void init();
	virtual void step();
};

class Menu_main_startmusic: public Module {
public:
	virtual void init();
};

class Menu_main: public Menu {
	Zone_bitmap *z_back;
	Zone *b_single, *b_multi, *b_setup, *b_help, *b_quit;
	Zone *b_tut, *b_option, *b_logo, *b_demo;
	int delay;
	bool old_registered;
	int old_language;
	void reset_delay();
	void redraw();
public:
	Menu_main();
	virtual void init();
	virtual void step();
};

class Menu_quitgame: public Menu_standard {
	Font *font2;
public:
	Menu_quitgame();
	virtual ~Menu_quitgame();
	virtual void init();
	virtual void step();
};

class Menu_help: public Menu_standard {
	Font *font2;
	Zone *b_quit;
	Zone *b_www, *b_email, *b_online, *b_register;
	void call_internet(const char *s);
public:
	Menu_help();
	virtual void init();
	virtual void step();
};

class Menu_register: public Menu_quit {
	Zone *b_quit, *b_ok, *z_invalid;
	Zone_text_input *z_pass;
	char name[64];
	char pass[64];
public:
	Menu_register(Inter *in, const Palette &p);
	virtual void step();
};

class Menu_option: public Menu_standard {
	Bitmap *bit;
	Font *font2;
	Zone *b_quit;
	int old_language,old_music,old_mouse_speed,old_port;
	char old_server[256];
public:
	Menu_option();
	virtual ~Menu_option();
	virtual void step();
};

class Zone_color_select;
class Zone_set_key;

class Menu_setup: public Menu_standard {
	Zone *b_quit, *b_player, *b_all_key;
	Zone_text_input *z_nameinput;
	Zone_text_input *z_passwdinput;
	Zone_text_input *z_tnameinput;
	Zone_text_input *z_tpasswdinput;
	Zone_state_text *z_continuousdown;
	Zone_state_text *z_h_repeat;
	Zone_state_text *z_v_repeat;
	Zone_state_text *z_shadow;
	Zone_state_text *z_smooth;
	Zone_set_key* z_key[7];
	Zone* b_key[7];
	Bitmap *bit;
public:
	Menu_setup();
	virtual ~Menu_setup();
	virtual void step();
};

class Menu_setup_all_key: public Menu {
	Zone_set_key **key;
	Bitmap *bit;
	Zone_text *z_text;
	int quel;
public:
	Menu_setup_all_key(Inter *in, Zone_set_key *k[]);
	virtual void step();
};

class Menu_setup_key: public Menu {
	Zone_set_key *key;
public:
	Menu_setup_key(Inter *in, Zone_set_key *k, const char *t);
	Menu_setup_key(Inter *in, Zone_set_key *k);
	virtual void init();
	virtual void step();
};

class Menu_single: public Menu_standard {
	Zone* normal, *sprint, *cancel;
	Bitmap *bit;
	Font *font2;
public:
	Menu_single();
	virtual ~Menu_single();
	virtual void step();
};

class Menu_multi: public Menu_standard {
	Zone* local, *net_lan, *net_internet, *cancel;
	Bitmap *bit;
	Font *font2;
public:
	Menu_multi();
	virtual ~Menu_multi();
	virtual void step();
};

class Menu_multi_join: public Menu, Net_callable {
	friend class Menu_multi_refresh;
	friend class Menu_multi_internet;
	Zone *b_create, *b_refresh, *b_join, *cancel, *b_info, *b_book;
	Zone *b_refresh_internet;
	Zone_text_field *z_game_status, *z_game_end, *z_game_minimum, *z_game_level, *z_game_speed, *z_game_type;
	Zone_listbox *list_game;
	Zone_listbox *list_player;
	int selected_game;
	Bitmap *bit_;
	Font *font2_;
	bool local_net;
	char address[256];
	char game_end_text[256];
	int game_level_start;
	void refresh_player();
	void clear_game_info();
public:
	Menu_multi_join(Bitmap *bit, Font *font, Font *font2, const Palette& p, bool plocal);
	virtual ~Menu_multi_join();
	virtual void init();
	virtual void step();
	void addwatch();
	void removewatch();
	void refresh();
	virtual void net_call(Packet *p2);
	void join_game(char *nam, Dword adr, int port);
};

class Zone_input_address: public Zone_text_input {
	Menu_multi_join *parent;
public:
	Zone_input_address(Menu_multi_join *p, const Palette &pal, Inter* in, char* s, int mlen, int px, int py, int pw):
		Zone_text_input(in, pal, s, mlen, px, py, pw) {
		parent = p;
	}
	virtual void lost_focus(int cancel);
};

class Menu_multi_refresh: public Menu {
	Menu_multi_join *parent;
	Zone *cancel;
	void find_local_games();
	void find_internet_games();
	void resolve();
public:
	Menu_multi_refresh(Menu_multi_join *p);
	virtual void init();
	virtual void step();
};

class Qserv;

class Menu_multi_internet: public Menu {
	Menu_multi_join *parent;
	Zone *cancel;
	Qserv *request;
	Attack parse_attack(const char *s);
	void parsegames();
public:
	Menu_multi_internet(Menu_multi_join *p);
	virtual ~Menu_multi_internet();
	virtual void init();
	virtual void step();
};

class Menu_highscore: public Menu_standard, public Zone_list {
	Bitmap *bit;
	Font *font2, *courrier, *courrier2;
	Zone *b_quit, *b_again, *playdemo[MAX_SCORE], *playdemog[MAX_SCORE], *playlast;
	Zone_text_button *sync;
	Qserv *sync_request;
	char *file_name;
	int *play_again;
	int time_demo;
	bool show_playback;
	Zone_text_field *status;
	void play_demo(const char *st);
	void start_sync();
	void step_sync();
	void stop_sync();
	void refresh_global(int& y);
public:
	Menu_highscore(int hscore=-1, int *playagain=NULL, bool show_playb=false);
	virtual ~Menu_highscore();
	virtual void step();
};

class Menu_stat: public Menu_standard, public Notifyable, Zone_list {
	class Colonne {
	public:
		int width, page;
		CS::Stat_type quel_stat;
		char titre[32];
		Zone_text_button *z_titre;
		bool sort_me;
		Colonne();
		void set_titre(const char *s);
	};
	Array<Colonne*> col;
	Bitmap *bit;
	Font *font2;
	Zone *b_quit, *b_restart, *b_stop;
	Array<Zone_text_button *> b_page;
	Score score;
	Font *fcourrier[MAXTEAMS];
	int active_sort, active_page;
	const int c_start;
	const int c_gap;
	void display();
	void calculate_total(bool force_blit);
	void add_title(Colonne &col, int *px, Bitmap *bit);
	void set_sort(int quel);
	void change_page(int p);
	void init_columns(Bitmap *bit);
	void sort(int quel_stat, int new_order[], bool reversed=false);
public:
	Menu_stat();
	virtual ~Menu_stat();
	virtual void init();
	virtual void step();
	virtual void notify();
};

class Menu_multi_checkip: public Menu {
	Bitmap *bit_;
	Zone *cancel;
public:
	Menu_multi_checkip(Bitmap *bit, Font *font, Font *font2, const Palette& p);
	virtual void step();
};

class Menu_multi_book: public Menu {
	Bitmap *bit_;
	Font *font2_;
	Zone *cancel;
	char buf[10][256];
	Zone *b_connect[10];
	Zone_text_field *status;
	bool looking, connect_failed;
	const char* address;
public:
	Menu_multi_book(Bitmap *bit, Font *font, Font *font2, const Palette& p, const char *adr);
	virtual void init();
	virtual void step();
};

class Menu_internet: public Menu {
	const char *command;
public:
	Menu_internet(const char *c);
	virtual void init();
	virtual void step();
};

class Menu_startserver: public Menu_quit {
	Bitmap *bit;
public:
	Menu_startserver();
	virtual ~Menu_startserver();
	virtual void step();
};

class Menu_startconnect: public Menu_quit {
	Bitmap *bit;
	Module *module;
	Font *font2;
public:
	Menu_startconnect(const char *adr, bool rejoin);
	virtual ~Menu_startconnect();
	virtual void step();
};

#endif
