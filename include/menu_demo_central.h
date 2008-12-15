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
#ifndef _HEADER_MENU_DEMO_CENTRAL
#define _HEADER_MENU_DEMO_CENTRAL

#include <vector>

#include "menu_base.h"
#include "zone_list.h"
#include "score.h"
#include "listbox.h"

class Zone;
class Bitmap;
class Zone_listbox;
class Zone_text_input;
class Zone_text;
class Res;
class Playback;
class Dict;

class Menu_demo_central: public Menu_standard, Zone_list {
	class Listitem: public Listable {
	public:
		bool isfolder;
		char file_date[1024];
		Dword file_size;
		Listitem(const char *n, Font *f);
		virtual ~Listitem();
	};
	class Player_infos {
	public:
		Byte team;
		Byte player;
		char name[40];
		Player_infos(int pplayer);
	};
	Score score;
	std::vector<Player_infos*> pinfos;
	Zone_text_field *z_status, *z_name, *z_date, *z_version, *z_duration, *z_type, *z_end;
	Zone *z_play, *z_delete;
	char s_date[1024], s_version[1024], s_duration[1024], s_type[1024], s_end[1024], s_name[1024];
	Playback *play;
	Font *fcourrier[MAXTEAMS];
	void clear_detail();
	void refresh_detail();
	void drive_playback(const char *n);
	void populate_dict(Dict *d);
public:
	Zone *cancel;
	Bitmap *bit;
	Zone_listbox *z_list;
	Zone_text_input *z_dir;
	int quel;
	char find_directory[1024];
	Menu_demo_central();
	virtual ~Menu_demo_central();
	virtual void step();
	void reload();
};

#endif
