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

#include "input.h"
#include "config.h"
#include "multi_player.h"
#include "game.h"
#include "net_stuff.h"
#include "chat_text.h"
#include "texte.h"
#include "global.h"
#include "menu_base.h"
#include "quadra.h"
#include "menu.h"
#include "canvas.h"
#include "main.h"
#include "nglog.h"
#include "clock.h"
#include "game_menu.h"

RCSID("$Id$")

int Create_game::game_end_y=0;

Create_game::Create_game(Bitmap *bit, Font *font, Font *font2, const Palette& p, bool pnet_game, bool plocal_net) {
	pal = p;
	bit_ = bit;
	inter->set_font(font, false);
	(void)new Zone_bitmap(inter, bit, 0, 0);
	net_game=pnet_game;
	local_net = plocal_net;

	int y=20, inc=30;
	Zone *z;

	(void)new Zone_text(inter, ST_CREATEGAMETITLE, y);
	y+=inc;
	y+=inc;

	strcpy(name, config.info.game_name);
	game_public = config.info.game_public;
	z=new Zone_text(fteam[7], inter, ST_ENTERGAMENAME, 20, y);
	z=new Zone_text_input(inter, pal, name, 32, 20+z->w+10, y, 320-10-20-z->w-10);
	int name_x=z->x;
	//This one is created here to line up stuff
	z=new Zone_text(fteam[7], inter, ST_GAMESPEED, 340, y+inc);
	int public_x=340+z->w+10;
	if(net_game) {
		if(!local_net) {
			(void)new Zone_text(fteam[7], inter, ST_ALLOWPUBLICGAME, 340, y);
			Zone_state_text *temp = new Zone_state_text2(inter, &game_public, public_x, y);
			temp->add_string(ST_NO);
			temp->add_string(ST_YES);
		}
	}
	y+=inc;

	z=new Zone_text(fteam[7], inter, ST_SELECTGAMETYPE, 20, y);
	selected = config.info.game_type;
	{
		Zone_state_text2 *temp = new Zone_state_text2(inter, &selected, name_x, y);
		temp->add_string(ST_GAMETYPE1);
		temp->add_string(ST_GAMETYPE2);
		temp->add_string(ST_GAMETYPE4);
		temp->add_string(ST_GAMETYPE5);
		temp->add_string(ST_GAMETYPE3);
		game_type = temp;
	}
	game_type->add_watch(this);

	level_up = config.info.level_up;
	{
		Zone_state_text2 *temp = new Zone_state_text2(inter, &level_up, public_x, y);
		temp->add_string(ST_GAMELEVELUP1);
		temp->add_string(ST_GAMELEVELUP2);
	}

	y+=inc;

	int i;
	for(i=0; i<9; i++) {
		game_desc[i] = new Zone_text(fteam[3], inter, "", 40, y);
		y+=inc/2;
	}
	game_desc[9]=NULL; //Only 9 lines displayed finally...
	y+=inc/2;
	game_descriptions=new Stringtable(ST_GAMETYPEDESCRIPTIONS);

	game_end = config.info.game_end;
	game_end_value = config.info.game_end_value;
	game_end_watch = NULL;
	game_end_selector = NULL;
	game_end_text = NULL;
	game_end_num = NULL;
	game_end_y=y;
	recreate_game_end();
	y+=inc;
	y+=inc;

	z=new Zone_text(fteam[7], inter, ST_RECORDGAME, 20, y);
	record_game = 0;
	Zone_state_text2 *temp = new Zone_state_text2(inter, &record_game, 20+z->w+10, y);
	temp->add_string(ST_NO);
	temp->add_string(ST_YES);
	record_watch = temp;
	record_watch->add_watch(this);
	record_zone = temp;
	strcpy(record_name, Clock::absolute_time());
	z_record_name = NULL;
	y+=inc;

	slog = 0;
	if(!Config::xtreme) {
		(void)new Zone_text(fteam[7], inter, ST_SLOG, 20, y);
		temp = new Zone_state_text2(inter, &slog, 20+z->w+10, y);
		temp->add_string(ST_NO);
		temp->add_string(ST_YES);
		slog_watch = temp;
		slog_watch->add_watch(this);
		slog_zone = temp;
		z_slog_name = NULL;
		strcpy(slog_name, Clock::absolute_time());
		z_slog_name = NULL;
		y+=inc;
	}
	else {
		slog_watch = NULL;
		slog_zone = NULL;
	}

	save = new Zone_text_button2(inter, bit, font2, ST_SAVESETTING, 20, 450);
	start = new Zone_text_button2(inter, bit, font2, ST_STARTGAME, 400, 450);
	cancel = new Zone_text_button2(inter, bit, font2, ST_BACK, 560, 450);

	notify();
}

Create_game::~Create_game() {
	if(game_end_watch)
		game_end_watch->remove_watch(this);
	if(game_type)
		game_type->remove_watch(this);
	if(record_watch)
		record_watch->remove_watch(this);
	if(slog_watch)
		slog_watch->remove_watch(this);
	if(game_descriptions)
		delete game_descriptions;
}

void Create_game::recreate_game_end() {
	static int old_selected=-1;
	//If changed to peace and not endnever, adjust for missing option
	if(selected==2 && old_selected!=2 && game_end)
		game_end--;
	//Same thing but the other way around
	if(selected!=2 && old_selected==2 && game_end)
		game_end++;
	old_selected=selected;
	if(game_end_watch)
		game_end_watch->remove_watch(this);
	if(game_end_selector)
		delete game_end_selector;
	int y=game_end_y;
	Zone *z;
	z=new Zone_text(fteam[7], inter, ST_SETGAMEEND, 20, y);
	Zone_state_text2 *temp = new Zone_state_text2(inter, &game_end, z->x+z->w+10, y);
	temp->add_string(ST_GAMEEND1);
	if(selected!=2) //endfrags only when !peace
		temp->add_string(ST_GAMEEND2);
	temp->add_string(ST_GAMEEND3);
	temp->add_string(ST_GAMEEND4);
	//temp->add_string(ST_GAMEEND5);
	game_end_selector = temp;
	game_end_watch = temp;
	game_end_watch->add_watch(this);
	z=temp;
	if(game_end_text)
		delete game_end_text;
	if(game_end_num)
		delete game_end_num;
	int num_x=z->x+z->w+10, text_x=num_x+50+10;
	switch(game_end+(selected==2 && game_end? 1:0)) {
		case 1:
			game_end_num = new Zone_input_numeric(inter, &game_end_value, 5, 1, 9999, pal, num_x, y, 50);
			game_end_text = new Zone_text(fteam[7], inter, ST_GAMEENDFRAG, text_x, y);
			break;
		case 2:
			game_end_num = new Zone_input_numeric(inter, &game_end_value, 5, 1, 9999, pal, num_x, y, 50);
			game_end_text = new Zone_text(fteam[7], inter, ST_GAMEENDMINUTE, text_x, y);
			break;
		case 3:
			game_end_num = new Zone_input_numeric(inter, &game_end_value, 5, 1, 99999, pal, num_x, y, 50);
			char st[64];
			sprintf(st, "x 1000 %s", ST_GAMEENDSCORE);
			game_end_text = new Zone_text(fteam[7], inter, st, text_x, y);
			break;
		case 4:
			game_end_num = new Zone_input_numeric(inter, &game_end_value, 5, 1, 99999, pal, num_x, y, 50);
			game_end_text = new Zone_text(fteam[7], inter, ST_GAMEENDLINES, text_x, y);
			break;
		default:
			game_end_text = NULL;
			game_end_num = NULL;
			break;
	}
}

void Create_game::notify() {
	video->need_paint = 2;
	int base=selected*10;
	int i;
	for(i=0; i<10; i++)
		if(game_desc[i])
			game_desc[i]->set_text(game_descriptions->get(base+i));
	recreate_game_end();
	if(record_watch) {
		if(z_record_name)
			delete z_record_name;
		if(record_game == 1) {
			int x=record_zone->x+record_zone->w+10;
			z_record_name = new Zone_text_input(inter, pal, record_name, 32, x, record_zone->y, 220);
		}
		else 
			z_record_name = NULL;
	}
	if(slog_watch) {
		if(z_slog_name)
			delete z_slog_name;
		if(slog == 1) {
			int x=slog_zone->x+slog_zone->w+10;
			z_slog_name = new Zone_text_input(inter, pal, slog_name, 32, x, slog_zone->y, 220);
		}
		else 
			z_slog_name = NULL;
	}
}

void Create_game::step() {
	Menu::step();
	if(input->quel_key == 1 || result==cancel || quitting) {
		input->quel_key = 0;
		ret();
	}
	if(result==start) {
		call(new Fade_in(pal));
		bool publi = false;
		if(net_game && !local_net) // force une game non-internet a "public=false" toujours
			publi = game_public? true:false;
		if(!name[0])
			strcpy(name, ST_GAMENONAME);
		Game_params p;
		p.name=name;
		switch(selected) {
			case 0: p.set_preset(PRESET_FFA); break;
			case 1: p.set_preset(PRESET_SURVIVOR); break;
			case 2: p.set_preset(PRESET_PEACE); break;
			case 3: p.set_preset(PRESET_BLIND); break;
			case 4: p.set_preset(PRESET_HOT_POTATO); break;
		}
		p.level_up=level_up? false:true;
		p.level_start=1;
		p.allow_handicap=true;
		p.game_end=(End_type) (game_end+(selected==2 && game_end? 1:0));
		p.game_end_value=game_end_value;
		p.game_public=publi;
		p.network=net_game;
		(void)new Game(&p);
		if(record_game == 1)
			game->prepare_recording(record_name);
		if(slog==1)
			game->prepare_logging(slog_name);
		call(new Create_game_start(pal, bit_, inter->font));
	}
	if(result==save)
		save_setting();
}

void Create_game::save_setting() {
	strcpy(config.info.game_name, name);
	config.info.game_type = selected;
	config.info.level_up = level_up;
	config.info.level_start = 1;
	config.info.combo_min = 0;
	config.info.game_end = game_end;
	config.info.game_end_value = game_end_value;
	config.info.game_public = game_public;
	config.write();
}

Create_game_start::Create_game_start(const Palette &pal_, Bitmap *bit, Font *font):
	pal(pal_) {
	bit_ = bit;
	font_ = font;
	sprintf(st, ST_GAMEBOBCREATED, game->name);
	message(-1, st, true, false, true);
}

Create_game_start::~Create_game_start() {
	Packet_bye p;
	net->sendtcp(&p);
	net->stop_client();
	net->stop_server();
	if(game) {
		delete game;
		if(chat_text)
			chat_text->clear();
	}
	msgbox("Create_game_start::~Create_game_start\n");
}

void Create_game_start::init() {
	char *tube;
	tube = net->failed();
	if(tube) {
		exec(new Menu_net_problem(tube, ST_CREATESERVERFAILED, bit_, font_));
		return;
	}

	game->loopback_connection=net->start_loopback_client();
	game->loopback_connection->joined=true;
	game->loopback_connection->trusted=true;
	ret();

	if(game->game_public) {
		call(new Create_game_end(pal, bit_, font_));
	}
	//We assume video_is_dumb==true means dedicated
	if(video_is_dumb) {
		call(new Menu_do_nothing());
	}
	else {
		call(new Call_setfont(pal, new Multi_player_launcher()));
		call(new Fade_out(pal));
	}
}

Create_game_end::Create_game_end(const Palette &pal_, Bitmap *bit, Font *font): pal(pal_) {
	inter->set_font(font, false);
	new Zone_bitmap(inter, bit, 0, 0);
	new Zone_text(fteam[7], inter, ST_UPDATINGGAMESERVER, 140);
	new Zone_text(fteam[7], inter, ST_ONEMOMENTPLEASE, 180);
	cancel = new Zone_text_button2(inter, bit, font, ST_CLICKTOCANCEL, 250);
}

Create_game_end::~Create_game_end() {
}

void Create_game_end::init() {
	if(!game->game_public) { // si partie pu publique, abort tout!
		ret();
		return;
	}
	net->stop_server(); //Disconnect all clients
	Menu::init();
	call(new Fade_in(pal));
	game->sendgameinfo(true);
}

void Create_game_end::step() {
	Menu::step();
	game->stepgameinfo();
	if(result==cancel || game->gameinfo_completed()) {
		exec(new Fade_out(pal));
	}
}

Join_game::Join_game(Bitmap *bit, Font *font, Font *font2, const Palette& p, const char *n, Dword sa, int sport, bool prejoin) {
	rejoin=prejoin;
	address = sa;
	port = sport;
	pal = p;
	bit_ = bit;
	font2_ = font2;
	inter->set_font(font, false);
	(void)new Zone_bitmap(inter, bit, 0, 0);
	(void)new Zone_text(inter, ST_JOINGAMETITLE, 20);
	if(n) {
		sprintf(st, ST_WAITJOINGAME, n);
	} else {
		char tube[256], tube2[256];
		Net::stringaddress(tube, sa);
		if(port) {
			sprintf(tube2, "%s:%i", tube, port);
			sprintf(st, ST_WAITJOINGAME3, tube2);
		} else {
			sprintf(st, ST_WAITJOINGAME3, tube);
		}
	}
	msgbox("Join_game::Join_game: address=%x, port=%i, n=[%s]\n", address, port, n? n:"NULL");
	status = new Zone_text(fteam[7], inter, st, 210);
	cancel = new Zone_text_button2(inter, bit, font2, ST_BACK, 560, 450);

	eping=NULL;
	delay = 0;
}

void Join_game::init() {
	Menu::init();
	if(!rejoin) {
		net->start_client(address, port);
		char *tube = net->failed();
		if(tube) {
			exec(new Menu_net_problem(tube, ST_JOINGAMEFAILED, bit_, inter->font));
		}
	}
}

void Join_game::step() {
	Menu::step();
	if(input->quel_key == 1 || result==cancel) {
		input->quel_key = 0;
		ret();
	}
	bool connect = net->connected();
	char *tube = net->failed();
	if(tube) {
		exec(new Menu_net_problem(tube, ST_JOINGAMEFAILED, bit_, inter->font));
		return;
	}
	if(connect && !eping) {
		status->set_text(ST_WAITJOINGAME2);
		video->need_paint = 2;
		eping=new Exec_ping(&pac, P_GAMESERVER, this);
	}
	if(eping && !connect) {
		status->set_text(ST_NOTQUADRASERVER);
		(void)new Zone_text(fteam[7], inter, ST_JOINREFUSED2, 240);
		video->need_paint = 2;
		delete eping;
		eping=NULL;
	}
	delay++;
	if(delay == 4000) {
		(void)new Zone_text(fteam[7], inter, ST_WAITJOINGAMEDELAY, 270);
	}
}

void Join_game::net_call(Packet *p2) {
	Packet_gameserver *p=(Packet_gameserver *) p2;
	bool ok = true;
	if(!p->accepted) {
		status->set_text(ST_JOINREFUSED);
		(void)new Zone_text(fteam[7], inter, ST_JOINREFUSED2, 240);
		video->need_paint = 2;
		ok = false;
	}
	//Hot potato and Peace games are created with net_version==22
	//  but we leave net_version at 20 to remain compatible
	//  with other game types, so here we allow joining
	//  games of net_version==22.
	if(p->version < 20 || (p->version > Config::net_version && p->version != 22)) {
		if(p->version < Config::net_version) {
			sprintf(st, ST_JOINOLDERVERSION, p->version);
			status->set_text(st);
			sprintf(st, ST_JOINOLDERVERSION2, Config::net_version);
			(void)new Zone_text(fteam[7], inter, st, 240);
		} else {
			sprintf(st, ST_JOINNEWERVERSION, p->version);
			status->set_text(st);
			(void)new Zone_text(fteam[7], inter, ST_JOINNEWERVERSION2, 240);
		}
		video->need_paint = 2;
		ok = false;
	}
	if(ok) {
		(void)new Game(p);
		sprintf(st,ST_GAMEBOBJOINED, game->name);
		message(-1, st);
		exec(new Join_download(bit_, inter->font, font2_, pal, rejoin));
	}
}

Join_game::~Join_game() {
	if(net->connected()) {
		Packet_bye p;
		net->sendtcp(&p);
		net->stop_client();
	}
	if(eping)
		delete eping;
	if(game) {
		delete game;
		if(chat_text)
			chat_text->clear();
	}
}

Join_download::Join_download(Bitmap *bit, Font *font, Font *font2, const Palette& p, bool prejoin) {
	rejoin=prejoin;
	font2_ = font2;
	pal = p;
	bit_ = bit;
	nb_total = game->net_list.size();
	nb_current = nb_percent = 0;
	inter->set_font(font, false);
	new Zone_bitmap(inter, bit, 0, 0);
	new Zone_text(inter, ST_JOINGAMETITLE, 120);
	new Zone_text(fteam[7], inter, ST_JOINDOWNLOADING, 210);
	new Zone_text(fteam[7], inter, "%", 340, 240);
	new Zone_text_field(inter, &nb_percent, 290, 240, 40);
}

Join_download::~Join_download() {
}

void Join_download::step() {
	if(!net->connected()) {
		exec(new Menu_net_problem(ST_DOWNLOADDECONNECT1, ST_DOWNLOADDECONNECT2, bit_, inter->font));
		return;
	}
	nb_current=0;
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=game->net_list.get(i);
		if(c && !c->wait_download)
			nb_current++;
	}
	if(nb_total)
		nb_percent = nb_current*100/nb_total;
	if(nb_current == nb_total) {
		nb_percent = 100;
		if(!rejoin) {
			exec(new Fade_in(pal));
			call(new Call_setfont(pal, new Multi_player_launcher()));
			call(new Fade_out(pal));
		}
		else {
			exec(new Call_setfont(pal, new Multi_player_launcher()));
			call(new Fade_out(pal));
		}
	}
}
