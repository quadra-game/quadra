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

#ifndef _HEADER_CANVAS
#define _HEADER_CANVAS

#include "random.h"
#include "stats.h"
#include "array.h"
#include "id.h"
#include "global.h"
#include "attack.h"
#include "video.h"
#include "buf.h"

class Zone_next;
class Packet_clientmoves;
class Bloc;
class Net_connection;
class Zone;
class Executor;
class Overmind;
class Inter;

class Canvas: public CS, public Identifyable {
public:
	//These states are used only in survivor mode
	enum State {
		PLAYING,
		WAITFORWINNER,
		WAITFORRESTART,
		LAST
	};
	class Watcher {
	public:
		Net_connection *nc;
		Watcher(Net_connection *pnc) {
			nc = pnc;
		}
	};

	void add_text_scroller(const char *st, int xoffset=4, int yoffset=0);
	void calc_speed();
	void set_next();
	void set_message(const char *m1, const char *m2);
	void change_level_single();
	static void change_level(const int level, Palette *pal, Bitmap *bit);
	void delete_bloc();
	bool check_collide(Bloc *blo, Byte px, Byte py, Byte rot);
	bool collide(Byte px, Byte py, Byte rot);
	void clear_key_all();
	void calc_shadow();
	void reinit();
	void restart();
	void clear_tmp();
	void give_line();
	void blit_bloc(Bloc *blo);
	void blit_level_up();
	void blit_back();
	void blit_flash();
	void small_blit_bloc(Bloc *blo);
	void small_blit_back();
	void small_blit_flash();
	Byte check_key(int i);
	void clear_key(int i);
	void unrelease_key(int i);
	void dirt_rect(int x1, int y1, int w1, int h1);
	void add_packet(Canvas *sender, Byte nb, Byte nc, Byte lx, Attack attack, Word hole_pos[]);
	void start_moves();
	void send_p_moves();
	void start_byte();
	void set_bit(int v);
	void write_byte();
	Canvas(int game_seed, Byte team, const char *nam, int ph_repeat, int pv_repeat, bool psmooth, bool pshadow, int phandicap, Net_connection *adr, int qplayer, bool wait_down);
	Canvas(int qplayer, int game_seed, Palette *p);
	virtual ~Canvas();
	void set_canvas_pos(int px, int py, Bitmap *fo, Video_bitmap *s, Zone_next *z, Zone_next *z2, Zone_next *z3, Inter *in);
	void hide();
	bool islocal() const; 
	void add_watcher(Watcher *w);
	void remove_watcher(Net_connection *nc);

	Array<Watcher *> watchers;
	//Potato stuff (server only)
	Byte potato_team_on_last_stamp;
	Dword potato_lines;
  Zone *z_lines;
  Zone* z_potatolines;
  Zone* z_linestot;
  Zone* z_potatolinestot;
  int team_potato_lines;
  int team_potato_linestot;
	bool should_remove_bonus; //Remove bonus when we have the chance
	Dword gone_time;
  /* possible values of canvas->idle:
    0: busy, canvas not available (being recursed)
    1: idle, doing nothing (moving a block)
    2: dead, waiting
    3: gone, the player has left the game
  */
	int idle;
	State state;
	bool dying; // Set to true while in Player_dead or Player_firstfrag
	bool wait_download; // Set when waiting for a P_DOWNLOAD (clients only)
	bool trying_to_drop;
	Word watch_date;
	bool small_watch;
	Palette *pal;
	Bitmap *bit;
	Random rnd;
	Byte color;
	Net_connection *remote_adr;
	bool local_player;
	bool occupied[36][18];
	Byte block[36][18];
	Byte blinded[36][18];
	Byte bflash[36][18];
	Byte flash[20];
	Byte dirted[36][18];
	Byte tmp[33][18];
	//unit block moved in last Player_check_link anim
	bool moved[36][18];
  struct {
    Byte x;   // position of the "hole"
    Byte color;
		Byte blind_time;
		Word hole_pos; //Hole positions
		bool final;
  } bon[20];  // the waiting annoying lines
	Byte last_x;  // position of the last dropped block (for line sending)
	char snapshot[32*10*2+1]; // 32 lines * 10 columns * 2 chars per block + 1 string terminator
  Bloc* bloc;
  Bloc* next;
  Bloc* next2;
  Bloc* next3;
  Bloc* bloc_shadow;
	Byte color_flash;
	Dword frame_start;
  Byte attacks[MAXPLAYERS];
  Byte last_attacker;
	// stuff for latest new handicap code (version 24)
	enum { stamp_per_handicap = 3 };
	Byte handicaps[MAXPLAYERS]; // stamp counter vis-a-vis other players
	Byte handicap_crowd; // similar counter but for the crowdedness factor
	// If you read this, get yourself a cookie, you deserve it!
	Inter *inter;
	Overmind *over;
	Executor *myself;
	int handicap;
  int level;
  int speed;
  int player; // player: LOCAL player number (0 to 2)
	int num_player; // num_player: net_list player number (0 to MAXPLAYERS)
  int h_repeat;
  int h_repeat_delay;
  int v_repeat;
	int continuous;
	int level_up;
  int side_speed;
  int down_speed;
  int depth;
  int complexity;
  int bonus;
	bool send_for_clean;
  int x;
  int y;
	char name[40];
	Byte player_hash[16];
	char *long_name(bool handi=true, bool gone=true);
	char team_name[40];
	Byte team_hash[16];
	bool smooth;
	bool shadow;

	//After a call to collide (or check_collide) that returned
	//  true, collide_side_only says if the collision
	//  was solely with the canvas' side walls.
	bool collide_side_only;
	Buf da_moves;

private:
	void blind_all(Byte time);
	void draw_block(int j, int i) const;
	void init_block();
	void init();
	void step_bflash();
	void small_draw_block(int j, int i) const;

	Word best_move; //MSB: depth, LSB: complexity
	Word best_clean; //same here
	Word best_recurse; //MSB: complexity, LSB: depth
  Zone_next* znext;
  Zone_next* znext2;
  Zone_next* znext3;
  char msg1[64];
  char msg2[64];
  int v_repeat_delay;
	Bitmap *fond;
	Video_bitmap *screen;
	SDL_Surface *sprlevel_up;
	// multiplayer recording stuff
	Packet_clientmoves *moves;
};

#endif
