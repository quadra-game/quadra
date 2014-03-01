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

#include <stdint.h>
#include <vector>

#include "attack.h"
#include "buf.h"
#include "global.h"
#include "id.h"
#include "random.h"
#include "stats.h"

class Bitmap;
class Bloc;
class Executor;
class Inter;
class Net_connection;
class Overmind;
class Packet_clientmoves;
class Palette;
class Sprite;
class Video_bitmap;
class Zone;
class Zone_next;

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
	std::vector<Watcher*> watchers;
	void add_watcher(Watcher *w);
	void remove_watcher(Net_connection *nc);
	//Potato stuff (server only)
	uint8_t potato_team_on_last_stamp;
	uint32_t potato_lines;
	Zone *z_lines, *z_potatolines, *z_linestot, *z_potatolinestot;
	int team_potato_lines, team_potato_linestot;
	bool should_remove_bonus; //Remove bonus when we have the chance

	uint32_t gone_time;
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
	uint16_t watch_date;
	bool small_watch;
	Palette *pal;
	Bitmap *bit;
	Random rnd;
	uint8_t color;
	Net_connection *remote_adr;
	bool local_player;
	bool occupied[36][18];
	uint8_t block[36][18];
	uint8_t blinded[36][18];
	uint8_t bflash[36][18];
	uint8_t flash[20];
	uint8_t dirted[36][18];
	uint8_t tmp[33][18];
	//unit block moved in last Player_check_link anim
	bool moved[36][18];
  struct {
    uint8_t x;   // position of the "hole"
    uint8_t color;
		uint8_t blind_time;
		uint16_t hole_pos; //Hole positions
		bool final;
  } bon[20];  // the waiting annoying lines
	uint8_t last_x;  // position of the last dropped block (for line sending)
	char snapshot[32*10*2+1]; // 32 lines * 10 columns * 2 chars per block + 1 string terminator
	uint16_t best_move; //MSB: depth, LSB: complexity
	uint16_t best_clean; //same here
	uint16_t best_recurse; //MSB: complexity, LSB: depth
	Bloc* bloc, *next, *next2, *next3, *bloc_shadow;
	uint8_t color_flash;
	Zone_next* znext, *znext2, *znext3;
	char msg1[64], msg2[64];
	uint32_t frame_start;
	uint8_t attacks[MAXPLAYERS], last_attacker;
	// stuff for latest new handicap code (version 24)
	enum { stamp_per_handicap = 3 };
	uint8_t handicaps[MAXPLAYERS]; // stamp counter vis-a-vis other players
	uint8_t handicap_crowd; // similar counter but for the crowdedness factor
	Inter *inter;
	Overmind *over;
	Executor *myself;
	int handicap;
	int level, speed, player; // player: LOCAL player number (0 to 2)
	int num_player; // num_player: net_list player number (0 to MAXPLAYERS)
	int h_repeat, h_repeat_delay;
	int v_repeat, v_repeat_delay;
	int continuous;
	int level_up;
	int side_speed, down_speed, depth, complexity, bonus;
	bool send_for_clean;
	int x,y;
	char name[40];
	uint8_t player_hash[16];
	char *long_name(bool handi=true, bool gone=true);
	char team_name[40];
	uint8_t team_hash[16];
	Bitmap *fond;
	Video_bitmap *screen;
	Sprite *sprlevel_up;
	bool smooth;
	bool shadow;
	void add_text_scroller(const char *st, int xoffset=4, int yoffset=0);
	void blind_all(uint8_t time);
	void calc_speed();
	void set_next();
	void set_message(const char *m1, const char *m2);
	void change_level_single();
	static void change_level(const int level, Palette *pal, Bitmap *bit);
	void delete_bloc();
	void draw_block(int j, int i) const;
	void init_block();
	//After a call to collide (or check_collide) that returned
	//  true, collide_side_only says if the collision
	//  was solely with the canvas' side walls.
	bool collide_side_only;
	bool check_collide(Bloc *blo, uint8_t px, uint8_t py, uint8_t rot);
	bool collide(uint8_t px, uint8_t py, uint8_t rot);
	void clear_key_all();
	void calc_shadow();
	void init();
	void reinit();
	void restart();
	void clear_tmp();
	void give_line();
	void step_bflash();
	void blit_bloc(Bloc *blo);
	void blit_level_up();
	void blit_back();
	void blit_flash();
	void small_blit_bloc(Bloc *blo);
	void small_blit_back();
	void small_draw_block(int j, int i) const;
	void small_blit_flash();
	uint8_t check_key(int i);
	void clear_key(int i);
	void unrelease_key(int i);
	void dirt_rect(int x1, int y1, int w1, int h1);
	void add_packet(Canvas *sender, uint8_t nb, uint8_t nc, uint8_t lx, Attack attack, uint16_t hole_pos[]);
	// multiplayer recording stuff
	Packet_clientmoves *moves;
	void start_moves();
	void send_p_moves();
	void start_byte();
	void set_bit(int v);
	void write_byte();
	Buf da_moves;
	Canvas(int game_seed, uint8_t team, const char *nam, int ph_repeat, int pv_repeat, bool psmooth, bool pshadow, int phandicap, Net_connection *adr, int qplayer, bool wait_down);
	Canvas(int qplayer, int game_seed, Palette *p);
	virtual ~Canvas();
	void set_canvas_pos(int px, int py, Bitmap *fo, Video_bitmap *s, Zone_next *z, Zone_next *z2, Zone_next *z3, Inter *in);
	void hide();
	bool islocal() const; 
};

#endif
