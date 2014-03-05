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

#include "quadra.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <exception>

#include "SDL.h"

#ifdef WIN32
#define _WIN32_IE 0x0500
#include <shlobj.h>
#else
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "attack.h"
#include "bloc.h"
#include "cfgfile.h"
#include "chat_text.h"
#include "clock.h"
#include "command.h"
#include "cursor.h"
#include "fonts.h"
#include "game.h"
#include "global.h"
#include "highscores.h"
#include "input.h"
#include "main.h"
#include "menu.h"
#include "multi_player.h"
#include "net_stuff.h"
#include "recording.h"
#include "res_compress.h"
#include "resmanager.h"
#include "sons.h"
#include "texte.h"
#include "update.h"

using std::max;
using std::min;
using std::string;

Color *color[9];
Font *fteam[8];

void set_fteam_color(const Palette& pal) {
	fteam[0]->colorize(pal, 255,125,0);
	fteam[1]->colorize(pal, 0,225,255);
	fteam[2]->colorize(pal, 255,0,0);
	fteam[3]->colorize(pal, 255,0,255);
	fteam[4]->colorize(pal, 255,255,0);
	fteam[5]->colorize(pal, 0,255,0);
	fteam[6]->colorize(pal, 40,40,255);
	fteam[7]->colorize(pal, 170,170,170);
}

void raw_draw_bloc_corner(const Video_bitmap* bit, int x, int y, uint8_t side, Color* col, uint8_t to[4]) {
	raw_draw_bloc(bit, x, y, side, col);
	if(!(side&1) && !(side&2) && to[0]&2 && to[1]&1) {
		bit->put_pel(x, y, col->shade(7));
		bit->put_pel(x+1, y, col->shade(6));
		bit->put_pel(x, y+1, col->shade(6));
		bit->put_pel(x+2, y, col->shade(5));
		bit->put_pel(x+1, y+1, col->shade(5));
		bit->put_pel(x, y+2, col->shade(5));
	}
	if(!(side&4) && !(side&2) && to[2]&2 && to[1]&4) {
		bit->put_pel(x+17, y, col->shade(1+2));
		bit->put_pel(x+16, y, col->shade(2+1));
		bit->put_pel(x+17, y+1, col->shade(2+2));
		bit->put_pel(x+15, y, col->shade(3));
		bit->put_pel(x+16, y+1, col->shade(3+1));
		bit->put_pel(x+17, y+2, col->shade(3+2));
	}
	if(!(side&1) && !(side&8) && to[0]&8 && to[3]&1) {
		bit->put_pel(x, y+17, col->shade(7));
		bit->put_pel(x+1, y+17, col->shade(6));
		bit->put_pel(x, y+16, col->shade(6));
		bit->put_pel(x+2, y+17, col->shade(5));
		bit->put_pel(x+1, y+16, col->shade(5));
		bit->put_pel(x, y+15, col->shade(5));
	}
	if(!(side&4) && !(side&8) && to[2]&8 && to[3]&4) {
		bit->put_pel(x+17, y+17, col->shade(1));
		bit->put_pel(x+16, y+17, col->shade(2));
		bit->put_pel(x+17, y+16, col->shade(2));
		bit->put_pel(x+15, y+17, col->shade(3));
		bit->put_pel(x+16, y+16, col->shade(3));
		bit->put_pel(x+17, y+15, col->shade(3));
	}
}

void raw_draw_bloc(const Video_bitmap* bit, int x, int y, uint8_t side, Color* col) {
	int tx,tl,rx=0,ry=0,rw=18,rh=18;
	if(side&1) {
		bit->vline(x, y, 18, col->shade(7));
		bit->vline(x+1, y, 18, col->shade(6));
		bit->vline(x+2, y, 18, col->shade(5));
		rx=3; rw-=3;
	}
	if(side&2) {
		bit->hline(y, x, 18, col->shade(7));
		if(side&1)
			tx = x+1;
		else
			tx = x;
		bit->hline(y+1, tx, x-tx+18, col->shade(6));
		if(side&1)
			tx = x+2;
		else
			tx = x;
		bit->hline(y+2, tx, x-tx+18, col->shade(5));
		ry=3; rh-=3;
	}
	if(side&4) {
		bit->vline(x+17, y, 18, col->shade(1));
		if(side&2)
			tx = y+1;
		else
			tx = y;
		bit->vline(x+16, tx, y-tx+18, col->shade(2));
		if(side&2)
			tx = y+2;
		else
			tx = y;
		bit->vline(x+15, tx, y-tx+18, col->shade(3));
		rw-=3;
	}
	if(side&8) {
		bit->hline(y+17, x, 18, col->shade(1));
		if(side&1)
			tx = x+1;
		else
			tx = x;
		tl = x-tx+18;
		if(side&4)
			tl--;
		bit->hline(y+16, tx, tl, col->shade(2));
		if(side&1)
			tx = x+2;
		else
			tx = x;
		tl = x-tx+18;
		if(side&4)
			tl -= 2;
		bit->hline(y+15, tx, tl, col->shade(3));
		rh-=3;
	}
	uint8_t main_color=col->shade(4);
	for(int i=0; i<rh; i++)
		bit->hline(y+ry+i, x+rx, rw, main_color);
}

void raw_small_draw_bloc(const Video_bitmap* bit, int x, int y, uint8_t side, Color* col) {
	int i,rx=0,ry=0,rw=6,rh=6;
	if(side&1) {
		bit->vline(x, y, 6, col->shade(7));
		rx++; rw--;
	}
	if(side&2) {
		bit->hline(y, x, 6, col->shade(7));
		ry++; rh--;
	}
	if(side&4) {
		bit->vline(x+5, y, 6, col->shade(1));
		rw--;
	}
	if(side&8) {
		bit->hline(y+5, x, 6, col->shade(1));
		rh--;
	}
	for(i=0; i<rh; i++)
		bit->hline(y+ry+i, x+rx, rw, col->shade(4));
}

namespace {

void init_directory() {
	strcpy(quadradir, exe_directory);
#ifdef WIN32
	if(SHGetFolderPath(0, CSIDL_APPDATA|CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, quadradir) < 0) {
		msgbox("SHGetFolderPath failed, using exe_directory");
	}
	else {
		strcat(quadradir, "\\Quadra");
		CreateDirectory(quadradir, 0);
	}
#else
	struct passwd *pw = NULL;

	pw = getpwuid(getuid());
	if(pw) {
		strcpy(quadradir, pw->pw_dir);
		strcat(quadradir, "/.quadra");
		mkdir(quadradir, 0777);
	}
#endif
}

void init_stuff(bool need_sound=true, bool need_video=true) {
	int i;

	video = Video::New(640, 480, "Quadra", !need_video);

	if(!video)
		fatal_msgbox("Could not initialize video subsystem");

	fonts.init();
	//If we init a dumb video, we need a dumb input too
	input = Input::New(!need_video);
	if(need_sound && need_video) // don't need sound if no video
		sound = Sound::New();
	else
		sound = NULL;

	for(i=0; i<256; i++)
		noir.setcolor(i, 40,40,40);

	chat_text = new Chat_text(fonts.normal, 212);
	net_starter = new Net_starter();
	sons.pause = new Sample(Res_doze(res_cuckoo_wav));
	sons.start = new Sample(Res_doze(res_hooter03_wav));
	sons.bonus1 = new Sample(Res_doze(res_whizz1_wav));
	sons.levelup = new Sample(Res_doze(res_glissup_wav));
	// quand le canvas 'coule'
	sons.depose4 = new Sample(Res_doze(res_clang3_wav));
	sons.flash = NULL;
	sons.depose3 = NULL;
	sons.depose2 = NULL;
	sons.depose = NULL;
	sons.drip = NULL;
	sons.glass = new Sample(Res_doze(res_glass01_wav));
	sons.enter = new Sample(Res_doze(res_tapdrip_wav));
	sons.fadein = new Sample(Res_doze(res_w_bayo_0_wav));
	sons.fadeout = new Sample(Res_doze(res_fadeout_wav));
	sons.point = new Sample(Res_doze(res_click_1_wav));
	sons.click = new Sample(Res_doze(res_blip1_wav));
	sons.msg = new Sample(Res_doze(res_handbell_wav));
	sons.potato_get = new Sample(Res_doze(res_potato_get_wav));
	sons.potato_rid = new Sample(Res_doze(res_zingle_wav));
	//-roncli 4/29/01 Load countdown samples
	sons.minute = new Sample(Res_doze(res_t1min_wav));
	sons.thirty = new Sample(Res_doze(res_t30sec_wav));
	sons.twenty = new Sample(Res_doze(res_t20sec_wav));
	sons.ten = new Sample(Res_doze(res_t10sec_wav));
	sons.five = new Sample(Res_doze(res_t5sec_wav));
	sons.four = new Sample(Res_doze(res_t4sec_wav));
	sons.three = new Sample(Res_doze(res_t3sec_wav));
	sons.two = new Sample(Res_doze(res_t2sec_wav));
	sons.one = new Sample(Res_doze(res_t1sec_wav));
	cursor = new Cursor;
	for(i=0; i<8; i++)
		fteam[i] = new Font(*fonts.normal);
}

void deinit_stuff() {
	for(int i=0; i<8; i++)
		if(fteam[i]) {
			delete fteam[i];
			fteam[i] = NULL;
		}

	delete sons.click; sons.click = NULL;
	delete sons.point; sons.point = NULL;
	delete sons.fadeout; sons.fadeout = NULL;
	delete sons.fadein; sons.fadein = NULL;
	delete sons.drip; sons.drip = NULL;
	delete sons.glass; sons.glass = NULL;
	delete sons.depose; sons.depose = NULL;
	delete sons.depose2; sons.depose2 = NULL;
	delete sons.depose3; sons.depose3 = NULL;
	delete sons.depose4; sons.depose4 = NULL;
	delete sons.flash; sons.flash = NULL;
	delete sons.enter; sons.enter = NULL;
	delete sons.levelup; sons.levelup = NULL;
	delete sons.bonus1; sons.bonus1 = NULL;
	delete sons.start; sons.start = NULL;
	delete sons.pause; sons.pause = NULL;
	delete sons.msg; sons.msg = NULL;
	delete sons.potato_get; sons.potato_get = NULL;
	delete sons.potato_rid; sons.potato_rid = NULL;
	delete sons.minute; sons.minute = NULL;
	delete sons.thirty; sons.thirty = NULL;
	delete sons.twenty; sons.twenty = NULL;
	delete sons.ten; sons.ten = NULL;
	delete sons.five; sons.five = NULL;
	delete sons.four; sons.four = NULL;
	delete sons.three; sons.three = NULL;
	delete sons.two; sons.two = NULL;
	delete sons.one; sons.one = NULL;

	delete net_starter; net_starter=NULL;
	delete chat_text; chat_text=NULL;

	config.write();
	Highscores::freemem();

	delete cursor; cursor = NULL;

	fonts.deinit();
}

const char *command_get_param(const char *t, const char *def=NULL) {
	const char *temp = command.get_param();
	if(!temp)
		temp=def;
	if(!temp) {
		sprintf(st, "Command line parameter not found for '%s'", t);
		fatal_msgbox(st);
	}
	return temp;
}

Attack read_attack_param(const char *s) {
	Attack ret;
	char temp[1024];
	strncpy(temp, s, sizeof(temp));
	temp[sizeof(temp)-1]=0;
	char *param=strchr(temp, ' ');
	if(param) {
		*param=0;
		param++;
	}
	if(!strcmp(temp, "lines"))
		ret.type=ATTACK_LINES;
	if(!strcmp(temp, "none"))
		ret.type=ATTACK_NONE;
	if(!strcmp(temp, "blind") || !strcmp(temp, "fullblind")) {
		if(param) {
			int p=atoi(param);
			p=min(max(1, p), 255);
			ret.param=p;
		}
	}
	if(!strcmp(temp, "blind")) {
		ret.type=ATTACK_BLIND;
		if(!param)
			ret.param=30;
	}
	if(!strcmp(temp, "fullblind")) {
		ret.type=ATTACK_FULLBLIND;
		if(!param)
			ret.param=12;
	}
	return ret;
}

void display_command_line_help() {
	const ResName *res;
	switch(config.info.language) {
		default:
		case 0:
			res = &res_help_en_txt; break;
		case 1:
			res = &res_help_fr_txt; break;
	}
	string cmdline = resmanager->get(*res);
	if(video)
		delete video;
	printf("%s\n%s\n",ST_CMDLINE, cmdline.c_str());
}

void read_script(const char *fn, bool second=false) {
	char st[32768];
	Res_dos script(fn);
	if(script.exist) {
		//-2 because Stringtable is strange
		uint32_t size = min(static_cast<uint32_t>(sizeof(st)-2), script.size());
		strncpy(st, (char *)script.buf(), size);
		st[size] = 0;
		Stringtable str((uint8_t *)st, size);
		int i;
		for(i=0; i<str.size(); i++) {
			if(second) {
				if(game && str.get(i)[0]=='/')
					game->net_list.got_admin_line(str.get(i), NULL);
			}
			else {
				if(str.get(i)[0]=='-')
					command.add(str.get(i));
			}
		}
	}
	else
		msgbox("Can't find script %s, ignoring.\n", fn);
}

void main_loop(Executor& menu, bool demo_verif) {
	overmind.start(&menu);

	uint32_t acc(0);
	bool reset_time(false);
	while (!menu.done) {
		const uint32_t last(SDL_GetTicks());
		if (demo_verif) {
			acc=500;
			while (acc--)
				overmind.step();
		} else {
#ifdef PAINTDETECTOR2000
			bool sounded(false);
#endif
			while (acc >= 10) {
				if (reset_time) { // remet 'normal' seulement si au moins 1 frame s'est ecoule
					time_control = TIME_NORMAL;
					reset_time = false;
				}
				acc -= 10;
				try {
					overmind.step();
				}
				catch(std::exception *e) {
					msgbox("Exception caught from overmind.step(): %s\n", e->what());
				}
#ifdef PAINTDETECTOR2000
				if (video->need_paint == 2 && !sounded) {
					sons.msg->play(0, 0, 11025);
					sounded = true;
				}
#endif
				reset_time = true;
				if (time_control == TIME_FREEZE)
					break;
			}
		}

		input->check();

		if (ecran && !video_is_dumb) {
			try {
				ecran->draw_zone();
			}
			catch(std::exception *e) {
				msgbox("Exception caught from ecran->draw_zone(): %s\n", e->what());
			}

#ifdef FRAMECOUNTER
			static uint32_t lastvideoframe(0), lastoverframe(0);
			if (ecran->font) {
				if (overmind.framecount - lastoverframe > 500) {
					lastoverframe = overmind.framecount;
					lastvideoframe = video->framecount;
				}
				int up(999);
				if (overmind.framecount - lastoverframe > 0)
					up = ((video->framecount - lastvideoframe) * 100) / (overmind.framecount - lastoverframe);
				video->vb->rect(0,0,50,20,0);
				char st[80];
				sprintf(st, "%i", up);
				ecran->font->draw(st, video->vb, 0, 0);
			}
#endif // FRAMECOUNTER
		}
		video->end_frame();

#ifndef NDEBUG
		if (input->keys[SDL_SCANCODE_F8] & PRESSED) // F8 = buckage
			SDL_Delay(250);
		if (input->keys[SDL_SCANCODE_F9] & PRESSED) // F9 = slow motion mode
			time_control = TIME_SLOW;
		if (input->keys[SDL_SCANCODE_F10] & PRESSED) // F10 = turbo mode
			time_control = TIME_FAST;
#endif

		switch (time_control) {
		case TIME_FREEZE:
			acc = 10; break;
		case TIME_SLOW:
			acc += 1; break;
		case TIME_FAST:
			acc += 80; break;
		default:
			acc += SDL_GetTicks() - last;
		}
		if (acc > 300 && !video_is_dumb) {
			overmind.framecount += acc - 300;
			acc = 300; // pour eviter trop de depassement
		}
		if (acc > 10000) {
			msgbox("Not enough CPU time to be server!\n");
			overmind.framecount += acc - 10;
			acc = 10;
		}
	}
}

}  // namespace

int start_game() {
	if(command.token("113"))
		Config::net_version = 20;
	if(command.token("debug")) {
		_debug = true;
		msgbox("Debug mode enabled\n");
	}

	bool no_video = false;
	bool no_sound = false;
	bool demo_play = false;
	bool demo_verif = false;
	bool demo_verified_and_valid = false;
	char buf[512];
  /* FIXME: rather than using 1024 MAXPATHLEN should be used.  To do
     so requires all other filename lengths be MAXPATHLEN as well. */
	char fn[1024];

	init_directory();

	const char *dir=quadradir;
#ifdef WIN32
	dir = exe_directory;
#else
	dir = getenv("QUADRADIR");
	if(!dir)
		dir = DATAGAMESDIR;
#endif
	resmanager=new Resmanager();
	snprintf(fn, sizeof(fn) - 1, "%s/quadra.res", dir);
	resmanager->loadresfile(fn);
	snprintf(fn, sizeof(fn) - 1, "%s/quadra%i%i%i.res", dir, Config::major, Config::minor, Config::patchlevel);
	resmanager->loadresfile(fn);
	if(command.token("patch") || command.token("theme")) {
		const char *temp=command_get_param("patch <filename>");
		if(temp[0] != '/' && temp[0] != '\\')
			snprintf(fn, sizeof(fn) - 1, "%s/%s", dir, temp);
		else
			snprintf(fn, sizeof(fn) - 1, "%s", temp);
		resmanager->loadresfile(fn);
	}
	msgbox("Reading config: ");
	config.read();
	msgbox("Ok\n");
	//Read script and add to command line options if applicable
	if(command.token("exec")) {
		const char *temp=command_get_param("exec <filename>");
		msgbox("Reading script %s: ", temp);
		read_script(temp);
		msgbox("Ok\n");
	}
	bool dedicated=command.token("dedicated");
	if(command.token("english") || dedicated)
		config.info.language=0;
	if(command.token("french"))
		config.info.language=1;
	const ResName *language;
	int i;
	switch(config.info.language) {
		default:
		case 0:
			language = &res_anglais_txt; break;
		case 1:
			language = &res_francais_txt; break;
	}
	for(i=0; i<MAXTEAMS; i++)
		set_team_name(i, NULL);
	msgbox("Reading stringtable: ");
	stringtable=new Stringtable(*language);
	msgbox("Ok\n");

	if(command.token("h help ?")) {
		display_command_line_help();
		delete resmanager;
		return 0;
	}
	if(_debug && command.token("verify")) {
		const char *temp = command_get_param("verify <filename>");
		strncpy(buf, temp, sizeof(buf) - 1);
		demo_play = true;
		demo_verif = true;
		demo_verified_and_valid = false;
		no_video = true;
		no_sound = true;
	}
	if(command.token("play")) {
		const char *temp = command_get_param("play <filename>");
		strncpy(buf, temp, sizeof(buf) - 1);
		demo_play = true;
		demo_verif = false;
	}
	if(!demo_play && dedicated)
		no_video=true;
	if(command.token("novideo")) {
		no_video=true;
		no_sound=true;
	}
	if(command.token("nosound")) {
		no_sound=true;
	}
	msgbox("Calling init_stuff: ");
	init_stuff(!no_sound, !no_video); //No sound when checking demos
  msgbox("Ok\n");

  // Start auto-updater, but only if we have video enabled.
  if(!no_video)
    AutoUpdater::start();

	Executor menu;
	//Add Menu_intro so we get back there after -connect, -server or -play
	//  unless -thenquit option si specified
	if(!command.token("thenquit") && !demo_verif)
			menu.add(new Menu_intro());

	if(!demo_play) {
		if(command.token("server") || dedicated) {
			if(!net->active)
				fatal_msgbox("Network failed to initialize or not present\nCan't start server.\n");
			buf[0] = 0;
			if(command.token("port")) {
				const char *temp = command_get_param("port <TCP/IP port>");
				int port = atoi(temp);
				if(port<=0 || port>=65535)
					fatal_msgbox("Illegal port number.\n");
				config.info.port_number = port;
			}
			Game_params p;
			if(command.token("ffa"))
				p.set_preset(PRESET_FFA);
			if(command.token("survivor"))
				p.set_preset(PRESET_SURVIVOR);
			if(command.token("hotpotato"))
				p.set_preset(PRESET_HOT_POTATO);
			if(command.token("peace"))
				p.set_preset(PRESET_PEACE);
			if(command.token("blind")) {
				p.set_preset(PRESET_BLIND);
				const char *temp = command_get_param("blind <n>", "30");
				uint32_t time=atoi(temp);
				time=min(max(time, static_cast<uint32_t>(0)), static_cast<uint32_t>(255));
				p.normal_attack.param=time;
				p.clean_attack.param=time;
			}
			if(command.token("fullblind")) {
				p.set_preset(PRESET_FULLBLIND);
				const char *temp = command_get_param("fullblind <n>", "12");
				uint32_t time=atoi(temp);
				time=min(max(time, static_cast<uint32_t>(0)), static_cast<uint32_t>(255));
				p.normal_attack.param=time;
				p.clean_attack.param=time;
			}
			if(command.token("attack")) {
				const char *temp=command_get_param("attack <type> [strength]");
				p.normal_attack=read_attack_param(temp);
			}
			if(command.token("attackclean")) {
				const char *temp=command_get_param("attackclean <type> [strength]");
				p.clean_attack=read_attack_param(temp);
			}
			if(command.token("attack2")) {
				const char *temp=command_get_param("attack2 <type> [strength]");
				p.potato_normal_attack=read_attack_param(temp);
			}
			if(command.token("attack2clean")) {
				const char *temp=command_get_param("attack2clean <type> [strength]");
				p.potato_clean_attack=read_attack_param(temp);
			}
			if(command.token("boringrules"))
				p.boring_rules = true;
			if(command.token("nolevelup"))
				p.level_up = false;
			if(command.token("levelup"))
				p.level_up = true;
			if(command.token("level")) {
				const char *temp = command_get_param("level <level number>");
				p.level_start = atoi(temp);
				p.level_start = min(max(p.level_start, 1), 40);
			}
			if(command.token("name")) {
				const char *temp = command_get_param("name <game name>");
				if (temp)
					p.name = temp;
			}
			if(command.token("nohandicap"))
				p.allow_handicap=false;
			if(command.token("endfrag"))
				p.game_end = END_FRAG;
			if(command.token("endfrags"))
				p.game_end = END_FRAG;
			if(command.token("endtime"))
				p.game_end = END_TIME;
			if(command.token("endpoints"))
				p.game_end = END_POINTS;
			if(command.token("endscore"))
				p.game_end = END_POINTS;
			if(p.game_end != END_NEVER) {
				const char *temp = command_get_param("endfrag/endtime/endpoints <number>");
				p.game_end_value = atoi(temp);
				p.game_end_value = min(max(p.game_end_value, 1), p.game_end<=END_TIME? 9999:99999);
			}
			if(command.token("public"))
				p.game_public = true;
			menu.add(new Menu_startserver());
			(void)new Game(&p);
			if(dedicated && !command.token("once"))
				game->auto_restart = true;
			if(command.token("nomoves"))
				game->wants_moves = false;
			//Read script a second time, now that game is created
			if(command.token("exec")) {
				const char *temp=command_get_param("exec <filename>");
				read_script(temp, true);
			}
			if(command.token("admin")) {
				const char *temp = command_get_param("admin <password>");
				char line[1024];
				snprintf(line, sizeof(line) - 1, "/setpasswd %s", temp);
				game->net_list.got_admin_line(line, NULL);
			}
			if(command.token("record")) {
				const char *temp = command_get_param("record <filename>", Clock::absolute_time());
				game->prepare_recording(temp);
				game->prepare_logging();
			}
		}
		else {
			if(command.token("connectfile")) {
				const char *temp = command_get_param("connectfile <filename>");
				char st[1024];
				Res_dos file(temp);
				if(file.exist) {
					snprintf(st, sizeof(st), "-connect %*.*s", file.size(), file.size(), (char *)file.buf());
					st[sizeof(st) - 1] = 0;
					command.add(st);
				}
				else
					msgbox("Can't find connectfile %s, ignoring.\n", temp);

			}
			if(command.token("connect")) {
				if(!net->active)
					fatal_msgbox("Network failed to initialize or not present\nCan't connect.\n");
				const char *temp = command_get_param("connect <TCP/IP address>");
				strncpy(buf, temp, sizeof(buf) - 1);
				buf[sizeof(buf)-1] = 0;
				menu.add(new Menu_startconnect(buf, false));
				if(config.warning)
					menu.add(new Menu_setup());
			}
		}
	}
	else {
		Res_compress *res = new Res_compress(ResName(buf), RES_TRY);
		if(res->exist) {
			menu.add(new Demo_multi_player(res));
			// le 'delete res' est fait par ~Demo_multi_player
			if(playback)
				playback->set_verification_flag(&demo_verified_and_valid);
		}
		else {
			msgbox("Unable to open demo '%s'\n", buf);
			delete res;
		}
	}

	main_loop(menu, demo_verif);

	deinit_stuff();
	delete resmanager;

  return !demo_verif || demo_verified_and_valid ? 0 : 1;
}
