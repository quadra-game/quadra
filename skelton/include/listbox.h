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

#ifndef HEADER_LISTBOX
#define HEADER_LISTBOX
#include "types.h"
#include "error.h"
#include "inter.h"

class Zone_listbox;

class Zone_listupdown: public Zone_text_select {
	friend class Zone_listup;
	friend class Zone_listdown;
	Zone_listbox *parent;
	int count;
public:
	Zone_listupdown(Zone_listbox *par, const char *s, int py);
	virtual void waiting();
	virtual void leaved();
	virtual void dirt();
};

class Zone_listup: public Zone_listupdown {
public:
	Zone_listup(Zone_listbox *par);
	virtual void clicked(int quel);
};

class Zone_listdown: public Zone_listupdown {
public:
	Zone_listdown(Zone_listbox *par);
	virtual void clicked(int quel);
};

class Listable {
public:
	char list_name[64];
	Font *font;
	Listable(const char *s, Font *f=NULL);
	virtual ~Listable() {
	}
	virtual bool is_equal(Listable *source);
};

class Zone_listtext: public Zone_text {
	Zone_listbox *parent;
	int quel;
	bool high;
public:
	Zone_listtext(Zone_listbox *par, int i);
	virtual void clicked(int quel);
	virtual void draw();
	virtual void dirt();
	virtual void entered();
	virtual void leaved();
};

class Zone_listbox: public Zone_watch_int {
	friend class Zone_listupdown;
	friend class Zone_listup;
	friend class Zone_listdown;
	friend class Zone_listtext;
	Bitmap *back;
	Zone_listup *zup;
	Zone_listdown *zdown;
	Font *font2;
	Array<Listable *> elements; // list of the list_box elements
	int first_item; // first displayed item in list_box
	Array<Zone_listtext *> list; // list of the displayed zone_text
	Array<Listable *> sort_list; // temporary list of elements to sort
	static int compare_sort(const void *arg1, const void *arg2);
	Video_bitmap *screen;
	bool selectable;
public:
	Zone_listbox(Inter* in, Bitmap *fond, Font *f, int *pval, int px, int py, int pw, int ph);
	virtual ~Zone_listbox();
	virtual void draw();
	virtual void dirt();
	virtual void enable();
	virtual void disable();
	virtual void process();
	void add_item(const char *s) {
		add_item(new Listable(s));
	}
	void add_item(Listable *e);
	void replace_item(int i, Listable *e);
	void remove_item(Listable *e);
	void remove_item(int i);
	Listable *get_selected();
	void clear();
	void sync_list();
	void unselect();
	void empty();
	void select(int q);
	int search(Listable *source);
	bool in_listbox(const Zone *z); // asks if 'z' is an element of the listbox
	void init_sort();
	void add_sort(Listable *l);
	void end_sort();
};

#endif
