/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <stdio.h>
#include "input.h"
#include "sprite.h"
#include "pcx.h"
#include "zone.h"
#include "config.h"
#include "quadra.h"
#include "pane.h"
#include "chat_text.h"
#include "music.h"
#include "game.h"
#include "global.h"
#include "texte.h"
#include "net_stuff.h"
#include "recording.h"
#include "sons.h"
#include "main.h"
#include "menu.h"
#include "fonts.h"
#include "highscores.h"
#include "canvas.h"
#include "net_server.h"
#include "nglog.h"
#include "clock.h"
#include "multi_player.h"

Multi_player::Multi_player(int *got_high) {
	stop=false; //set to true to quit game
	got_highscore = got_high;
	last_countdown = -1;
	menu_stat = NULL;
	int i;
	{
		Res_doze res("Fond0.pcx");
		Pcx img(res);
		bit = new Bitmap(img);
	}
	pal.set_size(256);
	for(i=0; i<9; i++)
		color[i] = new Color(i, pal);
	font2 = fteam[4];
	if(game->single) {
		if(config.info.cdmusic == 1)
			music->play(2);
		i = 1;
	} else {
		i = game->get_multi_level();
		if(config.info.cdmusic == 1)
			music->play(i+1, true);
	}
	Canvas::change_level(i, &pal, bit);
	set_fteam_color(pal);
	inter->set_font(new Font(*fonts.normal, pal, 255, 255, 255));
	courrier = new Font(*fonts.courrier, pal, 255, 255, 255);
	(void)new Zone_bitmap(inter, bit, 0, 0);
	input->pause = pause = false;
	zone_pause = NULL;

	for(i=0; i<3; i++) {
		pane_exec[i] = new Executor;
		pane_info[i] = new Pane_info(bit, font2, inter, i, this);
		if(game->single) {
			switch(i) {
				case 0: pane[i] = new Pane_blockinfo(*pane_info[i]); break;
				case 1:
					if(playback && playback->old_mode)
						pane[i] = new Pane_startgame(*pane_info[i], playback->single_player);
					else
						pane[i] = new Pane_singleplayer(*pane_info[i]);
					break;
				case 2: pane[i] = new Pane_comboinfo(*pane_info[i]); break;
			}
		} else { // sinon mode multi_player
			pane[i] = new Pane_option(*pane_info[i]);
		}
		pane_exec[i]->add(pane[i]);
		overmind.start(pane_exec[i]);
	}
	if(playback && playback->single() && !playback->auto_demo) {
		(void)new Zone_slow_play(inter, bit, font2, ST_SLOWPLAY, 430, 420);
		(void)new Zone_fast_play(inter, bit, font2, ST_FASTPLAY, 430, 450);
		b_quit = new Zone_text_button2(inter, bit, font2, ST_BACK, 560, 455);
	}
	else {
		b_quit=NULL;
	}
}

void Multi_player::step() {
	Menu_fadein::step();

/* Generate small pics for ngStatsQuadra
	static Dword wait_timer=0;
	wait_timer++;
	if(wait_timer==500) {
		int col;
		Byte side;
		for(col=-1; col<9; col++) {
			for(side=0; side<16; side++) {
				char st[32];
				Bitmap the_bit(18, 18, 18);
				if(col!=-1) {
					raw_draw_bloc(video->vb, 0, 0, side, color[col]);
					video->vb->get_bitmap(&the_bit, 0, 0, 18, 18);
					sprintf(st, "%c%c.raw", '0'+col, 'a'+side);
				}
				else
					strcpy(st, "e0.raw");
				Raw raw(18, 18, col!=-1? 8:2);
				Res_dos res(st, RES_CREATE);
				if(!res.exist) {
					skelton_msgbox("Can't create file!\n");
					return;
				}
				raw.write(res);
				int i;
				Byte pa[3];
				if(col!=-1)
					for(i=color[col]->shade(0); i<color[col]->shade(0)+8; i++) {
						pa[0] = pal.r(i);
						pa[1] = pal.g(i);
						pa[2] = pal.b(i);
						res.write(pa, 3);
					}
				else {
					pa[0]=0;
					pa[1]=0;
					pa[2]=0;
					res.write(pa, 3);
					pa[0]=255;
					pa[1]=255;
					pa[2]=255;
					res.write(pa, 3);
				}
				for(i=0; i<18; i++) {
					Byte pel[18];
					memcpy(pel, the_bit[i], 18);
					for(int j=0; j<18; j++)
						pel[j] = pel[j] & 7;
					res.write(pel, 18);
				}
			}
		}
	}*/

	if(playback && playback->auto_demo)
		if(result || input->quel_key != -1) {
			input->quel_key = -1;
			stop = true;
		}

	check_pause();

	if(playback && playback->old_mode) {
		if(playback->completed) {
			game->endgame();
		}
	}
	if(game->single && game->terminated && game->net_list.all_gone()) {
		Canvas *c=game->net_list.get(0);
		if(recording && c) {
			recording->end_multi();
			game->net_server->stop_multi_recording();
			*got_highscore = Highscores::update(c);
		}
		stop = true;
	}
	if(input->quel_key == KEY_ESCAPE)
		game->abort = true;
	if(game->abort)
		stop = true;
	if(game->terminated && game->net_list.all_gone())
		stop = true;
	if(playback && result && result == b_quit)
		stop = true;
	if(stop) {
		if(game->server && !game->network) {
			game->net_list.send_end_signal(!game->abort);
		}
		if(!game->single && menu_stat)
			*menu_stat = new Menu_stat();
		exec(new Fade_out(pal));
		return;
	}

	if(_debug) {
		int snap_can=-1;
		if(input->keys[KEY_F2] & PRESSED) {
			input->keys[KEY_F2] = 0;
			snap_can = 0;
		}
		if(input->keys[KEY_F3] & PRESSED) {
			input->keys[KEY_F3] = 0;
			snap_can = 1;
		}
		if(input->keys[KEY_F4] & PRESSED) {
			input->keys[KEY_F4] = 0;
			snap_can = 2;
		}
		if(snap_can != -1) {
			video->vb->rect(snap_can * 214+142, 0, 60, 30, 0);
			video->snap_shot(snap_can * 214+6, 0, 203, 401);
		}
	}
}

Multi_player::~Multi_player() {
	int i;
	msgbox("Multi_player::~Multi_player\n");
	if(zone_pause)
		delete zone_pause;
	for(i=0; i<3; i++) {
		pane[i] = NULL; // Tres important car les delete pane_exec[i] detruisent des Pane
		                //  qui accedent a pane[]
		overmind.stop(pane_exec[i]);
		delete pane_exec[i];
		delete pane_info[i];
	}
	for(i=0; i<9; i++)
		delete color[i];
	delete bit;
	delete courrier;
	if(!game->single && !playback)
		config.write(); // save la config seulement en multi-player ET pas dans une demo
}

void Multi_player::check_pause() {
	if(input->pause) {
		bool ignore = false;
		if(game->delay_start != 0 && game->delay_start != 500)
			ignore = true;
		if(!ignore)
			game->clientpause();
		input->pause=false;
	}
	if(game->paused != pause) {
		pause = game->paused;
		if(pause) {
			if(game->delay_start == 0) // empeche de faire le son au debut
				Sfx stmp(sons.pause, 0, -300, 0, 11025);
			zone_pause = new Zone_sprite(inter, "GAMEPAUS.RAW");
		} else {
			if(zone_pause)
				delete zone_pause;
			zone_pause = NULL;
			video->need_paint = 2;
		}
	} else {
		if(game->delay_start && game->delay_start != 500) {
			if(game->delay_start == 1) {
				Sfx stmp(sons.start, 0, -300, 0, 11025);
				return;
			}
			int chiffre = game->delay_start / 100;
			if(chiffre != last_countdown) {
				if(zone_pause)
					delete zone_pause;
				video->need_paint = 2;
				sprintf(st, "GAME_%i.RAW", chiffre+1);
				zone_pause = new Zone_sprite(inter, st);
				last_countdown = chiffre;
				Sfx stmp(sons.pause, 0, -300, 0, 20025);
			}
		}
	}
}

void Multi_player::set_menu_stat(Module **module) {
	menu_stat = module;
}

Demo_multi_player::Demo_multi_player(Res *r, bool auto_demo) {
	playback = new Playback(r);
	delete r;
	playback->auto_demo = auto_demo;
//	if(playback->auto_demo)
	playback->shit_skipper2000(true);
	init_playback();
}

Demo_multi_player::Demo_multi_player(Playback *p) {
	playback = p;
	init_playback();
}

void Demo_multi_player::init_playback() {
	if(!playback->valid)
		return;
	playback->create_game();
	playback->multi_level = config.info.multi_level;
	if(playback && playback->valid) {
		if(game)
			game->seed = playback->seed;
	}
	if(game)
		game->frame_start = overmind.framecount; // re-ajuste le frame_start avant de commencer le playback
}

Demo_multi_player::~Demo_multi_player() {
	delete playback;
	playback=NULL;
	if(game) {
		delete game;
		if(chat_text)
			chat_text->clear();
	}
}

void Demo_multi_player::init() {
	Module::init();
	if(playback && playback->valid && !playback->completed) {
		if(playback->single() || playback->auto_demo)
			exec(new Multi_player(NULL));
		else
			exec(new Multi_player_launcher());
	}
	else
		ret();
}

Single_player::Single_player(Game_preset pgp): gp(pgp) {
	play_again = 1;
}

void Single_player::step() {
	if(play_again) {
		play_again = 0;
		call(new Single_player_iterate(&play_again, gp));
	}
	else {
		ret();
	}
}

Single_player_iterate::Single_player_iterate(int *play, Game_preset pgp): gp(pgp) {
	hscore = -1;
	play_again = play;
}

void Single_player_iterate::init() {
	Game_params p;
	p.name="";
	p.set_preset(gp);
	(void)new Game(&p);
	if(!playback) {
		game->loopback_connection=net->start_loopback_client();
		game->loopback_connection->joined=true;
		game->loopback_connection->trusted=true;
		char fn[1024];
		sprintf(fn, "%s/last.rec", quadradir);
		game->prepare_recording(fn);
		game->prepare_logging(Clock::absolute_time());
	}
	call(new Multi_player(&hscore));
}

void Single_player_iterate::step() {
	if(game) {
		delete game;
		if(chat_text)
			chat_text->clear();
	}
	exec(new Menu_highscore(hscore, play_again, true));
}

Zone_slow_play::Zone_slow_play(Inter *in, Bitmap *bit, Font *f, const char *t, int px, int py):
	Zone_text_button2(in, bit, f, t, px, py) {
}

void Zone_slow_play::waiting() {
	Zone_text_button2::waiting();
	if(input->mouse.button[0] & PRESSED) {
		time_control = TIME_SLOW;
	}
}

void Zone_slow_play::process() {
	Zone_text_button2::process();
	if(high && (input->keys[KEY_ENTER] & PRESSED || input->keys[KEY_SPACE] & PRESSED)) {
		time_control = TIME_SLOW;
	}
}

Zone_fast_play::Zone_fast_play(Inter *in, Bitmap *bit, Font *f, const char *t, int px, int py):
	Zone_text_button2(in, bit, f, t, px, py) {
}

void Zone_fast_play::waiting() {
	Zone_text_button2::waiting();
	if(input->mouse.button[0] & PRESSED) {
		time_control = TIME_FAST;
	}
}

void Zone_fast_play::process() {
	Zone_text_button2::process();
	if(high && (input->keys[KEY_ENTER] & PRESSED || input->keys[KEY_SPACE] & PRESSED)) {
		time_control = TIME_FAST;
	}
}

void Multi_player_launcher::init() {
	menu = NULL;
	if(!game || !game->terminated) {
		Multi_player *multi = new Multi_player(NULL);
		multi->set_menu_stat(&menu);
		call(multi);
	}
	else
		call(new Menu_stat());
}

void Multi_player_launcher::step() {
	if(menu)
		call(menu);
	ret();
}
