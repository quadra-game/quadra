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

#include "canvas.h"

#include <stdio.h>
#include "input.h"
#include "random.h"
#include "bloc.h"
#include "quadra.h"
#include "image_png.h"
#include "cfgfile.h"
#include "res.h"
#include "zone.h"
#include "game.h"
#include "global.h"
#include "sons.h"
#include "recording.h"
#include "chat_text.h"
#include "nglog.h"
#include "net_server.h"
#include "packets.h"

using std::max;
using std::min;

Canvas::Canvas(int qplayer, int game_seed, Palette *p):
  bit(NULL), //Somebody somewhere will set that. Sucks.
  rnd(game_seed),
  sprlevel_up(NULL) {
// constructs a local Canvas
	snapshot[0]=0;
	best_move=best_clean=best_recurse=0;
	memset(player_hash, 0, sizeof(player_hash));
	memset(team_hash, 0, sizeof(team_hash));
	collide_side_only=false;
	should_remove_bonus=false;
	wait_download=false;
	z_lines=z_potatolines=z_linestot=z_potatolinestot=NULL;
	team_potato_lines=team_potato_linestot=0;
	send_for_clean=false;
	handicap_crowd=0;
	potato_team_on_last_stamp=255;
	potato_lines=0;
	gone_time=0;
	inter=NULL;
	h_repeat = v_repeat = 0;
	smooth = shadow = false;
	moves=NULL;
  pal = p;
  player = qplayer;
	if(playback) {
		color = playback->player[player].color;
		strcpy(name, playback->player[player].name);
		continuous=1;
	} else {
		handicap = config.player2[player].handicap;
		color = config.player[player].color;
		strcpy(name, config.player[player].name);
		strcpy(team_name, config.player2[player].ngTeam);
		continuous=config.player2[player].continuous;
	}
  remote_adr=NULL;
	local_player = true;
	hide();
  init();
}

Canvas::Canvas(int game_seed, Byte team, const char *nam, int ph_repeat,
               int pv_repeat, bool psmooth, bool pshadow, int phandicap,
               Net_connection *adr, int qplayer, bool wait_down):
  bit(NULL),
  rnd(game_seed),
  sprlevel_up(NULL) {
// constructs a remote Canvas
	snapshot[0]=0;
	best_move=best_clean=best_recurse=0;
	memset(player_hash, 0, sizeof(player_hash));
	memset(team_hash, 0, sizeof(team_hash));
	collide_side_only=false;
	should_remove_bonus=false;
	wait_download=wait_down;
	z_lines=z_potatolines=z_linestot=z_potatolinestot=NULL;
	team_potato_lines=team_potato_linestot=0;
	send_for_clean=false;
	handicap_crowd=0;
	potato_team_on_last_stamp=255;
	potato_lines=0;
	gone_time=0;
	inter=NULL;
	handicap = phandicap;
	h_repeat = ph_repeat;
	v_repeat = pv_repeat;
	continuous=1; //Doesn't matter for remote canvas
	smooth = psmooth;
	shadow = pshadow;
	moves=NULL;
  pal = NULL;
  player = qplayer;
  color = team;
	strncpy(name, nam, sizeof(name));
	name[sizeof(name)-1]=0;
  remote_adr=adr;
	local_player = false;
  hide();
  init();
}

Canvas::~Canvas() {
  delete over;
  delete myself;
	if(moves)
		delete moves;
  delete_bloc();
  if (sprlevel_up)
    SDL_FreeSurface(sprlevel_up);
	watchers.deleteall();
}

char *Canvas::long_name(bool handi, bool gone) {
	static char ret[64];
	strcpy(ret, name);
	if(handi) {
		const char *h="";
		switch(handicap) {
			case 0: h=" (-)"; break;
			case 1: h=" (A)"; break;
			case 3: h=" (M)"; break;
			case 4: h=" (+)"; break;
		}
		strcat(ret, h);
	}
	if(gone)
		if(idle==3)
			strcat(ret, " *");
	return ret;
}

void Canvas::delete_bloc() {
  if(bloc)
    delete bloc;
  if(next)
    delete next;
  if(next2)
    delete next2;
  if(next3)
    delete next3;
  if(bloc_shadow)
    delete bloc_shadow;
  bloc = next = next2 = next3 = bloc_shadow = NULL;
}

void Canvas::init() {
	trying_to_drop=false;
  {
    Res_doze res("gamelvup.png");
    Png raw(res);
    sprlevel_up = raw.new_surface();
    SDL_SetColorKey(sprlevel_up, SDL_SRCCOLORKEY, 0);
  }
  over = new Overmind();
  bloc = next = next2 = next3 = bloc_shadow = NULL;
	reinit();
  myself = new Executor();
  myself->add(new Player_init(this));
  over->start(myself);
}

void Canvas::reinit() {
	set_message("", "");
  int i,j;
  watch_date = 0;
  for(j=0; j<36; j++)
    for(i=0; i<18; i++) {
			occupied[j][i] = false;
      block[j][i] = 0;
			blinded[j][i] = 0;
			bflash[j][i] = 0;
		}
  for(i=0; i<20; i++)
    flash[i] = 0;
  if(islocal()) {
		if(playback) {
			//playback->multi_mode is certainly false since this canvas
			//  is local. Thus, we don't care about handicap
			h_repeat = v_repeat = playback->player[player].repeat;
			continuous = 1;
			smooth = playback->player[player].smooth ? true:false;
			shadow = playback->player[player].shadow ? true:false;
		} else {
			handicap = config.player2[player].handicap;
			h_repeat = config.player2[player].h_repeat;
			v_repeat = config.player2[player].v_repeat;
			continuous = config.player2[player].continuous;
			smooth = config.player[player].smooth ? true:false;
			shadow = config.player[player].shadow ? true:false;
		}
  }
	if(game->normal_attack.type==ATTACK_BLIND || game->normal_attack.type==ATTACK_FULLBLIND)
		shadow=true;
	if(game->potato_normal_attack.type==ATTACK_BLIND || game->potato_normal_attack.type==ATTACK_FULLBLIND)
		shadow=true;
  switch(h_repeat) {
    case 0:
      h_repeat_delay = 11;
      break;
    case 1:
      h_repeat_delay = 6;
      break;
    case 2:
      h_repeat_delay = 3;
      break;
    case 3:
      h_repeat_delay = 1;
      break;
  }
  side_speed = (18<<4)/h_repeat_delay;
  switch(v_repeat) {
    case 0:
      v_repeat_delay = 11;
      break;
    case 1:
      v_repeat_delay = 6;
      break;
    case 2:
      v_repeat_delay = 3;
      break;
    case 3:
      v_repeat_delay = 1;
      break;
  }
  down_speed = (340)/v_repeat_delay;
  if(down_speed > 180)
    down_speed = 180;
  level = game->level_start;
  depth = complexity = bonus = 0;
	stats[LINESCUR].set_value(0);
  level_up = color_flash = 0;
  over->framecount = 0; // initialize the counter when the player starts
	if(game->net_version()>=23 && game->survivor)
		idle = 2;
	else
		idle = 1;
	dying = false;
	state = PLAYING;
	handicap_crowd=0;
	potato_team_on_last_stamp=255;
	send_for_clean=false;
	should_remove_bonus=false;
}

void Canvas::restart() {
  if(islocal())
    clear_key_all();
  init_block();
  clear_tmp();
  delete_bloc();
  set_next();
  last_attacker = 255;
  for(int i=0; i<MAXPLAYERS; i++) {
    attacks[i] = 0;
		handicaps[i] = 0;
	}
  level = game->level_start;
  depth = complexity = bonus = 0;
	stats[LINESCUR].set_value(0);
  level_up = color_flash = 0;
  calc_speed();
  idle = 1; // starts 'idle' to allow joins if the game is on pause and the player just started
	state = PLAYING;
	dying=false;
	handicap_crowd=0;
	potato_team_on_last_stamp=255;
	send_for_clean=false;
	should_remove_bonus=false;
	set_message("", "");
}

void Canvas::clear_key_all() {
  for(int i=0; i<7; i++) // clears the key states of the player
    clear_key(i);
}

void Canvas::calc_shadow() {
  if(!bloc_shadow)
    bloc_shadow = new Bloc(bloc->quel, 8, 0, 0);
  bloc_shadow->rot = bloc->rot;
  bloc_shadow->bx = bloc->bx;
  bloc_shadow->by = bloc->by;
  while(!check_collide(bloc_shadow, bloc_shadow->bx, bloc_shadow->by+1, bloc_shadow->rot))
    bloc_shadow->by++;
  bloc_shadow->calc_xy();
  //bloc_shadow->x = bloc->x;
}

void Canvas::init_block() {
  int x, y;
  for(y=32; y < 36; y++)
    for(x = 0; x < 18; x++)
      occupied[y][x] = true;
  for(y = 0; y < 32; y++) {
    for(x = 0; x < 4; x++) {
      occupied[y][x] = true;
      occupied[y][x+14] = true;
    }
    for(x = 4; x < 14; x++) {
      block[y][x] = 0;
			occupied[y][x] = false;
		}
  }
  for(x = 0; x < 18; x++)
    tmp[32][x] = 1;
  for(y = 0; y < 32; y++) {
    for(x = 0; x < 4; x++)
      tmp[y][x] = 1;
    for(x = 14; x < 18; x++)
      tmp[y][x] = 1;
  }
  for(y = 0; y < 36; y++)
    for(x = 0; x < 14; x++)
      dirted[y][x]=2;
	for(y = 0; y < 36; y++)
		for(x = 0; x < 18; x++) {
			blinded[y][x] = 0;
			bflash[y][x] = 0;
		}
}

void Canvas::draw_block(int j, int i) const {
  Byte side, col, to[4];
  side = block[j][i]&15;
  col = block[j][i]>>4;
  to[0] = block[j][i-1];
  to[1] = block[j-1][i];
  to[2] = block[j][i+1];
  to[3] = block[j+1][i];
  raw_draw_bloc_corner(*screen, (i-4)*18, (j-12)*18, side, ::color[col],to);
}

void Canvas::calc_speed() {
  //speed = level * 4;
  if(level<=10)
    speed = 4 + (level-1)*5;
  else
    speed = 50 + (level-10)*3;
}

void Canvas::set_next() {
  if(znext) {
    znext->set_next(next);
    znext2->set_next(next2);
    znext3->set_next(next3);
  }
}

void Canvas::set_message(const char *m1, const char *m2) {
	strncpy(msg1, m1, sizeof(msg1));
	msg1[sizeof(msg1)-1]=0;
	strncpy(msg2, m2, sizeof(msg2));
	msg2[sizeof(msg2)-1]=0;
}

void Canvas::add_text_scroller(const char *st, int xoffset, int yoffset) {
	if(inter && !small_watch) { // if the canvas is currently visible
		Executor *tmp2 = new Executor(true);
		tmp2->add(new Player_text_scroll(this, st, xoffset, yoffset));
		over->start(tmp2);
	}
}

void Canvas::blind_all(Byte time) {
	if(idle<2 && !dying) {
		int x, y;
		for(y = 0; y < 36; y++)
			for(x = 0; x < 18; x++) {
				int tmp = blinded[y][x];
				if(occupied[y][x]) {
					if(!tmp && time) {
						bflash[y][x]=32;
						dirted[y][x]=2;
					}
					tmp=min(255, tmp+time);
					blinded[y][x] = tmp;
				}
			}
	}
}

void Canvas::add_packet(Canvas *sender, Byte nb, Byte nc, Byte lx, Attack attack, Word hole_pos[]) {
	int x, qui;
	if(!sender)
		return;

	Packet_serverlog log("player_attacked");
	log.add(Packet_serverlog::Var("id", id()));
	log.add(Packet_serverlog::Var("attacker_id", sender->id()));
	log.add(Packet_serverlog::Var("type", attack.log_type()));
	log.add(Packet_serverlog::Var("size", attack.type==ATTACK_FULLBLIND? nb*nc:nb));
	if(game->net_server)
		game->net_server->record_packet(&log);

	//Nothing further to do if attack is none.
	if(attack.type==ATTACK_NONE)
		return;

	//Update last_attacker and attacks array
	qui = game->net_list.canvas2player(sender);
	int temp = attacks[qui] + nb*2;
	if(temp > 255)
		temp = 255;
	attacks[qui] = temp;
	if(last_attacker != 255) { // if there is a last attacker
		if(attacks[qui] >= attacks[last_attacker]) // if larger or equal to the last best
			last_attacker = qui; // it's the one that becomes the best
	} else
		last_attacker = qui;

	//If full-blind attack, blind canvas and return
	if(attack.type==ATTACK_FULLBLIND) {
		blind_all(nb*nc*attack.param);
		return;
	}

	//Attack type is either blind or lines, add bonuses
	if(bonus < 20) {
		if(nb+bonus > 20)
			nb = 20-bonus;
		nc--;
		int normal = max(nb - nc, 0);
		int fucked = nb - normal;
		if(game->net_version()>=23) {
			for(x=0; x<nb; x++) {
				bon[x+bonus].x = 127;
				bon[x+bonus].color = sender->color;
				bon[x+bonus].blind_time = attack.type==ATTACK_BLIND? attack.param:0;
				bon[x+bonus].hole_pos=hole_pos[x];
				if(x==nb-1)
					bon[x+bonus].final=true;
				else
					bon[x+bonus].final=false;
			}
			bonus += nb;
		}

		if(game->net_version()<23) {
			for(x=0; x<normal; x++) {
				bon[x+bonus].x = lx;
				bon[x+bonus].color = sender->color;
				bon[x+bonus].blind_time = attack.type==ATTACK_BLIND? attack.param:0;
			}
			bonus += normal;

			for(x=0; x<fucked; x++) {
				lx += nc;
				while(lx > 13)
					lx -= 10;
				bon[x+bonus].x = lx;
				bon[x+bonus].color = sender->color;
				bon[x+bonus].blind_time = attack.type==ATTACK_BLIND? attack.param:0;
			}
			bonus += fucked;
		}
	}
}

void Canvas::give_line() {
	if(!depth)
		return;
  int i, score_add;
	int clean_bonus=0;
	bool log_it=false;
	Word move_value=(depth<<8)+complexity;
	if(send_for_clean) {
		clean_bonus=(1+depth)/2;
		if(move_value>best_clean) {
			log_it=true;
			best_clean=move_value;
		}
	}
	if(move_value>best_move) {
		log_it=true;
		best_move=move_value;
	}
	move_value=(complexity<<8)+depth;
	if(move_value>best_recurse) {
		log_it=true;
		best_recurse=move_value;
	}
	if(log_it) {
		Packet_serverlog log("player_snapshot");
		log.add(Packet_serverlog::Var("id", id()));
		log.add(Packet_serverlog::Var("lines", depth));
		log.add(Packet_serverlog::Var("clean", send_for_clean? "true":"false"));
		log.add(Packet_serverlog::Var("combo", complexity));
		log.add(Packet_serverlog::Var("snapshot", snapshot));
		if(game->net_server)
			game->net_server->record_packet(&log);
	}
  switch(depth) {
    case 1: score_add = 250; break;
    case 2: score_add = 500; break;
    case 3: score_add = 1000; break;
    case 4: score_add = 2000; break;
    default: score_add = 200 * depth * depth; break;
  }
	int complexity_points=1000*(complexity-1);
	if(game->net_version()>=23)
		complexity_points=200*(complexity-1)*(complexity-1);
  score_add += complexity_points;
	//0 clean_points for net_version<23 cause it was added in check_clean
	if(send_for_clean && game->net_version()>=23) {
		int clean_points;
		if(depth<=4)
			clean_points=depth*1250;
		else
			clean_points=depth*depth*500;
		score_add += clean_points;
	}
  score_add += (score_add/10)*level;
  stats[SCORE].add(score_add);
  i = depth-1;
	bool enough=(depth >= game->combo_min);
	if(game->net_version()==23) {
		int alive_count=0;
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=game->net_list.get(i);
			if(c && c->idle<2)
				alive_count++;
		}
		//alive_count adjustment only if there's at least 5 alive
		if(alive_count>4)
			alive_count-=4;
		else
			alive_count=0;
		i = max(0, depth-1-alive_count);
		// this is a bug, it should have been done like net_version >= 24 (below)
		//   but it must remain as is for network compatibility
		enough=i? true:false;
	}
	if(game->net_version()>=24) {
		if(!send_for_clean && !game->boring_rules)
			while(i && handicap_crowd >= stamp_per_handicap) {
				handicap_crowd -= stamp_per_handicap;
				--i;
			}
		if(!i)
			enough = false;
	}

	Packet_serverlog log("player_lines_cleared");
	log.add(Packet_serverlog::Var("id", id()));
	log.add(Packet_serverlog::Var("lines", depth));
	log.add(Packet_serverlog::Var("clean", send_for_clean? "true":"false"));
	log.add(Packet_serverlog::Var("attack_size", clean_bonus+(enough? i:0)));
	log.add(Packet_serverlog::Var("combo", complexity));
	log.add(Packet_serverlog::Var("points", score_add));
	if(game->net_server)
		game->net_server->record_packet(&log);

	Attack clean_att, normal_att;
	normal_att=game->normal_attack;
	clean_att=game->clean_attack;
	if(game->hot_potato && color==game->potato_team) {
		normal_att=game->potato_normal_attack;
		clean_att=game->potato_clean_attack;
	}
	if(color==game->potato_team && enough)
		potato_lines += (i+clean_bonus);
	if(send_for_clean) {
		game->net_list.send(this, clean_bonus, complexity, last_x, clean_att, true);
		if(chat_text) {
			char st[256];
			int num;
			if(normal_att.type==ATTACK_NONE) {
				num=depth;
				if(num>1)
					sprintf(st, "Clean canvas: %s clears %i lines!", name, num);
				else
					sprintf(st, "Clean canvas: %s clears 1 line!", name);
			}
			else {
				num=clean_bonus;
				if(enough)
					num+=i;
				if(num>1)
					sprintf(st, "Clean canvas: %s sends %i lines!", name, num);
				else
					sprintf(st, "Clean canvas: %s sends 1 line!", name);
			}
			message(color, st);
		}
	}
  if(i && enough) { // sends nothing if depth < combo_min
		if(i>=3 && chat_text && !send_for_clean) {
			// if does a 'quad' minimally and not clean
			char st[256];
			if(normal_att.type == ATTACK_NONE)
				sprintf(st, "%s clears %i line%s.", name, depth, depth!=1? "s":"");
			else
				sprintf(st, "%s sends %i line%s.", name, i, i != 1 ? "s" : "");
			message(color, st);
		}
    game->net_list.send(this, i, complexity, last_x, normal_att, false);
    if(inter && !small_watch) { // if the canvas is currently visible
			char st[256];
			if(depth == 2)
				sprintf(st, "Double! %i pts", score_add);
			if(depth == 3)
				sprintf(st, "Triple! %i pts", score_add);
			if(depth == 4)
				sprintf(st, "Quad! %i pts", score_add);
			if(depth > 4)
				sprintf(st, "%i-lines! %i pts", depth, score_add);
			add_text_scroller(st, 20);
    }
  }
	i=depth-1; //For stats accounting, use old numbers
  if(i >= 14) {
		stats[CLEAR14+i-14].add(1);
		i = 14; //Add in CLEARMORE
	}
	stats[CLEAR00+i].add(1);
	stats[COMBO00+complexity-1].add(1);

  stats[LINESCUR].add(depth);
  stats[LINESTOT].add(depth);
  if(game->level_up && stats[LINESCUR].get_value() >= level*15) {
    level++;
    calc_speed();
    Executor *tmp = new Executor(true);
    tmp->add(new Player_level_up(this));
    over->start(tmp);
  }
  depth = 0;
  complexity=0;
  send_for_clean=false;
}

void Canvas::change_level_single() {
  change_level(level, pal, bit);
  //video->setpal(*pal);
  if(level <= 10 && (level-1) > config.info.unlock_theme && !playback) {
    config.info.unlock_theme = level-1;
    config.write();
  }
}

void Canvas::change_level(const int level, Palette *pal, Bitmap *bit) {
  int num, i;
  num = (level-1)%10;
//	if(level>5)
//		num=config.info.multi_level-1;
  sprintf(st, "fond%i.png", num);
	if(level==-1)
		strcpy(st, "black.png");
  Res_doze *res = new Res_doze(st);
  Png img(*res);
  bit->reload(img);
  Palette pal2(img);
  for(i=0; i<256; i++) // was 184
    pal->setcolor(i, pal2.r(i), pal2.g(i), pal2.b(i));

  for(i=0; i<MAXTEAMS-1; i++)
		fteam[i]->colorize(*pal, pal2.r(i*8+184), pal2.g(i*8+184), pal2.b(i*8+184));
	fteam[MAXTEAMS-1]->colorize(*pal, 170, 170, 170);

  video->need_paint = 2;
  delete res;

  delete sons.flash;
  delete sons.depose3;
  delete sons.depose2;
  delete sons.depose;
  delete sons.drip;

  sons.flash = sons.depose3 = sons.depose2 = sons.depose = sons.drip = NULL;
  const char *foo0, *foo1, *foo2, *foo3, *foo4;
  switch(num) {
    case 1:
      foo0="Pwap2.wav";
      foo1=foo2=foo3="Knock.wav";
      foo4="click_3.wav";
      break;
    case 2:
      foo0="Blip1.wav";
      foo1="metal3.wav";
      foo2="Metal1.wav";
      foo3="Metal6.wav";
      foo4="click_1.wav";
      break;
    case 3:
      foo0="Whistle1.wav";
      foo1="Tapdrip.wav";
      foo2="Click01.wav";
      foo3="click_3.wav";
      foo4="Click01.wav";
      break;
    case 4:
      foo0="Spring.wav";
      foo1="Pop1.wav";
      foo2="bloop.wav";
      foo3="Pwap2.wav";
      foo4="corkpop.wav";
      break;
    case 5:
      foo0="Whistle2.wav";
      foo1="Knock.wav";
      foo2="Splodge.wav";
      foo3="Pop1.wav";
      foo4="Tapdrip.wav";
      break;
    case 6:
      foo0="Glass04.wav";
      foo1="Glass01.wav";
      foo2="Glass03.wav";
      foo3="Glass03.wav";
      foo4="Click01.wav";
      break;
    case 7:
      foo0="Bubble2.wav";
      foo1="Water05_1.wav";
      foo2="water05_2.wav";
      foo3="water05_3.wav";
      foo4="Click01.wav";
      break;
    case 8:
      foo0="Ceramic3.wav";
      foo1="Explod03.wav";
      foo2="Explod05.wav";
      foo3="Explod06.wav";
      foo4="Tapdrip.wav";
      break;
    case 9:
      foo0="Smash2.wav";
      foo1="Knock.wav";
      foo2="bloop.wav";
      foo3="click_1.wav";
      foo4="Pop1.wav";
      break;
    default:
      foo0="Pwap2.wav";
      foo1="Hitwood1.wav";
      foo2="Chop2.wav";
      foo3="metal3.wav";
      foo4="Tapdrip.wav";
      break;
  }
  sons.flash = new Sample(Res_doze(foo0)); // when we do a ligne (flash)
  sons.depose3 = new Sample(Res_doze(foo1)); // drop
  sons.depose2 = new Sample(Res_doze(foo2)); // drop
  sons.depose = new Sample(Res_doze(foo3)); // drop
  sons.drip = new Sample(Res_doze(foo4)); // rotate
}

void Canvas::clear_tmp() {
  int i, j;
  for(j = 0; j < 32; j++)
    for(i = 4; i < 14; i++)
      tmp[j][i] = 0;
	for(j=0; j<36; j++)
		for(i=0; i<18; i++)
			moved[j][i]=false;
}

void Canvas::set_canvas_pos(int px, int py, Bitmap *fo, Video_bitmap *s, Zone_next *z, Zone_next *z2, Zone_next *z3, Inter *in) {
  x = px;
  y = py;
  fond = fo;
  screen = s;
  znext = z;
  znext2 = z2;
  znext3 = z3;
  set_next();
  inter = in;
  small_watch = false;
}

void Canvas::hide() {
  x = 0;
  y = 0;
  fond = NULL;
  screen = NULL;
  znext = NULL;
  znext2 = NULL;
  znext3 = NULL;
  inter = NULL;
	z_lines=z_potatolines=z_linestot=z_potatolinestot=NULL;
}

Byte Canvas::check_key(int i) {
  if(ecran && ecran->focus) {  // prevents controlling while inputting
                               // into a zone_text_input that has the
                               // focus
    clear_key(i); // prevents rotating from happening after an input
                  // (because bit 'was released!')
    input->allow_key_repeat(true);
    return 0;
  } else {
		input->allow_key_repeat(false);
		if(i < 5)
			return input->keys[config.player[player].key[i]];
		else
			return input->keys[config.player2[player].key[i - 5]];
  }
}

void Canvas::clear_key(int i) {
	if(i < 5)
		input->keys[config.player[player].key[i]] = 0;
	else
		input->keys[config.player2[player].key[i - 5]] = 0;
}

void Canvas::unrelease_key(int i) {
	if(i<5)
		input->keys[config.player[player].key[i]] &= ~RELEASED;
	else
		input->keys[config.player2[player].key[i-5]] &= ~RELEASED;
}

void Canvas::blit_level_up() {
  screen->put_surface(sprlevel_up, 10, level_up - 30);
  dirt_rect(10, level_up - 30, sprlevel_up->w, sprlevel_up->h);
}

void Canvas::blit_flash() {
  int i, j, pj;
  for(j = 0; j < 20; j++) {
    pj = flash[j];
    if(pj) {
      for(i=0; i<18; i++)
        screen->hline((pj-12)*18+i, 0, 10*18-1, color_flash);
      dirt_rect(0, (pj-12)*18, 10*18, 18);
    }
  }
}

void Canvas::dirt_rect(int x1, int y1, int w1, int h1) {
  int i,j;
  w1 = (x1+w1+17)/18;
  h1 = (y1+h1+17)/18;
  x1 = x1/18;
  y1 = y1/18;
  x1 = max(0,x1);
  y1 = max(0,y1);
  w1 = min(10,w1);
  h1 = min(20,h1);
  for(j=y1; j<h1; j++)
    for(i=x1; i<w1; i++)
      dirted[j+12][i+4]=2;
}

bool Canvas::collide(Byte px, Byte py, Byte rot) {
  return check_collide(bloc, px, py, rot);
}

bool Canvas::check_collide(Bloc *blo, Byte px, Byte py, Byte rot) {
	collide_side_only=true;
	bool ret=false;
  int i,j;
  for(j = 0; j < 4; j++)
    for(i = 0; i < 4; i++) {
      if(blo->bloc[bloc->quel][rot][j][i])
        if(occupied[py + j][px + i]) {
					if(px+i>=4 && px+i<14)
						collide_side_only=false;
          ret=true;
				}
    }
	if(ret) {
		return ret;
	}
	else {
		collide_side_only=false;
		return false;
	}
}

void Canvas::step_bflash() {
  int j, i;
  for(j = 12; j < 32; j++)
    for(i = 4; i < 14; i++) {
			if(occupied[j][i] && bflash[j][i]) {
				if(!(bflash[j][i]&3))
					dirted[j][i] = 2;
				bflash[j][i]--;
				if(!bflash[j][i])
					dirted[j][i] = 2;
			}
		}
}

void Canvas::blit_back() {
	step_bflash();
	SDL_SetColors(fond->surface, video->surface()->format->palette->colors, 0, video->surface()->format->palette->ncolors);
  int j, i, x2, y2;
  for(j = 12; j < 32; j++)
    for(i = 4; i < 14; i++) {
      if(dirted[j][i]) {
				bool blitbloc=true;
				if(!occupied[j][i])
					blitbloc=false;
				if(bflash[j][i])
					if(bflash[j][i]&4)
						blitbloc=false;
				if(!bflash[j][i] && blinded[j][i])
					blitbloc=false;
        if(blitbloc) {
          draw_block(j,i);
        } else {
          x2=(i-4)*18;
          y2=(j-12)*18;
          SDL_Rect rect;
          rect.x = x2;
          rect.y = y2;
          rect.w = rect.h = 18;
          screen->put_surface(fond->surface, rect, x2, y2);
        }
        dirted[j][i]--;
      }
		}
	if(msg1[0] && inter)
		inter->font->draw(msg1, video->vb, x+5, y+40);
	if(msg2[0] && inter)
		inter->font->draw(msg2, video->vb, x+5, y+60);
}

void Canvas::blit_bloc(Bloc *blo) {
  int j,i,bx,by,tx,ty;
  if(!blo)
    return;
  if(smooth) {
    blo->draw(*screen);
  } else {
    blo->draw(*screen, (blo->bx-4)*18, (blo->by-12)*18);
  }
  for(j=0; j<4; j++)
    for(i=0; i<4; i++) {
      if(blo->bloc[blo->quel][blo->rot][j][i]) {
        if(smooth) {
          tx=(blo->x>>4)+4*18;
          ty=(blo->y>>4)+12*18;
        } else {
          tx=blo->bx*18;
          ty=blo->by*18;
        }
        tx += i*18;
        ty += j*18;
        bx=tx/18;
        by=ty/18;
        if(by>=0)
          dirted[by][bx]=2;
        bx=(tx+17)/18;
        if(by>=0)
          dirted[by][bx]=2;
        by=(ty+17)/18;
        if(by>=0)
          dirted[by][bx]=2;
        bx=tx/18;
        if(by>=0)
          dirted[by][bx]=2;
      }
    }
}

void Canvas::small_draw_block(int j, int i) const {
  Byte side, col;
  side = block[j][i]&15;
  col = block[j][i]>>4;
  raw_small_draw_bloc(*screen, (i-4)*6, (j-12)*6, side, ::color[col]);
}

void Canvas::small_blit_back() {
	step_bflash();
	SDL_SetColors(fond->surface, video->surface()->format->palette->colors, 0, video->surface()->format->palette->ncolors);
  int j, i, x2, y2;
  for(j = 12; j < 32; j++)
    for(i = 4; i < 14; i++)
      if(dirted[j][i]) {
				bool blitbloc=true;
				if(!occupied[j][i])
					blitbloc=false;
				if(bflash[j][i])
					if(bflash[j][i]&4)
						blitbloc=false;
				if(!bflash[j][i] && blinded[j][i])
					blitbloc=false;
        if(blitbloc) {
          small_draw_block(j,i);
        } else {
          x2=(i-4)*6;
          y2=(j-12)*6;
          SDL_Rect rect;
          rect.x = x2;
          rect.y = y2;
          rect.w = rect.h = 6;
          screen->put_surface(fond->surface, rect, x2, y2);
        }
        dirted[j][i]--;
      }
}

void Canvas::small_blit_bloc(Bloc *blo) {
  int j,i,bx,by,tx,ty;
  if(!blo)
    return;
  blo->small_draw(*screen, (blo->bx-4)*6, (blo->by-12)*6);
  for(j=0; j<4; j++)
    for(i=0; i<4; i++) {
      if(blo->bloc[blo->quel][blo->rot][j][i]) {
        tx=blo->bx*6;
        ty=blo->by*6;
        tx += i*6;
        ty += j*6;
        bx=tx/6;
        by=ty/6;
        if(by>=0)
          dirted[by][bx]=2;
        bx=(tx+5)/6;
        if(by>=0)
          dirted[by][bx]=2;
        by=(ty+5)/6;
        if(by>=0)
          dirted[by][bx]=2;
        bx=tx/6;
        if(by>=0)
          dirted[by][bx]=2;
      }
    }
}

void Canvas::small_blit_flash() {
  int i, j, pj;
  for(j = 0; j < 20; j++) {
    pj = flash[j];
    if(pj) {
      for(i=0; i<6; i++)
        screen->hline((pj-12)*6+i, 0, 10*6-1, color_flash);
      dirt_rect(0, (pj-12)*18, 10*18, 18);
    }
  }
}

void Canvas::add_watcher(Watcher *w) {
  watchers.add(w);
}

void Canvas::remove_watcher(Net_connection *nc) {
  for(int i=0; i<watchers.size(); i++)
    if(watchers[i]->nc == nc) {
      delete watchers[i];
      watchers.remove(i);
			i--;
    }
}

bool Canvas::islocal() const {
	return local_player;
}

void Canvas::start_moves() {
	if(game->wants_moves) {
		if(moves)
			delete moves;
		moves=new Packet_clientmoves();
		moves->player = num_player;
	}
}

void Canvas::send_p_moves() {
	if(game->wants_moves) {
		if(moves) {
			net->sendtcp(moves);
			delete moves;
			moves=NULL;
		}
	}
}

void Canvas::start_byte() {
	if(game->wants_moves) {
		if(!moves)
			start_moves();
		if(moves->size>=50) {
			send_p_moves();
			start_moves();
		}
		moves->start_byte();
	}
}

void Canvas::set_bit(int v) {
	if(moves)
		moves->set_bit(v);
}

void Canvas::write_byte() {
	if(moves)
		moves->write_byte();
}
