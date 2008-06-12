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

#include "config.h"
#include "SDL.h"
#ifdef UGS_LINUX
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#ifdef WIN32
#include <shlobj.h>
#endif
#include <stdlib.h>
#include <exception>
#include "packets.h"
#include "types.h"
#include "net.h"
#include "video.h"
#include "cursor.h"
#include "image_png.h"
#include "palette.h"
#include "input.h"
#include "sound.h"
#include "sprite.h"
#include "bitmap.h"
#include "inter.h"
#include "random.h"
#include "game.h"
#include "bloc.h"
#include "color.h"
#include "menu.h"
#include "main.h"
#include "overmind.h"
#include "command.h"
#include "multi_player.h"
#include "stringtable.h"
#include "net_stuff.h"
#include "chat_text.h"
#include "recording.h"
#include "canvas.h"
#include "global.h"
#include "sons.h"
#include "cfgfile.h"
#include "fonts.h"
#include "res_compress.h"
#include "highscores.h"
#include "crypt.h"
#include "unicode.h"
#include "nglog.h"
#include "clock.h"
#include "net_server.h"
#include "update.h"

using std::max;
using std::min;

Color *color[9];
Font *fteam[8];
bool video_is_dumb = false;

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

void raw_draw_bloc_corner(const Video_bitmap* bit, int x, int y, Byte side, Color* col, Byte to[4]) {
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

void raw_draw_bloc(const Video_bitmap* bit, int x, int y, Byte side, Color* col) {
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
	Byte main_color=col->shade(4);
	for(int i=0; i<rh; i++)
		bit->hline(y+ry+i, x+rx, rw, main_color);
}

void raw_small_draw_bloc(const Video_bitmap* bit, int x, int y, Byte side, Color* col) {
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

Player_check_link::Player_check_link(Canvas *c): Player_base(c) {
	anim = 0;
	tombe = 0;
	//Reset Canvas::moved
	int i, j;
	for(j=0; j<36; j++)
		for(i=0; i<18; i++)
			canvas->moved[j][i]=false;
}

void Player_check_link::step() {
	Player_base::step();
	int i, j, k = 1;
	if(anim == 0) {
		while(k) {												 // rescan plusieurs pass
			k = 0;
			for(j = 31; j >= 0; j--) {						 // pour chaque rangee :
				for(i = 4; i < 14; i++) 						 // scan gauche a droite ->
					if((canvas->block[j][i]&8) && (!canvas->tmp[j][i]) && canvas->tmp[j+1][i]) {
						fill_bloc(i,j);
						k = 1;
					}
			}
		}
		anim++;
	} else {
		anim = 0;
		k = 0;
		//Temp move array
		bool moved[36][18];
		for(j=0; j<36; j++)
			for(i=0; i<18; i++)
				moved[j][i]=false;
		for(j = 31; j >= 0; j--)
			for(i = 4; i < 14; i++)
				if(canvas->occupied[j][i] && (!canvas->tmp[j][i])) {
					k = 1;
					canvas->block[j+1][i] = canvas->block[j][i];
					canvas->occupied[j+1][i] = canvas->occupied[j][i];
					moved[j+1][i] = true;
					canvas->blinded[j+1][i] = canvas->blinded[j][i];
					canvas->bflash[j+1][i] = canvas->bflash[j][i];
					canvas->dirted[j+1][i] = 2;
					canvas->block[j][i] = 0;
					canvas->occupied[j][i] = false;
					canvas->blinded[j][i] = 0;
					canvas->bflash[j][i] = 0;
					canvas->dirted[j][i] = 2;
				}
		if(k) {
			tombe++;
			//Some blocks moved, overwrite Canvas::moved
			for(j=0; j<36; j++) {
				bool movement_in_line=false;
				for(i=0; i<18; i++)
					if(moved[j][i])
						movement_in_line=true;
				if(movement_in_line)
					for(i=0; i<18; i++)
						canvas->moved[j][i]=moved[j][i];
			}
		}
		if(!k) {
			if(tombe) {
				i = -500 + tombe * 50;
				if(!canvas->islocal())
					i -= 1000;
				if(i>0)
					i=0;
				if(canvas->inter) {
					play_sound(sons.depose4, i, -1, 11000+ugs_random.rnd(127) - (canvas->complexity<<8));
				}
			}
			ret();//exec(new Player_check_line(canvas));
		}
	}
}

void Player_check_link::fill_bloc(Byte x, Byte y) {
	canvas->tmp[y][x] = 1;
	if((!(canvas->block[y][x]&8)) && (!canvas->tmp[y+1][x]))
		fill_bloc(x, y+1);
	if((!(canvas->block[y][x]&1)) && (!canvas->tmp[y][x-1]))
		fill_bloc(x-1, y);
	if((!(canvas->block[y][x]&4)) && (!canvas->tmp[y][x+1]))
		fill_bloc(x+1, y);
	if((!(canvas->block[y][x]&2)) && (!canvas->tmp[y-1][x]))
		fill_bloc(x, y-1);
}

Player_flash_lines::Player_flash_lines(Canvas *c): Player_base(c) {
	anim = 0;
	canvas->color_flash = 255;
	if(canvas->inter) {
		int vo = -300 - ugs_random.rnd(255);
		if(!canvas->islocal())
			vo -= 1000;
		play_sound(sons.flash, vo, -1, 11000+ugs_random.rnd(127) - (canvas->complexity<<8));
	}
}

void Player_flash_lines::step() {
	Player_base::step();
	int i;
	if(anim < 16) {
		if((anim>>1)&1)
			canvas->color_flash = 200;
		else
			canvas->color_flash = 255;
		anim++;
	} else {
		canvas->color_flash = 0;
		canvas->clear_tmp();
		anim = 0;
		for(i = 0; i < 20; i++)
			canvas->flash[i] = 0;
		exec(new Player_check_link(canvas));
	}
}

void Player_base::step() {
	if(canvas->idle<3)
		canvas->gone_time=0;
	if(game && game->net_version()<23 && !game->paused && (canvas->idle==0 || canvas->idle==1))
		canvas->stats[CS::PLAYING_TIME].add(1);
	if(!((overmind.framecount+1)&127)) {
		int pm=0;
		int time=canvas->stats[CS::PLAYING_TIME].get_value();
		if(time)
			pm=(int)(canvas->stats[CS::SCORE].get_value()*6000.0/time);
		canvas->stats[CS::PPM].set_value(pm);
		pm=0;
		if(time)
			pm=(int)(canvas->stats[CS::COMPTETOT].get_value()*6000.0/time);
		canvas->stats[CS::BPM].set_value(pm);
	}
	if(game && !game->paused && !(overmind.framecount&15)) {
		int i, j;
		for(j=0; j<36; j++)
			for(i=0; i<18; i++)
				if(canvas->blinded[j][i]) {
					if(!--canvas->blinded[j][i]) {
						canvas->bflash[j][i]=24;
						canvas->dirted[j][i]=2;
					}
				}
	}
}

Byte Player_base::calc_by(int py) const {
	return (((py+15+(12*18<<4))>>4)+17)/18;
}

bool Player_base::check_gone() {
	{
		Packet_gone *p=(Packet_gone *) game->peekpacket(P_GONE);
		if(p && p->player==canvas->num_player) {
			bool chat_msg=p->chat_msg;
			game->removepacket();
			msgbox("Player_base::check_gone: Player %i is gone\n", canvas->num_player);
			exec(new Player_gone(canvas, chat_msg));
			ret();
			return true;
		}
	}
	if(game->abort || game->terminated) {
		if(canvas->islocal()) {
			msgbox("Player_base::check_gone: local player %i gone\n", canvas->num_player);
			Player_base *pb;
			if(game->terminated || game->server || canvas->idle==2 || canvas->dying || game->delay_start==500) {
				Packet_clientgone p;
				p.player=canvas->num_player;
				p.chat_msg=!game->terminated;
				net->sendtcp(&p);
				pb = new Player_gone(canvas, !game->terminated);
			}
			else {
				//Player_dead::Player_dead sends the packet
				pb = new Player_dead(canvas, true);
			}
			exec(pb);
			ret();
			return true;
		}
	}
	return false;
}

void Player_base::check_state() {
	Packet_state *p=(Packet_state *) game->peekpacket(P_STATE);
	if(p && p->player==canvas->num_player) {
		msgbox("Player_base::check_state: player=%i, state=%i\n", p->player, p->state);
		canvas->state=(Canvas::State) p->state;
		game->removepacket();
	}
}

void Player_base::remove_bonus() {
	canvas->should_remove_bonus=false;
	canvas->bonus=0;
	if(canvas->islocal()) {
		Packet_clientremovebonus p;
		p.player=canvas->num_player;
		net->sendtcp(&p);
	}
}

void Player_base::check_bonus() {
	Packet_removebonus *p=(Packet_removebonus *) game->peekpacket(P_REMOVEBONUS);
	if(p && p->player==canvas->num_player) {
		remove_bonus();
		game->removepacket();
	}
}

void Player_base::play_sound(Sample *s, int vol, int pan, int freq) {
	if(time_control == TIME_SLOW)
		freq = freq*2/3;
	if(time_control == TIME_FAST)
		freq = freq*3/2;
  s->play(vol, pan, freq);
}

Player_text_scroll::Player_text_scroll(Canvas *c, const char *texte, int xoffset, int yoffset): Player_base(c) {
	(void)new Zone_combo(&combo, canvas, texte, canvas->x + xoffset, canvas->y + 330 + yoffset);
	if(game)
		game->net_list.add_watch(this);
}

Player_text_scroll::~Player_text_scroll() {
	if(game) // if ALT-F4: game is destroyed before this module... grr...
		game->net_list.remove_watch(this);
}

void Player_text_scroll::notify() {
	if(!game->net_list.get(canvas->num_player)) {
		stop();
	}
}

void Player_text_scroll::step() {
	if(combo) {
		combo->y-=2;
		if(combo->y < canvas->y || canvas->inter==NULL || canvas->small_watch) {
			stop();
		}
	}
}

void Player_text_scroll::stop() {
	if(combo) {
		delete combo;
		combo=NULL;
	}
	ret();
}

Player_add_bonus::Player_add_bonus(Canvas *c): Player_base(c) {
	anim = 0;
	first_done=false;
	if(canvas->inter) {
		int vo = -600;
		if(!canvas->islocal())
			vo -= 1000;
		play_sound(sons.bonus1, vo, -1, 11000+ugs_random.rnd(511));
	}
	nb=canvas->bonus;
}

Player_add_bonus::~Player_add_bonus() {
}

void Player_add_bonus::step() {
	Player_base::step();
	int y, i, j;
	bool old_net_version=false;
	if(game->net_version()<23)
		old_net_version=true;
	if(nb && canvas->bonus) {
		if(anim == 0) {
			for(j = 0; j < 31; j++)
				for(i = 4; i < 14; i++) {
					canvas->block[j][i] = canvas->block[j+1][i];
					canvas->occupied[j][i] = canvas->occupied[j+1][i];
					canvas->blinded[j][i] = canvas->blinded[j+1][i];
					canvas->bflash[j][i] = canvas->bflash[j+1][i];
					canvas->dirted[j+1][i] = 2;
				}
			Word hole_pos=canvas->bon[0].hole_pos;
			if(old_net_version)
				hole_pos=(1 << (9-(canvas->bon[0].x-4)));
			Byte top_bottom_add=0;
			if(!first_done || old_net_version)
				top_bottom_add |= 2; //Add top
			if(canvas->bon[0].final || old_net_version)
				top_bottom_add |= 8; //Add bottom
			for(i = 4; i < 14; i++) {
				canvas->block[31][i] = top_bottom_add + (canvas->bon[0].color<<4);
				canvas->occupied[31][i] = true;
			}
			for(i = 4; i < 14; i++) {
				if(hole_pos&512) {
					canvas->block[31][i] = 0;
					canvas->occupied[31][i] = false;
					if(first_done && canvas->occupied[30][i]) {
						//First line done in this packet and corresponding
						//  unit block above is not a hole: add bottom
						canvas->block[30][i] |= 8;
					}
					if(i > 4) {
						if(canvas->occupied[31][i-1])
							canvas->block[31][i-1] |= 4;
						if(canvas->occupied[31][4])
							canvas->block[31][4] |= 1;
					}
					if(i < 14) {
						canvas->block[31][i+1] |= 1;
						canvas->block[31][14] |= 4;
					}
				}
				else {
					if(first_done && !canvas->occupied[30][i]) {
						//First line done in this packet and corresponding
						//  unit block above is a hole: add top
						canvas->block[31][i] |= 2;
					}
				}
				canvas->blinded[31][i] = 0;
				canvas->bflash[31][i] = 0;
				hole_pos <<= 1;
			}
			first_done=true;
			if(canvas->bon[0].final)
				first_done=false;
		}
		anim++;
		if(anim == 7) {
			for(y = 1; y < canvas->bonus; y++)
				canvas->bon[y-1] = canvas->bon[y];
			canvas->bonus--;
			nb--;
			anim=0;
		}
	} else {
		for(i = 4; i < 14; i++)
			if(canvas->occupied[0][i])
				canvas->block[0][i] |= 2; 		// clip top
		canvas->clear_tmp();
		call(new Player_check_link(canvas));
		ret();
	}
}

Player_change_level::Player_change_level(Canvas *c): Player_base(c) {
	canvas->myself->pause();
}

Player_change_level::~Player_change_level() {
	canvas->myself->unpause();
}

void Player_change_level::init() {
	Player_base::init();
	canvas->change_level_single();
	if(canvas->inter) {
		canvas->inter->font->colorize(*canvas->pal, 255, 255, 255);
		video->need_paint = 2;
	}
	int nframe=16;
	if(time_control == TIME_SLOW)
		nframe = nframe*3/2;
	if(time_control == TIME_FAST)
		nframe = 3;
	call(new Fade_to(*canvas->pal, noir, nframe));
	call(new Wait_time(14)); // so that the 'level up' doesn't jerk
	ret();
}

Player_level_up::Player_level_up(Canvas *c): Player_base(c) {
	canvas->level_up = 220;
	if(canvas->islocal() || (canvas->inter && !canvas->small_watch))
		Player_base::play_sound(sons.levelup, -200-ugs_random.rnd(127), 0, 11000);
}

void Player_level_up::init() {
	Player_base::init();
	if(game->single) {  // && (canvas->level-1<=9 || game->net_version()<23)) {
		call(new Player_change_level(canvas));
		int nframe=16;
		if(time_control == TIME_SLOW)
			nframe = nframe*3/2;
		if(time_control == TIME_FAST)
			nframe = 3;
		call(new Fade_to(noir, *canvas->pal, nframe));
	}
}

void Player_level_up::step() {
	Player_base::step();
	canvas->level_up--;
	if(canvas->level_up < 0) {
		canvas->level_up = 0;
		ret();
	}
}

Player_check_line::Player_check_line(Canvas *c): Player_base(c) {
}

void Player_check_line::step() {
	Player_base::step();
	int co = check_nb_line();
	if(co) {
		canvas->depth += co;
		canvas->complexity++;
		check_clean();
		call(new Player_flash_lines(canvas));
	} else {
		if(canvas->idle < 2) { // if alive
			//Copy hole positions into moved array so that give_line
			//  and Net_list::send can use them
			int i, j;
			for(j=0; j<36; j++)
				for(i=0; i<18; i++)
					canvas->moved[j][i]=hole_pos[j][i];
			canvas->give_line();
		} else { // if dead, useless
			canvas->depth = 0;
			canvas->complexity=0;
			canvas->send_for_clean=0;
		}
		ret();
	}
}

int Player_check_line::check_nb_line() {
	int co = 0, i, j;
	for(j = 12; j < 32; j++) {
		for(i = 4; i < 14; i++)
			if(!canvas->occupied[j][i])
				break;
		if(i == 14) {
			if(co < 20)
				canvas->flash[co] = j;
			for(i = 4; i < 14; i++) {
				if(canvas->occupied[j-1][i] && !(canvas->block[j-1][i]&8)) {
					canvas->block[j-1][i] |= 8;
					canvas->dirted[j-1][i] = 2;
				}
				hole_pos[canvas->depth+co][i]=canvas->moved[j][i];
				canvas->block[j][i] = 0;
				canvas->occupied[j][i] = false;
				canvas->blinded[j][i] = 0;
				canvas->bflash[j][i] = 0;
				canvas->dirted[j][i] = 2;
				if(canvas->occupied[j+1][i] && !(canvas->block[j+1][i]&2)) {
					canvas->block[j+1][i] |= 2;
					canvas->dirted[j+1][i] = 2;
				}
			}
			co++;
		}
	}
	return co;
}

void Player_check_line::check_clean() {
	int i, j;
	for(j = 12; j < 32; j++)
		for(i = 4; i < 14; i++)
			if(canvas->occupied[j][i])
				return;
	canvas->add_text_scroller("Clean Canvas!!");
	if(game->net_version()<23)
		canvas->stats[CS::SCORE].add(5000);
	canvas->send_for_clean=true;
}

Player_process_key::Player_process_key(Canvas *c): Player_base(c) {
	hold_left = hold_right = 0;
	last_video_frame = last_overmind_frame = 0;
	canvas->idle = 1;
	canvas->state = Canvas::PLAYING;
	block_rotated=0;
	time_held=0;
}

bool Player_base::rotate_left() {
	bool ret=false;
	int t, i;
	t = (canvas->bloc->rot-1)&3;
	if(!canvas->collide(canvas->bloc->bx, canvas->bloc->by, t)) {
		ret=true;
		canvas->bloc->rot = t;
		if(canvas->inter) {
			i = (canvas->bloc->bx-9) * 300;
			play_sound(sons.drip, -200-ugs_random.rnd(127), i, 10500+ugs_random.rnd(1023));
		}
	}
	canvas->set_bit(8);
	return ret;
}

bool Player_base::rotate_right(bool twice) {
	bool ret=false;
	int t, i;
	int inc=1;
	if(twice)
		inc=2;
	t = (canvas->bloc->rot+inc)&3;
	if(!canvas->collide(canvas->bloc->bx, canvas->bloc->by, t)) {
		ret=true;
		canvas->bloc->rot = t;
		if(canvas->inter) {
			i = (canvas->bloc->bx-9) * 300;
			play_sound(sons.drip, -200-ugs_random.rnd(127), i, 10500+ugs_random.rnd(1023));
		}
	}
	if(twice)
		canvas->set_bit(32);
	else
		canvas->set_bit(16);
	return ret;
}

void Player_base::move_down() {
	int t;
	t = calc_by(canvas->bloc->y+canvas->down_speed);
	if(!canvas->collide(canvas->bloc->bx, t, canvas->bloc->rot)) {
		canvas->bloc->y += canvas->down_speed;
		canvas->bloc->by = calc_by(canvas->bloc->y);
	} else { // is used to "settle" the block so that it drops
		canvas->bloc->y = ((canvas->bloc->by-12)*18<<4)-1;
	}
	canvas->set_bit(1);
}

void Player_base::drop_down() {
	while(!canvas->check_collide(canvas->bloc, canvas->bloc->bx, canvas->bloc->by+1, canvas->bloc->rot))
		canvas->bloc->by++;
	canvas->bloc->calc_xy();
	canvas->set_bit(64);
}

bool Player_base::move_left() {
	bool ret=false;
	if(!canvas->collide(canvas->bloc->bx-1, canvas->bloc->by, canvas->bloc->rot)) {
		ret=true;
		canvas->bloc->bx--;
	}
	canvas->set_bit(2);
	return ret;
}

bool Player_base::move_right() {
	bool ret=false;
	if(!canvas->collide(canvas->bloc->bx+1, canvas->bloc->by, canvas->bloc->rot)) {
		ret=true;
		canvas->bloc->bx++;
	}
	canvas->set_bit(4);
	return ret;
}

void Player_process_key::playback_control() {
	Byte r = playback->get_byte();
	if(r & 8)
		rotate_left();
	if(r & 16)
		rotate_right();
	if(r & 1)
		move_down();
	if(r & 2)
		move_left();
	if(r & 4)
		move_right();
}

void Player_process_key::keyboard_control() {
	if(last_video_frame == video->framecount && overmind.framecount - last_overmind_frame > 3)
		return;
	if(last_video_frame != video->framecount) {
		last_video_frame = video->framecount;
		last_overmind_frame = overmind.framecount;
	}

	bool bouge_left = false;
	bool bouge_right = false;
	bool auto_bouge = false;
	if(canvas->check_key(2) & RELEASED) {
		if(rotate_left())
			block_rotated++;
		else {
			if(canvas->collide_side_only) {
				int inc;
				if(canvas->bloc->bx<5)
					inc=1;
				else
					inc = -1;
				if(!canvas->collide(canvas->bloc->bx+inc, canvas->bloc->by, canvas->bloc->rot)) {
					canvas->bloc->bx+=inc;
					if(rotate_left()) {
						auto_bouge=true;
						block_rotated++;
					}
					else
						canvas->bloc->bx-=inc;
				}
			}
		}
		canvas->clear_key(2);
	}
	if(canvas->check_key(4) & RELEASED) {
		if(rotate_right())
			block_rotated++;
		else {
			if(canvas->collide_side_only) {
				int inc;
				if(canvas->bloc->bx<5)
					inc=1;
				else
					inc = -1;
				if(!canvas->collide(canvas->bloc->bx+inc, canvas->bloc->by, canvas->bloc->rot)) {
					canvas->bloc->bx+=inc;
					if(rotate_right()) {
						auto_bouge=true;
						block_rotated++;
					}
					else
						canvas->bloc->bx-=inc;
				}
			}
		}
		canvas->clear_key(4);
	}
	if(canvas->check_key(5) & RELEASED) {
		if(rotate_right(true))
			block_rotated++;
		else {
			if(canvas->collide_side_only) {
				int inc;
				if(canvas->bloc->bx<5)
					inc=1;
				else
					inc = -1;
				if(!canvas->collide(canvas->bloc->bx+inc, canvas->bloc->by, canvas->bloc->rot)) {
					canvas->bloc->bx+=inc;
					if(rotate_right(true)) {
						auto_bouge=true;
						block_rotated++;
					}
					else
						canvas->bloc->bx-=inc;
				}
			}
		}
		canvas->clear_key(5);
	}
	if(canvas->check_key(3) & PRESSED || canvas->check_key(3) & RELEASED) {
		move_down();
		canvas->unrelease_key(3);
	}
	if(canvas->check_key(6) & PRESSED) {
		drop_down();
		canvas->clear_key(6);
	}
	if(canvas->check_key(0) & PRESSED || canvas->check_key(0) & RELEASED) {
		if(!bouge_left) {
			bouge_left = true;
			canvas->unrelease_key(0);
		}
	}
	if(canvas->check_key(1) & PRESSED || canvas->check_key(1) & RELEASED) {
		if(!bouge_right) {
			bouge_right = true;
			canvas->unrelease_key(1);
		}
	}
	if(bouge_left && !bouge_right && !auto_bouge) {
		if(hold_left < 2) {
			if(hold_left == 0)
				hold_left = canvas->h_repeat_delay+10;	// initial repeating delay
			else
				hold_left = canvas->h_repeat_delay;
			if(!move_left())
				hold_left=1;
		} else
			hold_left--;
	} else
		hold_left = 0;
	if(bouge_right && !bouge_left && !auto_bouge) {
		if(hold_right < 2) {
			if(hold_right == 0)
				hold_right = canvas->h_repeat_delay+10;
			else
				hold_right = canvas->h_repeat_delay;
			if(!move_right())
				hold_right=1;
		} else
			hold_right--;
	} else
		hold_right = 0;
}

void Player_process_key::init() {
	Player_base::init();
//	canvas->clear_key(2);
//	canvas->clear_key(4);
//	canvas->clear_key(5);
	canvas->clear_key(6);
	if(!canvas->continuous)
		canvas->clear_key(3);
	canvas->start_moves();
}

void Player_process_key::step() {
	Player_base::step();
	if(check_gone())
		return;

	if(canvas->should_remove_bonus)
		remove_bonus();

	int t, i;
	if(game->survivor)
		if(check_first_frag())
			return;
	if(game->paused)
		return;

	time_held++;
	canvas->start_byte();

	if(playback)
		playback_control();
	else {
		if(canvas->inter) // if the canvas is hidden, don't listen to keys
			keyboard_control();
	}

	canvas->write_byte();

	t = ((canvas->bloc->bx-4)<<4)*18; // compute the position where the block should be
	int nx = canvas->bloc->x; // new position 'x' to obtain
	if(nx < t) {
		nx += canvas->side_speed;
		if(nx > t) // if past the position
			nx = t;
	}
	if(nx > t) {
		nx -= canvas->side_speed;
		if(nx < t) // if past the position
			nx = t;
	}
	t = calc_by(canvas->bloc->y-(17<<4)-15);
	if(!canvas->collide(canvas->bloc->bx, t, canvas->bloc->rot))
		canvas->bloc->x = nx; // accepte le deplacement 'smooth' seulement s'il ne passe pas par-dessus un bloc
	t = calc_by(canvas->bloc->y+canvas->speed);
	if(canvas->collide(canvas->bloc->bx, t, canvas->bloc->rot)) {
		canvas->send_p_moves();
		Packet_clientstampblock p;
		p.rotate=canvas->bloc->rot;
		p.x=canvas->bloc->bx;
		p.y=canvas->bloc->by;
		p.player=canvas->num_player;
		if(game->net_version()<23) {
			i = max(overmind.framecount - canvas->frame_start - 50, static_cast<unsigned int>(0));
			p.score=max(0, 100 - i) >> 1;
		}
		else
			p.score=0;
		p.date = canvas->watch_date++;
		if(block_rotated>255)
			block_rotated=255;
		p.block_rotated=block_rotated;
		if(time_held>65535)
			time_held=65535;
		p.time_held=time_held;
		net->sendtcp(&p);
		exec(new Player_stamp(canvas, &p));
		return;
	}
	else {
		canvas->bloc->y+=canvas->speed;
		canvas->bloc->by = calc_by(canvas->bloc->y);
		if(canvas->shadow)
			canvas->calc_shadow();
	}
}

bool Player_process_key::check_first_frag() {
	if(game->net_list.syncpoint==Canvas::PLAYING)
		return false;
	if(game->net_list.syncpoint==Canvas::LAST)
		return false;
	//If syncing toward WAITFORWINNER or WAITFORRESTART,
	//	go to first_frag state
	canvas->send_p_moves();
	Packet_clientfirst_frag pf;
	pf.player = canvas->num_player;
	net->sendtcp(&pf);
	exec(new Player_first_frag(canvas));
	return true;
}

void Player_get_next::step() {
	Player_base::step();
	while(!canvas->bloc)
		shift_next();

	canvas->set_next();
	canvas->frame_start = overmind.framecount;
	ret();
}

void Player_get_next::shift_next() {
	canvas->bloc = canvas->next3;
	canvas->next3 = canvas->next2;
	canvas->next2 = canvas->next;
	Byte the_next;
	if(game->net_version()>=23)
		the_next=canvas->rnd.rnd()%7;
	else
		the_next=canvas->rnd.crap_rnd()%7;
	canvas->next = new Bloc(the_next, -1, 7, 10);
}

Player_normal::Player_normal(Canvas *c): Player_base(c) {
	canvas->idle = 1;
	canvas->state = Canvas::PLAYING;
}

void Player_normal::step() {
	Player_base::step();

	if(!canvas->bloc)
		call(new Player_get_next(canvas));
	else {
		//If this is a local player, check death condition
		if(canvas->islocal()) {
			if(canvas->collide(canvas->bloc->bx, canvas->bloc->by, canvas->bloc->rot)) {
				bool tg=false;
				if(game->single)
					tg=true;
				call(new Player_dead(canvas));
			}
			else {
				call(new Player_process_key(canvas));
			}
		}
		else {
			//Not local, wait for next block or death
			call(new Player_wait_block(canvas));
		}
	}
}

Player_dead::Player_dead(Canvas *c, bool tg): Player_base(c), then_gone(tg) {
	canvas->dying=true;
  for(j=0; j<36; j++)
    for(i=0; i<18; i++) {
			if(canvas->blinded[j][i]) {
				canvas->bflash[j][i] = 24;
				canvas->dirted[j][i] = 2;
			}
			canvas->blinded[j][i] = 0;
		}
	if(canvas->islocal()) {
		//If local, tell server that we died
		Packet_clientdead p;
		p.player=canvas->num_player;
		p.then_gone=then_gone;
		net->sendtcp(&p);
	}
	Canvas* fragger = NULL;
	if(canvas->last_attacker != 255)
		fragger = game->net_list.get(canvas->last_attacker);
	if(fragger)
		msgbox("Player_dead::Player_dead: %s a ete fragger par %s. attacks[]=", canvas->name, fragger->name);
	else
		msgbox("Player_dead::Player_dead: %s est mort sans frag. attacks[]=", canvas->name);
	for(int i2=0; i2<MAXPLAYERS; i2++)
		msgbox("%i,", canvas->attacks[i2]);
	msgbox("\n");
	if(fragger) {
		fragger->stats[CS::FRAG].add(1);
		sprintf(st, "%s fragged %s!", fragger->name, c->name);
		message(fragger->color, st);

		couleur = fragger->color<<4; // couleur du 'fragger'
		sprintf(st, "Fragged %s!", c->name);
		fragger->add_text_scroller(st);
	} else {
		sprintf(st, "%s died.", c->name);
		message(-1, st);
		couleur = 8<<4; // gris pour mort naturelle
	}
	canvas->stats[CS::DEATH].add(1);
	const char *death_type="normal";
	//Suicide: less than 9 unit blocks in the 2 lines right
	//  below the top
	int num_units=0;
	for(i=14; i<16; i++)
		for(j=4; j<14; j++)
			if(canvas->occupied[i][j])
				num_units++;
	bool suicide=false;
	if(num_units<9)
		suicide=true;
	if(then_gone) {
		death_type="left";
		suicide=false;
	}
	//Overkill and maximum overkill
	for(i=0; i<12; i++) {
		num_units=0;
		for(j=4; j<14; j++)
			if(canvas->occupied[i][j])
				num_units++;
		if(num_units>=8)
			break;
	}
	if(i<10 && i>=7) {
		death_type="overkill";
		if(fragger)
			fragger->stats[CS::OVERKILLER].add(1);
		canvas->stats[CS::OVERKILLEE].add(1);
		suicide=false;
	}
	if(i<7) {
		death_type="maximum_overkill";
		if(fragger)
			fragger->stats[CS::MAXOVERKILLER].add(1);
		canvas->stats[CS::MAXOVERKILLEE].add(1);
		suicide=false;
	}
	if(suicide) {
		death_type="suicide";
		canvas->stats[CS::SUICIDES].add(1);
	}
	Packet_serverlog log("player_dead");
	log.add(Packet_serverlog::Var("id", canvas->id()));
	log.add(Packet_serverlog::Var("fragger_id", fragger? fragger->id():0));
	log.add(Packet_serverlog::Var("type", death_type));
	if(game && game->net_server)
		game->net_server->record_packet(&log);
	i = 11;
	j = 4;
	Player_dead::c = 0;
	canvas->delete_bloc();
	canvas->set_next();
}

void Player_dead::step() {
	Player_base::step();

	if(c++&1)
		return;
	if(j < 9) {
		if(canvas->occupied[i][j]) {
			canvas->block[i][j] = (canvas->block[i][j] & 15) + couleur;
			canvas->dirted[i][j] = 2;
		}
		if(canvas->occupied[42-i][j]) {
			canvas->block[42-i][j] = (canvas->block[42-i][j] & 15) + couleur;
			canvas->dirted[42-i][j] = 2;
		}
		if(canvas->occupied[i][17-j]) {
			canvas->block[i][17-j] = (canvas->block[i][17-j] & 15) + couleur;
			canvas->dirted[i][17-j] = 2;
		}
		if(canvas->occupied[42-i][17-j]) {
			canvas->block[42-i][17-j] = (canvas->block[42-i][17-j] & 15) + couleur;
			canvas->dirted[42-i][17-j] = 2;
		}
		int vo = -600;
		if(!canvas->islocal())
			vo -= 1000;

		i++;
		if(i == 22) {
			j++;
			i = 11;
			if(canvas->inter)
				play_sound(sons.flash, vo, -1, 22500+ugs_random.rnd(1023));
		}
	}
	else {
		canvas->dying=false;
		if(then_gone)
			exec(new Player_gone(canvas, true));
		else
			if(canvas->islocal())
				exec(new Player_dead_wait(canvas));
			else
				exec(new Player_wait_respawn(canvas));
	}
}

Player_dead_wait::Player_dead_wait(Canvas *c, bool ab): Player_base(c), add_bonus(ab) {
	if(canvas->inter)
		canvas->clear_key_all();
	canvas->idle = 2;
	canvas->state = Canvas::WAITFORWINNER;
}

void Player_dead_wait::init() {
	Player_base::init();
	if(game->single || game->delay_start)
		return;
	if(game->survivor)
		canvas->set_message("Waiting for the", "        next round...");
	else
		canvas->set_message("Press a key to", "        restart.");
}

void Player_dead_wait::step() {
	Player_base::step();
	if(check_gone())
		return;

	if(canvas->should_remove_bonus)
		remove_bonus();

	bool restart=false;

	if(playback && playback->old_mode) {
		if(playback->get_byte())
			restart = true;
	}
	else {
		if(canvas->inter) {// si canvas invisible, ecoute pas les touches!
			for(int i=0; i<7; i++)
				if(canvas->check_key(i))
					restart = true;
		}
	}

	if(game->survivor) {
		restart = check_first_frag();
	}
	else
		if(game->hot_potato && canvas->color==game->potato_team) {
			//Sorry mister, your buddies are still juggling the hot
			//  potato, you can't go help them yet! :)
			restart=false;
		}
	//Never restart in single-player games
	if(game->single)
		restart=false;
	if(restart) {
		Packet_clientrespawn p;
		p.player=canvas->num_player;
		net->sendtcp(&p);
		canvas->restart();
		ret();
		Packet_serverlog log("player_respawn");
		log.add(Packet_serverlog::Var("id", canvas->id()));
		if(game && game->net_server)
			game->net_server->record_packet(&log);
	}
	else {
		if(canvas->bonus && !canvas->bon[0].blind_time && add_bonus)
			call(new Player_add_bonus(canvas));
	}
}

bool Player_dead_wait::check_first_frag() {
	switch(canvas->state) {
		case Canvas::WAITFORWINNER:
			if(game->net_list.syncpoint==Canvas::WAITFORRESTART) {
				canvas->state = Canvas::WAITFORRESTART;
				Packet_clientstate p;
				p.player = canvas->num_player;
				p.state = canvas->state;
				net->sendtcp(&p);
			}
			break;
		case Canvas::WAITFORRESTART:
			if(game->net_list.syncpoint==Canvas::PLAYING)
				return true;
		case Canvas::PLAYING:
		case Canvas::LAST:
			break;
	}
	return false;
}

Player_first_frag::Player_first_frag(Canvas *c): Player_base(c) {
	canvas->dying=true;
  for(j=0; j<36; j++)
    for(i=0; i<18; i++) {
			if(canvas->blinded[j][i]) {
				canvas->bflash[j][i] = 24;
				canvas->dirted[j][i] = 2;
			}
			canvas->blinded[j][i] = 0;
		}
	couleur = 8<<4;
	i = 11;
	j = 4;
	this->c = 0;
	canvas->delete_bloc();
	canvas->set_next();
	canvas->stats[CS::ROUND_WINS].add(1);
	Packet_serverlog log("player_survived");
	log.add(Packet_serverlog::Var("id", canvas->id()));
	if(game && game->net_server)
		game->net_server->record_packet(&log);
}

void Player_first_frag::step() {
	Player_base::step();

	if(c++&1)
		return;
	if(j < 9) {
		canvas->block[i][j] = 15 + couleur;
		canvas->occupied[i][j] = true;
		canvas->dirted[i][j] = 2;
		canvas->block[42-i][j] = 15 + couleur;
		canvas->occupied[42-i][j] = true;
		canvas->dirted[42-i][j] = 2;
		canvas->block[i][17-j] = 15 + couleur;
		canvas->occupied[i][17-j] = true;
		canvas->dirted[i][17-j] = 2;
		canvas->block[42-i][17-j] = 15 + couleur;
		canvas->occupied[42-i][17-j] = true;
		canvas->dirted[42-i][17-j] = 2;
		int vo = -600;
		if(!canvas->islocal())
			vo -= 1000;

		i++;
		if(i == 22) {
			j++;
			i = 11;
			if(canvas->inter)
				play_sound(sons.flash, vo, 1000, 32500+ugs_random.rnd(1023));
		}
	} else {
		canvas->dying=false;
		Player_base *pb;
		if(canvas->islocal()) {
			pb=new Player_dead_wait(canvas, false);
		}
		else {
			pb=new Player_wait_respawn(canvas, false);
		}
		exec(pb);
	}
}

Player_gone::Player_gone(Canvas *c, bool chat_msg): Player_base(c) {
	canvas->local_player = false; // enleve le statut de 'local' a un joueur gone
	canvas->idle = 3;
	canvas->state = Canvas::LAST;

	canvas->delete_bloc();
	canvas->set_next();
	if(chat_msg) {
		sprintf(st, "%s has left the game.", canvas->name);
		message(-1, st);
	}
	Packet_serverlog log("player_gone");
	log.add(Packet_serverlog::Var("id", canvas->id()));
	if(game && game->net_server)
		game->net_server->record_packet(&log);
}

void Player_gone::step() {
	Player_base::step();
	if(canvas->islocal()) {
		if(canvas->should_remove_bonus)
			remove_bonus();
	}
	else {
		check_bonus();
	}
	canvas->gone_time++;

	{
		Packet_dead *p=(Packet_dead *) game->peekpacket(P_DEAD);
		if(p && p->player==canvas->num_player && p->then_gone)
			game->removepacket(); //Remove superfluous gone packet sent by server if client disconnects too fast
	}

	{
		Packet_gone *p=(Packet_gone *) game->peekpacket(P_GONE);
		if(p && p->player==canvas->num_player)
			game->removepacket(); //Remove superfluous gone packet sent by server if client disconnects too fast
	}

	Packet_rejoin *p=(Packet_rejoin *) game->peekpacket(P_REJOIN);
	if(p && p->player==canvas->num_player) {
		canvas->h_repeat = p->h_repeat;
		canvas->v_repeat = p->v_repeat;
		canvas->smooth = p->smooth? true:false;
		canvas->shadow = p->shadow? true:false;
		canvas->handicap = p->handicap;
		sprintf(st, "%s has joined back!", canvas->long_name(true, false));
		message(-1, st);
		msgbox("Player_gone::step: player %i is no longer a goner! Rejoining.\n", p->player);
		game->removepacket();

		game->net_list.rejoin_player(canvas);
		canvas->delete_bloc();
		canvas->reinit();
		canvas->restart();
		game->net_list.notify_all();
		if(game->net_version()>=23 && game->survivor) {
			if(canvas->islocal())
				exec(new Player_dead_wait(canvas));
			else
				exec(new Player_wait_respawn(canvas));
		}
		ret();
	}
}

Player_wait_block::Player_wait_block(Canvas *c): Player_base(c) {
	canvas->idle = 1;
	canvas->state = Canvas::PLAYING;
	move_index=0;
}

Player_wait_block::~Player_wait_block() {
	int total_left=0;
	if(move_index<canvas->da_moves.size())
		total_left+=canvas->da_moves.size()-move_index;
	if(total_left)
		msgbox("Player_wait_block::deletemoves: moves not exhausted, %i left\n", total_left);
	canvas->da_moves.resize(0);
}

void Player_wait_block::init() {
	Player_base::init();
}

void Player_wait_block::step() {
	Player_base::step();
	bool wait_more=false;
	if(playback && !playback->old_mode) {
		int remain=canvas->da_moves.size()-move_index;
		if(remain && remain<=50)
			wait_more=true;
	}

	if(!wait_more)
		if(check_gone())
			return;

	check_bonus();

	if(!wait_more)
		if(game->survivor)
			if(check_first_frag())
				return;

	if(!(playback && playback->old_mode)) {
		{
			Packet_moves *p=(Packet_moves *) game->peekpacket(P_MOVES);
			if(p && p->player==canvas->num_player) {
				if(canvas->da_moves.size()+p->size<4000)
					canvas->da_moves.append(p->moves, p->size);
				game->removepacket();
			}
		}
		if(move_index<canvas->da_moves.size() && !game->paused) {
			Byte r = canvas->da_moves[move_index++];
			if(r & 8)
				rotate_left();
			if(r & 16)
				rotate_right();
			if(r & 32)
				rotate_right(true);
			if(r & 1)
				move_down();
			if(r & 64)
				drop_down();
			if(r & 2)
				move_left();
			if(r & 4)
				move_right();

			int t;
			t = ((canvas->bloc->bx-4)<<4)*18; // calcul la position ou le bloc devrait etre
			int nx = canvas->bloc->x; // nouvelle position 'x' a obtenir
			if(nx < t) {
				nx += canvas->side_speed;
				if(nx > t) // si depasse la position
					nx = t;
			}
			if(nx > t) {
				nx -= canvas->side_speed;
				if(nx < t) // si depasse la position
					nx = t;
			}
			t = calc_by(canvas->bloc->y-(17<<4)-15);
			if(!canvas->collide(canvas->bloc->bx, t, canvas->bloc->rot))
				canvas->bloc->x = nx; // accepte le deplacement 'smooth' seulement s'il ne passe pas par-dessus un bloc
			t = calc_by(canvas->bloc->y+canvas->speed);
			if(!canvas->collide(canvas->bloc->bx, t, canvas->bloc->rot)) {
				canvas->bloc->y+=canvas->speed;
				canvas->bloc->by = calc_by(canvas->bloc->y);
			}
			if(canvas->shadow)
				canvas->calc_shadow();
		}
	}

	if(wait_more)
		return;

	//Check whether this player placed a block
	Packet_stampblock *p=(Packet_stampblock *) game->peekpacket(P_STAMPBLOCK);
	if(p && p->player==canvas->num_player) {
		exec(new Player_stamp(canvas, p));
		game->removepacket();
		return;
	}

	//Check whether this player died
	Packet_dead *pd=(Packet_dead *) game->peekpacket(P_DEAD);
	if(pd && pd->player==canvas->num_player) {
		exec(new Player_dead(canvas, pd->then_gone));
		game->removepacket();
		return;
	}
}

bool Player_wait_block::check_first_frag() {
	Packet_first_frag *p=(Packet_first_frag *) game->peekpacket(P_FIRST_FRAG);
	if(p && p->player==canvas->num_player) {
		msgbox("Player_wait_block::check_first_frag: recu P_FIRST_FRAG: %s gagnant!\n", canvas->name);
		exec(new Player_first_frag(canvas));
		game->removepacket();
		return true;
	}
	return false;
}

Player_stamp::Player_stamp(Canvas *c, Packet_stampblock *p): Player_base(c) {
	/* Flash blinded blocks when stamping
	int i, j;
  for(j=0; j<36; j++)
    for(i=0; i<18; i++)
			if(canvas->blinded[j][i]) {
				canvas->bflash[j][i] = 8;
			}
	*/
	int i, j;
	//Reset Canvas::moved
	for(j=0; j<36; j++)
		for(i=0; i<18; i++)
			canvas->moved[j][i]=false;
	canvas->idle = 0;
	canvas->potato_team_on_last_stamp=game->potato_team;
	canvas->bloc->rot=p->rotate;
	canvas->bloc->bx=p->x;
	canvas->bloc->by=p->y;
	if(canvas->collide(p->x, p->y, p->rotate) || !canvas->collide(p->x, p->y+1, p->rotate)) {
		char st[256];
		sprintf(st, "*** %s dropped an invalid block! ***", canvas->name);
		if(game->server) {
			message(-1, st, true, false, true);
			game->net_list.server_drop_player(canvas->num_player, DROP_INVALID_BLOCK);
			Net_connection *nc=canvas->remote_adr;
			send_msg(nc, "You have been dropped because you placed an invalid block. This may be due to a bug in the game or a network failure.");
			send_msg(nc, "Quitting and rejoining the game should fix the problem. If not, please contact support@ludusdesign.com.");
		}
		msgbox("%s\n", st);
	}
	canvas->stats[CS::COMPTE0+canvas->bloc->quel].add(1);
	canvas->stats[CS::COMPTETOT].add(1);
	canvas->stats[CS::ROTATED0+canvas->bloc->quel].add(p->block_rotated);
	canvas->stats[CS::ROTATEDTOT].add(p->block_rotated);
	canvas->stats[CS::SCORE].add(p->score);
	if(game->net_version()>=23)
		canvas->stats[CS::PLAYING_TIME].add(p->time_held);

	Packet_serverlog log("player_stampblock");
	log.add(Packet_serverlog::Var("id", canvas->id()));
	log.add(Packet_serverlog::Var("block", canvas->bloc->quel));
	log.add(Packet_serverlog::Var("times_rotated", p->block_rotated));
	log.add(Packet_serverlog::Var("time_held", p->time_held));
	log.add(Packet_serverlog::Var("points", p->score));
	if(game && game->net_server)
		game->net_server->record_packet(&log);

	canvas->watch_date = p->date;
	stamp_bloc();
	if(canvas->bonus && !canvas->bon[0].blind_time)
		addbonus=new Player_add_bonus(canvas);
	else
		addbonus=NULL;

	for(i=0; i<MAXPLAYERS; i++) {
		if(canvas->attacks[i] > 0) {
			canvas->attacks[i]--;
			if(canvas->attacks[i] == 0 && canvas->last_attacker == i) // si c'etait lui le last_attacker,
				if(!game->survivor)
					canvas->last_attacker = 255; // on l'oublie.
		}

		// new handicap code for net_version >= 24
		Canvas* other_canvas = game->net_list.get(i);
		if(other_canvas) {
			int diff=0;
			if(canvas->handicap>other_canvas->handicap)
				diff=canvas->handicap-other_canvas->handicap;
			if(canvas->handicaps[i] < diff*Canvas::stamp_per_handicap)
				canvas->handicaps[i]++;
		}
	}

	// adjust handicap_crowd considering crowdedness of the game (i.e. number of players alive)
	int max_handicap_crowd = max(0, int(game->net_list.count_alive())-4);
	max_handicap_crowd *= Canvas::stamp_per_handicap;
	if(canvas->handicap_crowd < max_handicap_crowd)
		++canvas->handicap_crowd;
	else
		canvas->handicap_crowd = max_handicap_crowd;
}

void Player_stamp::init() {
	if(addbonus) {
		call(new Player_check_line(canvas));
		call(addbonus);
	}
	call(new Player_check_line(canvas));
	ret();
}

void Player_stamp::stamp_bloc() {
	int i,j;
	Byte t;
	Byte blindness=0;
	if(canvas->bonus && canvas->bon[0].blind_time) {
		blindness=canvas->bon[0].blind_time;
		int y;
		for(y = 1; y < canvas->bonus; y++)
			canvas->bon[y-1] = canvas->bon[y];
		canvas->bonus--;
	}
	for(j = 0; j < 4; j++)
		for(i = 0; i < 4; i++) {
			t = canvas->bloc->bloc[canvas->bloc->quel][canvas->bloc->rot][j][i];
			if(t) {
				canvas->block[canvas->bloc->by+j][canvas->bloc->bx+i] = t + (canvas->bloc->quel<<4);
				canvas->occupied[canvas->bloc->by+j][canvas->bloc->bx+i] = true;
				canvas->blinded[canvas->bloc->by+j][canvas->bloc->bx+i] = blindness;
				canvas->bflash[canvas->bloc->by+j][canvas->bloc->bx+i] = blindness? 16:0;
				canvas->dirted[canvas->bloc->by+j][canvas->bloc->bx+i] = 2;
				canvas->last_x = i;
				canvas->moved[canvas->bloc->by+j][canvas->bloc->bx+i] = true;
			}
		}
	canvas->last_x += canvas->bloc->bx;
	int startline=0;
	for(j = 0; j < 32; ++j)
		for(i = 4; i < 14; ++i)
			if(canvas->occupied[j][i]) {
				startline = j;
				// break out of both loops
				j=32;
				break;
			}
	canvas->snapshot[0]=0;
	for(j = startline; j < 32; j++)
		for(i = 4; i < 14; i++) {
			if(canvas->occupied[j][i]) {
				char bl[3];
				Byte color;
				if(canvas->moved[j][i])
					color=8;
				else
					color=canvas->block[j][i]>>4;
				Byte side=canvas->block[j][i]&15;
				sprintf(bl, "%c%c", '0'+color, 'a'+side);
				strcat(canvas->snapshot, bl);
			}
			else
				strcat(canvas->snapshot, "e0");
		}
	i = (canvas->bloc->bx-9) * 300;
	Sample *dep;
	switch(ugs_random.rnd()%3) {
		case 0: dep = sons.depose;
			break;
		case 1: dep = sons.depose2;
			break;
		default: dep = sons.depose3;
			break;
	}
	if(canvas->inter) {
		int vo = -200 - ugs_random.rnd(255);
		if(!canvas->islocal())
			vo -= 1000;
		play_sound(dep, vo, i, 10500+ugs_random.rnd(1023));
	}

	delete canvas->bloc;
	canvas->bloc = NULL;
	if(canvas->bloc_shadow) {
		delete canvas->bloc_shadow;
		canvas->bloc_shadow = NULL;
	}
}

Player_wait_respawn::Player_wait_respawn(Canvas *c, bool ab): Player_base(c), add_bonus(ab) {
	canvas->idle = 2;
	canvas->state = Canvas::WAITFORWINNER;
}

void Player_wait_respawn::step() {
	Player_base::step();
	if(check_gone())
		return;

	check_bonus();

	check_state();

	Packet_respawn *p=(Packet_respawn *) game->peekpacket(P_RESPAWN);
	if(p && p->player==canvas->num_player) {
		canvas->restart();
		ret();
		game->removepacket();
		Packet_serverlog log("player_respawn");
		log.add(Packet_serverlog::Var("id", canvas->id()));
		if(game && game->net_server)
			game->net_server->record_packet(&log);
	} else {
		if(canvas->bonus && !canvas->bon[0].blind_time && add_bonus)
			call(new Player_add_bonus(canvas));
	}
}

Player_init::Player_init(Canvas *canvas): Player_base(canvas) {
	canvas->restart();
	if(game->net_version()>=23 && game->survivor)
		canvas->idle = 2;
	else
		canvas->idle = 1;
	canvas->state = Canvas::PLAYING;
	if(canvas->wait_download)
		net->addwatch(P_DOWNLOAD, this);
}

void Player_init::step() {
	Player_base::step();
	//Don't start until we got our download packet
	if(canvas->wait_download)
		return;
	Byte idle=canvas->idle; //Cause Player_normal::Player_normal sets it
	exec(new Player_normal(canvas));
	if(idle == 2) {
		if(canvas->islocal())
			call(new Player_dead_wait(canvas));
		else
			call(new Player_wait_respawn(canvas));
	}
	if(idle == 3) {
		call(new Player_gone(canvas, false));
		canvas->remote_adr = NULL; // empeche de watcher un joueur qui est 'gone' lorsqu'on join
	}
}

void Player_init::net_call(Packet *p2) {
	Packet_download *p=(Packet_download *) p2;
	{
		Canvas *c = game->net_list.get(p->player);
		if(!c || c!=canvas) {
			msgbox("Player_init::net_call: got a P_DOWNLOAD but canvas does not exist or not even mine, something screwy is going on\n");
			delete p2;
			return;
		}
	}
	for(int y=0; y<32; y++)
		for(int x=0; x<10; x++) {
			canvas->block[y][x+4] = p->can[y][x];
			canvas->occupied[y][x+4] = p->occ[y][x];
			canvas->blinded[y][x+4] = p->blinded[y][x];
		}
	canvas->rnd.set_seed(p->seed);
	if(p->bloc != 255) {
		canvas->bloc = new Bloc(p->bloc, -1, 7, 10);
		canvas->next = new Bloc(p->next, -1, 7, 10);
		canvas->next2 = new Bloc(p->next2, -1, 7, 10);
		canvas->next3 = new Bloc(p->next3, -1, 7, 10);
	}
	canvas->bonus = p->bonus;
	int i;
	for(i=0; i<20; i++) {
		canvas->bon[i].x = p->bon[i].x;
		canvas->bon[i].color = p->bon[i].color;
		canvas->bon[i].blind_time = p->bon[i].blind_time;
		canvas->bon[i].hole_pos = p->bon[i].hole_pos;
		canvas->bon[i].final = p->bon[i].final;
	}
	for(i=0; i<MAXPLAYERS; i++)
		canvas->attacks[i] = p->attacks[i];
	canvas->last_attacker = p->last_attacker;
	canvas->idle = p->idle;
	canvas->state = (Canvas::State) p->state;
	msgbox("Player_init::net_call: name=%s, idle=%i, bonus=%i, bloc=%i\n", canvas->name, canvas->idle, canvas->bonus, p->bloc);
	delete p2;
	canvas->wait_download=false;
	net->removewatch(P_DOWNLOAD, this);
}

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
#endif
#ifdef UGS_LINUX
	struct passwd *pw = NULL;

	pw = getpwuid(getuid());
	if(pw) {
		strcpy(quadradir, pw->pw_dir);
		strcat(quadradir, "/.quadra");
		mkdir(quadradir, 0777);
		chown(quadradir, pw->pw_uid, pw->pw_gid);
	}
#endif
}

static void init_stuff(bool need_sound, bool need_video) {
	int i;

	if (!need_video) {
		video_is_dumb = true;
		SDL_putenv("SDL_VIDEODRIVER=dummy");
	}

  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

	video = Video::New();

	if(!video)
		fatal_msgbox("Could not initialize video subsystem");

	fonts.init();
	//If we init a dumb video, we need a dumb input too
	input = Input::New(!need_video);
  // don't need sound if no video
	if(need_sound && need_video)
		sound = Sound::New();
	else
		sound = NULL;

	for(i=0; i<256; i++)
		noir.setcolor(i, 40, 40, 40);

	chat_text = new Chat_text(fonts.normal, 212);
	net_starter = new Net_starter();
  sons.pause = new Sample(Res_doze("cuckoo.wav"));
  sons.start = new Sample(Res_doze("hooter03.wav"));
  sons.bonus1 = new Sample(Res_doze("Whizz1.wav"));
  sons.levelup = new Sample(Res_doze("glissup.wav"));
  // when the canvas 'flows'
  sons.depose4 = new Sample(Res_doze("Clang3.wav"));
	sons.flash = NULL;
	sons.depose3 = NULL;
	sons.depose2 = NULL;
	sons.depose = NULL;
	sons.drip = NULL;
  sons.glass = new Sample(Res_doze("Glass01.wav"));
  sons.enter = new Sample(Res_doze("Tapdrip.wav"));
  sons.fadein = new Sample(Res_doze("W_BAYO_0.wav"));
  sons.fadeout = new Sample(Res_doze("fadeout.wav"));
  sons.point = new Sample(Res_doze("click_1.wav"));
  sons.click = new Sample(Res_doze("Blip1.wav"));
  sons.msg = new Sample(Res_doze("handbell.wav"));
  sons.potato_get = new Sample(Res_doze("potato_get.wav"));
  sons.potato_rid = new Sample(Res_doze("zingle.wav"));
  sons.minute = new Sample(Res_doze("t1min.wav"));
  sons.thirty = new Sample(Res_doze("t30sec.wav"));
  sons.twenty = new Sample(Res_doze("t20sec.wav"));
  sons.ten = new Sample(Res_doze("t10sec.wav"));
  sons.five = new Sample(Res_doze("t5sec.wav"));
  sons.four = new Sample(Res_doze("t4sec.wav"));
  sons.three = new Sample(Res_doze("t3sec.wav"));
  sons.two = new Sample(Res_doze("t2sec.wav"));
  sons.one = new Sample(Res_doze("t1sec.wav"));
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
	if(!temp)
		fatal_msgbox("Command line parameter not found for '%s'", t);
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
	char st[4096];
	Res_doze cmdline("help_en.txt");
	Dword size = min(static_cast<Dword>(sizeof(st)-1), cmdline.size());
	strncpy(st, (char *)cmdline.buf(), size);
	st[size] = 0;
	if(video)
		delete video;
	user_output("Quadra - Command line", st);
}

void read_script(const char *fn, bool second=false) {
	char st[32768];
	Res_dos script(fn);
	if(script.exist) {
		//-2 because Stringtable is strange
		Dword size = min(static_cast<Dword>(sizeof(st)-2), script.size());
		strncpy(st, (char *)script.buf(), size);
		st[size] = 0;
		Stringtable str((Byte *)st, size);
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

void start_game() {
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
	int i;
	for(i=0; i<MAXTEAMS; i++)
		set_team_name(i, NULL);

	if(command.token("h help ?")) {
		display_command_line_help();
		delete resmanager;
		return;
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

	Dword last=0;
	Dword acc=0;
	Executor *menu = new Executor();
	//Add Menu_intro so we get back there after -connect, -server or -play
	//  unless -thenquit option si specified
	if(!command.token("thenquit") && !demo_verif)
			menu->add(new Menu_intro());

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
				Dword time=atoi(temp);
				time=min(max(time, static_cast<Dword>(0)), static_cast<Dword>(255));
				p.normal_attack.param=time;
				p.clean_attack.param=time;
			}
			if(command.token("fullblind")) {
				p.set_preset(PRESET_FULLBLIND);
				const char *temp = command_get_param("fullblind <n>", "12");
				Dword time=atoi(temp);
				time=min(max(time, static_cast<Dword>(0)), static_cast<Dword>(255));
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
				strncpy(buf, temp, sizeof(buf) - 1);
				p.name=buf;
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
			menu->add(new Menu_startserver());
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
				menu->add(new Menu_startconnect(buf, false));
				if(config.warning)
					menu->add(new Menu_setup());
			}
		}
	}
	else {
		Res_compress *res = new Res_compress(buf, RES_TRY);
		if(res->exist) {
			menu->add(new Demo_multi_player(res));
			// le 'delete res' est fait par ~Demo_multi_player
			if(playback)
				playback->set_verification_flag(&demo_verified_and_valid);
		}
		else {
			msgbox("Unable to open demo '%s'\n", buf);
			delete res;
		}
	}

	overmind.start(menu);
	bool reset_time=false;
	while(!menu->done) {
		last=getmsec();
		if(demo_verif) {
			acc=500;
			while(acc--)
				overmind.step();
		}
		else {
			while(acc>=10) {
				if(reset_time) { // remet 'normal' seulement si au moins 1 frame s'est ecoule
					time_control = TIME_NORMAL;
					reset_time = false;
				}
				acc-=10;
				try {
					overmind.step();
				}
				catch(std::exception *e) {
					msgbox("Exception caught from overmind.step(): %s\n", e->what());
				}
				reset_time=true;
				if(time_control == TIME_FREEZE)
					break;
			}
		}
		start_frame();
		if(ecran && !video_is_dumb) {
			try {
				ecran->draw_zone();
			}
			catch(std::exception *e) {
				msgbox("Exception caught from ecran->draw_zone(): %s\n", e->what());
			}

#ifdef FRAMECOUNTER
			static Dword lastvideoframe=0, lastoverframe=0;
			if(ecran->font) {
				if(overmind.framecount-lastoverframe > 500) {
					lastoverframe = overmind.framecount;
					lastvideoframe = video->framecount;
				}
				int up = 999;
				if(overmind.framecount-lastoverframe > 0)
					up = ((video->framecount-lastvideoframe) * 100) / (overmind.framecount-lastoverframe);
				video->vb->rect(0,0,50,20,0);
				char st[80];
				sprintf(st, "%i", up);
				ecran->font->draw(st, video->vb, 0, 0);
			}
#endif /* FRAMECOUNTER */
		}
		end_frame();

#ifndef NDEBUG
    Uint8 *keystate = SDL_GetKeyState(NULL);
    if(keystate[SDLK_F8]) // F8 = buckage
			for(int j=0; j<8000000; j++)
				;
    if(keystate[SDLK_F9]) // F8 = slow motion mode
			time_control = TIME_SLOW;
    if(keystate[SDLK_F10]) // F8 = turbo mode
			time_control = TIME_FAST;
#endif

		switch(time_control) {
		case TIME_FREEZE: acc = 10; break;
		case TIME_SLOW: acc += 1; break;
		case TIME_FAST: acc += 80; break;
		default: acc+=getmsec()-last;
		}
		if(acc > 300 && !video_is_dumb) {
			overmind.framecount+=acc-300;
			acc = 300; // pour eviter trop de depassement
		}
		if(acc > 10000) {
			msgbox("Not enough CPU time to be server!\n");
			overmind.framecount+=acc-10;
			acc=10;
		}
	}
	delete menu;

	deinit_stuff();
	delete resmanager;

	// FIXME: This is not the smoothest exit ever, a better way?
	if(demo_verif)
		exit(demo_verified_and_valid? 0 : 1);
}
