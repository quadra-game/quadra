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

#ifndef _HEADER_QUADRA
#define _HEADER_QUADRA

#include <stdlib.h>
#include "overmind.h"
#include "inter.h"
#include "notify.h"
#include "sprite.h"
#include "color.h"
#include "net_call.h"
#include "zone_list.h"

class Canvas;
class Sample;

extern Inter* ecran;
extern Overmind overmind;

extern Color* color[];
extern Font *fteam[];

extern bool video_is_dumb;

void raw_draw_bloc(const Video_bitmap* bit, int x, int y, Byte side, Color* col);
void raw_draw_bloc_corner(const Video_bitmap* bit, int x, int y, Byte side, Color* col, Byte to[4]);
void raw_small_draw_bloc(const Video_bitmap* bit, int x, int y, Byte side, Color* col);
void set_fteam_color(const Palette& pal);

class Zone_combo;
class Zone_list;

class Player_base: public Module {
protected:
	Canvas *canvas;
	bool move_left();
	bool move_right();
	bool rotate_left();
	bool rotate_right(bool twice=false);
	void move_down();
	void drop_down();
	Byte calc_by(int py) const;
	bool check_gone();
	void check_state();
	void remove_bonus();
	void check_bonus();
	void play_sound(Sample *s, int vol, int pan, int freq);
public:
	Player_base(Canvas *c) {
		canvas = c;
	}
	virtual void step();
};

class Player_normal: public Player_base {
public:
	Player_normal(Canvas *c);
	virtual void step();
};

class Player_get_next: public Player_base {
	void shift_next();
public:
	Player_get_next(Canvas *c): Player_base(c) { }
	virtual void step();
};

class Player_process_key: public Player_base {
	int hold_left, hold_right;
	Dword last_video_frame, last_overmind_frame;
	void keyboard_control();
	void playback_control();
	bool check_first_frag();
	Dword block_rotated;
	Dword time_held;
public:
	Player_process_key(Canvas *c);
	virtual void init();
	virtual void step();
};

class Player_check_line: public Player_base {
	int check_nb_line();
	void check_clean();
	bool hole_pos[36][18];
public:
	Player_check_line(Canvas *c);
	virtual void step();
};

class Player_text_scroll: public Player_base, public Notifyable {
protected:
	Zone_combo *combo;
	void stop();
public:
	Player_text_scroll(Canvas *c, const char *texte, int xoffset=4, int yoffset=0);
	virtual ~Player_text_scroll();
	virtual void step();
	virtual void notify();
};

class Player_add_bonus: public Player_base {
	int nb;
	int anim;
	bool first_done;
public:
	Player_add_bonus(Canvas *c);
	virtual ~Player_add_bonus();
	virtual void step();
};

class Player_flash_lines: public Player_base {
	int anim;
public:
	Player_flash_lines(Canvas *c);
	virtual void step();
};

class Player_check_link: public Player_base {
	int anim;
	void fill_bloc(Byte x, Byte y);
	int tombe;
public:
	Player_check_link(Canvas *c);
	virtual void step();
};

class Player_init: public Player_base, public Net_callable {
public:
	Player_init(Canvas *canvas);
	virtual void step();
	virtual void net_call(Packet *p2);
};

class Player_change_level: public Player_base {
public:
	Player_change_level(Canvas *c);
	virtual ~Player_change_level();
	virtual void init();
};

class Player_level_up: public Player_base {
public:
	Player_level_up(Canvas *c);
	virtual void init();
	virtual void step();
};

class Player_dead_wait: public Player_base {
	bool add_bonus;
	bool check_first_frag();
public:
	Player_dead_wait(Canvas *c, bool ab=true);
	virtual void init();
	virtual void step();
};

class Player_first_frag: public Player_base {
	int i,j,c;
	Byte couleur;
public:
	Player_first_frag(Canvas *c);
	virtual void step();
};

class Player_dead: public Player_base {
	int i,j,c;
	Byte couleur;
	bool then_gone;
public:
	Player_dead(Canvas *c, bool tg=false);
	virtual void step();
};

class Packet_moves;

class Player_wait_block: public Player_base {
	bool check_first_frag();
	Dword move_index;
public:
	Player_wait_block(Canvas *c);
	virtual ~Player_wait_block();
	virtual void init();
	virtual void step();
};

class Packet_stampblock;

class Player_stamp: public Player_base {
	Player_add_bonus* addbonus;
	void stamp_bloc();
public:
	Player_stamp(Canvas *c, Packet_stampblock *p);
	virtual void init();
};

class Player_wait_respawn: public Player_base {
	bool add_bonus;
public:
	Player_wait_respawn(Canvas *c, bool ab=true);
	virtual void step();
};

class Player_gone: public Player_base {
public:
	Player_gone(Canvas *c, bool chat_msg);
	virtual void step();
};

#endif
