/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <string.h>
#include "res_compress.h"
#include "game.h"
#include "config.h"
#include "canvas.h"
#include "net.h"
#include "net_buf.h"
#include "packets.h"
#include "dict.h"
#include "stringtable.h"
#include "clock.h"
#include "chat_text.h"
#include "recording.h"

Recording *recording = NULL;
Playback *playback = NULL;

Recording::Recording() {
	res=NULL;
	all_output=NULL;
	all_output_size=0;
}

Recording::~Recording() {
	if(res)
		delete res;
	if(all_output)
		free(all_output);
}

bool Recording::create(const char *n) {
	res = new Res_compress(n, RES_CREATE);
	return res->exist;
}

void Recording::write_hunk(Byte h) {
	if(!res)
		return;
	res->write(&h, sizeof(h));
}

void Recording::start_for_multi(Packet* p) {
	frame=0;
	write_packet(p);
}

void Recording::step() {
	frame++;
}

void Recording::write_packet(Packet* p) {
	if(!res)
		return;
	write_hunk(11);
	res->write(&frame, sizeof(frame));
	Net_buf n;
	p->write(&n);
	Word size=n.len();
	res->write(&size, sizeof(size));
	res->write(n.buf, size);
}

void Recording::end_single(Canvas* c) {
	if(!res)
		return;
	if(!c)
		return;
	write_hunk(3);
	memset(playername, 0, sizeof(playername));
	strcpy(playername, c->name);
	score=c->stats[CS::SCORE].get_value();
	lines=c->lines;
	level=c->level;
	res->write(playername, sizeof(playername));
	res->write(&score, sizeof(score));
	res->write(&lines, sizeof(lines));
	res->write(&level, sizeof(level));
}

void Recording::end_multi() {
	if(!res)
		return;
	if(game->single)
		end_single(game->net_list.get(0));
	write_summary();
	all_output=res->write_compress(&all_output_size);
	delete res;
	res=NULL;
}

void Recording::write_summary() {
	if(!res)
		return;
	Textbuf buf;
	buf.append("quadra_version %i.%i.%i\n", Config::major, Config::minor, Config::patchlevel);
	buf.append("duration %i\n", frame);
	buf.append("time %i\n", Clock::get_time());
	game->addgameinfo(&buf);

	Dword size=buf.len();
	write_hunk(13);
	res->write(&size, sizeof(size));
	res->write(buf.get(), size);
}

Playback::Playback(Res* r): data(0, 1024) {
	packet_gameserver=NULL;
	auto_demo = false;
	sum=NULL;
	int i;
	nextByte=0;
	completed = false;
	old_mode=false;
	valid=false;
	for(i = 0; i < 3; i++) {
		player[i].name[0] = 0;
		player[i].repeat = 2;
		player[i].color = i;
		player[i].shadow = 1;
		player[i].smooth = 1;
	}
	res=r;
	read_all();
}

Playback::~Playback() {
	while(packets.size()) {
		Demo_packet *dp=packets.last();
		packets.removelast();
		if(dp->p)
			delete dp->p;
		delete dp;
	}
	if(sum)
		delete sum;
	if(packet_gameserver)
		delete packet_gameserver;
}

Byte Playback::get_byte() {
	if(!old_mode)
		return 0;
	if(nextByte>=data.size())
		completed=true;
	if(completed)
		return 0;
	Byte tmp=data[nextByte++];
	if(nextByte>=data.size()) {
		completed=true;
	}
	return tmp;
}

bool Playback::single() {
	if(old_mode)
		return true;
	if(packet_gameserver && packet_gameserver->single)
		return true;
	return false;
}

bool Playback::check_scores(Canvas* c) {
	if(strcmp(c->name, player[0].name))
		return false;
	if(c->stats[CS::SCORE].get_value() != score)
		return false;
	if(c->lines != lines)
		return false;
	if(c->level != level)
		return false;
	return true;
}

Byte Playback::read_hunk() {
	Byte temp;
	res->read(&temp, sizeof(temp));
	return temp;
}

void Playback::read_all() {
	bool got_summary=false;
	bool got_info=false;
	bool got_seed=false;
	bool got_data=false;
	bool got_packet=false;
	while(res && !res->eof()) {
		Byte hunk = read_hunk();
		switch(hunk) {
			case 0: read_seed(); got_seed=true; break;
			case 1: read_single(); break;
			case 2: read_data(); got_data=true; break;
			case 3: read_info(); got_info=true; break;
			case 11: read_packet(); got_packet=true; break;
			case 13: read_summary(); got_summary=true; break;
			default:
				msgbox("Playback::read_all: bad hunk %i!\n", hunk);
				break;
		}
	}
	old_mode=!got_packet;
	//Single-player demos from 1.0.1 have infos but no summary so
	//  we build a fake summary (with real stats :))
	if(old_mode && !got_summary && got_info) {
		char st[1024];
		sum = new Dict();
		sprintf(st, "players/0/name %s", player[0].name);
		sum->add(st);
		sprintf(st, "players/0/score %i", score);
		sum->add(st);
		sprintf(st, "players/0/lines %i", lines);
		sum->add(st);
		sprintf(st, "players/0/level %i", level);
		sum->add(st);
	}
	valid=false;
	if(got_seed && got_data && got_info)
		valid=true;
	if(packet_gameserver)
		valid=true;
}

void Playback::read_seed() {
	res->read(&seed, sizeof(seed));
	for(int i=0; i<3; i++) {
		res->read(&player[i].repeat, sizeof(player[0].repeat));
	}
}

void Playback::read_single() {
	res->read(&single_player, sizeof(single_player));
}

void Playback::read_data() {
	data.reserve(res->size());
	Byte b;
	while(!res->eof()) {
		res->read(&b, sizeof(b));
		if(b==255)
			break;
		data.append(&b, sizeof(b));
	}
}

void Playback::read_info() {
	res->read(&player[0].name, sizeof(player[0].name));
	player[0].name[sizeof(player[0].name)-1]=0;
	res->read(&score, sizeof(score));
	res->read(&lines, sizeof(lines));
	res->read(&level, sizeof(level));
}

void Playback::read_packet() {
	Dword frame=0;
	Word size=0;
	Net_buf n;
	res->read(&frame, sizeof(frame));
	res->read(&size, sizeof(size));
	if(size>sizeof(n.buf))
		return;
	res->read(n.buf, size);
	Packet *p=net->net_buf2packet(&n, true);
	if(!p)
		return;
/*	char st[1024];
	if(net && net->net_param) {
		net->net_param->print_packet(p, st);
		msgbox("Playback::read_packet: %s\n", st);
	}*/
	if(p->packet_id==P_GAMESERVER) {
		Packet_gameserver *p2=(Packet_gameserver *) p;
		if(p2->version<20 || p2->version>23) {
			msgbox("  Demo game version is %i. Current version is %i. Invalid demo file\n", p2->version, Config::net_version);
			res=NULL;
			valid=false;
			completed=true;
			delete p2;
			return;
		}
		seed = p2->game_seed;
		packet_gameserver=p2;
	}
	else {
		//Ignore packets that appear before Packet_gameserver
		if(!packet_gameserver) {
			delete p;
			return;
		}
		Demo_packet *demo_packet=new Demo_packet(frame, p);
		packets.add(demo_packet);
	}
}

void Playback::read_summary() {
	Dword size;
	res->read(&size, sizeof(size));
	Buf buf(size+1);
	res->read(buf.get(), size);
	buf[size]=0;
	Stringtable st(buf.get(), size);
	sum=new Dict();
	int i;
	for(i=0; i<st.size(); i++) {
		sum->add(st.get(i));
		msgbox("Playback::read_summary: %s\n", st.get(i));
	}
}

void Playback::create_game() {
	if(old_mode) {
		Game_params p;
		p.name="";
		p.set_preset(PRESET_SINGLE);
		(void)new Game(&p);
		return;
	}
	if(!packet_gameserver)
		return;
	new Game(packet_gameserver);
	game->wants_moves=false;
}

Demo_packet Playback::next_packet() {
	if(packets.size())
		return *packets[0];
	else
		return Demo_packet(0xFFFFFFFF, NULL);
}

void Playback::remove_packet() {
	if(packets.size()) {
		Demo_packet *dp=packets[0];
		packets.remove(0);
		//Caller will delete the packet
		delete dp;
	}
}

void Playback::shit_skipper2000(bool remove_chat) {
	//This is the Shit-skipper 2000(tm)
	//  call this if you want to clean up a messy
	//  multiplayer demo: it removes innane chatter at the
	//  beginning and correct all packet times so that the
	//  game starts immediatly. Best used with auto_demo==true
	Dword shit_skipper_bias=0;
	bool got_pause=false;
	bool got_player=false;
	int i;
	for(i=0; i<packets.size(); i++) {
		Demo_packet *dp=packets[i];
		if(dp->p && dp->p->packet_id==P_PLAYER) {
			if(got_pause) {
				shit_skipper_bias=dp->frame;
				break;
			}
			else
				got_player=true;
		}
		if(dp->p && dp->p->packet_id==P_PAUSE) {
			if(got_player) {
				shit_skipper_bias=dp->frame;
				break;
			}
			else
				got_pause=true;
		}
	}
	if(i<packets.size()) {
		for(i=0; i<packets.size(); i++) {
			Demo_packet *dp=packets[i];
			if((remove_chat || dp->frame<=shit_skipper_bias) && dp->p && dp->p->packet_id==P_CHAT) {
				delete dp->p;
				delete dp;
				packets.remove(i);
				i--;
			}
			else {
				if(dp->frame<=shit_skipper_bias)
					dp->frame=0;
				else
					dp->frame-=shit_skipper_bias;
			}
		}
	}
}

Demo_packet::Demo_packet(const Demo_packet& dp): frame(dp.frame), p(dp.p) {
}

Demo_packet::Demo_packet(Dword pframe, Packet* pp): frame(pframe), p(pp) {
}

Demo_packet::~Demo_packet() {
}
