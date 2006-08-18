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

#include "net_list.h"

#include "types.h"
#include "error.h"
#include "canvas.h"
#include "net.h"
#include "chat_text.h"
#include "game.h"
#include "net_stuff.h"
#include "texte.h"
#include "global.h"
#include "sons.h"
#include "main.h" //For alt_tab
#include "recording.h"
#include "net_server.h"
#include "quadra.h"
#include "nglog.h"
#include "packets.h"

//Objectives are number of remaining goals to reach before it is
//  announced. Must end with 0.
static int frag_objectives[] = {
	20, 10, 5, 4, 3, 2, 1, 0
};

static int score_objectives[] = {
	200000, 100000, 75000, 50000, 25000, 10000, 5000, 0
};

static int line_objectives[] = {
	200, 100, 75, 50, 25, 10, 5, 0
};

int Net_list::check_goals(Byte team, int remain) {
	if(!objectives) //No objectives, no goal is achievable
		return 0;
	int ret=0;
	int *i=objectives;
	while(*i && remain<=*i) {
		if(!reached[i-objectives][team]) {
			reached[i-objectives][team]=true;
			ret=*i;
		}
		i++;
	}
	//Mark as unattained objectives beyond current remain
	//  (in case a player is dropped, causing a team's
	//  goal count to drop suddenly).
	while(*i) {
		reached[i-objectives][team]=false;
		i++;
	}
	return ret;
}

void Net_list::reset_objectives() {
	Byte obj, team;
	for(obj=0; obj<10; obj++)
		for(team=0; team<MAXTEAMS; team++)
			reached[obj][team]=false;
	switch(game->game_end) {
		case END_NEVER: goal_stat=CS::FRAG; objectives=NULL; break;
		case END_FRAG: goal_stat=CS::FRAG; objectives=frag_objectives; break;
		case END_TIME: goal_stat=CS::FRAG; objectives=NULL; break;
		case END_POINTS: goal_stat=CS::SCORE; objectives=score_objectives; break;
		case END_LINES: goal_stat=CS::LINESTOT; objectives=line_objectives; break;
		default: goal_stat=CS::FRAG; objectives=NULL; break;
	}
	if(!game->any_attack() && goal_stat==CS::FRAG) {
		msgbox("Net_list::reset_objectives: goal_stat==FRAG inconsistent with peace game type, forcing SCORE\n");
		goal_stat=CS::SCORE;
		objectives=NULL;
	}
	//Makes goal_stat the first sort criteria, so that the team in
	//  score.team_order[0] will always be the closest to victory.
	score.sort(goal_stat);
}

void Net_list::restart() {
	end_signaled = false;
	reset_objectives();
	syncpoint = Canvas::LAST;
	winner_signaled = false;
}

void Net_list::sendlines(Packet_lines *p) {
	Canvas *c=get(p->player);
	Canvas *sender=NULL;
	if(p->sender!=255)
		sender=get(p->sender);
	if(c) {
		c->add_packet(sender, p->nb, p->nc, p->lx, p->attack, p->hole_pos);
	}
}

void Net_list::send(Canvas *c, Byte nb, Byte nc, Byte lx, Attack attack, bool clean) {
	msgbox("Net_list::send\n");
	Packet_clientlines p;
	p.sender=c->num_player;
	if(game->net_version()>=23)
		p.lx=127; //Magic code that says new complexity info is present
	else
		p.lx=lx;
	//Fill in new info anyway
	int i, j;
	for(j=0; j<nb; j++) {
		p.hole_pos[j]=0;
		if(clean) {
			p.hole_pos[j]=j&1? 585:72; //give really crappy holes
			continue; //go to next line
		}
		for(i=4; i<14; i++) {
			p.hole_pos[j] <<= 1;
			if(c->moved[j][i])
				p.hole_pos[j] |= 1;
		}
	}
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *receiver=get(i);
		//NULL canvases can't receive anything
		if(!receiver)
			continue;
		//We can only send to a local canvas; remote canvases will send
		//  their own Packet_clientlines
		if(!receiver->islocal())
			continue;
		//Sender and receiver are on the same team, don't send
		if(receiver->color == c->color)
			continue;
		p.player=i;
		p.attack = attack;
		//Potato special rules
		if(game->hot_potato) {
			Byte potato_team=c->potato_team_on_last_stamp;
			//There wasn't a potato team at time of last stamp, don't
			//  send any lines
			if(potato_team==255)
				continue;
			//If neither receiver or sender are on the potato team,
			//  don't send the lines
			if(receiver->color!=potato_team && c->color!=potato_team)
				continue;
		}
		//Consider handicap difference between players
		if(game->net_version()<23) {
			//  Note that if the sender has a higher handicap, the
			//  multiplier is always 1. When the receiver has the higher
			//  handicap, the multiplier is (handicap difference)/2+1.
			//  Complexity is also multiplied to prevent an handicapped
			//  receiver from counter-attacking too easily.
			int multiplier=0;
			if(receiver->handicap>c->handicap)
				multiplier=receiver->handicap-c->handicap;
			p.nb=nb+(nb*multiplier+1)/2; //round up
			p.nc=nc+(nc*multiplier+1)/2;
			if(multiplier && p.nc>8) //Avoid "bad" complexity counts
				p.nc=8;
		}
		if(game->net_version()==23) {
			//New handicap code: handicapees send less lines instead of
			//  receiving more.
			int diff=0;
			if(c->handicap>receiver->handicap && !clean)
				diff=c->handicap-receiver->handicap;
			if(nb>diff)
				p.nb=nb-diff;
			else
				p.nb=0;
			p.nc=nc;
		}
		if(game->net_version()>=24) {
			//Even newer handicap code
			p.nb = nb;
			p.nc = nc;
			if(!clean) {
				while(p.nb && c->handicaps[i]>=Canvas::stamp_per_handicap) {
					p.nb--;
					c->handicaps[i] -= Canvas::stamp_per_handicap;
				}
			}
			// c->handicap_crowd is handled in Canvas::give_line
		}
		if(p.nb) {
			sendlines(&p);
			net->sendtcp(&p);
		}
	}
}

Net_list::Net_list() {
	ppm_limit=0;
	lag_limit=3000;
	gone_time_limit=18000;
	admin_password[0]=0;
	motd[0]=0;
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		list[i]=NULL;
		last_use[i]=0;
		idle_on_last_notify[i]=0;
	}
	end_signaled = false;
	demo_completed = false;
	lastgameinfo=0;
	syncpoint = Canvas::LAST;
	winner_signaled = false;
	stepper=new Net_list_stepper(this);
}

Net_list::~Net_list() {
	for(int i=0; i<MAXPLAYERS; i++)
		if(list[i])
			delete list[i];
	cmd_cache.deleteall();
	deny_list.deleteall();
	allow_list.deleteall();
	stepper->ret(); //overmind will delete it correcly
}

void Net_list::notify_all() {
	for(int i=0; i<MAXPLAYERS; i++)
		if(list[i])
			idle_on_last_notify[i]=list[i]->idle;
		else
			idle_on_last_notify[i]=0;
	Observable::notify_all();
	if(lastgameinfo>1000)
		lastgameinfo=500;
	msgbox("Net_list::notify_all: done\n");
}

int Net_list::add_player(Canvas *c) {
	msgbox("Net_list::add_player\n");
	Byte smallest=0xFF;
	if(!game->single) {
		Dword smallest_frame=0xFFFFFFFF;
		for(int i=0; i<MAXPLAYERS; i++)
			if(!list[i] && last_use[i]<smallest_frame) {
				smallest=i;
				smallest_frame=last_use[i];
			}
	}
	else
		smallest=0;
	if(smallest==0xFF)
		return -1;
	else {
		set_player(c, smallest, true);
		return smallest;
	}
}

void Net_list::set_player(Canvas *c, int pos, bool msg) {
	msgbox("Net_list::set_player(%08X, %i)\n", c, pos);
	Dword id=0;
	if(list[pos]) {
		id=list[pos]->id();
		delete list[pos];
		msg=false;
	}
	if(msg) {
		char name[256];
		if(c->handicap!=2) {
			const char *h;
			switch(c->handicap) {
				case 0: h=ST_BEGINNER; break;
				case 1: h=ST_APPRENTICE; break;
				case 2: h=ST_INTERMEDIATE; break;
				case 3: h=ST_MASTER; break;
				default:
				case 4: h=ST_GRANDMASTER; break;
			}
			sprintf(name, "%s (%s)", c->name, h);
		}
		else
			strcpy(name, c->name);
		sprintf(st, ST_BOBJOIN, name, team_name[c->color]);
		message(-1, st);
	}
	//If a canvas was replaced, keep it's id for the new canvas
	if(id!=0)
		c->set_id(id);
	list[pos]=c;
	last_use[pos]=overmind.framecount;
	c->num_player = pos;
//  if(game->paused)
//		c->over->pause();
	notify_all();
}

void Net_list::remove_player(Canvas *c) {
	msgbox("Net_list::remove_player(%08X, player=%i)\n", c, c->num_player);
	for(int i=0; i<MAXPLAYERS; i++)
		if(list[i]==c) {
			list[i]=NULL;
			last_use[i]=overmind.framecount;
			break;
		}
	notify_all();
	delete c;
}

int Net_list::canvas2player(Canvas *c) {
	if(!c)
		return -1;
	for(int i=0; i<MAXPLAYERS; i++)
		if(list[i]==c)
			return i;
	return -1;
}

void Net_list::step_all() {
	int i;

	if(recording)
		recording->step();

	{
		Packet_serverrandom *p=(Packet_serverrandom *) game->peekpacket(P_SERVERRANDOM);
		if(p) {
			game->set_seed(p);
			game->removepacket();
		}
	}
	//This sucks, see ya in 1.3.0 (maybe :))
	/*{
		Packet_servernameteam *p=(Packet_servernameteam *) game->peekpacket(P_SERVERNAMETEAM);
		if(p) {
			set_team_name(p->team, p->name);
			game->removepacket();
		}
	}*/

	static Byte syncframes=0;
	//Look for a new syncpoint
	if(syncframes>=5) {
		Packet_serverstate *pst=(Packet_serverstate *) game->peekpacket(P_SERVERSTATE);
		if(pst) {
			//Found a syncpoint
			syncpoint=pst->state;
			game->removepacket();
			syncframes=0;
		}
	}
	else
		syncframes++;

	//Flush packets from dropped players that may be on the stack
	bool got_one;
	do {
		got_one=false;
		Packet *p2=game->peekpacket(255);
		Packet_playerbase *ppb=NULL;
		if(p2) {
			switch(p2->packet_id) {
				case P_STAT:
				case P_DROPPLAYER:
				case P_STAMPBLOCK:
				case P_DEAD:
				case P_RESPAWN:
				case P_STARTWATCH:
				case P_DOWNLOAD:
				case P_LINES:
				case P_FIRST_FRAG:
				case P_GONE:
				case P_REJOIN:
				case P_MOVES:
				case P_STATE:
				case P_REMOVEBONUS:
					ppb=(Packet_playerbase *) p2;
			}
		}
		if(ppb && !get(ppb->player)) {
			msgbox("Net_list::step_all: got a Packet_playerbase but player has been dropped, ignoring.\n");
			game->removepacket();
			got_one=true;
		}
	} while(got_one);

	Packet_lines *p;
	while((p = (Packet_lines *)game->peekpacket(P_LINES))) {
		msgbox("  sendlines, player=%i, nb=%i, nc=%i, lx=%i\n", p->player, p->nb, p->nc, p->lx);
		sendlines(p);
		game->removepacket();
	}
	if(game->network || !alt_tab) {
		game->count_playing_time();
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=get(i);
			if(c) {
				c->over->step();
			}
		}
	}
	score.updateFromGame();
	if(playback && !playback->old_mode) {
		Demo_packet dp=playback->next_packet();
		if(dp.p) {
			if(dp.frame <= overmind.framecount-game->frame_start || dp.p->packet_id==P_MOVES) {
				Net_callable *nc=game->net_client;
				if(dp.p->packet_id==P_CHAT)
					nc=chat_text;
				if(nc)
					nc->net_call(dp.p);
				playback->remove_packet();
			}
		}
		else {
			if(!demo_completed) {
				message(-1, ST_DEMOCOMPLETED);
				demo_completed=true;
			}
		}
	}

	check_admin();
	//check_first_frag tells us whether it's a good time to end the
	//  game, which will be done by check_end_game if all other
	//  end-game conditions are met
	bool end_it=check_first_frag();
	check_end_game(end_it);
	check_pause();
	check_drop();
	check_potato();
	check_gone();
	check_player();
	check_stat();

	//Send stat refreshes every lag_limit/2 or 1500 frames
	static Dword stat_timer=0;
	if(game->server) {
		bool reset_timer=false;
		stat_timer++;
		for(i=0; i<net->connections.size(); i++) {
			Net_connection *nc=net->connections[i];
			if(stat_timer>=(lag_limit? lag_limit/2 : 1500)) {
				if(nc!=game->loopback_connection && nc->packet_based && nc->joined) {
					Packet_gamestat p;
					p.add_stat(GS::PLAYING_TIME, game->stats[GS::PLAYING_TIME].get_value());
					p.add_stat(GS::ROUND_NUMBER, game->stats[GS::ROUND_NUMBER].get_value()); //why not?
					net->sendtcp(nc, &p);
					int j;
					for(j=0; j<MAXPLAYERS; j++) {
						Canvas *c=get(j);
						if(c) {
							Packet_stat p;
							p.player=j;
							p.add_stat(CS::PLAYING_TIME, c->stats[CS::PLAYING_TIME].get_value());
							net->sendtcp(nc, &p);
						}
					}
				}
				reset_timer=true;
			}
			if(lag_limit && nc->incoming_inactive>lag_limit/2 && nc->packet_based && nc->joined) {
				//Keep clients talking
				Packet_servertestping p;
				nc->sendtcp(&p);
			}
		}
		if(reset_timer)
			stat_timer=0;
	}
	//Drop laggy connections
	if(game->server && lag_limit) {
		for(i=0; i<net->connections.size(); i++) {
			Net_connection *nc=net->connections[i];
			if(nc->incoming_inactive>lag_limit && nc!=game->loopback_connection && nc->packet_based && nc->joined && !nc->trusted) {
				send_msg(nc, "Your connection has been dropped for exceeding the lag limit on this server.");
				send_msg(nc, "You can try to join again to resume playing or find a closer server.");
				nc->disconnect();
			}
		}
	}
	//Drop players who exceed the ppm_limit after 4 minutes of
	//  gameplay
	if(game->server && ppm_limit) {
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=get(i);
			if(c && !c->trying_to_drop && c->stats[CS::PLAYING_TIME].get_value()>=24000 && (Dword)(c->stats[CS::SCORE].get_value())>4*ppm_limit) {
				if(!c->remote_adr || c->remote_adr->trusted)
					continue;
				server_drop_player(i, DROP_AUTO);
				send_msg(c->remote_adr, "%s: you have been dropped for exceeding the PPM (points per minute) limit on this server.", c->name);
				send_msg(c->remote_adr, "Please join an expert server.");
			}
		}
	}
	//Drop players that have been gone for a while without
	//  maintaining a connection (do that every 2.55 seconds)
	if(game->server && gone_time_limit && !game->terminated && !(overmind.framecount&255)) {
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=get(i);
			if(c && c->idle==3) {
				if(c->gone_time>gone_time_limit) {
					if(!c->remote_adr) {
						server_drop_player(i, DROP_AUTO);
					}
				}
			}
		}
	}
	//Re-enable acceptconnects if dedicated and no connections
	//  video_is_dumb==true is assumed to mean dedicated
	if(video_is_dumb && net->connections.size()<=1)
		game->server_accept_connection = 0;

	//notify_all if anybody has gone
	static Dword gone_notify_timer=0;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->idle==3 && idle_on_last_notify[i]<3)
			break;
	}
	if(i<MAXPLAYERS) {
		if(gone_notify_timer++>=1000)
			notify_all();
	}
	else
		gone_notify_timer=0;

	//Manage gameinfo
	if(!lastgameinfo) {
		game->sendgameinfo(false);
		if(size())
			lastgameinfo=3000;
		else
			lastgameinfo=15000;
	}
	else
		lastgameinfo--;
	game->stepgameinfo();
}

void Net_list::check_end_game(bool end_it) {
	if(game->server && game->terminated && all_gone() && !game->single) {
		//Wait 15 seconds for famous last words
		static int restart_timer=1500;
		if(restart_timer) {
			restart_timer--;
		}
		else {
			//Stop recording and logging
			game->stop_stuff();
			if(game->auto_restart) {
				game->restart();
				//Return if restart was successful
				if(!game->terminated) {
					restart_timer=1500; //Reset timer for next time
					return;
				}
			}
			else {
				//Quit when -dedicated -once and all connections are gone
				if(net->connections.size()==1 && video_is_dumb)
          quitting = true;
			}
		}
	}
	bool should_end=false;
	//Assume there's at least some time left if paused
	int time_left=1;
	if(!game->terminated && !end_signaled && !game->paused) {
		if(game->game_end == END_TIME) {
			time_left = game->game_end_value - gettimer();
      // 2001-04-29: Moved sound effects for countdown into this
      // switch. -- roncli
			switch(time_left) {
				case 6000:
          sons.minute->play(0, -1, 11025);
					break;
				case 3000:
					sons.thirty->play(0, -1, 11025);
					break;
				case 2000:
					sons.twenty->play(0, -1, 11025);
					break;
				case 1000:
					sons.ten->play(0, -1, 11025);
					break;
				case 500:
					sons.five->play(0, -1, 11025);
					break;
				case 400:
					sons.four->play(0, -1, 11025);
					break;
				case 300:
					sons.three->play(0, -1, 11025);
					break;
				case 200:
					sons.two->play(0, -1, 11025);
					break;
				case 100:
					sons.one->play(0, -1, 11025);
					break;
			}
			switch(time_left) {
				case 6000:	
				case 3000:	
				case 2000:	
				case 1000:	
				case 500:	
				case 400:	
				case 300:	
				case 200:	
				case 100:	
					sprintf(st, ST_SECONDSREMAINING, time_left/100);
					message(-1, st);
					break;
			}
		}
	}
	Byte leading_team=score.team_order[0];
	int leading_total=score.team_stats[leading_team].stats[goal_stat].get_value();
	bool draw=false;
	bool something_changed=false;
	//Check whether second team has same total: draw
	Byte second_team=score.team_order[1];
	if(score.player_count[leading_team] && score.player_count[second_team])
		if(score.team_stats[second_team].stats[goal_stat].get_value() == leading_total)
			draw=true;
	Byte team;
	if(game->game_end == END_FRAG || game->game_end == END_POINTS || game->game_end == END_LINES) {
		for(team=0; team<MAXTEAMS; team++) {
			int team_goals=score.team_stats[team].stats[goal_stat].get_value();
			//Total goals for this team changed, output chat message
			if(team_goals==leading_total && score.team_goals_changed[team])
				something_changed=true;
			int remaining=game->game_end_value-team_goals;
			int goal_attained=check_goals(team, remaining);
			if(goal_attained && team_goals && remaining>0) {
				team2name(team, st);
				char unit[64];
				int freq_change;
				switch(game->game_end) {
					case END_FRAG: strcpy(unit, ST_FRAG); freq_change = 100 * remaining; break;
					case END_POINTS: strcpy(unit, ST_POINT); freq_change = (int)(0.01 * remaining); break;
					case END_LINES: strcpy(unit, ST_LINE); freq_change = 10 * remaining; break;
					default: strcpy(unit, "frog"); freq_change = 0; break;
				}
				if(remaining!=1)
					strcat(unit, "s");
				char st2[256];
				sprintf(st2, ST_BOBBOBSREMAINING, remaining, unit);
				strcat(st, st2);
				message(-1, st);
				sons.depose4->play(-300, -1, 22500 - freq_change);
			}
		}
	}

	bool could_end=false;
	if(game->game_end == END_TIME && time_left<=0) {
		//Timed game with time exhausted, could end
		could_end=true;
	}
	if(leading_total >= game->game_end_value && (game->game_end == END_FRAG || game->game_end == END_POINTS || game->game_end == END_LINES)) {
		//Counted game with goal reached, could end
		could_end=true;
	}
	if(game->single) {
		//Single-player is dead and done dying, could end
		Canvas *c=get(0);
		if(c && c->idle==2 && !c->dying)
			could_end=true;
	}

	if(could_end)
		if(draw) {
			if(something_changed || (game->game_end==END_TIME && time_left==0)) {
				//Drawn at a total higher or equal to end_value: suspense!
				//  or maybe timer just elapsed and the game is drawn
				//  (still suspense! :))
				sprintf(st, ST_GAMETIED);
				message(-1, st);
				sons.levelup->play(0, -1, 18050);
				sons.levelup->play(0, -1, 18100);
				sons.levelup->play(0, -1, 18150);
			}
		}
		else
			should_end=true;

	if(!end_it) {
		//Don't signal the end of the game if it's not a good time
		//  (i.e. we're in the middle of a survivor round)
		should_end=false;
	}
	if(!winner_signaled && game->terminated && all_gone()) {
		//Winner found for the first time, declare winner
		winner_signaled=true;
		char *team="none";
		if(score.team_stats[leading_team].stats[CS::SCORE].get_value()) {
			char st[256];
			team2name(leading_team, st);
			strcat(st, ST_WINSGAME);
			message(-1, st);
			team=log_team(leading_team);
		}
		Packet_serverlog log("playing_end");
		log.add(Packet_serverlog::Var("winning_team", team));
		if(game->net_server)
			game->net_server->record_packet(&log);
/*		for(int i=0; i<MAXPLAYERS; i++) {
			Canvas *c=get(i);
			if(c) {
				int total_cleared=c->stats[CS::LINESTOT].get_value();
				int total_sent=0;
				int s;
				for(s=CS::CLEAR01; s<CS::CLEARMORE; s++)
					total_sent+=(s-CS::CLEAR00)*c->stats[s].get_value();
				for(s=CS::CLEAR14; s<CS::CLEAR20; s++)
					total_sent+=(s-CS::CLEAR14+14)*c->stats[s].get_value();
				char st[256];
				sprintf(st, "%s: %.2f", c->name, total_sent*100.0/total_cleared);
				message(c->color, st);
			}
		}*/
	}
	if(game->server)
		if(should_end)
			send_end_signal(true);
	Packet_endgame *p=(Packet_endgame *) game->peekpacket(P_ENDGAME);
	if(p) {
		char *reason;
		if(p->auto_end)
			reason="auto";
		else
			reason="manual";
		game->removepacket();
		game->endgame();
		message(-1, ST_GAMEEND);
    sons.start->play(-300, 0, 11025);
		Packet_serverlog log("playing_end_signal");
		log.add(Packet_serverlog::Var("reason", reason));
		if(game->net_server)
			game->net_server->record_packet(&log);
	}
}

void Net_list::send_end_signal(bool auto_end) {
	if(!game->server)
		return;
	if(end_signaled)
		return;
	msgbox("Net_list::send_end_signal: end signaled.\n");
	end_signaled=true;
	Packet_endgame p;
	p.auto_end=auto_end;
	net->dispatch(&p, P_ENDGAME);
	if(game->net_server)
		game->net_server->record_packet(&p);
}

void Net_list::check_potato() {
	game->check_potato();
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c) {
			if(c->color==game->potato_team) {
				c->team_potato_lines=0;
				int j;
				for(j=0; j<MAXPLAYERS; j++) {
					Canvas *c2=get(j);
					if(c2 && c2->color==c->color)
						c->team_potato_lines+=c2->potato_lines;
				}
				c->team_potato_linestot=game->potato_lines[game->potato_team];
			}
			else {
				c->team_potato_lines=0;
				c->team_potato_linestot=0;
			}
		}
	}
}

bool Net_list::check_first_frag() {
	if(!game->survivor)
		return true;
	if(!game->server)
		return true;
	if(game->delay_start) //Not started yet, do nothing
		return true;
	bool ret=false;
	if(game->game_end == END_TIME) {
		//Timed games could always end
		ret=true;
	}
	int i;
	bool allwaiting=true;
	bool onewaiting=false;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->idle<3)
			if(c->state == Canvas::WAITFORWINNER)
				onewaiting=true;
			else
				allwaiting=false;
	}
	if(allwaiting && onewaiting) {
		syncto(Canvas::WAITFORRESTART);
		ret=true; //We just finished a round, maybe we should end the game?
	}
	allwaiting=true;
	onewaiting=false;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->idle<3)
			if(c->state==Canvas::WAITFORRESTART)
				onewaiting=true;
			else
				allwaiting=false;
	}
	if(allwaiting && onewaiting) {
		if(syncpoint==Canvas::WAITFORRESTART) {
			Packet_serverrandom *p=new Packet_serverrandom();
			Random rand;
			p->seed=rand.get_seed();
			net->dispatch(p, P_SERVERRANDOM, game->loopback_connection);
			if(game->net_server)
				game->net_server->record_packet(p);
			game->set_seed(p);
			delete p;
		}
		syncto(Canvas::PLAYING);
		game->reset_potato();
	}
	allwaiting=true;
	onewaiting=false;
	bool all_gone=true;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->idle!=3)
			all_gone=false;
		if(c && c->idle<3)
			if(c->state==Canvas::PLAYING)
				onewaiting=true;
			else
				allwaiting=false;
	}
	if(allwaiting && onewaiting || all_gone)
		syncto(Canvas::LAST);
	int alive_team = -1;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->idle<2) {
			alive_team=c->color;
			break;
		}
	}
	char st1[32], st[256];
	if(alive_team!=-1)
		team2name(alive_team, st1);
	else
		strcpy(st1, ST_NOBODY);
	sprintf(st, ST_BOBWINSROUND, st1);
	if(syncpoint==Canvas::LAST) {
		if(competitive()) {
			if(!game->valid_frag) {
				game->valid_frag=true;
				game->stats[GS::ROUND_NUMBER].add(1);
				Packet_serverlog log("round_start");
				if(game->net_server)
					game->net_server->record_packet(&log);
			}
		}
		else {
			bool round_end;
			if(game->net_version()>=23)
				round_end=would_be_competitive();
			else
				round_end=game->valid_frag;
			if(round_end) {
				message(alive_team, st);
				//Not competitive but some people are ready to jump in:
				//  tell winner(s) to first frag.
				syncto(Canvas::WAITFORWINNER);
				if(game->valid_frag) {
					Packet_serverlog log("round_end");
					log.add(Packet_serverlog::Var("surviving_team", log_team(alive_team)));
					if(game->net_server)
						game->net_server->record_packet(&log);
				}
				game->valid_frag=false;
			}
		}
	}
	return ret;
}

void Net_list::team2name(Byte team, char *st) {
	if(team>=MAXTEAMS) {
		st[0]=0;
		return;
	}
	int count=0;
	Canvas *c2=NULL;
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->color==team) {
			c2=c;
			if(count++)
				break;
		}
	}
	if(count==1)
		strcpy(st, c2->name);
	else
		sprintf(st, ST_BOBTEAM, team_name[team]);
}

void Net_list::update_team_names() {
	//This sucks, see ya in 1.3.0 (maybe :))
	return;
	if(!game->server)
		return;
	if(game->net_version()<23)
		return;
	bool changed=false;
	int team, i;
	for(team=0; team<MAXTEAMS; team++) {
		char name[40];
		Byte hash[16];
		bool found_name=false;
		bool found_no_name=false;
		bool found_one=false;
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=get(i);
			if(c && c->color==team) {
				found_one=true;
				if(c->team_name[0]) {
					if(found_no_name) {
						found_name=false;
						break;
					}
					if(found_name) {
						if(strcmp(name, c->team_name) || memcmp(hash, c->team_hash, sizeof(hash))) {
							//Already had a name but this one is different
							found_name=false;
							found_no_name=true;
							break;
						}
					}
					else {
						strcpy(name, c->team_name);
						memcpy(hash, c->team_hash, sizeof(hash));
						found_name=true;
					}
				}
				else {
					if(found_name) {
						//Had a name, but this one has none
						found_name=false;
						found_no_name=true;
						break;
					}
					found_no_name=true;
				}
			}
		}
		if(found_no_name || !found_one) {
			if(named_team[team]) {
				Packet_servernameteam p;
				p.team=team;
				p.name[0]=0;
				net->dispatch(&p, P_SERVERNAMETEAM, game->loopback_connection);
				set_team_name(team, p.name);
				changed=true;
			}
		}
		else {
			if(!named_team[team] || strcmp(name, team_name[team])) {
				Packet_servernameteam p;
				p.team=team;
				strcpy(p.name, name);
				net->dispatch(&p, P_SERVERNAMETEAM, game->loopback_connection);
				set_team_name(team, p.name);
				changed=true;
			}
		}
	}
	if(changed)
		notify_all();
}

bool Net_list::all_dead_or_gone() const {
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->idle<2)
			return false;
	}
	return true;
}

bool Net_list::all_gone() const {
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c && c->idle!=3)
			return false;
	}
	return true;
}

void Net_list::syncto(Byte syncpoint) {
	if(!game->server)
		return;
	if(this->syncpoint==syncpoint)
		return;
	msgbox("Net_list::syncto: syncing to %i\n", syncpoint);
	this->syncpoint=syncpoint;
	Packet_serverstate ps;
	ps.state=syncpoint;
	net->dispatch(&ps, P_SERVERSTATE, game->loopback_connection);
}

Dword Net_list::gettimer() const {
	return game->stats[GS::PLAYING_TIME].get_value();
}

bool Net_list::competitive() const {
	Byte one_team=255;
	Canvas *c;
	for(int i=0; i<MAXPLAYERS; i++) {
		c = get(i);
		if(c) {
			if(c->idle < 2) { // alive
				if(one_team==255) {
					one_team=c->color;
				}
				else {
					if(one_team!=c->color) { // another alive in another team
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool Net_list::would_be_competitive() const {
	Byte one_team=255;
	Canvas *c;
	for(int i=0; i<MAXPLAYERS; i++) {
		c = get(i);
		if(c) {
			if(c->idle < 3) { // Not gone
				if(one_team==255) {
					one_team=c->color;
				}
				else {
					if(one_team!=c->color) { // another alive in another team
						return true;
					}
				}
			}
		}
	}
	return false;
}

void Net_list::pause_all() {
	for(int i=0; i<MAXPLAYERS; i++)
		if(list[i])
			list[i]->over->pause();
}

void Net_list::unpause_all() {
	for(int i=0; i<MAXPLAYERS; i++)
		if(list[i])
			list[i]->over->unpause();
}

void Net_list::check_pause() {
	if(game->delay_start && game->delay_start != 500) {
		game->delay_start--;
		if(game->delay_start == 0) {
			message(-1, ST_GAMESTARTNOW);
			game->paused = false;
		}
	}
}

void Net_list::check_drop() {
	{
		Packet_serverdropplayer *p=(Packet_serverdropplayer *) game->peekpacket(P_SERVERDROPPLAYER);
		if(p) {
			Canvas *canvas=get(p->player);
			Drop_reason reason=p->reason;
			game->removepacket();
			if(canvas) {
				msgbox("Net_list::check_drop: server says player %i should go\n", canvas->num_player);
				bool drop_it=false;
				if(canvas->islocal())
					drop_it=true;
				if(game->server && canvas->remote_adr==NULL)
					drop_it=true;
				if(game->server && canvas->idle==3)
					drop_it=true;
				if(drop_it) {
					//The server wants me, poor helpless canvas, to drop
					//  dead. So, with neither shame nor glory, I'll
					//  unceremoniously send a P_CLIENTDROPPLAYER before
					//  killing my sorry self.
					//  Thus I beg you, fellow Net_clients all over the
					//  planet, to wait in anguish for the dreaded
					//  P_DROPPLAYER that will extinguish my pathetic
					//  little life. Ah! The tragedy!
					//(or maybe the sucker is already gone so the server
					// will speak in his stead)
					Packet_clientdropplayer p;
					p.player=canvas->num_player;
					p.reason=reason;
					net->sendtcp(&p);
					Packet_dropplayer p2;
					p2.player=canvas->num_player;
					p2.reason=reason;
					game->net_list.drop_player(&p2, true);
				}
			}
		}
	}
	//Drop connections not answering the P_SERVERDROPPLAYER we sent
	if(game->server) {
		int i;
		for(i=0; i<MAXPLAYERS; i++) {
			Canvas *c=get(i);
			if(c && c->trying_to_drop) {
				//Force drop when disconnected
				if(c->remote_adr==NULL || c->idle==3) {
					Packet_dropplayer p;
					p.player=i;
					p.reason=DROP_MANUAL;
					net->dispatch(&p, P_DROPPLAYER);
					if(game->net_server)
						game->net_server->record_packet(&p);
					if(c->remote_adr) {
						//This may have the effect of dropping other players
						//  on this connection. That's ok.
						c->remote_adr->disconnect();
					}
				}
			}
		}
	}

	Packet_dropplayer *p;
	while((p=(Packet_dropplayer *) game->peekpacket(P_DROPPLAYER))) {
		drop_player(p, true);
		game->removepacket();
	}
}

void Net_list::check_gone() {
	Packet_gone *p=(Packet_gone *) game->peekpacket(P_GONE);
	if(p && !get(p->player)) {
		msgbox("Net_list::check_gone: Received a P_GONE but Player %i has been dropped! Ignoring.\n", p->player);
		game->removepacket();
	}
}

void Net_list::check_stat() {
	{
		Packet_stat *p=(Packet_stat *) game->peekpacket(P_STAT);
		if(p) {
			Canvas *c=get(p->player);
			if(c && !c->wait_download) {
				if(!c->islocal()) {
					for(int i=0; i<p->net_stats.size(); i++) {
						Net_stat *ns=p->net_stats[i];
						c->stats[ns->st].set_value(ns->value);
					}
					const int linescur = c->stats[CS::LINESCUR].get_value();
					if(linescur) {
						// adjust level considering game settings and the number of lines cleared
						//   during the current incarnation of the player
						// servers older than 1.1.9 do not send the LINESCUR stat, in which
						//   case the level cannot be calculated accurately and the old incorrect
						//   behavior will be emulated perfectly
						if(game->level_up)
							c->level = max(game->level_start, linescur/15+1);
						else
							c->level = game->level_start;
						c->calc_speed();
					}
				}
				game->removepacket();
			}
		}
	}
	{
		Packet_gamestat *p=(Packet_gamestat *) game->peekpacket(P_GAMESTAT);
		if(p) {
			for(int i=0; i>p->net_stats.size(); i++) {
				Net_stat *ns=p->net_stats[i];
				*(game->stats[ns->st].get_address())=ns->value;
			}
			game->removepacket();
		}
	}
}

void Net_list::server_drop_player(Byte player, Drop_reason reason) {
	if(!game->server)
		return;
	msgbox("Net_list::server_drop_player: player==%i.\n", player);
	Packet_serverdropplayer p;
	p.player = player;
	p.reason = reason;
	Canvas *c=get(p.player);
	if(c && !c->trying_to_drop) {
		net->dispatch(&p, P_SERVERDROPPLAYER);
		c->trying_to_drop=true;
	}
}

void Net_list::drop_player(Packet_dropplayer *p, bool chat) {
	Canvas *c=get(p->player);
	if(c==NULL)
		return;

	if(chat) {
		sprintf(st, ST_BOBWASDROP, c->name);
		message(-1, st);
	}

	char *reason="unknown";
	switch(p->reason) {
		case DROP_AUTO: reason="auto"; break;
		case DROP_MANUAL: reason="manual"; break;
		case DROP_INVALID_BLOCK: reason="invalid_block"; break;
		default: break;
	}
	Packet_serverlog log("player_drop");
	log.add(Packet_serverlog::Var("id", c->id()));
	log.add(Packet_serverlog::Var("reason", reason));
	if(game->net_server)
		game->net_server->record_packet(&log);
	/*
	Can't do this crap: see comment in rejoin_player below.

	for(int i=0; i<MAXPLAYERS; i++) {
		Canvas *c2=get(i);
		if(c2) {
			c2->attacks[c->num_player] = 0; // flush its total
			if(c2->last_attacker == c->num_player)
				c2->last_attacker = 255;
		}
	}
	*/
	remove_player(c);
	update_team_names();
}

void Net_list::rejoin_player(Canvas *c) {
	/*
	Can't do this crap: imagine if the computer where c2 is local
	is dying right now, it will possibly add a frag to c but when
	we (eventually) get the P_DEAD on the other clients, we won't
	add a	frag to the rejoined player because his attack count
	would be zero...

	for(int i=0; i<MAXPLAYERS; i++) {
		Canvas *c2=get(i);
		if(c2) {
			c2->attacks[c->num_player] = 0; // flush its total
			if(c2->last_attacker == c->num_player)
				c2->last_attacker = 255;
		}
	}
	*/
	list[c->num_player] = NULL;
	c->hide(); // this canvas must absolutely disappear when there is a rejoin
	if(!game->server) // if server, remote_adr must be the net_connection of the new canvas owner
		c->remote_adr = net->server_addr();
	// warning: in the case of a rejoin, the canvas is not deleted, but we put NULL in the array to indicate to the watchers to close their windows for this player.
	notify_all();
	list[c->num_player] = c;
	//Now tell them about this "new" player
	notify_all();
}

unsigned Net_list::count_teams(bool include_gone) const {
	unsigned ret=0;
	bool team[MAXTEAMS] = {false};
	int i;
	for(i=0; i<MAXPLAYERS; ++i) {
		Canvas* c = get(i);
		if(c)
			if(include_gone || c->idle<3)
				team[c->color] = true;
	}
	for(i=0; i<MAXTEAMS; ++i)
		if(team[i])
			ret++;
	return ret;
}

unsigned Net_list::count_alive() const {
	unsigned ret=0;
	for(int i=0; i<MAXPLAYERS; ++i) {
		Canvas* c = get(i);
		if(c && c->idle < 2)
			++ret;
	}
	return ret;
}

unsigned Net_list::size(bool include_gone) const {
	unsigned ret=0;
	int i;
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas* c = get(i);
		if(c)
			if(include_gone || c->idle<3)
				ret++;
	}
	return ret;
}

void Net_list::check_player() { // check for player joins
	Packet_player *p=(Packet_player *) game->peekpacket(P_PLAYER);
	if(!p)
		return;
	Canvas *c;
	if(game->server) {
		c=get(p->pos);
	}
	else {
		c=new Canvas(game->seed, p->team, p->name, p->h_repeat, p->v_repeat, p->smooth? true:false, p->shadow? true:false, p->handicap, net->server_addr(), 0, false);
		c->set_id(p->player_id);
		set_player(c, p->pos, true);
	}
	game->removepacket();
}

void Net_list::check_admin() {
	if(!game->server)
		return;
	int co;
	for(co=0; co<net->connections.size(); co++) {
		Net_connection *nc=net->connections[co];
		if(!nc->packet_based && nc->incoming->size()) {
			Byte *buf=nc->incoming->get();
			Dword size=nc->incoming->size();
			char line[256];
			unsigned int line_size = 0;
			bool got_line=false;
			while(line_size < 255U && line_size < size) {
				if(*buf=='\r' || *buf=='\n') {
					got_line=true;
					break;
				}
				line[line_size]=*buf++;
				if(line[line_size]>0 && line[line_size]<' ')
					line[line_size]=' ';
				line_size++;
			}
			if(got_line) {
				line[line_size]=0;
				//Skip any amount of CR/LF
				while(line_size < size && (*buf=='\r' || *buf=='\n')) {
					line_size++;
					buf++;
				}
				nc->incoming->remove_from_start(line_size);
				got_admin_line(line, nc);
			}
		}
	}
}

bool Net_list::accept_connection(Net_connection *nc) {
	if(nc==game->loopback_connection)
		return true;
	bool ret=true;
	int i;
	for(i=0; i<deny_list.size(); i++) {
		IP_addr *ip=deny_list[i];
		IP_addr c(nc->address());
		if(*ip>=c) {
			ret=false;
			break;
		}
	}
	for(i=0; i<allow_list.size(); i++) {
		IP_addr *ip=allow_list[i];
		IP_addr c(nc->address());
		if(*ip>=c) {
			ret=true;
			break;
		}
	}
	return ret;
}

void Net_list::client_deconnect(Net_connection *nc) {
	int i;
	//Clean up command cache
	for(i=0; i<cmd_cache.size(); i++) {
		if(cmd_cache[i]->nc == nc) {
			//Remove cache entry
			delete cmd_cache[i];
			cmd_cache.remove(i);
			i--;
		}
	}
	//Clean up watchers
	for(i=0; i<MAXPLAYERS; i++) {
		Canvas *c=get(i);
		if(c)
			c->remove_watcher(nc);
	}
}

void Net_list::got_admin_line(const char *line, Net_connection *nc) {
	if(!game || !game->server)
		return;
	if(!*line)
		return;
	if(line && nc) {
		char st[64];
		net->stringaddress(st, nc->address(), nc->getdestport());
		msgbox("Net_list::got_admin_line: %s from %s\n", line, st);
	}
	if(*line!='/' && nc) {
		char st[1024];
		sprintf(st, "%u: %s", nc->id(), line);
		Packet_serverlog log("chat");
		log.add(Packet_serverlog::Var("id", nc->id()));
		log.add(Packet_serverlog::Var("address", st));
		if(game->net_server)
			game->net_server->record_packet(&log);
		message(-1, st, true, false, false, nc);
		return;
	}
	//process server commands here
	char cmd[256];
	char params[256];
	strcpy(cmd, line+1); //Skip '/'
	char *sp=strchr(cmd, ' ');
	if(sp) {
		*sp=0;
		strcpy(params, sp+1);
	}
	else {
		params[0]=0;
	}
	int i;
	if(!cmd[0]) {
		//Repeat last command
		for(i=0; i<cmd_cache.size(); i++)
			if(cmd_cache[i]->nc == nc) {
				strcpy(cmd, cmd_cache[i]->cmd);
				if(!params[0])
					strcpy(params, cmd_cache[i]->params);
			}
	}
	if(nc && strcmp(cmd, "admin") && strcmp(cmd, "setpasswd")) {
		//Update cache if real connection and command not admin or
		//  setpasswd
		for(i=0; i<cmd_cache.size(); i++) {
			if(cmd_cache[i]->nc == nc) {
				//Update existing cache entry
				cmd_cache[i]->set(cmd, params);
				break;
			}
		}
		if(i == cmd_cache.size()) {
			//Not found, create new cache entry
			cmd_cache.add(new Lastline(nc, cmd, params));
		}
	}
	bool trusted=false;
	if(!nc)
		trusted=true;
	if(nc && nc->trusted)
		trusted=true;
	if(!strcmp(cmd, "help")) {
		send_msg(nc, "/help                 This help text.");
		send_msg(nc, "/admin <password>     Turn on administrator mode.");
		send_msg(nc, "/setpasswd [password] Set admin password or disable remote admin.");
		send_msg(nc, "/list                 List client connections.");
		send_msg(nc, "/drop                 Drop a player or connection.");
		send_msg(nc, "/allowstart [0|1]     (Dis)allow anybody to start the game.");
		send_msg(nc, "/allowpause [0|1]     (Dis)allow anybody to pause/unpause the game.");
		send_msg(nc, "/pause                Pause/unpause the game.");
		send_msg(nc, "/acceptconnects [0|1] (Dis)allow new client computers to join game.");
		send_msg(nc, "/acceptplayers [0|1]  (Dis)allow new players to join game.");
		send_msg(nc, "/minplayers [0-8]     Set minimum number of players.");
		send_msg(nc, "/maxplayers [0-8]     Set maximum number of players.");
		send_msg(nc, "/minteams [0-8]       Set minimum number of teams.");
		send_msg(nc, "/maxteams [0-8]       Set maximum number of teams.");
		send_msg(nc, "/laglimit [limit]     Display/change lag limit.");
		send_msg(nc, "/ppmlimit [limit]     Display/change maximum ppm.");
		send_msg(nc, "/autorestart [0|1]    Disable/enable auto-restart at game end.");
		send_msg(nc, "/autodrop [seconds]   Set or disable (0) auto-drop time.");
		send_msg(nc, "/public [0|1]         Make game invisible (0) or public (1).");
		send_msg(nc, "/name [name]          Display/change game name.");
		send_msg(nc, "/motd [message]       Display/change message of the day.");
		send_msg(nc, "/endgame              End current game.");
		send_msg(nc, "/quit                 End game and shut down server.");
		send_msg(nc, "/score                Scoreboard.");
		send_msg(nc, "/version              Display Quadra version and build.");
		send_msg(nc, "/bye                  Terminate session.");
	}
	if(!strcmp(cmd, "admin"))
		if(admin_password[0] && !strcmp(params, admin_password) && nc) {
			if(nc->trusted) {
				send_msg(nc, "Already administrator");
			}
			else {
				char st[256], st1[256];
				Net::stringaddress(st1, nc->address());
				snprintf(st, sizeof(st) - 1, "%s:%i", st1, nc->getdestport());
				snprintf(st1, sizeof(st1) - 1, "Granting admin privileges to %s", st);
				message(-1, st1, true, false, true);
				nc->trusted=true;
				send_msg(nc, "Admin commands available");
			}
		}
	if(!strcmp(cmd, "score")) {
		if(!size())
			send_msg(nc, "No players in game");
		else {
			Byte team, o;
			for(o=0; o<MAXTEAMS; o++) {
				team=score.team_order[o];
				if(score.player_count[team]>=1) {
					char st[64];
					strcpy(st, english_teams[team]);
					strcat(st, " team:");
					send_msg(nc, "%45.45s", st);
				}
				Dword frag, death;
				if(score.player_count[team]) {
					for(i=0; i<MAXPLAYERS; i++) {
						Byte player=score.order[i];
						Canvas *c=get(player);
						if(c && c->color==team) {
							frag=score.stats[player].stats[CS::FRAG].get_value();
							death=score.stats[player].stats[CS::DEATH].get_value();
							send_msg(nc, "%45.45s  %4i  %4i", c->long_name(true, false), frag, death);
						}
					}
				}
				if(score.player_count[team]>=1) {
					frag=score.team_stats[team].stats[CS::FRAG].get_value();
					death=score.team_stats[team].stats[CS::DEATH].get_value();
					send_msg(nc, "%45.45s  %4i  %4i", "Total:", frag, death);
				}
			}
		}
	}
	if(!strcmp(cmd, "version")) {
		send_msg(nc, "Quadra %i.%i.%i", Config::major, Config::minor, Config::patchlevel);
		send_msg(nc, "%s", built);
	}
	if(!strcmp(cmd, "bye") && nc) {
		if(!(game && game->loopback_connection==nc)) {
			send_msg(nc, "See ya");
			nc->disconnect();
		}
	}
	if(!strcmp(cmd, "motd")) {
		if(params[0] && trusted) {
			if(!strcmp(params, "-")) {
				motd[0]=0;
				send_msg(nc, "Message of the day deleted");
			}
			else {
				strncpy(motd, params, sizeof(motd));
				motd[sizeof(motd)-1]=0;
				send_msg(nc, "Message of the day changed");
			}
		}
		else
			if(motd[0])
				send_msg(nc, "%s", motd);
			else
				send_msg(nc, "No message of the day defined");
	}
	if(!strcmp(cmd, "setpasswd") && trusted) {
		strncpy(admin_password, params, sizeof(admin_password)-1);
		admin_password[sizeof(admin_password)-1]=0;
		if(admin_password[0])
			send_msg(nc, "Password changed");
		else
			send_msg(nc, "Remote administration disabled");
	}
	if(!strcmp(cmd, "name")) {
		if(params[0] && trusted) {
			strncpy(game->name, params, sizeof(game->name)-1);
			game->name[sizeof(game->name)-1]=0;
		}
		send_msg(nc, "Game name: %s", game->name);
	}
	if(!strcmp(cmd, "endgame") && trusted) {
		send_end_signal(false);
		send_msg(nc, "End signaled");
	}
	if(!strcmp(cmd, "list") && trusted) {
		//-1 so we don't count loopback connection
		send_msg(nc, "Total connections: %i", net->connections.size()-1);
		for(i=0; i<net->connections.size(); i++) {
			Net_connection *nc2=net->connections[i];
			if(nc2) {
				char st[256];
				if(nc2 == game->loopback_connection)
					continue; // skip l'adresse local
				Net::stringaddress(st, nc2->address());
				{
					char st2[16];
					sprintf(st2, ":%i", nc2->getdestport());
					strcat(st, st2);
				}
				if(nc2->trusted)
					strcat(st, " *");
				bool got_one=false;
				int p;
				for(p=0; p<MAXPLAYERS; p++) {
					Canvas *c=get(p);
					if(c && c->remote_adr==nc2) {
						if(got_one)
							strcat(st, ", ");
						else {
							got_one=true;
							strcat(st, ": ");
						}
						strcat(st, "[");
						strcat(st, c->name);
						if(c->idle==3)
							strcat(st, " *");
						strcat(st, "]");
					}
				}
				send_msg(nc, "%s", st);
			}
		}
	}
	if(!strcmp(cmd, "drop") && trusted) {
		bool dropped=false;
		//Look for a connection to drop
		char addr[256];
		int port;
		strcpy(addr, params);
		char *col=strchr(addr, ':');
		if(col) {
			*col=0; //Cut address at ':'
			port=atoi(col+1);
			if(!strcmp(col+1, "*"))
				port = -1;
		}
		else {
			//If port wasn't specified, we'll drop every connection
			//  originating from that addr
			port = -1;
		}
		Dword ad=Net::dotted2addr(addr);
		if(ad!=INADDR_NONE) {
			for(i=0; i<net->connections.size(); i++) {
				Net_connection *nc2=net->connections[i];
				if(game && nc2==game->loopback_connection)
					continue; //Don't ever drop loopback connection
				if(nc2->address()==ad) {
					if(nc2->getdestport()==port || port==-1) {
						char ad[16];
						Net::stringaddress(ad, nc2->address());
						Word po=nc2->getdestport();
						nc2->disconnect();
						send_msg(nc, "Dropping connection %s:%i", ad, po);
						dropped=true;
					}
				}
			}
		}
		if(!dropped) {
			//Haven't found a connection to drop, look for a player
			for(i=0; i<MAXPLAYERS; i++) {
				Canvas *c=get(i);
				if(c && !strcasecmp(c->name, params)) {
					send_msg(nc, "Dropping player %s", c->name);
					server_drop_player(i, DROP_MANUAL);
					dropped=true;
				}
			}
		}
		if(!dropped)
			send_msg(nc, "No match");
	}
	if(!strcmp(cmd, "acceptplayers")) {
		if(params[0] && trusted) {
			i=atoi(params);
			game->server_accept_player=i? 0:1;
		}
		send_msg(nc, "Accept player: %s", game->server_accept_player? "off":"on");
	}
	if(!strcmp(cmd, "maxplayers")) {
		if(params[0] && trusted) {
			i=atoi(params);
			if(i) {
				if(i>MAXPLAYERS)
					i=MAXPLAYERS;
				if(i<1)
					i=1;
			}
			game->server_max_players=i;
		}
		send_msg(nc, "Max players: %i", game->server_max_players);
	}
	if(!strcmp(cmd, "minplayers")) {
		if(params[0] && trusted) {
			i=atoi(params);
			if(i) {
				if(i>MAXPLAYERS)
					i=MAXPLAYERS;
				if(i<1)
					i=1;
			}
			game->server_min_players=i;
		}
		send_msg(nc, "Min players: %i", game->server_min_players);
	}
	if(!strcmp(cmd, "maxteams")) {
		if(params[0] && trusted) {
			i=atoi(params);
			if(i) {
				if(i>MAXTEAMS)
					i=MAXTEAMS;
				if(i<1)
					i=1;
			}
			game->server_max_teams=i;
		}
		send_msg(nc, "Max teams: %i", game->server_max_teams);
	}
	if(!strcmp(cmd, "minteams")) {
		if(params[0] && trusted) {
			i=atoi(params);
			if(i) {
				if(i>MAXTEAMS)
					i=MAXTEAMS;
				if(i<1)
					i=1;
			}
			game->server_min_teams=i;
		}
		send_msg(nc, "Min teams: %i", game->server_min_teams);
	}
	if(!strcmp(cmd, "acceptconnects")) {
		if(params[0] && trusted) {
			i=atoi(params);
			game->server_accept_connection=i? 0:1;
		}
		send_msg(nc, "Accept connection: %s", game->server_accept_connection? "off":"on");
	}
	if(!strcmp(cmd, "ppmlimit")) {
		if(params[0] && trusted) {
			Dword i=atoi(params);
			ppm_limit=i;
		}
		if(ppm_limit)
			send_msg(nc, "PPM limit: %u", ppm_limit);
		else
			send_msg(nc, "PPM limit: disabled");
	}
	if(!strcmp(cmd, "laglimit")) {
		if(params[0] && trusted) {
			Dword i=atoi(params);
			lag_limit=i;
		}
		if(lag_limit)
			send_msg(nc, "Lag limit: %u", lag_limit);
		else
			send_msg(nc, "Lag limit: disabled");
	}
	if(!strcmp(cmd, "autorestart")) {
		if(params[0] && trusted) {
			i=atoi(params);
			game->auto_restart=i? true:false;
		}
		send_msg(nc, "Auto restart: %s", game->auto_restart? "on":"off");
	}
	if(!strcmp(cmd, "public")) {
		if(params[0] && trusted) {
			i=atoi(params);
			game->game_public=i? true:false;
		}
		send_msg(nc, "Public game: %s", game->game_public? "on":"off");
	}
	if(!strcmp(cmd, "autodrop")) {
		if(params[0] && trusted) {
			float i=atof(params);
			gone_time_limit=(Dword) (i*100.0);
		}
		if(gone_time_limit)
			send_msg(nc, "Auto drop time: %.2f seconds", gone_time_limit/100.0);
		else
			send_msg(nc, "Auto drop: disabled");
	}
	if(!strcmp(cmd, "allowstart")) {
		if(params[0] && trusted) {
			i=atoi(params);
			game->net_server->allow_start=i? true:false;
		}
		send_msg(nc, "Allow start: %s", game->net_server->allow_start? "on":"off");
	}
	if(!strcmp(cmd, "allowpause")) {
		if(params[0] && trusted) {
			i=atoi(params);
			game->net_server->allow_pause=i? true:false;
		}
		send_msg(nc, "Allow pause: %s", game->net_server->allow_pause? "on":"off");
	}
	if(!strcmp(cmd, "pause")) {
		Packet_clientpause p;
		p.from=nc;
		game->net_server->clientpause(&p);
	}
	if(!strcmp(cmd, "quit") && trusted) {
    quitting = true;
		send_end_signal(false);
		send_msg(nc, "Shutting down server");
	}
	if(!strcmp(cmd, "stack") && trusted) {
		send_msg(nc, "Dumping game stack. overmind framecount is %i:", overmind.framecount);
		send_msg(nc, "stack size=%i:", game->stack.size());
		for(int j=0; j<game->stack.size(); j++) {
			Packet *p = game->stack[j];
			if(net && net->net_param) {
				char st[4096];
				net->net_param->print_packet(p, st);
				send_msg(nc, "%s", st);
			}
		}
	}
	if(!strcmp(cmd, "in") && trusted) {
		int i, c=0;
		for(i=0; i<net->connections.size(); i++) {
			Net_connection *nc2=net->connections[i];
			if(nc2 && nc2!=game->loopback_connection && nc2->packet_based && nc2->joined) {
				char st[64];
				net->stringaddress(st, nc2->address(), nc2->getdestport());
				send_msg(nc, "%s: %i", st, nc2->incoming_inactive);
				c++;
			}
		}
		send_msg(nc, "Total: %i", c);
	}
	if(!strcmp(cmd, "netstat") && trusted) {
		int i;
		send_msg(nc, "Incoming (min, max, nz-avg):");
		for(i=0; i<net->connections.size(); i++) {
			Net_connection *nc2=net->connections[i];
			if(nc2 && nc2!=game->loopback_connection) {
				char st[64];
				net->stringaddress(st, nc2->address(), nc2->getdestport());
				send_msg(nc, "%21.21s: %u %u %.2f", st, nc2->incoming_min, nc2->incoming_max, (float)nc2->incoming_total/nc2->commit_count_in);
			}
		}
		send_msg(nc, "");
		send_msg(nc, "Outgoing (min, max, nz-avg):");
		for(i=0; i<net->connections.size(); i++) {
			Net_connection *nc2=net->connections[i];
			if(nc2 && nc2!=game->loopback_connection) {
				char st[64];
				net->stringaddress(st, nc2->address(), nc2->getdestport());
				send_msg(nc, "%21.21s: %u %u %.2f", st, nc2->outgoing_min, nc2->outgoing_max, (float)nc2->outgoing_total/nc2->commit_count_out);
			}
		}
	}
	bool allow=!strcmp(cmd, "allow");
	bool deny=!strcmp(cmd, "deny");
	if((allow || deny) && trusted) {
		int i;
		char st[64];
		if(params[0]) {
			IP_addr ad(params);
			if(ad.ip || ad.mask) {
				IP_addr *ip;
				bool would_be_denied=false;
				for(i=0; i<deny_list.size(); i++) {
					ip=deny_list[i];
					if(ad>*ip) {
						if(allow) {
							ip->print(st);
							send_msg(nc, "%15.15s no longer denied", st);
						}
						delete ip;
						deny_list.remove(i);
						i--;
					}
					if(*ip>=ad) {
						would_be_denied=true;
						if(deny && *ip>=ad) {
							deny=false;
							break;
						}
					}
				}
				for(i=0; i<allow_list.size(); i++) {
					ip=allow_list[i];
					if(ad>*ip) {
						if(deny) {
							ip->print(st);
							send_msg(nc, "%15.15s no longer allowed", st);
						}
						delete ip;
						allow_list.remove(i);
						i--;
					}
					if(allow && *ip>=ad) {
						allow=false;
						break;
					}
				}
				if(deny) {
					ip=new IP_addr(ad);
					deny_list.add(ip);
					ip->print(st);
					send_msg(nc, "%15.15s denied", st);
				}
				if(allow && would_be_denied) {
					ip=new IP_addr(ad);
					allow_list.add(ip);
					ip->print(st);
					send_msg(nc, "%15.15s allowed", st);
				}
			}
		}
		else {
			send_msg(nc, "Deny list:");
			for(i=0; i<deny_list.size(); i++) {
				IP_addr *ip=deny_list[i];
				ip->print(st);
				send_msg(nc, "%15.15s", st);
			}
			send_msg(nc, "Total: %i", i);
			send_msg(nc, "Allow list:");
			for(i=0; i<allow_list.size(); i++) {
				IP_addr *ip=allow_list[i];
				ip->print(st);
				send_msg(nc, "%15.15s", st);
			}
			send_msg(nc, "Total: %i", i);
		}
	}
}

Net_list_stepper::Net_list_stepper(Net_list *nl) {
	the_net_list=nl;
}

void Net_list_stepper::step() {
	the_net_list->step_all();
}
