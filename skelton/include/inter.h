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

#ifndef HEADER_INTER
#define HEADER_INTER
#include "error.h"
#include "array.h"
#include "types.h"
#include "video.h"
#include "bitmap.h"
#include "sprite.h"

class Zone;
class Inter;

class Inter {
	Zone* in;
	int first_zone;

	static int last_mouse_x, last_mouse_y;
	static bool kb_visible;
	int kb_x, kb_y, kb_anim;
	bool kb_active;
	int double_click_delay;
	Zone *double_clicked_first;
	Array<int> kb_keys;
	Zone *kb_focus;
	Zone *kb_find_upmost();
	Zone *kb_find_downmost();
	Zone *kb_find_down();
	Zone *kb_find_up();
	Zone *kb_find_left();
	Zone *kb_find_right();
	Zone *kb_find_closest();
	Zone *kb_find_next();
	Zone *kb_find_prev();
	void kb_draw_focus();
	void de_tag(Zone *z);
	void tag(Zone *z);
	bool kb_check_key(const int i) const;
public:
	Font* font;
	bool del_font;
	Array<Zone *> zone;
	Zone* focus;
	Zone* clicked, *double_clicked;
	Inter();
	Inter(Inter *in);
	void set_font(Font* f1, bool del=true); // del=true if we must delete 'font'
	int nzone() const {
		return zone.size();
	}
	void add(Zone* zon, bool back=false) {
		if(back)
			zone.add_before(zon, 0);
		else
			zone.add(zon);
	}
	void remove(int i);
	void remove(Zone *z);
	Zone* do_frame();
	void dirt_all();
	void draw_zone();
	void flush();
	virtual ~Inter();
	void process();
	void select_zone(Zone *z, int quel); // selects a zone (gives focus and/or click)
	bool is_kb_visible() const {
		return kb_visible;
	}
	void kb_deactivate();
	void kb_reactivate();
	void kb_alloc_key(const int i);
	void kb_free_key(const int i);
};

class Zone {
	TRACKED;
public:
	Zone* child,* parent;
	int enabled;
	int dirty, focusable;
	int x, y, w, h;
	bool kb_focusable, stay_on_top;
	Inter* inter;
	Zone(Inter* in=NULL, int px=0, int py=0, int pw=0, int ph=0);
	virtual ~Zone();
	virtual void disable() {
		if(enabled == 0)
			video->need_paint = 2; // to ensure that this zone will disappear
		enabled--;
	}
	virtual void enable() {
		enabled++;
	}
	virtual void dirt() {
		dirty = 2;
	}
	virtual void draw() { } //called when the zone is dirty
	virtual int in() const;
	virtual void leaved();
	virtual void entered();
	virtual void waiting();
	virtual void clicked(int quel);
	virtual void double_clicked();
	virtual void process() { }
	virtual void lost_focus(int cancel) { }
	void set_child(Zone* chil);
};

class Zone_sprite: public Zone {
	Sprite *sp;
public:
	Zone_sprite(Inter *in, const char *nam, int px = -1, int py = -1);
	virtual ~Zone_sprite();
	virtual void draw();
};

class Zone_bitmap: public Zone {
	bool del_bit;
public:
	Bitmap* bit_, *bit2_, *actual;
	Zone_bitmap(Inter* in, Bitmap* bit, int px, int py, Bitmap* bit2=NULL);
	Zone_bitmap(Inter* in, Bitmap* bit, int px, int py, bool del);
	virtual ~Zone_bitmap();
	virtual void draw() {
		actual->draw(video->vb, x, y);
	}
	virtual void leaved() {
		if(bit2_) {
			actual = bit_;
			dirt();
		}
		Zone::leaved();
	}
	virtual void entered() {
		if(bit2_) {
			actual = bit2_;
			dirt();
		}
		Zone::entered();
	}
};

class Zone_watch_int: public Zone {
public:
	int *val, last_val;
	Zone_watch_int(Inter* in, int *pval, int px=0, int py=0, int pw=0, int ph=0):
	Zone(in, px, py, pw, ph) {
		val = pval;
		set_val(pval);
	}
	virtual void process();
	void set_val(int *pv);
};

class Zone_state: public Zone_watch_int {
public:
	int nstate;
	Zone_state(Inter* in, int *pval, int px, int py, int pw=0, int ph=0, int pnstate=1);
	virtual void clicked(int quel);
};

class Zone_state_bit: public Zone_state {
	Bitmap *state[3];
public:
	Zone_state_bit(Inter* in, const char* b1, int *pval, int px, int py, const char* b2=NULL, const char* b3=NULL);
	virtual ~Zone_state_bit() {
		for(int i=0; i < nstate; i++)
			delete state[i];
	}
	virtual void draw() {
		state[last_val]->draw(video->vb, x, y);
	}
};

class Zone_text: public Zone {
	friend class Zone_text_select;
protected:
	Font *font;
	bool lock_size;
	int text_x;
public:
	char st[256];
	Zone_text(Inter* in, const char* s, int px, int py);
	Zone_text(Font *f2, Inter* in, const char* s, int px, int py);
	Zone_text(Font *f2, Inter* in, const char* s, int py);
	Zone_text(Inter* in, const char* s, int px, int py, int pw);
	Zone_text(Inter* in, const char* s, int py);
	virtual void set_text(const char* s);
	virtual void set_font(Font* f);
	virtual void draw();
};

class Zone_text_select: public Zone_text {
protected:
	Font* font2;
	Font* actual;
public:
	Zone_text_select(Inter* in, Font* f, const char* s, int px, int py);
	Zone_text_select(Inter* in, Font* f, const char* s, int px, int py, int pw);
	virtual void draw();
	virtual void leaved();
	virtual void entered();
	virtual void set_font(Font* f);
};

class Zone_text_button: public Zone_text_select {
protected:
	Bitmap *bit;
	bool high;
	void set_bit(Bitmap *fond);
public:
	Zone_text_button(Inter* in, Bitmap* fond, Font* f, const char* s, int px, int py);
	Zone_text_button(Inter* in, Bitmap* fond, Font* f, const char* s, int py);
	Zone_text_button(Inter* in, Bitmap* fond, Font* f, const char* s, int px, int py, int pw);
	virtual ~Zone_text_button();
	virtual void draw();
	virtual void leaved();
	virtual void entered();
	virtual void set_text(const char* s);
};

class Zone_clear: public Zone {
	int color;
public:
	Zone_clear(Inter* in, int px=0, int py=0, int pw=video->width, int ph=video->height, int c=0);
	virtual void draw();
};

class Zone_panel: public Zone {
protected:
	bool draw_frame;
public:
	Video_bitmap* pan;
	bool high;
	Zone_panel(Inter* in, int px, int py, int pw, int ph);
	virtual ~Zone_panel() {
		delete pan;
	}
	virtual void draw();
	void resize();
};

class Zone_state_text: public Zone_state {
	Zone_panel* pan;
public:
	const char* state[256];
	Font* fonts[256];
	Zone_state_text(Inter* in, int *pval, int px, int py, int pw=50, int ph=20);
	virtual ~Zone_state_text() {
		delete pan;
	}
	void add_string(const char* s, Font *f=NULL);
	virtual void draw();
	virtual void leaved();
	virtual void entered();
};

class Zone_text_input: public Zone_panel {
protected:
	char st[1024];
	char* val;
	int focus, curpos, actual_len, maxlen, panx, maxwidth;
	void input_char(const Byte c);
	void check_clipboard();
	Font *font_selected;
	int select_start;
	Byte curcolor;
	bool first_click;
	void set_mouse_curpos();
	bool cut_selection();
public:
	Zone_text_input(Inter* in, const Palette& pal, char* s, int mlen, int px, int py, int pw, int mwidth = -1);
	virtual ~Zone_text_input();
	virtual void clicked(int quel);
	virtual void lost_focus(int cancel);
	void set_val(char* s);
	virtual void draw();
	virtual void process();
	virtual void leaved();
	virtual void entered();
	virtual void waiting();
};

class Zone_input_numeric: public Zone_text_input {
	char temp_st[64];
	int var_min, var_max, *num_var;
public:
	Zone_input_numeric(Inter* in, int *pvar, int ncar, int pmin, int pmax, const Palette &pal, int px, int py, int pw);
	void lost_focus(int cancel);
};

class Zone_text_field: public Zone_panel {
protected:
	char st[256];
	int val;
	int *var;
	Font *font;
public:
	Zone_text_field(Inter* in, int* s, int px, int py, int pw, Font *f2=NULL, bool frame=true);
	Zone_text_field(Inter* in, const char* s, int px, int py, int pw, Font *f2=NULL, bool frame=false);
	void set_val(int* s);
	void set_val(const char* s);
	virtual void draw();
	virtual void process();
};

class Zone_text_numeric: public Zone_text_field {
public:
	Zone_text_numeric(Inter* in, int* s, int px, int py, int pw):
	Zone_text_field(in, s, px, py, pw) {
		draw_frame = false;
	}
	Zone_text_numeric(Font *f2, Inter* in, int* s, int px, int py, int pw):
	Zone_text_field(in, s, px, py, pw, f2) {
		draw_frame = false;
	}
};

#endif
