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

#include "listbox.h"

#include <stdio.h>
#include <string.h>
#include "input.h"
#include "cursor.h"

Zone_listbox::Zone_listbox(Inter* in, Bitmap *fond, Font *f, int *pval, int px, int py, int pw, int ph):
	Zone_watch_int(in, pval, px, py, pw, ph) {
	if(fond) 
		back = new Bitmap((*fond)[py+1]+px+1, pw-2, ph-2, fond->realwidth);
	else
		back = NULL;
	screen = Video_bitmap::New(px+1, py+1, pw-2, ph-2);
	font2 = f;
	zup = new Zone_listup(this);
	zdown = new Zone_listdown(this);
	for(int i=y+18; i<y+h-18-f->height(); i+=f->height()) {
		list.add(new Zone_listtext(this, i));
	}
	first_item = 0;
	if(val)
		select(*val);
}

Zone_listbox::~Zone_listbox() {
	empty();
	while(list.size()) {
		delete list.last();
		list.removelast();
	}
	delete zdown;
	delete zup;
	if(back)
		delete back;
	delete screen;
}

void Zone_listbox::draw() {
	screen->setmem();
	if(back)
		back->draw(screen, 0, 0);
	video->vb->hline(y, x, w, 210);
	video->vb->hline(y+h-1, x, w, 210);
	video->vb->vline(x, y, h, 210);
	video->vb->vline(x+w-1, y, h, 210);
	//video->vb->hline(y+20, x, w, 210);
	//video->vb->hline(y+h-1-20, x, w, 210);
}

void Zone_listbox::dirt() {
	if(dirty != 2) {
		Zone_watch_int::dirt();
		zup->dirt();
		zdown->dirt();
		for(int i=0; i<list.size(); i++)
			list[i]->dirt();
	}
}

void Zone_listbox::enable() {
	Zone_watch_int::enable();
	zup->enable();
	zdown->enable();
	for(int i=0; i<list.size(); i++)
		list[i]->enable();
}

void Zone_listbox::disable() {
	Zone_watch_int::disable();
	zup->disable();
	zdown->disable();
	for(int i=0; i<list.size(); i++)
		list[i]->disable();
}

void Zone_listbox::init_sort() {
	sort_list.clear();
}

void Zone_listbox::add_sort(Listable *l) {
	sort_list.add(l);
}

void Zone_listbox::end_sort() {
	qsort((void *) &sort_list[0], sort_list.size(), sizeof(sort_list[0]), compare_sort);
	for(int i=0; i<sort_list.size(); i++)
		add_item(sort_list[i]);
	sort_list.clear();
}

int Zone_listbox::compare_sort(const void *arg1, const void *arg2) {
	char *s1 = (*(Listable **) arg1)->list_name;
	char *s2 = (*(Listable **) arg2)->list_name;
  return strcasecmp(s1, s2);
}


void Zone_listbox::add_item(Listable *e) {
	elements.add(e);
	sync_list();
}

void Zone_listbox::replace_item(int i, Listable *e) {
	delete elements[i];
	elements.replace(i, e);
	sync_list();
}

void Zone_listbox::remove_item(Listable *e) {
	int i;
	for(i=0; i<elements.size(); i++)
		if(elements[i]==e)
			break;
	if(i==elements.size())
		return;
	if(get_selected()==e)
		unselect();
	delete elements[i];
	elements.remove(i);
	sync_list();
}

void Zone_listbox::remove_item(int i) {
	delete elements[i];
	elements.remove(i);
	sync_list();
}

Listable *Zone_listbox::get_selected() {
	if(!val || *val == -1)
		return NULL;
	else
		return elements[*val];
}

void Zone_listbox::process() {
	Zone_watch_int::process();
	if(input) {
		if(cursor->x > x && cursor->x < x+w && cursor->y > y && cursor->y < y+h) {
			int z = input->mouse.dz;
			if(z > 0)
				zup->clicked(0);
			if(z < 0)
				zdown->clicked(0);
		}
	}
}

int Zone_listbox::search(Listable *source) {
	for(int i=0; i<elements.size(); i++)
		if(elements[i]->is_equal(source))
			return i;
	return -1;
}

bool Zone_listbox::in_listbox(const Zone *z) {
	for(int i=0; i<list.size(); i++)
		if(list[i] == z)
			return true;
	return false;
}

void Zone_listbox::sync_list() {
	for(int i=0; i<list.size(); i++) {
		Font *f = inter->font;
		list[i]->kb_focusable = false;
		if(i+first_item >= elements.size()) {
			list[i]->set_text("");
		} else {
			if(val)
				list[i]->kb_focusable = true;
			Listable *li = elements[i+first_item];
			list[i]->set_text(li->list_name);
			if(li->font)
				f = li->font;
		}
		list[i]->set_font(f);
	}
	if(val)
		select(*val);
	dirt();
}

void Zone_listbox::empty() {
	while(elements.size()) {
		delete elements.last();
		elements.removelast();
	}
}

void Zone_listbox::clear() {
	empty();
	first_item = 0;
	if(val)
		*val = -1;
	sync_list();
}

void Zone_listbox::unselect() {
	if(!val)
		return;
	if(*val >= first_item && *val < first_item+list.size()) {
		Font *f = inter->font;
		if(elements[*val]->font)
			f = elements[*val]->font;
		list[*val-first_item]->set_font(f);
	}
	*val = -1;
}

void Zone_listbox::select(int q) {
	if(!val)
		return;
	*val = q;
	if(*val >= first_item && *val < first_item+list.size()) {
		list[*val-first_item]->set_font(font2);
	}
}

Zone_listupdown::Zone_listupdown(Zone_listbox *par, const char *s, int py):
	Zone_text_select(par->inter, par->font2, s, par->x, py, par->w) {
	parent = par;
	leaved();
	kb_focusable = true;
}

void Zone_listupdown::waiting() {
	Zone_text_select::waiting();
	if(input->mouse.button[0] & PRESSED) {
		count--;
		if(count < 0) {
			clicked(0);
			count = 5;
		}
	}
}

void Zone_listupdown::leaved() {
	Zone_text_select::leaved();
	count = 40;
}

void Zone_listupdown::dirt() {
	Zone_text_select::dirt();
	parent->dirt();
}

Zone_listup::Zone_listup(Zone_listbox *par):
	Zone_listupdown(par, "·3", par->y) {
}

void Zone_listup::clicked(int quel) {
	if(parent->first_item > 0) {
		parent->first_item--;
		parent->sync_list();
		parent->clicked(quel);
	}
}

Zone_listdown::Zone_listdown(Zone_listbox *par):
	Zone_listupdown(par, "·4", par->y+par->h-18) {
}

void Zone_listdown::clicked(int quel) {
	if(parent->first_item < parent->elements.size() - parent->list.size()) {
		parent->first_item++;
		parent->sync_list();
		parent->clicked(quel);
	}
}

Listable::Listable(const char *s, Font *f) {
	strcpy(list_name, s);
	font = f;
}

bool Listable::is_equal(Listable *source) {
	return !strcmp(list_name, source->list_name);
}

Zone_listtext::Zone_listtext(Zone_listbox *par, int i):
	Zone_text(par->inter, "", par->x+2, i, par->w-4) {
	parent = par;
	quel = par->list.size();
	high = false;
}

void Zone_listtext::clicked(int quel) {
	//Watch out! Param 'quel' is mouse button; this->quel is...
	//  hmmm... something else... Ask Remz
	parent->unselect();
	if(this->quel < parent->elements.size()) {
		parent->select(this->quel + parent->first_item);
		//inter->clicked = parent; // eww!
		parent->clicked(quel);
	}
}

void Zone_listtext::draw() {
	parent->screen->setmem();
	font->draw(st, parent->screen, text_x-parent->x, y-parent->y);
	if(high) {
		if(!kb_focusable) 
			high=false;
		else
			video->vb->box(x, y, w, h, 255);
	}
}

void Zone_listtext::dirt() {
	Zone_text::dirt();
	parent->dirt();
}

void Zone_listtext::entered() {
	Zone_text::entered();
	if(parent->val && kb_focusable) {
        /* kb_focusable also indicates that this zone_listtext
           currently contains something */
		high=true;
		dirt();
	}
}

void Zone_listtext::leaved() {
	Zone_text::leaved();
	if(parent->val && kb_focusable) {
        /* kb_focusable also indicates that this zone_listtext
           currently contains something */
		high=false;
		dirt();
	}
}
