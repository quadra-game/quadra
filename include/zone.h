/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_ZONE
#define _HEADER_ZONE

#include "array.h"
#include "types.h"
#include "global.h"
#include "notify.h"
#include "bitmap.h"
#include "zone_list.h"
#include "listbox.h"

class Canvas;
class Bloc;

class Zone_menu: public Zone_bitmap {
	int del_bit;
public:
	Zone_menu(Inter* in, Bitmap* bit, int px, int py, Bitmap* bit2);
	Zone_menu(Inter* in, const char* b1, int px, int py, const char* b2);
	Zone_menu(Inter* in, Bitmap* fond, const char* b1, int px, int py);
	Zone_menu(Inter* in, const Image& b1, int px, int py, const Image& b2);
	virtual ~Zone_menu();
	virtual void entered();
	virtual void clicked(int quel);
};

class Zone_text_select2: public Zone_text_select {
public:
	Zone_text_select2(Inter* in, Font* f, const char* s, int px, int py):
		Zone_text_select(in, f, s, px, py) { }
	Zone_text_select2(Inter* in, Font* f, const char* s, int px, int py, int pw):
		Zone_text_select(in, f, s, px, py, pw) { }
	virtual void entered();
	virtual void clicked(int quel);
};

class Zone_text_button2: public Zone_text_button {
public:
	Zone_text_button2(Inter* in, Bitmap *b, Font* f, const char* s, int px, int py):
		Zone_text_button(in, b, f, s, px, py) { }
	Zone_text_button2(Inter* in, Bitmap *b, Font* f, const char* s, int px, int py, int pw):
		Zone_text_button(in, b, f, s, px, py, pw) { }
	Zone_text_button2(Inter* in, Bitmap* b, Font* f, const char* s, int py):
		Zone_text_button(in, b, f, s, py) { }
	virtual void entered();
	virtual void clicked(int quel);
};

class Zone_listbox2: public Zone_listbox {
public:
	Zone_listbox2(Inter* in, Bitmap *fond, Font *f, int *pval, int px, int py, int pw, int ph):
		Zone_listbox(in, fond, f, pval, px, py, pw, ph) { }
	virtual void clicked(int quel);
};

class Zone_state_text2: public Zone_state_text, public Observable {
public:
	Zone_state_text2(Inter* in, int *pval, int px, int py, int pw=50, int ph=20):
		Zone_state_text(in, pval, px, py, pw, ph) { }
	virtual void clicked(int quel);
};

class Zone_color_select: public Zone_state {
	Byte col[MAXTEAMS];
public:
	Zone_color_select(Inter* in, int *pv, int px, int py, Byte co[MAXTEAMS]);
	virtual void draw();
};

class Zone_color_select_noclick: public Zone_color_select {
public:
	Zone_color_select_noclick(Inter* in, int *pv, int px, int py, Byte co[MAXTEAMS]);
	virtual void clicked(int quel) { }
};

class Zone_set_key: public Zone_state_text {
public:
	Zone_set_key(Inter* in, int *pv, int px, int py);
	virtual void clicked(int quel) { }
};

class Zone_next: public Zone {
protected:
	Bitmap* back;
	Bloc* next;
public:
	Zone_next(Inter* in, const Bitmap& fond, int px, int py, int pw=4*18, int ph=2*18);
	virtual ~Zone_next();
	void set_next(Bloc* n);
	virtual void draw();
};

class Zone_small_next: public Zone_next {
public:
	Zone_small_next(Inter* in, const Bitmap& fond, int px, int py);
	virtual void draw();
};

class Zone_combo: public Zone_text {
	Canvas *canvas;
	Zone_combo **zone;
public:
	Zone_combo(Zone_combo **z, Canvas *c, const char *text, int px, int py);
	virtual ~Zone_combo();
	virtual void draw();
};

class Zone_bonus: public Zone_watch_int {
protected:
	Canvas *canvas;
	Bitmap *back;
public:
	Zone_bonus(Inter* in, int px, int py, int *v, Canvas *c, const Bitmap& bit, int pw=18, int ph=18*20);
	virtual ~Zone_bonus();
	virtual void draw();
};

class Zone_canvas: public Zone {
protected:
	Bitmap *fond;
	Video_bitmap *screen;
	Canvas *canvas;
	Zone_next *znext, *znext2, *znext3;
	Zone *zbonus;
public:
	Zone_canvas(Inter* in, Bitmap& bit, int px, int py, Canvas *can, int pw=18*10, int ph=18*20, bool small_watch=false);
	virtual ~Zone_canvas();
	virtual void draw();
};

class Zone_canvas_bloc: public Zone {
protected:
	Canvas *canvas;
public:
	Zone_canvas_bloc(Inter* in, Canvas *can): Zone(in) {
		canvas = can;
	}
	virtual void draw();
};

class Zone_small_canvas: public Zone_canvas {
public:
	Zone_small_canvas(Inter* in, Bitmap& bit, int px, int py, Canvas *can);
	virtual void draw();
};

class Zone_small_canvas_bloc: public Zone_canvas_bloc {
public:
	Zone_small_canvas_bloc(Inter* in, Canvas *can): Zone_canvas_bloc(in, can) {
	}
	virtual void draw();
};

class Zone_small_bonus: public Zone_bonus {
public:
	Zone_small_bonus(Inter* in, int px, int py, int *v, Canvas *c, const Bitmap& bit);
	virtual void draw();
};

#endif
