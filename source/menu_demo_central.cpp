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
#include "bitmap.h"
#include "dict.h"
#include "clock.h"
#include "res_compress.h"
#include "find_file.h"
#include "fonts.h"
#include "texte.h"
#include "zone.h"
#include "global.h"
#include "quadra.h"
#include "image_png.h"
#include "recording.h"
#include "game.h"
#include "multi_player.h"
#include "menu_demo_central.h"

RCSID("$Id$")

class Zone_change_dir: public Zone_text_input {
	Menu_demo_central *menu_demo_central;
public:
	Zone_change_dir(Inter *in, const Palette &p, char *s, int max, int x, int y, int w, Menu_demo_central *m);
	virtual void lost_focus(int cancel);
};

Zone_change_dir::Zone_change_dir(Inter *in, const Palette &p, char *s, int max, int x, int y, int w, Menu_demo_central *m):
	Zone_text_input(in, p, s, max, x, y, w) {
	menu_demo_central = m;
}

void Zone_change_dir::lost_focus(int cancel) {
	Zone_text_input::lost_focus(cancel);
	if(cancel == 0)
		menu_demo_central->reload();
}

Menu_demo_central::Listitem::Listitem(const char *n, Font *f):
	Listable(n, f) {
	file_date[0] = 0;
	file_size = 0;
}

Menu_demo_central::Listitem::~Listitem() {
}

Menu_demo_central::Player_infos::Player_infos(int pplayer) {
	name[0]=0;
	player=pplayer;
	team=255;
}

Menu_demo_central::Menu_demo_central() {
	{
		Res_doze res("multi.png");
		Png img(res);
		bit = new Bitmap(img);
		pal.load(img);
	}
	pal.set_size(256);
	set_fteam_color(pal);

	fcourrier[0] = new Font(*fonts.courrier, pal, 255,125,0);
	fcourrier[1] = new Font(*fonts.courrier, pal, 0,225,255);
	fcourrier[2] = new Font(*fonts.courrier, pal, 255,0,0);
	fcourrier[3] = new Font(*fonts.courrier, pal, 255,0,255);
	fcourrier[4] = new Font(*fonts.courrier, pal, 255,255,0);
	fcourrier[5] = new Font(*fonts.courrier, pal, 0,255,0);
	fcourrier[6] = new Font(*fonts.courrier, pal, 0,0,255);
	fcourrier[7] = new Font(*fonts.courrier, pal, 170,170,170);

	inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
	(void)new Zone_bitmap(inter, bit, 0, 0, true);
	(void)new Zone_text(inter, ST_DEMOCENTRAL, 20);
	(void)new Zone_text(fteam[7], inter, ST_CURRENTDIRECTORY, 10, 50);
	Find_file::get_current_directory(find_directory);
	z_dir = new Zone_change_dir(inter, pal, find_directory, 900, 170, 50, 460, this);
	z_list = new Zone_listbox2(inter, bit, fteam[4], &quel, 10, 90, 180, 300);
	z_status = new Zone_text_field(inter, "", 0, 460, 640);
	z_play = new Zone_text_button2(inter, bit, fteam[4], ST_STARTPLAYBACK, 30, 400);
	z_delete = new Zone_text_button2(inter, bit, fteam[4], ST_DELETEDEMO, 30, 430);
	cancel = new Zone_text_button2(inter, bit, fteam[4], ST_BACK, 560, 430);
	int y=90,ys=21;
	(void)new Zone_text(fteam[7], inter, ST_DEMONAME, 210, y);
	z_name = new Zone_text_field(inter, (char *) NULL, 335, y, 295, fteam[1]); y+=ys;
	(void)new Zone_text(fteam[7], inter, ST_DEMODATE, 210, y);
	z_date = new Zone_text_field(inter, (char *) NULL, 335, y, 295, fteam[1]); y+=ys;
	(void)new Zone_text(fteam[7], inter, ST_DEMOVERSION, 210, y);
	z_version = new Zone_text_field(inter, (char *) NULL, 335, y, 70, fteam[1]); y+=ys;
	(void)new Zone_text(fteam[7], inter, ST_DEMODURATION, 210, y);
	z_duration = new Zone_text_field(inter, (char *) NULL, 335, y, 295, fteam[1]); y+=ys;
	(void)new Zone_text(fteam[7], inter, ST_DEMOTYPE, 210, y);
	z_type = new Zone_text_field(inter, (char *) NULL, 335, y, 295, fteam[1]); y+=ys;
	(void)new Zone_text(fteam[7], inter, ST_DEMOEND, 210, y);
	z_end = new Zone_text_field(inter, (char *) NULL, 335, y, 295, fteam[1]); y+=ys;

	play = NULL;
	reload();
}

Menu_demo_central::~Menu_demo_central() {
	for(int i=0; i<MAXTEAMS; i++)
		delete fcourrier[i];
	if(play)
		delete play;
	pinfos.deleteall();
}

void Menu_demo_central::clear_detail() {
	s_date[0] = 0;
	s_name[0] = 0;
	s_version[0] = 0;
	s_duration[0] = 0;
	s_type[0] = 0;
	s_end[0] = 0;
	if(play) {
		delete play;
		play = NULL;
	}
	zone.deleteall();
	pinfos.deleteall();
	video->need_paint = 2;
}

void Menu_demo_central::refresh_detail() {
	z_date->set_val(s_date);
	z_name->set_val(s_name);
	z_version->set_val(s_version);
	z_duration->set_val(s_duration);
	z_type->set_val(s_type);
	z_end->set_val(s_end);
}

void Menu_demo_central::drive_playback(const char *n) {
	char temp[1024];
	snprintf(temp, sizeof(temp) - 1, "%s/%s", find_directory, n);
	Res_compress *res = new Res_compress(temp, RES_TRY);
	if(res->exist) {
		play = new Playback(res);
		if(play->completed || !play->valid) {
			sprintf(st, ST_DEMOBADFILE, temp);
			z_status->set_val(st);
			delete play;
			play = NULL;
		}
		else {
			z_status->set_val("");
			strcpy(s_version, "1.0.1");
			strcpy(s_name, "-");
			strcpy(s_duration, "-");
			strcpy(s_date, "-");
			if(play->packet_gameserver) {
				strcpy(s_type, ST_GAMETYPE1);
				if(play->packet_gameserver->survivor)
					strcpy(s_type, ST_GAMETYPE2);
				Attack_type nat=play->packet_gameserver->normal_attack.type;
				Attack_type cat=play->packet_gameserver->clean_attack.type;
				if(nat==ATTACK_NONE && cat==ATTACK_NONE)
					strcpy(s_type, ST_GAMETYPE4);
				if(nat==ATTACK_BLIND || nat==ATTACK_FULLBLIND)
					strcpy(s_type, ST_GAMETYPE5);
				if(play->packet_gameserver->hot_potato)
					strcpy(s_type, ST_GAMETYPE3);
				switch(play->packet_gameserver->game_end) {
					case END_NEVER:
						strcpy(s_end, ST_DEMONEVER);
						break;
					case END_FRAG:
						sprintf(s_end, ST_DEMOAFTERFRAG, play->packet_gameserver->game_end_value);
						break;
					case END_TIME:
						sprintf(s_end, ST_DEMOAFTERMINUTE, play->packet_gameserver->game_end_value/6000);
						if(play->single())
							strcpy(s_type, ST_GAMETYPE6);
						break;
					case END_POINTS:
						sprintf(s_end, ST_DEMOAFTERSCORE, play->packet_gameserver->game_end_value);
						break;
					case END_LINES:
						sprintf(s_end, ST_DEMOAFTERLINES, play->packet_gameserver->game_end_value);
						break;
					default:
						strcpy(s_end, ST_DEMOUNKNOWN);
						break;
				}
			}
			if(play->single())
				strcpy(s_type, ST_DEMOSINGLE);
			populate_dict(play->sum);
		}
	}
	else {
		sprintf(st, ST_DEMOBADFILE, temp);
		z_status->set_val(st);
	}
	delete res;
}

void Menu_demo_central::populate_dict(Dict *d) {
	if(!d)
		return;
	const char *temp;
	temp = d->find("name");
	if(temp)
		strcpy(s_name, temp);
	temp = d->find("quadra_version");
	if(temp)
		strcpy(s_version, temp);
	temp = d->find("duration");
	if(temp) {
		int dur = atoi(temp);
		int min=dur/6000;
		int sec=(dur%6000)/100;
		if(min)
			sprintf(s_duration, ST_BOBMINUTES, min);
		else
			s_duration[0]=0;
		if(sec) {
			char st[100];
			sprintf(st, ST_BOBSECONDS, sec);
			if(min)
				strcat(s_duration, " ");
			strcat(s_duration, st);
		}
	}
	temp = d->find("time");
	if(temp) {
		int dur = atoi(temp);
		temp = Clock::time2char(dur);
		if(temp)
			strcpy(s_date, temp);
	}
	int cx[5], cw[5];
	cx[0] = 210;
	cw[0] = 120;
	cx[1] = cx[0] + cw[0] + 5;
	cw[1] = 60;
	cx[2] = cx[1] + cw[1] + 5;
	cw[2] = 60;
	cx[3] = cx[2] + cw[2] + 5;
	cw[3] = 90;
	cx[4] = cx[3] + cw[3] + 5;
	cw[4] = 80;
	int y=220, ys=21, i, j;
	zone.add(new Zone_text(fteam[7], inter, ST_PLAYERS, cx[0], y));
	if(!play->single()) {
		zone.add(new Zone_text(fteam[7], inter, ST_RESULTFRAG, cx[1], y));
		zone.add(new Zone_text(fteam[7], inter, ST_RESULTDEATH, cx[2], y));
	}
	zone.add(new Zone_text(fteam[7], inter, ST_SCORE, cx[3], y));
	zone.add(new Zone_text(fteam[7], inter, ST_LINES, cx[4], y)); y+=ys;
	pinfos.deleteall();
	Dict *players = d->find_sub("players");
	if(players && players->size()<=MAXPLAYERS) {
		for(i = 0; i < (int)players->size(); i++) {
			const Dict *d2 = players->get_sub(i);
			Player_infos *pi=new Player_infos(i);
			pinfos.add(pi);
			const char *name = d2->find("name");
			if(name)
				strcpy(pi->name, name);
			temp = d2->find("team");
			if(temp)
				pi->team = atoi(temp);
		}
		//Reset score with freshly constructed Score object
		Score score2;
		score=score2;
		score.updateFromDict(players);
		Byte team_pos, team;
		for(team_pos=0; team_pos<MAXTEAMS; team_pos++) {
			team=score.team_order[team_pos];
			for(j=0; j<MAXPLAYERS; j++) {
				for(i=0; i<MAXPLAYERS; i++)
					if(score.order[j]==i && score.player_team[i]==team)
						break;
				if(i<pinfos.size()) {
					Player_infos *pi=pinfos[i];
					Font *f=inter->font;
					if(pi->team<=6)
						f=fteam[pi->team];
					zone.add(new Zone_text(f, inter, pi->name, cx[0], y));
					int *statp;
					if(!play->single()) {
						statp=score.stats[i].stats[CS::FRAG].get_address();
						zone.add(new Zone_text_field(inter, statp, cx[1], y, cw[1], fcourrier[pi->team], false));
						statp=score.stats[i].stats[CS::DEATH].get_address();
						zone.add(new Zone_text_field(inter, statp, cx[2], y, cw[2], fcourrier[pi->team], false));
					}
					statp=score.stats[i].stats[CS::SCORE].get_address();
					zone.add(new Zone_text_field(inter, statp, cx[3], y, cw[3], fcourrier[pi->team], false));
					statp=score.stats[i].stats[CS::LINESTOT].get_address();
					zone.add(new Zone_text_field(inter, statp, cx[4], y, cw[4], fcourrier[pi->team], false));
					y+=ys;
				}
			}
		}
	}
}

void Menu_demo_central::step() {
	Menu_standard::step();
	if(!result)
		return;
	if(result == cancel)
		quit=true;
	if(z_list->in_listbox(result)) {
		Listitem *e = (Listitem *) z_list->get_selected();
		if(e && !e->isfolder) {
			clear_detail();
			drive_playback(e->list_name);
			refresh_detail();
		} else {
			clear_detail();
		}
	} 
	Zone *dbl = inter->double_clicked;
	if(dbl && z_list->in_listbox(dbl)) {
		Listitem *e = (Listitem *) z_list->get_selected();
		if(e && e->isfolder) {
			if(!strcmp(e->list_name, "..")) {
				// traite le .. differemment
				char *t = strrchr(find_directory, '/');
				if(t)
					*t = 0;
			}
			else {
				strcat(find_directory, "/");
				strcat(find_directory, e->list_name);
			}
			z_dir->set_val(find_directory);
			reload();
		}
	}
	if((dbl && z_list->in_listbox(dbl)) || result == z_play) {
		Listitem *e = (Listitem *) z_list->get_selected();
		if(e && !e->isfolder) {
			if(!play) {
				clear_detail();
				drive_playback(e->list_name);
				refresh_detail();
			}
			if(play) {
				play->shit_skipper2000(false);
				call(new Fade_in(pal));
				call(new Call_setfont(pal, new Demo_multi_player(play)));
				call(new Fade_out(pal));
				// le 'delete play' est fait par ~Demo_multi_player
				play = NULL;
			}
		}
	}
	if(result == z_delete) {
		Listitem *e = (Listitem *) z_list->get_selected();
		if(e && !e->isfolder) {
			char temp[1024];
			snprintf(temp, sizeof(temp) - 1, "%s/%s", find_directory, e->list_name);
			remove(temp);
			z_list->remove_item(e);
		}
	}
}

void Menu_demo_central::reload() {
	quel = -1;
	#ifdef UGS_DIRECTX
		do {
			char *t = strchr(find_directory, '\\');
			if(t)
				*t = '/';
			else
				break;
		} while(1);
	#endif

	z_list->clear();
	clear_detail();
	refresh_detail();
	z_status->set_val("");

	//Remove slashes from the end
	char *temp=find_directory+strlen(find_directory);
	while(temp>=find_directory) {
		if(*temp == '/')
			*temp=0;
		else
			break;
		temp--;
	}
	z_dir->set_val(find_directory);
	char temp_search[1024];
	snprintf(temp_search, sizeof(temp_search) - 1, "%s/*", find_directory);

	msgbox("Menu_demo_central::find_all: Finding directories in [%s]...\n", temp_search);
	{
		Find_file *find_file = Find_file::New(temp_search);
		z_list->init_sort();
		while(!find_file->eof()) {
			Find_file_entry ff = find_file->get_next_entry();
			if(ff.is_folder) {
				if(ff.name[0] == '.' && ff.name[1] == 0) // ignore le repertoire "."
					continue;
				Listitem *e = new Listitem(ff.name, fteam[1]);
				e->isfolder = true;
				z_list->add_sort(e);
			}
		}
		z_list->end_sort();
		delete find_file;
	}

	snprintf(temp_search, sizeof(temp_search) - 1, "%s/*.rec", find_directory);

	msgbox("Menu_demo_central::find_all: Finding files in [%s]...\n", temp_search);
	{
		Find_file *find_file = Find_file::New(temp_search);
		z_list->init_sort();
		while(!find_file->eof()) {
			Find_file_entry ff = find_file->get_next_entry();
			if(!ff.is_folder) {
				Listitem *e = new Listitem(ff.name, fteam[5]);
				e->file_size = ff.size;
				strcpy(e->file_date, ff.date);
				e->isfolder = false;
				z_list->add_sort(e);
			}
		}
		z_list->end_sort();
		delete find_file;
	}
}
