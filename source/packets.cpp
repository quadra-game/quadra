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

#include "net_buf.h"
#include "error.h"
#include "net_stuff.h"
#include "stats.h"
#include "config.h"
#include "canvas.h"
#include "packets.h"

RCSID("$Id$")

void Packet_wantjoin::write(Net_buf *p) {
	Packet_ping::write(p);
	p->write_byte(1);
	p->write_byte(net_version);
	p->write_byte(language);
	p->write_byte(os);
	p->write_bool(true);
}

bool Packet_wantjoin::read(Net_buf *p) {
	if(!Packet_ping::read(p))
		return false;
	if(!p->read_byte())
		return false; // completely ignore 1.1.0 clients
	net_version=p->read_byte();
	language=p->read_byte();
	os=p->read_byte();
	p->read_bool();
	return true;
}

bool read_attack(Attack *a, Net_buf *p) {
	a->type=(Attack_type) p->read_byte();
	if(a->type>=ATTACK_LAST)
		return false;
	a->param=p->read_dword();
	return true;
}

void write_attack(Attack *a, Net_buf *p) {
	p->write_byte(a->type);
	p->write_dword(a->param);
}

Packet_gameinfo::Packet_gameinfo() {
	packet_id = P_GAMEINFO;
	name[0] = 0;
	version = 0;
	port = 0;
	game_end_value = 0;
	nolevel_up = 0;
	delay_start = false;
	terminated = false;
	level_start = 0;
	combo_min = 0;
	allow_handicap = true;
	survivor = false;
	hot_potato = false;
	game_end = 0;
}

Packet_gameinfo::~Packet_gameinfo() {
	players.deleteall();
}

void Packet_gameinfo::write(Net_buf *p) {
	Packet_udp::write(p);
	p->write_string(name);
	p->write_byte(version);
	p->write_dword(port);
	p->write_byte(players.size());
	int i;
	for(i=0; i<players.size(); i++) {
		p->write_byte(players[i]->team);
		p->write_string(players[i]->name);
	}
	p->write_dword(game_end_value);
	p->write_bool(nolevel_up);
	p->write_bool(delay_start);
	p->write_bool(terminated);
	p->write_byte(level_start);
	p->write_byte(combo_min);
	p->write_bool(survivor);
	p->write_byte(game_end);
	for(i=0; i<players.size(); i++) {
		p->write_dword(players[i]->handicap);
	}
	//allow_handicap is reversed because the default should be true
	//  but 1.1.1 and older will not supply it (thus we'll read a
	//  0 from those).
	p->write_bool(!allow_handicap);
	p->write_bool(hot_potato);
	if(version>=22) {
		write_attack(&normal_attack, p);
		write_attack(&clean_attack, p);
		write_attack(&potato_normal_attack, p);
		write_attack(&potato_clean_attack, p);
	}
}

bool Packet_gameinfo::read(Net_buf *p) {
	if(!Packet_udp::read(p))
		return false;
	if(!p->read_string(name, 32))
		return false;
	version = p->read_byte();
	port = p->read_dword();
	Byte num_player = p->read_byte();
	if(num_player>MAXPLAYERS)
		return false;
	int i;
	for(i=0; i<num_player; i++) {
		char tmp[40];
		Byte t=p->read_byte();
		if(t>=MAXTEAMS)
			return false;
		if(!p->read_string(tmp, 40))
			return false;
		add_player(0, t, tmp, -1, 0);
	}
	game_end_value = p->read_dword();
	nolevel_up = p->read_bool();
	delay_start = p->read_bool();
	terminated = p->read_bool();
	level_start = p->read_byte();
	if(level_start > 10)
		return false;
	combo_min = p->read_byte();
	if(combo_min > 10)
		return false;
	survivor = p->read_bool();
	game_end = p->read_byte();
	if(game_end > 4)
		return false;
	for(i=0; i<num_player; i++) {
		int handicap;
		handicap = p->read_dword();
		if(handicap<0 || handicap>4)
			return false;
		players[i]->handicap=handicap;
	}
	//allow_handicap is reversed because the default should be true
	//  but 1.1.1 and older will not supply it (thus we'll read a
	//  0 from those).
	allow_handicap=!p->read_bool();
	hot_potato=p->read_bool();
	if(version>=22) {
		if(!read_attack(&normal_attack, p))
			return false;
		if(!read_attack(&clean_attack, p))
			return false;
		if(!read_attack(&potato_normal_attack, p))
			return false;
		if(!read_attack(&potato_clean_attack, p))
			return false;
	}
	return true;
}

Packet_gameserver::~Packet_gameserver() {
	players.deleteall();
}

void Packet_gameserver::write(Net_buf *p) {
	Packet_ping::write(p);
	p->write_byte(version);
	p->write_string(name);
	p->write_bool(accepted);
	p->write_dword(game_seed);
	p->write_bool(paused);
	p->write_byte(players.size());
	p->write_bool(nolevel_up);
	p->write_byte(level_start);
	p->write_bool(survivor);
	p->write_byte(combo_min);
	p->write_word(delay_start);
	p->write_byte(game_end);
	p->write_dword(game_end_value);
	int i;
	for(i=0; i<players.size(); i++) {
		p->write_byte(players[i]->quel);
		p->write_byte(players[i]->team);
		p->write_string(players[i]->name);
	}
	p->write_bool(wants_moves);
	p->write_byte(syncpoint);
	p->write_byte(potato_team);
	for(i=0; i<players.size(); i++) {
		p->write_dword(players[i]->handicap);
	}
	//allow_handicap is reversed because the default should be true
	//  but 1.1.1 and older will not supply it (thus we'll read a
	//  0 from those).
	p->write_bool(!allow_handicap);
	p->write_bool(hot_potato);
	if(version>=22) {
		write_attack(&normal_attack, p);
		write_attack(&clean_attack, p);
		write_attack(&potato_normal_attack, p);
		write_attack(&potato_clean_attack, p);
	}
	p->write_bool(single);
	p->write_bool(terminated);
	for(i=0; i<players.size(); i++) {
		p->write_dword(players[i]->player_id);
	}
}

bool Packet_gameserver::read(Net_buf *p) {
	if(!Packet_ping::read(p))
		return false;
	version = p->read_byte();
	if(!p->read_string(name, 32))
		return false;
	accepted=p->read_bool();
	game_seed=p->read_dword();
	paused=p->read_bool();
	Byte num_player = p->read_byte();
	if(num_player>MAXPLAYERS)
		return false;
	nolevel_up=p->read_bool();
	level_start=p->read_byte();
	survivor=p->read_bool();
	combo_min=p->read_byte();
	delay_start=p->read_word();
	game_end=p->read_byte();
	if(game_end>=END_LAST)
		return false;
	game_end_value=p->read_dword();
	int i;
	for(i=0; i<num_player; i++) {
		char tmp[40];
		Byte quel=p->read_byte();
		if(quel>=MAXPLAYERS)
			return false;
		Byte t=p->read_byte();
		if(t>=MAXTEAMS)
			return false;
		if(!p->read_string(tmp, 40))
			return false;
		add_player(quel, t, tmp, 0, 0);
	}
	wants_moves=p->read_bool();
	syncpoint=p->read_byte();
	if(syncpoint>Canvas::LAST)
		return false;
	potato_team=p->read_byte();
	if(potato_team>=MAXTEAMS && potato_team!=255)
		return false;
	//Adjust player handicaps (they are at the end to maintain
	//  net compatibility with net_version <= 20
	for(i=0; i<num_player; i++) {
		int handicap=p->read_dword();
		if(handicap<0 || handicap>4)
			return false;
		players[i]->handicap=handicap;
	}
	//allow_handicap is reversed because the default should be true
	//  but 1.1.1 and older will not supply it (thus we'll read a
	//  0 from those).
	allow_handicap=!p->read_bool();
	hot_potato=p->read_bool();
	if(version>=22) {
		if(!read_attack(&normal_attack, p))
			return false;
		if(!read_attack(&clean_attack, p))
			return false;
		if(!read_attack(&potato_normal_attack, p))
			return false;
		if(!read_attack(&potato_clean_attack, p))
			return false;
	}
	single=p->read_bool();
	terminated=p->read_bool();
	for(i=0; i<num_player; i++) {
		players[i]->player_id=p->read_dword();
	}
	return true;
}

bool Packet_gameserver::any_attack() {
	if(normal_attack.type!=ATTACK_NONE || clean_attack.type!=ATTACK_NONE)
		return true;
	if(hot_potato && (potato_normal_attack.type!=ATTACK_NONE || potato_clean_attack.type!=ATTACK_NONE))
		return true;
	return false;
}

bool Packet_chat::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	team = p->read_byte();
	if(team>=MAXTEAMS || team<-1)
		return false;
	if(!p->read_string(text, 256))
		return false;
	to_team = p->read_byte();
	if(to_team>=MAXTEAMS || to_team<-1)
		return false;
	return true;
}

void Packet_chat::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_byte(team);
	p->write_string(text);
	p->write_byte(to_team);
}

bool Packet_playerwantjoin::read(Net_buf *p) {
	if(!Packet_ping::read(p))
		return false;
	team = p->read_byte();
	if(team>=MAXTEAMS)
		return false;
	if(!p->read_string(name, 40))
		return false;
	player = p->read_byte();
	if(player>2)
		return false;
	int repeat = p->read_dword();
	if(repeat<-1 || repeat>3)
		return false;
	smooth = p->read_dword();
	if(smooth<0 || smooth>1)
		return false;
	shadow = p->read_dword();
	if(shadow<0 || shadow>1)
		return false;
	handicap = p->read_dword();
	if(handicap<0 || handicap>4)
		return false;
	p->read_mem(player_hash, 16);
	if(!p->read_string(team_name, 40))
		return false;
	p->read_mem(team_hash, 16);
	h_repeat = p->read_dword();
	if(h_repeat<0 || h_repeat>3)
		return false;
	v_repeat = p->read_dword();
	if(v_repeat<0 || v_repeat>3)
		return false;
	return true;
}

void Packet_playerwantjoin::write(Net_buf *p) {
	Packet_ping::write(p);
	p->write_byte(team);
	p->write_string(name);
	p->write_byte(player);
	if(game->net_version()>=23)
		p->write_dword(static_cast<Dword>(-1));
	else
		p->write_dword(h_repeat);
	p->write_dword(smooth);
	p->write_dword(shadow);
	p->write_dword(handicap);
	p->write_mem(player_hash, 16);
	p->write_string(team_name);
	p->write_mem(team_hash, 16);
	p->write_dword(h_repeat);
	p->write_dword(v_repeat);
}

bool Packet_player::read(Net_buf *p) {
	if(!Packet_ping::read(p))
		return false;
	team = p->read_byte();
	if(team>=MAXTEAMS)
		return false;
	if(!p->read_string(name, 40))
		return false;
	player = p->read_byte();
	if(player>2)
		return false;
	int repeat = p->read_dword();
	if(repeat<-1 || repeat>3)
		return false;
	if(repeat!=-1)
		h_repeat = v_repeat = repeat;
	smooth = p->read_dword();
	if(smooth<0 || smooth>1)
		return false;
	shadow = p->read_dword();
	if(shadow<0 || shadow>1)
		return false;
	pos = p->read_byte();
	if(pos>=MAXPLAYERS)
		return false;
	handicap = p->read_dword();
	if(handicap<0 || handicap>4)
		return false;
	int h_repeat = p->read_dword();
	if(h_repeat<0 || h_repeat>3)
		return false;
	if(repeat==-1 || h_repeat)
		this->h_repeat = h_repeat;
	int v_repeat = p->read_dword();
	if(v_repeat<0 || v_repeat>3)
		return false;
	if(repeat==-1 || v_repeat)
		this->v_repeat = v_repeat;
	player_id = p->read_dword();
	return true;
}

void Packet_player::write(Net_buf *p) {
	Packet_ping::write(p);
	p->write_byte(team);
	p->write_string(name);
	p->write_byte(player);
	if(game->net_version()>=23)
		p->write_dword(static_cast<Dword>(-1));
	else
		p->write_dword(h_repeat);
	p->write_dword(smooth);
	p->write_dword(shadow);
	p->write_byte(pos);
	p->write_dword(handicap);
	p->write_dword(h_repeat);
	p->write_dword(v_repeat);
	p->write_dword(player_id);
}

bool Packet_playeraccepted::read(Net_buf *p) {
	if(!Packet_ping::read(p))
		return false;
	pos = p->read_byte();
	if(pos>=MAXPLAYERS)
		return false;
	accepted = p->read_byte();
	return true;
}

void Packet_playeraccepted::write(Net_buf *p) {
	Packet_ping::write(p);
	p->write_byte(pos);
	p->write_byte(accepted);
}

bool Packet_pause::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	player=p->read_byte();
	return true;
}

void Packet_pause::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_byte(player);
}

Packet_stat::~Packet_stat() {
	net_stats.deleteall();
}

void Packet_stat::add_stat(Byte s, int v) {
	Net_stat *n = new Net_stat(s, v);
	net_stats.add(n);
}

bool Packet_stat::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	num_stat=p->read_byte();
	for(int i=0; i<num_stat; i++) {
		Byte st=p->read_byte();
		if(st>=CS::LAST)
			continue; //Ignore stats we don't know about
		int val=p->read_dword();
		add_stat(st, val);
	}
	return true;
}

void Packet_stat::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_byte(net_stats.size());
	for(int i=0; i<net_stats.size(); i++) {
		p->write_byte(net_stats[i]->st);
		p->write_dword(net_stats[i]->value);
	}
}

Packet_gamestat::~Packet_gamestat() {
	net_stats.deleteall();
}

void Packet_gamestat::add_stat(Byte s, int v) {
	Net_stat *n = new Net_stat(s, v);
	net_stats.add(n);
}

bool Packet_gamestat::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	num_stat=p->read_byte();
	for(int i=0; i<num_stat; i++) {
		Byte st=p->read_byte();
		if(st>=GS::LAST)
			continue; //Ignore stats we don't know about
		int val=p->read_dword();
		add_stat(st, val);
	}
	return true;
}

void Packet_gamestat::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_byte(net_stats.size());
	for(int i=0; i<net_stats.size(); i++) {
		p->write_byte(net_stats[i]->st);
		p->write_dword(net_stats[i]->value);
	}
}

bool Packet_dropplayer::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	Byte r=p->read_byte();
	if(r>=DROP_LAST)
		return false;
	reason=(Drop_reason) r;
	return true;
}

void Packet_dropplayer::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_byte(reason);
}

bool Packet_playerbase::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	player=p->read_byte();
	if(player>=MAXPLAYERS)
		return false;
	return true;
}

void Packet_playerbase::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_byte(player);
}

bool Packet_stampblock::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	x=p->read_byte();
	y=p->read_byte();
	rotate=p->read_byte();
	score=p->read_byte();
	date=p->read_word();
	block_rotated=p->read_byte();
	time_held=p->read_word();
	return true;
}

void Packet_stampblock::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_byte(x);
	p->write_byte(y);
	p->write_byte(rotate);
	p->write_byte(score);
	p->write_word(date);
	p->write_byte(block_rotated);
	p->write_word(time_held);
}

bool Packet_dead::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	then_gone=p->read_bool();
	return true;
}

void Packet_dead::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_bool(then_gone);
}

bool Packet_startwatch::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	address=p->read_dword();
	stop=p->read_bool();
	update=p->read_byte();
	return true;
}

void Packet_startwatch::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_dword(address);
	p->write_bool(stop);
	p->write_byte(update);
}

bool Packet_download::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	p->read_mem(can, sizeof(can));
	int i, j;
	for(j=0; j<32; j++)
		for(i=0; i<10; i++) {
			occ[j][i]=can[j][i]? true:false;
			can[j][i] &= 0x7F;
		}
	seed = p->read_dword();
	idle = p->read_byte();
	if(idle>3)
		return false;
	state = p->read_byte();
	if(state>Canvas::LAST)
		return false;
	bloc = p->read_byte();
	if(bloc>6 && bloc!=255)
		return false;
	next = p->read_byte();
	if(next>6 && bloc!=255)
		return false;
	next2 = p->read_byte();
	if(next2>6 && bloc!=255)
		return false;
	next3 = p->read_byte();
	if(next3>6 && bloc!=255)
		return false;
	bonus = p->read_byte();
	if(bonus>20)
		return false;
	for(i=0; i<20; i++) {
		if(i<bonus) {
			bon[i].x=p->read_byte();
			bon[i].color=p->read_byte();
			bon[i].blind_time=0; //Will be fixed below
			bon[i].hole_pos=0;
			bon[i].final=false;
		}
		else {
			p->read_byte();
			p->read_byte();
			bon[i].x=0;
			bon[i].color=0;
			bon[i].blind_time=0;
			bon[i].hole_pos=0;
			bon[i].final=false;
		}
	}
	for(i=0; i<MAXPLAYERS; i++)
		attacks[i] = p->read_byte();
	last_attacker = p->read_byte();
	if(last_attacker>=MAXPLAYERS && last_attacker!=255)
		return false;
	p->read_mem(blinded, sizeof(blinded));
	for(i=0; i<20; i++)
		if(i<bonus)
			bon[i].blind_time=p->read_byte();
		else
			p->read_byte();
	for(i=0; i<bonus; i++) {
		Word tmp=p->read_word();
		bon[i].hole_pos=tmp & 0x3FF;
		bon[i].final=tmp&0x8000? true:false;
	}
	return true;
}

void Packet_download::write(Net_buf *p) {
	Packet_playerbase::write(p);
	Byte tmp[32][10];
	memcpy(tmp, can, sizeof(can));
	int i, j;
	if(game->net_version()>=23)
		for(j=0; j<32; j++)
			for(i=0; i<10; i++)
				if(occ[j][i])
					tmp[j][i] |= 0x80;
	p->write_mem(tmp, sizeof(can));
	p->write_dword(seed);
	p->write_byte(idle);
	p->write_byte(state);
	p->write_byte(bloc);
	p->write_byte(next);
	p->write_byte(next2);
	p->write_byte(next3);
	p->write_byte(bonus);
	for(i=0; i<20; i++)
		if(i<bonus) {
			p->write_byte(bon[i].x);
			p->write_byte(bon[i].color);
		}
		else {
			p->write_byte(0);
			p->write_byte(0);
		}
	for(i=0; i<MAXPLAYERS; i++)
		p->write_byte(attacks[i]);
	p->write_byte(last_attacker);
	p->write_mem(blinded, sizeof(blinded));
	for(i=0; i<20; i++)
		if(i<bonus)
			p->write_byte(bon[i].blind_time);
		else
			p->write_byte(0);
	for(i=0; i<bonus; i++) {
		Word tmp=bon[i].hole_pos;
		if(bon[i].final)
			tmp |= 0x8000;
		p->write_word(tmp);
	}
}

bool Packet_lines::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	nb=p->read_byte();
	if(nb>36)
		return false;
	nc=p->read_byte();
	lx=p->read_byte();
	//127 is magical code for new complexity stuff
	if((lx<4 || lx>=14) && lx!=127)
		return false;
	sender=p->read_byte();
	if(sender>=MAXPLAYERS && sender!=255)
		return false;
	attack.type=(Attack_type) p->read_byte();
	if(attack.type>=ATTACK_LAST)
		return false;
	attack.param=p->read_dword();
	for(int i=0; i<nb; i++)
		hole_pos[i]=p->read_word();
	return true;
}

void Packet_lines::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_byte(nb);
	p->write_byte(nc);
	p->write_byte(lx);
	p->write_byte(sender);
	p->write_byte(attack.type);
	p->write_dword(attack.param);
	for(int i=0; i<nb; i++)
		p->write_word(hole_pos[i]);
}

bool Packet_testping::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	frame=p->read_dword();
	return true;
}

void Packet_testping::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_dword(frame);
}

bool Packet_gone::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	chat_msg=!p->read_bool();
	return true;
}

void Packet_gone::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_bool(!chat_msg);
}

bool Packet_endgame::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	auto_end=p->read_bool();
	return true;
}

void Packet_endgame::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_bool(auto_end);
}

bool Packet_rejoin::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	int repeat = p->read_dword();
	if(repeat<-1 || repeat>3)
		return false;
	smooth = p->read_dword();
	if(smooth<0 || smooth>1)
		return false;
	shadow = p->read_dword();
	if(shadow<0 || shadow>1)
		return false;
	handicap = p->read_dword();
	if(handicap<0 || handicap>4)
		return false;
	h_repeat = p->read_dword();
	if(h_repeat<0 || h_repeat>3)
		return false;
	v_repeat = p->read_dword();
	if(v_repeat<0 || v_repeat>3)
		return false;
	return true;
}

void Packet_rejoin::write(Net_buf *p) {
	Packet_playerbase::write(p);
	if(game->net_version()>=23)
		p->write_dword(static_cast<Dword>(-1));
	else
		p->write_dword(h_repeat);
	p->write_dword(smooth);
	p->write_dword(shadow);
	p->write_dword(handicap);
	p->write_dword(h_repeat);
	p->write_dword(v_repeat);
}


void Packet_moves::start_byte() {
	if(size==255)
		(void)new Error("Packet_moves too big!");
	moves[size]=0;
}

void Packet_moves::set_bit(int v) {
	moves[size] |= v;
}

void Packet_moves::write_byte() {
	size++;
}

bool Packet_moves::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	size=p->read_byte();
	p->read_mem(moves, size);
	return true;
}

void Packet_moves::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_byte(size);
	p->write_mem(moves, size);
}

bool Packet_state::read(Net_buf *p) {
	if(!Packet_playerbase::read(p))
		return false;
	state=p->read_byte();
	if(state>Canvas::LAST)
		return false;
	return true;
}

void Packet_state::write(Net_buf *p) {
	Packet_playerbase::write(p);
	p->write_byte(state);
}

void Packet_serverrandom::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_dword(seed);
}

bool Packet_serverrandom::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	seed=p->read_dword();
	return true;
}

bool Packet_serverpotato::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	team=p->read_byte();
	if(team>=MAXTEAMS && team!=255)
		return false;
	potato_lines=p->read_dword();
	return true;
}

void Packet_serverpotato::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_byte(team);
	p->write_dword(potato_lines);
}

void Packet_servernameteam::write(Net_buf *p) {
	Packet_tcp::write(p);
	p->write_byte(team);
	p->write_string(name);
}

bool Packet_servernameteam::read(Net_buf *p) {
	if(!Packet_tcp::read(p))
		return false;
	team=p->read_byte();
	if(team>=MAXTEAMS)
		return false;
	if(!p->read_string(name, 40))
		return false;
	return true;
}

Packet_serverlog::Var::Var() {
	name[0]=0;
	value[0]=0;
}

Packet_serverlog::Var::Var(const char* n, const char* val) {
	strncpy(name, n, sizeof(name));
	name[sizeof(name)-1]=0;
	strncpy(value, val, sizeof(value));
	value[sizeof(value)-1]=0;
}

Packet_serverlog::Var::Var(const char* n, unsigned i) {
	strncpy(name, n, sizeof(name));
	name[sizeof(name)-1]=0;
	sprintf(value, "%u", i);
}

Packet_serverlog::Var::Var(const char* n, float f) {
	strncpy(name, n, sizeof(name));
	name[sizeof(name)-1]=0;
	sprintf(value, "%f", f);
}

bool Packet_serverlog::Var::read(Net_buf* p) {
	if(!p->read_string(name, sizeof(name)))
		return false;
	if(!p->read_string(value, sizeof(value)))
		return false;
	return true;
}

void Packet_serverlog::Var::write(Net_buf* p) {
	p->write_string(name);
	p->write_string(value);
}

void Packet_serverlog::write(Net_buf* p) {
	Packet_tcp::write(p);
	p->write_dword(vars.size());
	for(unsigned i=0; i<vars.size(); i++)
		vars[i].write(p);
}

bool Packet_serverlog::read(Net_buf* p) {
	if(!Packet_tcp::read(p))
		return false;
	vars.clear();
	unsigned size = p->read_dword();
	if(size > 100)
		return false;
	for(unsigned i=0; i<size; i++) {
		Var v;
		if(!v.read(p))
			return false;
		vars.add(v);
	}
	return true;
}
