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

#include "inter.h"
#include "input.h"
#include "config.h"
#include "quadra.h"
#include "sons.h"
#include "texte.h"
#include "canvas.h"
#include "zone.h"

RCSID("$Id$")

Zone_next::~Zone_next() {
	delete back;
}

Zone_next::Zone_next(Inter* in, const Bitmap& fond, int px, int py, int pw, int ph):
 Zone(in, px, py, pw, ph) {
	next = NULL;
	back = new Bitmap(fond[y]+x, w, h, fond.realwidth);
}

void Zone_next::draw() {
	back->draw(video->vb, x, y);
	if(next) {
		next->draw(video->vb,x,y-18);
	}
}

void Zone_next::set_next(Bloc* n) {
	next = n;
	dirt();
}

Zone_small_next::Zone_small_next(Inter* in, const Bitmap& fond, int px, int py):
 Zone_next(in, fond, px, py, 4*6, 2*6) {
}

void Zone_small_next::draw() {
	back->draw(video->vb, x, y);
	if(next) {
		next->small_draw(video->vb,x,y-6);
	}
}

void Zone_canvas_bloc::draw() {
	canvas->setscreen();
	canvas->blit_back();
	canvas->blit_bloc(canvas->bloc_shadow);
	canvas->blit_bloc(canvas->bloc);
	if(canvas->color_flash)
		canvas->blit_flash();
	if(canvas->level_up)
		canvas->blit_level_up();
	dirt();  // this zone is always dirty
}

Zone_canvas::Zone_canvas(Inter* in, Bitmap& bit, int px, int py, Canvas *can, int pw, int ph, bool small_watch):
	Zone(in, px, py, pw, ph)
{
	fond = new Bitmap(bit[y]+x, w, h, bit.realwidth);
	screen = Video_bitmap::New(x, y, w, h);
	canvas = can;
	if(!small_watch) {
		znext = new Zone_small_next(in,bit,x+5,2);
		znext2 = new Zone_small_next(in,bit,x+32,8);
		znext3 = new Zone_next(in,bit,x+60,0);
		zbonus = new Zone_bonus(in, x+182, y, &canvas->bonus, canvas, bit);
	} else {
		znext = new Zone_small_next(in,bit,x,y-30);
		znext2 = new Zone_small_next(in,bit,x+24,y-25);
		znext3 = new Zone_small_next(in,bit,x+48,y-20);
		zbonus = new Zone_small_bonus(in, x+61, y, &canvas->bonus, canvas, bit);
	}
	canvas->set_canvas_pos(x, y, fond, screen, znext, znext2, znext3, inter);
}

Zone_canvas::~Zone_canvas() {
	if(zbonus)
		delete zbonus;
	if(znext3)
		delete znext3;
	if(znext2)
		delete znext2;
	if(znext)
		delete znext;
	delete screen;
	delete fond;
}

void Zone_canvas::draw() {
	int i,j;
  for(j = 0; j < 36; j++)
    for(i = 0; i < 14; i++)
      canvas->dirted[j][i]=2;
}

Zone_combo::Zone_combo(Zone_combo **z, Canvas *c, const char *text, int px, int py):
Zone_text(c->inter, text, px, py) {
	canvas = c;
	zone = z;
	*zone = this;
}

Zone_combo::~Zone_combo() {
	*zone = NULL;
}

void Zone_combo::draw() {
	if(canvas->inter && !canvas->small_watch) {
		Zone_text::draw();
		dirt();
		canvas->dirt_rect(x-canvas->x,y-canvas->y,x+w-canvas->x,y+h-canvas->y);
	}
}

Zone_bonus::Zone_bonus(Inter* in, int px, int py, int *v, Canvas *c, const Bitmap& bit, int pw, int ph):
Zone_watch_int(in, v, px, py, pw, ph) {
	canvas = c;
	back = new Bitmap(bit[y]+x, w, h, bit.realwidth);
}

Zone_bonus::~Zone_bonus() {
	delete back;
}

void Zone_bonus::draw() {
	int i;
	back->draw(video->vb,x,y);
	bool first_done=false;
	for(i=0; i<canvas->bonus; i++) {
		Byte side=5; //Left and right
		if(game->net_version()<23)
			side=15; //All sides
		else {
			if(!first_done) {
				side |= 8; //Bottom
				first_done=true;
			}
			if(canvas->bon[i].final) {
				side |= 2; //Top
				first_done=false;
			}
		}
		raw_draw_bloc(video->vb, x, (19-i)*18+y, side, color[canvas->bon[i].color]);
	}
}

Zone_color_select::Zone_color_select(Inter* in, int *pv, int px, int py, Byte co[MAXTEAMS]):
	Zone_state(in, pv, px, py, 50, 20, MAXTEAMS)
{
	for(int i=0; i<MAXTEAMS; i++)
		col[i] = co[i];
}

void Zone_color_select::draw() {
	video->vb->hline(y, x, w, 255);
	for(int i=1; i < h-1; i++)
		video->vb->hline(y+i, x, w, col[last_val]);
	video->vb->hline(y+h-1, x, w, 0);
	video->vb->vline(x, y, h, 255);
	video->vb->vline(x+w-1, y, h, 0);
}

Zone_color_select_noclick::Zone_color_select_noclick(Inter* in, int *pv, int px, int py, Byte co[MAXTEAMS]):
	Zone_color_select(in, pv, px, py, co) {
}

Zone_menu::Zone_menu(Inter* in, Bitmap* bit, int px, int py, Bitmap* bit2):
	Zone_bitmap(in, bit, px, py, bit2) {
	del_bit = 0; // delete pas les pointeurs Bitmap
	kb_focusable = true;
}

Zone_menu::Zone_menu(Inter* in, const char* b1, int px, int py, const char* b2):
	Zone_bitmap(in, Bitmap::loadPng(b1), px, py, Bitmap::loadPng(b2))
{
	del_bit = 1; // delete les 2 pointers
	kb_focusable = true;
}

Zone_menu::Zone_menu(Inter* in, const Image& b1, int px, int py, const Image& b2):
	Zone_bitmap(in, new Bitmap(b1), px, py, new Bitmap(b2))
{
	del_bit = 1; // delete les 2 pointers
	kb_focusable = true;
}

Zone_menu::Zone_menu(Inter* in, Bitmap* fond, const char* b1, int px, int py):
	Zone_bitmap(in, Bitmap::loadPng(b1), px, py)
{
	bit2_ = bit_;

	bit_ = new Bitmap((*fond)[py]+px, bit2_->width, bit2_->height, fond->realwidth);
	actual = bit_;
	del_bit = 1;
	kb_focusable = true;
}

Zone_menu::~Zone_menu() {
	if(del_bit) {
		delete bit_;
		delete bit2_;
	}
}

void Zone_menu::entered() {
	Sfx stmp(sons.point, 0, -2000, 0, 22000+ugs_random.rnd(2047));
	Zone_bitmap::entered();
}

void Zone_menu::clicked(int quel) {
	Sfx stmp(sons.click, 0, -1000, 0, 14000+ugs_random.rnd(511));
	Zone_bitmap::clicked(quel);
}

void Zone_listbox2::clicked(int quel) {
	Sfx stmp(sons.enter, 0, -800, 0, 28000+ugs_random.rnd(1023));
	Zone_listbox::clicked(quel);
}

void Zone_state_text2::clicked(int quel) {
	Sfx stmp(sons.enter, 0, -800, 0, 26000+ugs_random.rnd(1023));
	Zone_state_text::clicked(quel);
	notify_all();
}

void Zone_text_select2::entered() {
	Sfx stmp(sons.enter, 0, -1000, 0, 22000+ugs_random.rnd(1023));
	Zone_text_select::entered();
}

void Zone_text_select2::clicked(int quel) {
	Sfx stmp(sons.glass, 0, -200, 0, 14000+ugs_random.rnd(511));
	Zone_text_select::clicked(quel);
}

void Zone_text_button2::entered() {
	Sfx stmp(sons.enter, 0, -1000, 0, 22000+ugs_random.rnd(1023));
	Zone_text_button::entered();
}

void Zone_text_button2::clicked(int quel) {
	Sfx stmp(sons.glass, 0, -200, 0, 14000+ugs_random.rnd(511));
	Zone_text_button::clicked(quel);
}

Zone_set_key::Zone_set_key(Inter* in, int *pv, int px, int py):
	Zone_state_text(in, pv, px, py) {
	for(int i=0; i<256; i++) {
		if(keynames[i][0] == 0)
			add_string(ST_UNKNOWN);
		else
			add_string(keynames[i]);
	}
}

void Zone_small_canvas_bloc::draw() {
	canvas->setscreen();
	canvas->small_blit_back();
	canvas->small_blit_bloc(canvas->bloc);
	if(canvas->color_flash)
		canvas->small_blit_flash();
	dirt();  // this zone is always dirty
}

Zone_small_canvas::Zone_small_canvas(Inter* in, Bitmap& bit, int px, int py, Canvas *can):
	Zone_canvas(in, bit, px, py, can, 6*10, 6*20, true)
{
	canvas->small_watch = true; // override le setting par defaut
}

void Zone_small_canvas::draw() {
	int i,j;
  for(j = 0; j < 36; j++)
    for(i = 0; i < 14; i++)
      canvas->dirted[j][i]=2;
}

Zone_small_bonus::Zone_small_bonus(Inter* in, int px, int py, int *v, Canvas *c, const Bitmap& bit):
Zone_bonus(in, px, py, v, c, bit, 6, 6*20) {
}

void Zone_small_bonus::draw() {
	int i;
	back->draw(video->vb,x,y);
	bool first_done=false;
	for(i=0; i<canvas->bonus; i++) {
		Byte side=5; //Left and right
		if(game->net_version()<23)
			side=15; //All sides
		else {
			if(!first_done) {
				side |= 8; //Bottom
				first_done=true;
			}
			if(canvas->bon[i].final) {
				side |= 2; //Top
				first_done=false;
			}
		}
		raw_small_draw_bloc(video->vb, x, (19-i)*6+y, side, color[canvas->bon[i].color]);
	}
}
