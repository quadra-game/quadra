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

#ifndef _HEADER_PANE
#define _HEADER_PANE

#include "inter.h"
#include "listbox.h"
#include "overmind.h"
#include "video.h"
#include "zone_list.h"
#include "net_stuff.h"
#include "score.h"
#include "notify.h"

class Multi_player;
class Pane_option;

class Pane_info {
public:
	Font *font2;
	Inter *inter;
	int x, y, w, h;
	Multi_player *mp;
	Bitmap *back, *fond, *back_bottom;
	Byte quel_pane;
	Pane_info(Bitmap *bit, Font *f2, Inter *in, int j, Multi_player *pmp);
	virtual ~Pane_info();
};

class Pane: public Zone, public Module, public Zone_list {
	friend class Multi_player;
protected:
	Video_bitmap *screen;
	bool hiden;
	Zone *clicked;
	const Pane_info &pi;
	bool draw_background,draw_bottom;
	void hidecall(Module *m);
	void hideexec(Module *m);
	void set_net_pane(int i);
	void ifdone();
public:
	Pane(const Pane_info &p, bool dback=true, bool dbottom=false);
	virtual ~Pane();
	void hide();
	void show();
	virtual void hide_item();
	virtual void show_item();
	virtual void step();
	virtual void draw();
};

class Pane_option: public Pane, public Notifyable {
public:
	Zone *player_info, *block_info, *combo_info, *select_scheme;
	Zone *chat_window, *server, *quit;
	Zone_text_button *player[3];
	Pane_option(const Pane_info &p);
	virtual ~Pane_option();
	virtual void init();
	virtual void step();
	virtual void notify();
};

class Pane_singleplayer: public Pane {
public:
	Zone *player[3];
	Pane_singleplayer(const Pane_info &p);
	virtual void step();
};

class Pane_close: public Pane {
private:
	Dword seconds;
	Zone *clock;
	static bool global_clock_visible;
	bool clock_visible;
	void update_clock();
	void show_clock();
protected:
	Zone *close;
public:
	Pane_close(const Pane_info &p, bool dback=true, bool dbottom=false);
	virtual ~Pane_close();
	virtual void step();
	virtual void hide_item();
	virtual void show_item();
	void allow_clock();
};

class Pane_selectscheme: public Pane_close {
	Bitmap *bit;
	Palette *pal;
	Zone *level[10];
	Zone *debugscheme;
public:
	Pane_selectscheme(const Pane_info &p);
	virtual ~Pane_selectscheme();
	virtual void step();
};

class Pane_playerinfo: public Pane_close, public Notifyable {
	Zone_text *player[MAXPLAYERS], *show_button, *auto_button;
	int o_show_val;
	bool auto_watch;
	int show_quoi;
	int tagged[4];
	int total0[MAXTEAMS], total1[MAXTEAMS];
	void add_name(Canvas *c, int i, int x2, int y2);
	void add_total(int team, int x2, int y2);
	int quel_stat() const;
	void tag(int q);
	void clear_tag();
	void activate_auto_watch();
	void deactivate_auto_watch();
public:
	Pane_playerinfo(const Pane_info &p);
	virtual ~Pane_playerinfo();
	void refresh();
	virtual void notify();
	virtual void process();
	virtual void step();
	virtual void draw();
	void auto_watch_closed();
	bool auto_watch_started();
};

class Pane_server: public Pane_close {
	class Zone_update_rate: public Zone_text_input {
		char port_st[3];
	public:
		Zone_update_rate(Inter* in, const Palette &pal, int px, int py, int pw);
		void lost_focus(int cancel);
	};

	Zone *drop_player, *drop_connection, *accept_player, *accept_connection;
	Zone *test_ping, *check_ip;
public:
	Pane_server(const Pane_info &p);
	virtual ~Pane_server();
	virtual void step();
};

class Pane_server_drop_player: public Pane_close, public Notifyable {
	class List_player: public Listable {
	public:
		Byte player;
		List_player(const char *s, Byte p, Font *f): Listable(s, f) {
			player = p;
		}
	};
	Zone_listbox *list_player;
	Zone *b_drop;
	int selected_player;
public:
	Pane_server_drop_player(const Pane_info &p);
	virtual ~Pane_server_drop_player();
	virtual void step();
	virtual void notify();
};

class Pane_server_drop_connection: public Pane_close, public Notifyable {
	class List_connection: public Listable {
	public:
		Net_connection *c;
		List_connection(const char *s, Net_connection *p, Font *f=NULL): Listable(s, f) {
			c = p;
		}
	};
	Zone_listbox *list_connection;
	Zone *b_drop;
	int selected;
public:
	Pane_server_drop_connection(const Pane_info &p);
	virtual ~Pane_server_drop_connection();
	virtual void step();
	virtual void notify();
};

class Pane_server_ping: public Pane_close, Net_callable {
	void send_test();
	Dword last_frame;
	int pingtime;
	int moyenne, total, nombre;
	int test_delay;
public:
	Pane_server_ping(const Pane_info &p);
	virtual ~Pane_server_ping();
	virtual void step();
	virtual void net_call(Packet *p2);
};

class Pane_server_ip: public Pane_close {
public:
	Pane_server_ip(const Pane_info &p);
};

class Chat_interface: public Zone, Zone_list, public Notifyable {
	class Zone_chat_input: public Zone_text_input {
		Chat_interface *parent;
	public:
		Zone_chat_input(Chat_interface *p, const Palette &pal, Inter* in, char* s, int mlen, int px, int py, int pw);
		virtual void lost_focus(int cancel);
	};
	friend class Zone_chat_input;

	class Zone_to_team: public Zone_state_text {
	public:
		Zone_to_team(Inter *in, int *val, int px, int py);
		virtual void clicked(int quel);
	};

	Zone_chat_input *zinput;
	char buf[256];
	bool delete_screen;
	int y_offset;
	Video_bitmap *screen;
	Bitmap *back;
	Zone_state_text *z_from;
public:
	Chat_interface(Inter *in, const Palette &pal, Bitmap *bit, int px, int py, int pw, int ph, Video_bitmap *scr=NULL);
	virtual ~Chat_interface();
	virtual void draw();
	virtual void process();
	virtual void notify();
	void set_screen_offset(int o, Video_bitmap *vb);
};

class Pane_scoreboard: public Pane_close, public Notifyable {
	Zone_text_button *b_show_frag;
	Array<Zone *> zlist_frag;
	bool show_frag;
	Score score;
	Byte potato_team;
	Word old_size;
protected:
	Word size;
	virtual void activate_frag();
	virtual void deactivate_frag(bool temp);
	void scoreboard_invisible();
public:
	Pane_scoreboard(const Pane_info &p, bool control_button, bool dback=true, bool dbottom=false);
	virtual ~Pane_scoreboard();
	virtual void step();
	virtual void notify();
	virtual void process();
};

class Pane_chat: public Pane_scoreboard {
	Chat_interface *chat;
	Zone *b_quit;
	Word old_y;
	virtual void activate_frag();
	virtual void deactivate_frag(bool temp);
public:
	Pane_chat(const Pane_info &p);
	virtual ~Pane_chat();
	virtual void step();
};

class Bloc;

class Pane_blockinfo: public Pane_close, public Notifyable {
	Bloc *bloc[7];
	int px;
	void block_info(Canvas *can, int dx);
	int gauche, droite, old_gauche, old_droite;
public:
	Pane_blockinfo(const Pane_info &p);
	virtual ~Pane_blockinfo();
	virtual void draw();
	virtual void step();
	virtual void notify();
	void add_info();
};

class Pane_comboinfo: public Pane_close, public Notifyable {
	void combo_info(Canvas *can, int dx);
	int gauche, droite, old_gauche, old_droite;
public:
	Pane_comboinfo(const Pane_info &p);
	virtual ~Pane_comboinfo();
	virtual void step();
	virtual void notify();
	void add_info();
};

class Pane_playerstartup: public Pane_close, public Notifyable {
	Zone_listbox *list_player;
	Zone_state_text *list_team;
	Zone_state_text *list_handicap;
	Zone *b_start, *color_team;
	int qplayer;
	int color;
	int handicap;
public:
	Pane_playerstartup(const Pane_info &p, int q);
	virtual ~Pane_playerstartup();
	virtual void notify();
	virtual void step();
	void update_player();
};

class Pane_playerjoin: public Pane_close, public Net_callable {
	bool got_answer;
	int qplayer;
	Exec_ping *eping;
	Zone_text *status;
public:
	Pane_playerjoin(const Pane_info &p, int q);
	virtual ~Pane_playerjoin();
	virtual void step();
	virtual void net_call(Packet *p2);
};

class Pane_startgame: public Pane_close, public Notifyable {
	int qplayer;
	int num_player;
	bool delete_zone;
	void create_zone();
public:
	Canvas *canvas;
	Pane_startgame(const Pane_info &p, int q, Canvas *c=NULL, int pos=-1);
	virtual ~Pane_startgame();
	virtual void step();
	virtual void notify();
};

class Pane_pre_start: public Pane {
	int qplayer;
public:
	Pane_pre_start(const Pane_info &p, int q);
	virtual ~Pane_pre_start();
	virtual void init();
};

class Watch_canvas: public Zone_list {
	bool small_watch;
public:
	Canvas *c;
	int play, x, y;
	Watch_canvas(int player, bool s=false);
	void stop();
	void start();
	void small_canvas(const Pane_info &pi, int tx, int ty);
};

class Pane_startwatch: public Pane_startgame {
	Watch_canvas *watch;
	Pane_playerinfo *pinfo;
public:
	bool auto_watch;
	Pane_startwatch(const Pane_info &p, int player, Pane_playerinfo *ppinfo);
	virtual ~Pane_startwatch();
	virtual void step();
	virtual void notify();
};

class Pane_smallwatch: public Pane_scoreboard {
	Watch_canvas *watch[4];
	int compte;
	Pane_playerinfo *pinfo;
public:
	bool auto_watch;
	Pane_smallwatch(const Pane_info &p, int tagged[], Pane_playerinfo *ppinfo);
	virtual ~Pane_smallwatch();
	virtual void notify();
	virtual void draw();
};

#endif
