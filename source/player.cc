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

#include "player.h"

#include "canvas.h"
#include "chat_text.h"
#include "input.h"
#include "main.h"
#include "misc.h"
#include "net_server.h"
#include "recording.h"
#include "sons.h"
#include "sound.h"
#include "texte.h"
#include "zone.h"

using std::max;


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

uint8_t Player_base::calc_by(int py) const {
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

void Player_base::play_sound(Sample *s, int vol, int pan, int freq) {
	if(time_control == TIME_SLOW)
		freq = freq*2/3;
	if(time_control == TIME_FAST)
		freq = freq*3/2;
	s->play(vol, pan, freq);
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
#if 0
				// TODO: Not used, but should it?
				bool tg=false;
				if(game->single)
					tg=true;
#endif
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
	uint8_t the_next;
	if(game->net_version()>=23)
		the_next=canvas->rnd.rnd()%7;
	else
		the_next=canvas->rnd.crap_rnd()%7;
	canvas->next = new Bloc(the_next, -1, 7, 10);
}

Player_process_key::Player_process_key(Canvas *c): Player_base(c) {
	hold_left = hold_right = 0;
	last_video_frame = last_overmind_frame = 0;
	canvas->idle = 1;
	canvas->state = Canvas::PLAYING;
	block_rotated=0;
	time_held=0;
}

void Player_process_key::playback_control() {
	uint8_t r = playback->get_byte();
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
	canvas->add_text_scroller(ST_CLEANCANVAS);
	if(game->net_version()<23)
		canvas->stats[CS::SCORE].add(5000);
	canvas->send_for_clean=true;
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
			uint16_t hole_pos=canvas->bon[0].hole_pos;
			if(old_net_version)
				hole_pos=(1 << (9-(canvas->bon[0].x-4)));
			uint8_t top_bottom_add=0;
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

void Player_check_link::fill_bloc(uint8_t x, uint8_t y) {
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
	uint8_t idle=canvas->idle; //Cause Player_normal::Player_normal sets it
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
		canvas->set_message(ST_WAITINGFORROUND1, ST_WAITINGFORROUND2);
	else
		canvas->set_message(ST_WAITINGFORKEY1, ST_WAITINGFORKEY2);
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
		sprintf(st, ST_BOBFRAGBOB, fragger->name, c->name);
		message(fragger->color, st);

		couleur = fragger->color<<4; // couleur du 'fragger'
		sprintf(st, ST_YOUFRAGBOB, c->name);
		fragger->add_text_scroller(st);
	} else {
		sprintf(st, ST_BOBDIED, c->name);
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
			uint8_t r = canvas->da_moves[move_index++];
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
		sprintf(st, ST_BOBINVALIDBLOCK, canvas->name);
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
	uint8_t t;
	uint8_t blindness=0;
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
				uint8_t color;
				if(canvas->moved[j][i])
					color=8;
				else
					color=canvas->block[j][i]>>4;
				uint8_t side=canvas->block[j][i]&15;
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

Player_gone::Player_gone(Canvas *c, bool chat_msg): Player_base(c) {
	canvas->local_player = false; // enleve le statut de 'local' a un joueur gone
	canvas->idle = 3;
	canvas->state = Canvas::LAST;

	canvas->delete_bloc();
	canvas->set_next();
	if(chat_msg) {
		sprintf(st, ST_BOBHASGONE, canvas->name);
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
		sprintf(st, ST_BOBREJOIN, canvas->long_name(true, false));
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
