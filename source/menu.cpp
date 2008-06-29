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

#include "menu.h"

#include <stdlib.h>
#include "version.h"
#include "canvas.h"
#include "chat_text.h"
#include "color.h"
#include "cfgfile.h"
#include "crypt.h"
#include "dict.h"
#include "fonts.h"
#include "game.h"
#include "game_menu.h"
#include "global.h"
#include "highscores.h"
#include "http_request.h"
#include "image_png.h"
#include "input.h"
#include "inter.h"
#include "main.h"
#include "menu_demo_central.h"
#include "misc.h"
#include "multi_player.h"
#include "multi_provider.h"
#include "net.h"
#include "net_server.h"
#include "net_stuff.h"
#include "overmind.h"
#include "pane.h"
#include "qserv.h"
#include "quadra.h"
#include "random.h"
#include "recording.h"
#include "res_compress.h"
#include "sons.h"
#include "sprite.h"
#include "zone.h"
#include "config.h"

#ifdef UGS_DIRECTX
#include <shellapi.h>
#endif

void Menu_do_nothing::step() {
  if(quitting)
    ret();
}

Menu_highscore::Menu_highscore(int hscore, int *playagain, bool show_playb) {
  play_again = playagain;
  show_playback = show_playb;
  int i;
  for(i=0; i<MAX_SCORE; i++) {
    playdemo[i] = NULL;
    playdemog[i] = NULL;
  }

  {
    Res_doze res("hscore.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }
  inter->set_font(new Font(*fonts.normal, pal, 255,255,255, 0, 20, 40));
  font2 = new Font(*fonts.normal, pal, 255,255,0, 0,20,40);
  courrier = new Font(*fonts.courrier, pal, 255,255,255, 0,20,40);
  courrier2 = new Font(*fonts.courrier, pal, 255,255,0, 0,20,40);
  (void)new Zone_bitmap(inter, bit, 0, 0, true);

  const char *pic1, *pic2, *pic1s, *pic2s;
  pic1 = "hscore1.png";
  pic2 = "hscore2.png";
  pic1s = "hscore1s.png";
  pic2s = "hscore2s.png";
  b_again = b_quit = NULL;
  if(show_playback) {
    {
      Res_doze res1(pic2);
      Png png1(res1);
      Res_doze res2(pic2s);
      Png png2(res2);
      b_quit = new Zone_menu(inter, png1, 485, 410, png2);
    }
    if(play_again) {
      Res_doze res1(pic1);
      Png png1(res1);
      Res_doze res2(pic1s);
      Png png2(res2);
      b_again = new Zone_menu(inter, png1, 38, 410, png2);
    }
  } else {
    time_demo = 6000;
  }

  int x;
  if(show_playback)
    x = 60;
  else
    x = 100;
  int y = 50;
  (void)new Zone_text(inter, "Hero", x, y);
  (void)new Zone_text(inter, "Score", x+190, y);
  (void)new Zone_text(inter, "Lines", x+290, y);
  (void)new Zone_text(inter, "Level", x+390, y);
  y += 20;
  Highscores::load();
  refresh_global(y); //int&

  if(show_playback && net->active) {
    sync = new Zone_text_button2(inter, bit, font2, "·2 Update", x+460, 186);
    status = new Zone_text_field(inter, "", 30, 186, 470);
    status->set_val("Click on ·2 Update to update worldwide highscores.");
  } else {
    sync = NULL;
    status = NULL;
  }
  sync_request = NULL;

  y = 256;
  (void)new Zone_text(inter, "Hero", x, y);
  (void)new Zone_text(inter, "Score", x+190, y);
  (void)new Zone_text(inter, "Lines", x+290, y);
  (void)new Zone_text(inter, "Level", x+390, y);
  y += 20;
  for(i=0; i<Highscores::numLocal; i++) {
    if(hscore == i) {
      (void)new Zone_text(font2, inter, Highscores::bestlocal[i].name, x, y);
      (void)new Zone_text_numeric(courrier2, inter, &Highscores::bestlocal[i].score, x+160, y, 80);
      (void)new Zone_text_numeric(courrier2, inter, &Highscores::bestlocal[i].lines, x+260, y, 80);
      (void)new Zone_text_numeric(courrier2, inter, &Highscores::bestlocal[i].level, x+360, y, 80);
      sons.levelup->play(0, 0, 11000);
    } else {
      (void)new Zone_text(inter, Highscores::bestlocal[i].name, x, y);
      (void)new Zone_text_numeric(courrier, inter, &Highscores::bestlocal[i].score, x+160, y, 80);
      (void)new Zone_text_numeric(courrier, inter, &Highscores::bestlocal[i].lines, x+260, y, 80);
      (void)new Zone_text_numeric(courrier, inter, &Highscores::bestlocal[i].level, x+360, y, 80);
    }
    if(show_playback)
      playdemo[i] = new Zone_text_button2(inter, bit, font2, "Playback", x+460, y);
    y += 21;
  }

  playlast = NULL;
  if(show_playback) {
    snprintf(st, sizeof(st) - 1, "%s/last.qrec", quadradir);
    Res_dos *res = new Res_dos(st, RES_TRY);
    if(res->exist) {
      y += 6;
      (void)new Zone_text(inter, "Replay last single-player game:", x, y);
      playlast = new Zone_text_button2(inter, bit, font2, "Playback", x+460, y);
    }
    delete res;
  }
}

Menu_highscore::~Menu_highscore() {
  delete font2;
  delete courrier;
  delete courrier2;
  if(sync_request)
    delete sync_request;
}

void Menu_highscore::refresh_global(int& y) {
  int x;
  if(show_playback)
    x = 60;
  else
    x = 100;
  zone.deleteall();
  for(int i=0; i<Highscores::numGlobal; i++) {
    playdemog[i] = NULL;
    zone.add(new Zone_text(inter, Highscores::bestglobal[i].name, x, y));
    zone.add(new Zone_text_numeric(courrier, inter, &Highscores::bestglobal[i].score, x+160, y, 80));
    zone.add(new Zone_text_numeric(courrier, inter, &Highscores::bestglobal[i].lines, x+260, y, 80));
    zone.add(new Zone_text_numeric(courrier, inter, &Highscores::bestglobal[i].level, x+360, y, 80));
    if(show_playback) {
      playdemog[i] = new Zone_text_button2(inter, bit, font2, "Playback", x+460, y);
      zone.add(playdemog[i]);
    }
    y += 21;
  }
  video->need_paint = 2;
}

// Very specifically for URL-encoding of base64 buffers, mangles its source in
// the process.
static void fast_plusonly_url_encode(Textbuf& out, Textbuf& in) {
  char* ptr = in.get();
  out.reserve(in.len());
  while (*ptr != '\0') {
    char* index = strchr(ptr, '+');
    if (index) {
      *index = '\0';
      out.appendraw(ptr);
      out.appendraw("%2B");
      ptr = index + 1;
    } else {
      out.appendraw(ptr);
      break;
    }
  }
}

void Menu_highscore::start_sync() {
  sync->set_text("·2 Cancel");
  video->need_paint = 2;
  Res_dos *demofile;
  sync_request = new Qserv();
  if(Highscores::numLocal>=1) {
    char st[1024];
    Highscores::getFilename(st, 0, sizeof(st));
    demofile=new Res_dos(st, RES_TRY);
    if(!demofile->exist) {
      delete demofile;
      demofile=NULL;
    }
  }
  else
    demofile=NULL;
  if(demofile) {
    Playback *demo=Highscores::bestlocal[0].demo;
    sync_request->add_data("postdemo\n");
    sync_request->add_data("name %s\n", demo->player);
    sync_request->add_data("score %i\n", demo->score);
    sync_request->add_data("lines %i\n", demo->lines);
    sync_request->add_data("level %i\n", demo->level);
    //Add the recording
    sync_request->add_data("rec ");
    Textbuf buf;
    msgbox("Menu_highscore::start_sync: original size=%i\n", demofile->size());
    Http_request::base64encode((const Byte*)demofile->buf(), buf, demofile->size());
    msgbox("Menu_highscore::start_sync: encoded size=%i\n", strlen(buf.get()));
    delete demofile;
    demofile=NULL;
    Textbuf safebuf;
    fast_plusonly_url_encode(safebuf, buf);
    sync_request->add_data_large(safebuf);
    sync_request->add_data("\n");
  }
  else
    sync_request->add_data("gethighscores\n");

  //Expecting MAX_SCORE global highscores in answer
  sync_request->add_data("num %i\n", MAX_SCORE);
  sync_request->send();
  status->set_val("Connecting. Please wait...");
}

void Menu_highscore::step_sync() {
  bool done = sync_request->done();
  if(!done) {
    if(sync_request->isconnected()) {
      sprintf(st, "Connected. Transfer in progress. %i bytes.", sync_request->getnbrecv());
      status->set_val(st);
    }
    return;
  }

  const char *server_status = sync_request->get_status();
  if(!server_status || (strcmp(server_status, "Ok") && strcmp(server_status, "Error"))) {
    stop_sync();
    status->set_val("No Quadra game server found.");
    return;
  }
  Dict *reply=sync_request->get_reply();
  if(_debug) {
    reply->dump();
  }
  if(strcmp(server_status, "Ok")) {
	// qserv returned an "Error" status, display the supplied message
    const char *msg = reply->find("message");
	if(!msg)
      msg = "???";
    sprintf(st, "Error: the server returned [%s].", msg);
    status->set_val(st);
    stop_sync();
    return;
  }
  int i;
  for(i=0; i<MAX_SCORE; i++) {
    char dir[16];
    sprintf(dir, "high%03i", i);
    Dict *d=reply->find_sub(dir);
    if(d) {
      const char* rec=d->find("rec");
      if(rec) {
        char fn[1024];
        Highscores::getGlobalFilename(fn, i, sizeof(fn));
        Res_dos res(fn, RES_CREATE);
        if(res.exist) {
          Buf out;
          msgbox("Menu_highscore::step_sync: encoded size=%i\n", strlen(rec));
          Http_request::base64decode(rec, out, strlen(rec));
          msgbox("Menu_highscore::step_sync: decoded size=%i\n", out.size());
          res.write(out.get(), out.size());
				}
      }
    }
  }
  status->set_val("Transfer completed successfully.");
  Highscores::freemem();
  Highscores::load();
  int y=70;
  refresh_global(y);
  stop_sync();
}

void Menu_highscore::stop_sync() {
  sync->set_text("·2 Update");
  video->need_paint = 2;
  delete sync_request;
  sync_request=NULL;
}

void Menu_highscore::step() {
  Menu_standard::step();
  if(show_playback) {
    if(sync_request)
      step_sync();
    if(!result)
      return;

    if(!sync_request) {
      if(play_again) {
        if(result == b_again) {
          *play_again = 1;
          quit = true;
        }
      }
      if(result == b_quit) {
        quit = true;
      }
      if(sync && result == sync) {
        start_sync();
      }
      for(int i=0; i<MAX_SCORE; i++) {
        if(result == playdemo[i]) {
          Highscores::getFilename(st, i, sizeof(st));
          play_demo(st);
        }
        if(result == playdemog[i]) {
          Highscores::getGlobalFilename(st, i, sizeof(st));
          play_demo(st);
        }
      }
      if(result == playlast) {
        snprintf(st, sizeof(st) - 1, "%s/last.qrec", quadradir);
        play_demo(st);
      }
    } else {
      if(result == sync) {
        stop_sync();
        status->set_val("Transfer interrupted by user.");
      }
    }
  } else { // demo mode of the main menu
    time_demo--;
    if(result || input->last_key.sym != SDLK_UNKNOWN || time_demo == 0)
      quit = true;
  }
}

void Menu_highscore::play_demo(const char *st) {
  Res_compress *res = new Res_compress(st, RES_TRY);
  if(res->exist) {
    call(new Fade_in(pal));
    call(new Call_setfont(pal, new Demo_multi_player(res)));
    call(new Fade_out(pal));
    // the 'delete res' is done by ~Demo_multi_player
  } else {
    msgbox("Menu_highscore::step: Unable to open demo '%s'\n", st);
    delete res;
  }
}

Net_starter* net_starter=NULL;

Net_starter::Net_starter() {
  if(net)
    fatal_msgbox("Net already started!\n");
  time_control = TIME_FREEZE;
  net=new Net(new Quadra_param());
  if(!net->active) {
    msgbox("Net_starter::Net_starter: Can't initialise network.");
    const char *temp = net->last_error;
    if(temp)
      msgbox("Error [%s]\n", temp);
    else
      msgbox("No error reported.\n");
    net_exec = NULL;
  } else {
    net_exec = new Executor(true);
    net_exec->add(new Net_module());
    overmind.start(net_exec);
    if(chat_text)
      chat_text->addwatch();
  }
}

Net_starter::~Net_starter() {
  if(net_exec) {
    overmind.stop(net_exec);
    if(chat_text)
      chat_text->removewatch();
  }
  delete net;
  net=NULL;
}

Menu_multi_join::Menu_multi_join(Bitmap *bit, Font *font, Font *font2, const Palette& p, bool plocal) {
  pal = p;
  bit_ = bit;
  font2_ = font2;
  local_net = plocal;
  inter->set_font(font, false);
  (void)new Zone_bitmap(inter, bit, 0, 0);
  (void)new Zone_text(inter, "\"To create or to join, that is the "
                      "question\" - Rem, 1999", 20);
  b_create = new Zone_text_button2(inter, bit, font2, "·2 Create a new game",
                                   50);
  b_refresh = b_refresh_internet = NULL;
  if(local_net)
    b_refresh = new Zone_text_button2(inter, bit, font2,
                                      "·2 Refresh local games", 80);
  else
    b_refresh_internet = new Zone_text_button2(inter, bit, font2,
                                               "·2 Refresh Internet games",
                                               80);
  (void)new Zone_text(fteam[7], inter, "Server list:", 10, 120);
  selected_game = -1;
  list_game = new Zone_listbox2(inter, bit, font2, &selected_game, 2, 140, 220, 240);
  int y = 120;
  (void)new Zone_text(fteam[7], inter, "Type:", 235, y);
  z_game_type = new Zone_text_field(inter, "", 480, y, 155); y += 22;
  (void)new Zone_text(fteam[7], inter, "Level up:", 235, y);
  z_game_speed = new Zone_text_field(inter, "", 480, y, 155); y += 22;
  (void)new Zone_text(fteam[7], inter, "Start game at level:", 235, y);
  z_game_level = new Zone_text_field(inter, "", 480, y, 24); y += 22;
  (void)new Zone_text(fteam[7], inter, "Smallest offensive line combo:",
                      235, y);
  z_game_minimum = new Zone_text_field(inter, "", 480, y, 155); y += 22;
  (void)new Zone_text(fteam[7], inter, "The game ends when:", 235, y);
  z_game_end = new Zone_text_field(inter, "", 480, y, 155); y += 22;
  (void)new Zone_text(fteam[7], inter, "Status:", 235, y);
  z_game_status = new Zone_text_field(inter, "", 480, y, 155); y += 22;
  list_player = new Zone_listbox2(inter, bit, font2, NULL, 235, y, 390, 136);
  b_join = new Zone_text_button2(inter, bit, font2, "·2 Join selected game",
                                 395);

  (void)new Zone_text(fteam[7], inter, "Or enter the IP address of the server:",
                      30, 420);
  address[0] = 0;
  (void)new Zone_input_address(this, pal, inter, address, 254, 340, 420, 240);
  b_info = new Zone_text_button2(inter, bit, font2, "·2 Check IP info",
                                 35, 450);
  b_book = NULL;
  if(!local_net)
    b_book = new Zone_text_button2(inter, bit, font2, "·2 Address book", 345, 450);
  cancel = new Zone_text_button2(inter, bit, font2, "Back ·0", 560, 450);
}

Menu_multi_join::~Menu_multi_join() {
  removewatch();
}

void Menu_multi_join::addwatch() {
  net->addwatch(P_GAMEINFO, this);
}

void Menu_multi_join::removewatch() {
  net->removewatch(P_GAMEINFO, this);
}

void Menu_multi_join::init() {
  Menu::init();
  if(!net->active) {
    exec(new Menu_net_problem(net->last_error,
                              "You will not be able to play on the Internet.",
                              bit_, inter->font));
  } else {
    net->close_all_udp();
    net->init_all_udp();
    refresh();
  }
}

class Listgame: public Listable {
public:
  Packet_gameinfo *p;
  Listgame(const char *s, Packet_gameinfo *pp): Listable(s) {
    p = pp;
  }
  virtual ~Listgame() {
    delete p;
  }
  virtual bool is_equal(Listable *source) {
    Listgame *other=(Listgame *) source;
    return p->from_addr == other->p->from_addr &&
           p->port == other->p->port;
  }
};

void Menu_multi_join::step() {
  Menu::step();
  if(input->last_key.sym == SDLK_ESCAPE || quitting) {
    ret();
    return;
  }

  if(!result)
    return;

  if(result==b_create) {
    removewatch();
    if(address[0] == 0)
      refresh();
    call(new Create_game(bit_, inter->font, font2_, pal, true, local_net));
  }
  if(result==b_refresh || result==b_refresh_internet) {
    address[0] = 0; // ignore the IP address written in the box
    refresh();
  }
  if(result==b_info) {
    call(new Menu_multi_checkip(bit_, inter->font, font2_, pal));
  }
  if(result==b_book) {
    call(new Menu_multi_book(bit_, inter->font, font2_, pal, NULL));
  }
  Listgame *lg = (Listgame *) list_game->get_selected();
  if(lg && list_game->in_listbox(result))
    refresh_player();
  if(lg && (list_game->in_listbox(inter->double_clicked) || result==b_join)) {
    Packet_gameinfo *p = lg->p;
    if(p->version==20 || p->version==22 || p->version==23 || p->version==Config::net_version) {
      if(address[0] == 0)
        refresh();
      join_game(p->name, p->from_addr, p->port);
    }
  }
  if(result == cancel) {
    ret();
    return;
  }
}

void Menu_multi_join::join_game(char *nam, Dword adr, int port) {
  removewatch();
  call(new Join_game(bit_, inter->font, font2_, pal, nam, adr, port, false));
}

void Menu_multi_join::refresh() {
  call(new Menu_multi_refresh(this));
}

void Menu_multi_join::clear_game_info() {
  list_player->clear();
  z_game_type->set_val("");
  z_game_speed->set_val("");
  z_game_level->set_val("");
  z_game_minimum->set_val("");
  z_game_end->set_val("");
  z_game_status->set_val("");
}

void Menu_multi_join::refresh_player() {
  clear_game_info();
  Listgame *lg = (Listgame *) list_game->get_selected();
  Packet_gameinfo *p = lg->p;

  for(int i=0; i<p->players.size(); i++) {
    char name[256];
    strcpy(name, p->players[i]->name);
    if(p->players[i]->idle==3)
      strcat(name, " *");
    list_player->add_item(new Listable(name, fteam[p->players[i]->team]));
  }

  if(p->version >= 20) {
    const char *gtype = "Free for all";
    if(p->survivor)
      gtype = "Survivor";
    if(p->normal_attack.type == ATTACK_NONE
       && p->clean_attack.type == ATTACK_NONE)
      gtype = "Peace";
    if(p->normal_attack.type == ATTACK_BLIND
       || p->normal_attack.type == ATTACK_FULLBLIND)
      gtype = "Blind";
    if(p->hot_potato)
      gtype = "Hot potato";
    z_game_type->set_val(gtype);
    if(!p->nolevel_up)
      z_game_speed->set_val("At 15 lines");
    else
      z_game_speed->set_val("Disabled");
    game_level_start = (int) p->level_start;
    z_game_level->set_val(&game_level_start);
    if(p->combo_min == 1)
      z_game_minimum->set_val("Single");
    else if(p->combo_min == 2)
      z_game_minimum->set_val("Double");
    else if(p->combo_min == 3)
      z_game_minimum->set_val("Triple");
    else if(p->combo_min == 4)
      z_game_minimum->set_val("Quad");
    else {
      char stbuf[16];
      snprintf(stbuf, sizeof(stbuf), "%i-lines", p->combo_min);
      z_game_minimum->set_val(stbuf);
    }
    switch(p->game_end) {
      case 0: z_game_end->set_val("Never"); break;
      case 1:
        sprintf(game_end_text, "%i %s", p->game_end_value, "frags.");
        z_game_end->set_val(game_end_text);
        break;
      case 2:
        sprintf(game_end_text, "%i %s", p->game_end_value / 6000, "minutes.");
        z_game_end->set_val(game_end_text);
        break;
      case 3:
        sprintf(game_end_text, "%i %s", p->game_end_value, "points.");
        z_game_end->set_val(game_end_text);
        break;
      case 4:
        sprintf(game_end_text, "%i %s", p->game_end_value, "lines.");
        z_game_end->set_val(game_end_text);
        break;
    }
    if(p->terminated) {
      z_game_status->set_val("Terminated");
    } else {
      if(p->delay_start)
        z_game_status->set_val("Not yet started");
      else
        z_game_status->set_val("Playing");
    }
  }

  if(p->version < 20/*Config::net_version*/) {
    z_game_type->set_val("* older version *");
  }
  if(p->version > Config::net_version) {
    z_game_type->set_val("* newer version *");
  }
}

void Menu_multi_join::net_call(Packet *p2) {
  Packet_gameinfo *p=(Packet_gameinfo *)p2;
//  char ad[256];
//  Net::stringaddress(ad, p->from_addr);

  if(p->name[0])
    sprintf(st, "%s", p->name);
  else
    sprintf(st, "[No name]");
  Listgame *lg = new Listgame(st, p); // the packet 'p' will be deleted by ~Listgame

  int deja = list_game->search(lg);
  if(deja != -1) {
    list_game->replace_item(deja, lg);
    if(selected_game != -1)
      if(list_game->get_selected() == lg)
        refresh_player();
  } else {
    list_game->add_item(lg);
  }
}

void Zone_input_address::lost_focus(int cancel) {
  Zone_text_input::lost_focus(cancel);
  if(!cancel && val[0] != 0) {
    parent->refresh();
  }
}

Menu_multi_refresh::Menu_multi_refresh(Menu_multi_join *p): Menu(p->inter) {
  parent = p;
}

void Menu_multi_refresh::init() {
  Menu::init();
  parent->list_game->clear();
  parent->clear_game_info();

  if(parent->address[0] != 0) {
    resolve();
    return;
  }
  if(parent->local_net)
    find_local_games();
  else
    find_internet_games();
}

void Menu_multi_refresh::find_local_games() {
  Packet_findgame p;
  Dword to;
  parent->addwatch();
  to = INADDR_BROADCAST;
  net->sendudp(to, &p);
  const char *error = net->failed();
  if(error) {
    call(new Menu_net_problem(error, "You can enter the TCP/IP address of "
                              "the server instead.", parent->bit_,
                              inter->font));
  }
  ret();
}

void Menu_multi_refresh::resolve() {
  Dword to = net->getaddress(parent->address);
  if(to > 0) {
    ret();
    parent->join_game(NULL, to, net->port_resolve);
  } else {
    cancel = NULL;
    call(new Wait_time(50)); // waits 1/2 second (in case the hosts resolves itself very quickly)
  }
}

void Menu_multi_refresh::find_internet_games() {
  exec(new Menu_multi_internet(parent));
}

void Menu_multi_refresh::step() {
  Menu::step();
  if(!cancel) {
    (void)new Zone_bitmap(inter, parent->bit_, 0, 0);
    (void)new Zone_text(fteam[7], inter, "Looking for server name on "
                        "Internet. This can take several minutes...", 130);
    cancel = new Zone_text_button2(inter, parent->bit_, parent->font2_,
                                   "·2 Cancel", 190);
  }
  if(net->name_resolve != (unsigned int)-1) {
    Dword name_temp = net->name_resolve;
    net->name_resolve = (Dword)-1;
    if(name_temp == 0) {
      (void)new Zone_text(fteam[7], inter, "Unable to locate host. Please verify and try again.", 220);
    } else {
      int port = net->port_resolve;
      if(!port)
        port = config.info.port_number; // default value
      parent->join_game(NULL, name_temp, port);
      ret();
      return;
    }
  }
  if(result == cancel || input->last_key.sym == SDLK_ESCAPE) {
    net->gethostbyname_cancel();
    input->last_key.sym = SDLK_UNKNOWN;
    ret();
  }
}

Menu_multi_internet::Menu_multi_internet(Menu_multi_join *p): Menu(p->inter) {
  parent = p;
  new Zone_bitmap(inter, parent->bit_, 0, 0);
  new Zone_text(fteam[7], inter, "Searching for public Internet games...",
                130);
  cancel = new Zone_text_button2(inter, parent->bit_, parent->font2_,
                                 "·2 Cancel", 190);
  request = NULL;
}

Menu_multi_internet::~Menu_multi_internet() {
  if(request)
    delete request;
}

void Menu_multi_internet::init() {
  Menu::init();
  const char *msg;

  request = new Qserv();
  request->add_data("getgames\n");
  request->send();
  msg = net->failed();
  if(msg) {
    exec(new Menu_net_problem(msg, "Either the server is down, or this "
                              "address isn't a Quadra game server.",
                              parent->bit_, inter->font));
    delete request;
    request = NULL;
  }
}

void Menu_multi_internet::step() {
  Menu::step();
  if(!request)
    return;
  bool done = request->done();
  if(net->failed()) {
    exec(new Menu_net_problem("No Quadra server found at specified address.",
                              "Either the server is down, or this address "
                              "isn't a Quadra game server.", parent->bit_,
                              inter->font));
    return;
  }
  if(done)
    parsegames();
  if(result == cancel || input->last_key.sym == SDLK_ESCAPE) {
    input->last_key.sym = SDLK_UNKNOWN;
    ret();
  }
}

Attack Menu_multi_internet::parse_attack(const char *s) {
  Attack ret;
  if(s) {
    ret.type = (Attack_type) atoi(s);
    const char *sep=strchr(s, ' ');
    if(sep)
      ret.param=atoi(sep+1);
  }
  return ret;
}

void Menu_multi_internet::parsegames() {
  ret();
  if(!request->get_status() || strcmp(request->get_status(), "Current games"))
    return;

  Dict *games=request->get_reply();
  if(_debug) {
    games->dump();
  }
  unsigned int i, j;
  for(i = 0; i < games->size(); i++) {
    const Dict *d = games->get_sub(i);
    Packet_gameinfo *p=new Packet_gameinfo();

    p->from = NULL;
//    Word port=0;
    char cip[32];
    strcpy(cip, d->get_key());
    char *cport=strchr(cip, ':');
    if(cport) {
//      port=atoi(cport+1);
      *cport=0; //Cut string at ':' separator
    }
    p->from_addr = Net::dotted2addr(cip);
    const char *temp;
    temp = d->find("name");
    if(temp)
      strcpy(p->name, temp);
    temp = d->find("version");
    if(temp)
      p->version = atoi(temp);
    temp = d->find("port");
    if(temp)
      p->port = atoi(temp);

    const Dict *players = d->find_sub("players");
    if(players) {
      for(j=0; j < players->size(); j++) {
        const Dict *d2 = players->get_sub(j);
        const char *name = d2->find("name");
        temp = d2->find("team");
        int team=0;
        if(temp) {
          team = atoi(temp);
          if(team<0 || team>7)
            team=0;
        }
        int status = -1;
        temp = d2->find("status");
        if(temp) {
          status = atoi(temp);
          if(status<0 || status>3)
            status = -1;
        }
        int handicap=0;
        temp = d2->find("handicap");
        if(temp) {
          handicap = atoi(temp);
          if(handicap<0 || handicap>4)
            handicap=0;
        }
        p->add_player(0, team, name, status, handicap);
      }
    }

    const Dict *end = d->find_sub("end");
    if(end) {
      temp = end->find("value");
      if(temp)
        p->game_end_value = atoi(temp);
      temp = end->find("type");
      if(temp)
        p->game_end = atoi(temp);
    }

    const Dict *rules = d->find_sub("rules");
    if(rules) {
      temp = rules->find("levelup");
      if(temp)
        p->nolevel_up = atoi(temp) ? false:true;
      temp = rules->find("levelstart");
      if(temp)
        p->level_start = atoi(temp);
      temp = rules->find("mincombo");
      if(temp)
        p->combo_min = atoi(temp);
      temp = rules->find("allowhandicap");
      if(temp)
        p->allow_handicap = atoi(temp) ? true : false;
      temp = rules->find("survivor");
      if(temp)
        p->survivor = atoi(temp) ? true : false;
      temp = rules->find("hot_potato");
      if(temp)
        p->hot_potato = atoi(temp) ? true : false;
      const Dict *attacks = rules->find_sub("attacks");
      if(attacks) {
        temp = attacks->find("normal");
        p->normal_attack = parse_attack(temp);
        temp = attacks->find("clean");
        p->clean_attack = parse_attack(temp);
        temp = attacks->find("potato_normal");
        p->potato_normal_attack = parse_attack(temp);
        temp = attacks->find("potato_clean");
        p->potato_clean_attack = parse_attack(temp);
      }
    }

    const Dict *status = d->find_sub("status");
    if(status) {
      temp = status->find("started");
      if(temp)
        p->delay_start = atoi(temp) ? false:true;
      temp = status->find("terminated");
      if(temp)
        p->terminated = atoi(temp) ? true:false;
    }

    parent->net_call(p);
  }
}

Menu_single::Menu_single() {
  {
    Res_doze res("multi.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }

  pal.set_size(256); //Useless but whatever
  set_fteam_color(pal);

  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  font2 = new Font(*fonts.normal, pal, 255,255,0);
  (void)new Zone_bitmap(inter, bit, 0, 0, true);
  (void)new Zone_text(inter, "Select game type:", 20);
  normal = new Zone_text_button2(inter, bit, font2, "Normal", 180);
  sprint = new Zone_text_button2(inter, bit, font2, "Sprint (5 minutes)", 220);
  cancel = new Zone_text_button2(inter, bit, font2, "Back ·0", 560, 450);
}

void Menu_single::step() {
  Menu_standard::step();
  if(result == normal) {
    call(new Fade_in(pal));
    call(new Single_player(PRESET_SINGLE));
    call(new Fade_out(pal));
  }
  if(result == sprint) {
    call(new Fade_in(pal));
    call(new Single_player(PRESET_SINGLE_SPRINT));
    call(new Fade_out(pal));
  }
  if(result == cancel)
    quit=true;
}

Menu_single::~Menu_single() {
  delete font2;
}

Menu_multi::Menu_multi() {
  {
    Res_doze res("multi.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }

  pal.set_size(256); //Useless but whatever
  set_fteam_color(pal);

  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  font2 = new Font(*fonts.normal, pal, 255,255,0);
  (void)new Zone_bitmap(inter, bit, 0, 0, true);
  (void)new Zone_text(inter, "Please select a connection type:", 20);
  local = new Zone_text_button2(inter, bit, font2, "Local game (no network)",
                                180);
  net_lan = new Zone_text_button2(inter, bit, font2, "TCP/IP Local "
                                  "network (LAN)", 220);
  net_internet = new Zone_text_button2(inter, bit, font2, "TCP/IP Internet",
                                       260);
  cancel = new Zone_text_button2(inter, bit, font2, "Back ·0", 560, 450);
}

void Menu_multi::step() {
  Menu_standard::step();
  if(result == local) {
    call(new Create_game(bit, inter->font, font2, pal, false, false));
  }
  if(result == net_lan) {
    call(new Menu_multi_join(bit, inter->font, font2, pal, true));
  }
  if(result == net_internet) {
    call(new Menu_multi_join(bit, inter->font, font2, pal, false));
  }
  if(result == cancel)
    quit=true;
}

Menu_multi::~Menu_multi() {
  delete font2;
}

Menu_setup::Menu_setup() {
  {
    Res_doze res("setup.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }
  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  set_fteam_color(pal);
  (void)new Zone_bitmap(inter, bit, 0, 0, true);

  Zone_state_text2 *z_setup_player = new Zone_state_text2(inter, &config.info.setup_player, 333, 11);
  z_setup_player->add_string("1");
  z_setup_player->add_string("2");
  z_setup_player->add_string("3");

  z_nameinput = new Zone_text_input(inter, pal, config.player[config.info.setup_player].name, 40, 333, 106, 110, 100);

  z_passwdinput = new Zone_text_input(inter, pal, config.player2[config.info.setup_player].ngPasswd, 64, 333, 132, 200);

  z_shadow = new Zone_state_text2(inter, &config.player[config.info.setup_player].shadow, 333, 161);
  z_shadow->add_string("No");
  z_shadow->add_string("Yes");

  z_smooth = new Zone_state_text2(inter, &config.player[config.info.setup_player].smooth, 333, 192);
  z_smooth->add_string("No");
  z_smooth->add_string("Yes");

  z_h_repeat = new Zone_state_text(inter, &config.player2[config.info.setup_player].h_repeat, 333, 220);
  z_h_repeat->add_string("Slow");
  z_h_repeat->add_string("Normal");
  z_h_repeat->add_string("Fast");
  z_h_repeat->add_string("Faster");

  z_v_repeat = new Zone_state_text(inter, &config.player2[config.info.setup_player].v_repeat, 333, 252);
  z_v_repeat->add_string("Slow");
  z_v_repeat->add_string("Normal");
  z_v_repeat->add_string("Fast");
  z_v_repeat->add_string("Faster");

  z_continuousdown = new Zone_state_text(inter, &config.player2[config.info.setup_player].continuous, 333, 281);
  z_continuousdown->add_string("No");
  z_continuousdown->add_string("Yes");

  z_key[0] = new Zone_set_key(inter, &config.player[config.info.setup_player].key[0], 148, 347);
  z_key[1] = new Zone_set_key(inter, &config.player[config.info.setup_player].key[1], 148, 378);
  z_key[3] = new Zone_set_key(inter, &config.player[config.info.setup_player].key[3], 148, 408);
  z_key[6] = new Zone_set_key(inter, &config.player2[config.info.setup_player].key[1], 148, 439);
  z_key[4] = new Zone_set_key(inter, &config.player[config.info.setup_player].key[4], 513, 350);
  z_key[2] = new Zone_set_key(inter, &config.player[config.info.setup_player].key[2], 513, 380);
  z_key[5] = new Zone_set_key(inter, &config.player2[config.info.setup_player].key[0], 513, 411);

  b_player = new Zone_menu(inter, bit, "setup0.png", 195, 11);
  b_player->set_child(z_setup_player);
  (new Zone_menu(inter, bit, "setup1.png", 255, 101))->set_child(z_nameinput);
  (new Zone_menu(inter, bit, "setup2.png", 222, 132))->set_child(z_passwdinput);
  (new Zone_menu(inter, bit, "setup3.png", 237, 161))->set_child(z_shadow);
  (new Zone_menu(inter, bit, "setup4.png", 240, 192))->set_child(z_smooth);
  (new Zone_menu(inter, bit, "setup5.png", 157, 220))->set_child(z_h_repeat);
  (new Zone_menu(inter, bit, "setup6.png", 184, 252))->set_child(z_v_repeat);
  (new Zone_menu(inter, bit, "setup7.png", 153, 281))->set_child(z_continuousdown);
  b_key[0] = new Zone_menu(inter, bit, "setup8.png", 45, 347);
  b_key[1] = new Zone_menu(inter, bit, "setup9.png", 32, 378);
  b_key[3] = new Zone_menu(inter, bit, "setup10.png", 26, 408);
  b_key[6] = new Zone_menu(inter, bit, "setup11.png", 19, 439);
  b_key[4] = new Zone_menu(inter, bit, "setup12.png", 344, 350);
  b_key[2] = new Zone_menu(inter, bit, "setup13.png", 275, 380);
  b_key[5] = new Zone_menu(inter, bit, "setup14.png", 382, 411);
  b_all_key = new Zone_menu(inter, bit, "setup15.png", 390, 439);

  for(int i=0; i<7; i++)
    b_key[i]->set_child(z_key[i]);

  b_quit = new Zone_text_button2(inter, bit, fteam[4], "Back ·0", 560, 450);
}

Menu_setup::~Menu_setup() {
  int i;
  for(i=0; i<3; i++)
    if(!config.player[i].name[0]) {
      char st[16];
      sprintf(st,"#%i", i+1);
      strcpy(config.player[i].name, st);
    }
  config.write();
}

void Menu_setup::step() {
  Menu_standard::step();
  if(!result) {
    return;
  }

  if(result == b_quit) {
    quit = true;
  }
  if(result == b_player) {
    z_nameinput->set_val(config.player[config.info.setup_player].name);
    z_passwdinput->set_val(config.player2[config.info.setup_player].ngPasswd);
    z_shadow->set_val(&config.player[config.info.setup_player].shadow);
    z_smooth->set_val(&config.player[config.info.setup_player].smooth);
    z_h_repeat->set_val(&config.player2[config.info.setup_player].h_repeat);
    z_v_repeat->set_val(&config.player2[config.info.setup_player].v_repeat);
    z_continuousdown->set_val(&config.player2[config.info.setup_player].continuous);
    for(int i=0; i<7; i++)
      if(i<5)
        z_key[i]->set_val(&config.player[config.info.setup_player].key[i]);
      else
        z_key[i]->set_val(&config.player2[config.info.setup_player].key[i-5]);
  }
  if(result == b_all_key) {
    call(new Menu_setup_all_key(inter, z_key));
  }
  for(int i=0; i<7; i++) {
    if(result==b_key[i]) {
      call(new Menu_setup_key(inter, (Zone_set_key *) z_key[i],
                              "Press a key..."));
    }
  }
}

Menu_setup_all_key::Menu_setup_all_key(Inter *in, Zone_set_key *k[]): Menu(in) {
  key = k;
  quel=0;
}

void Menu_setup_all_key::step() {
  static char touche[7] = {0, 1, 3, 4, 2, 5, 6};
  Menu::step();
  if(quel > 6 || input->keys[SDLK_ESCAPE] & PRESSED) {
    ret();
    return;
  }
  const char *te = NULL;
  switch(touche[quel]) {
    case 0: te = "Press a key for left..."; break;
    case 1: te = "Right..."; break;
    case 2: te = "Rotate counter clock-wise..."; break;
    case 3: te = "Down..."; break;
    case 4: te = "Rotate clock-wise..."; break;
    case 5: te = "Rotate twice..."; break;
    case 6: te = "Instant drop..."; break;
  }
  call(new Menu_setup_key(inter, key[touche[quel]], te));
  quel++;
}

Menu_setup_key::Menu_setup_key(Inter *in, Zone_set_key *k, const char *t): Menu(in) {
  new Zone_text_field(inter, t, 180, 315, 280);
  key = k;
}

Menu_setup_key::Menu_setup_key(Inter *in, Zone_set_key *k): Menu(in) {
  key = k;
}

void Menu_setup_key::init() {
  input->clear_key();
}

void Menu_setup_key::step() {
  Menu::step();
  int loop = 0;
  for(int i = SDLK_FIRST; i < SDLK_LAST; ++i) {   
    if(input->keys[i] & PRESSED) {
      loop = i;
      break;
    }
  }
  if(loop == 0) return;
  
	input->last_key.sym = SDLK_UNKNOWN;
  // denies Enter (reserved for multiplayer chat)
	if(loop == SDLK_RETURN) return;
	if(loop != SDLK_ESCAPE)
	{
		*(key->val) = loop;
		key->process();
	}
	ret();
}

Menu_quitgame::Menu_quitgame() {
  int y;
  for(y=0; y<8; y++)
    pal.setcolor(y, y*255/7, y*255/7, y*255/7);
  pal.setcolor(255, 255, 255, 255);
  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  for(y=8; y<16; y++)
    pal.setcolor(y, 0, 0, y*255/7);
  font2 = new Font(*fonts.normal, pal, 0,0,255);
}

Menu_quitgame::~Menu_quitgame() {
  delete font2;
}

void Menu_quitgame::init() {
  Menu_standard::init();
  (void)new Zone_clear(inter);
  (void)new Zone_text(inter, "Quadra", 0);
  (void)new Zone_text(inter, "You have been playing an unregistered "
                      "version of Quadra.", 10, 40);
  int t,h,m,s;
  t = overmind.framecount / 100;
  h = t / 60 / 60;
  t = t % 3600;
  m = t / 60;
  s = t % 60;
  if(h > 0) {
    sprintf(st, "In fact, you played for %i hours and %i minutes!", h, m);
  } else if(m > 0) {
    sprintf(st, "In fact, you played for %i minutes %i seconds.", m, s);
  } else {
    sprintf(st, "You spent about %i seconds checking it.", s);
  }
  (void)new Zone_text(inter, st, 10, 60);
  (void)new Zone_text(inter, "Was it enjoyable?  Was it incredibly fun?",
                      10, 80);
  (void)new Zone_text(inter, "Feel free to distribute Quadra to everyone "
                      "you want!", 10, 300);
#ifdef UGS_DIRECTX
  (void)new Zone_text(inter, "We even got a Linux version, networkable "
                      "with yours!", 10, 360);
#else
  (void)new Zone_text(inter, "We even got a Windows version, networkable "
                      "with yours!", 10, 360);
#endif
  (void)new Zone_text(inter, "So watch out for Ludus Design products, "
                      "and remember:", 10, 380);
  (void)new Zone_text(font2, inter, "We do great stuff for you!", 20, 410);
}

void Menu_quitgame::step() {
  Menu_standard::step();
  if(!quit)
    call(new Wait_time(24000));
  quit=true;
}

Menu_help::Menu_help() {
  Bitmap *bit;
  {
    Res_doze res("multi.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }
  pal.set_size(256);
  set_fteam_color(pal);

  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  font2 = fteam[4];
  (void)new Zone_bitmap(inter, bit, 0, 0, true);
  b_quit = new Zone_text_button2(inter, NULL, font2, "Back ·0", 560, 450);
  int y;
  (void)new Zone_text(inter, "Thanks for downloading Quadra!", 20);
  y = 220;
  b_online=NULL;

  Zone_text *temp;
  (void)new Zone_text(fteam[3], inter, "The Quadra team:", y); y+=20;
  (void)new Zone_text(inter, "Rémi Veilleux: Programming, design, graphics", 10, y); y+=20;
  (void)new Zone_text(inter, "Stéphane Lajoie: Programming", 10, y); y+=20;
  (void)new Zone_text(inter, "Pierre Phaneuf: Linux port", 10, y); y+=20;
  (void)new Zone_text(inter, "Sylvain Fortin: Glue", 10, y); y+=20;
  (void)new Zone_text(inter, "Véronique Bruneau: Graphics", 10, y); y+=20;
  (void)new Zone_text(inter, "Our web site:", 10, y);
  b_www = temp = new Zone_text_select(inter, fteam[4],
                                      "http://quadra.sourceforge.net/",
                                      160, y);
  temp->set_font(fteam[6]);
}

void Menu_help::init() {
  Menu_standard::init();
  sons.levelup->play(0, 0, 11000);
}

void Menu_help::step() {
  Menu_standard::step();
  if(!result)
    return;
  if(result == b_quit)
    quit = true;
#ifdef UGS_DIRECTX
  if(result == b_www)
    call_internet("http://quadra.sourceforge.net/");
  if(result == b_online)
    call_internet("http://www.ludusdesign.com/quadra/register.html");
#endif
}

void Menu_help::call_internet(const char *s) {
  call(new Fade_in(pal));
  call(new Menu_internet(s));
  call(new Fade_to(Palette(), pal));
}

Menu_option::Menu_option() {
  {
    Res_doze res("multi.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }
  pal.set_size(256);
  set_fteam_color(pal);

  Palette ptemp;
  {
    Res_doze res("fond0.png");
    Png png(res);
    ptemp.load(png);
  }
  for(int i=184; i<256; i++) // copies the colors of the blocs from fond0.png
    pal.setcolor(i, ptemp.r(i), ptemp.g(i), ptemp.b(i));
  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  set_fteam_color(pal);
  font2 = fteam[4];
  (void)new Zone_bitmap(inter, bit, 0, 0, true);
  (void)new Zone_text(inter, "Game options", 20);
  b_quit = new Zone_text_button2(inter, bit, font2, "Back ·0", 560, 450);

  // FIXME: Configuration for the removed CD music player. Should be taken out.
  (void)new Zone_text(fteam[7], inter, "Select CD playing mode:", 40, 130);
  {
    Zone_state_text2 *temp =  new Zone_state_text2(inter,
                                                   &config.info.cdmusic,
                                                   380, 130);
    temp->add_string("No music");
    temp->add_string("Auto-change track");
    temp->add_string("Loop all tracks");
  }

  (void)new Zone_text(fteam[7], inter, "Mouse cursor speed (1-255):", 40, 160);
  (void)new Zone_input_numeric(inter, &config.info.mouse_speed, 4, 1, 255, pal, 380, 160, 60);
  old_port = config.info.port_number;

  (void)new Zone_text(inter, "Advanced options:", 40, 220);

  (void)new Zone_text(fteam[7], inter, "TCP/IP port number (1024-65535):",
                      40, 250);
  (void)new Zone_input_numeric(inter, &config.info.port_number, 6, 1024, 65535, pal, 380, 250, 60);
  (void)new Zone_text(fteam[3], inter, "(Note: you must have the same port "
                      "number to connect to another computer)", 40, 274);

  (void)new Zone_text(fteam[7], inter, "Public game server address:", 40, 310);
  (void)new Zone_text_input(inter, pal, config.info.game_server_address, 255, 380, 310, 240);
  char stbuf[512];
  snprintf(stbuf, sizeof(stbuf), "Default server: %s",
           config.info3.default_game_server_address);
  (void)new Zone_text(fteam[3], inter, stbuf, 40, 334);
  strcpy(old_server, config.info.game_server_address);

	(void)new Zone_text(fteam[7], inter, "HTTP proxy server:", 40, 370);
	(void)new Zone_text_input(inter, pal, config.info2.proxy_address, 127, 380, 370, 240);
	strcpy(old_proxy, config.info2.proxy_address);
}

Menu_option::~Menu_option() {
  config.write();
  if(strcmp(old_server, config.info.game_server_address) || strcmp(old_proxy, config.info2.proxy_address)) {
    Qserv::http_addr=0;
    Qserv::http_port=0;
  }
}

void Menu_option::step() {
  Menu_standard::step();
  if(result == b_quit)
    quit = true;
}

Menu_intro::Menu_intro() {
  int y;
  for(y=0; y<8; y++)
    pal.setcolor(y, y*255/7, y*255/7, y*255/7);
  for(y=8; y<16; y++)
    pal.setcolor(y, 0, y*255/7, y*255/7);
  pal.setcolor(255, 255, 255, 255);
  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  font2 = new Font(*fonts.normal, pal, 0,255,255);
  (void)new Zone_clear(inter);
  (void)new Zone_text(inter, "Quadra(tm) by Ludus Design", 10);
  (void)new Zone_text(inter, "Using Universal Game Skelton(tm)", 10, 40);
  (void)new Zone_text(font2, inter, "Free version released under the "
                      "GNU Lesser General Public License", 10, 80);

  y=130;
  warning=0;
  if(!sound) {
    (void)new Zone_text(inter, "Warning: Failed to initialise sound system",
                        10, y);
    warning++;
    y += 20;
  }
  if(config.warning == 1) {
    (void)new Zone_text(inter, "No configuration file found. Using defaults.", 10, y);
    y += 20;
  }
  if(config.warning == 2) {
    (void)new Zone_text(inter, "Warning: Wrong version in configuration "
                        "file. Using defaults.", 10, y);
    warning++;
    y += 20;
  }
  (void)new Zone_text(inter, "Hit something to continue...", 10, y);
#ifdef ENABLE_VERSION_CHECK
  if(strcmp(VERSION_STRING, config.info3.latest_version) != 0)
  {
    (void)new Zone_text(font2, inter, "A new version of Quadra is available!",
                        10, y + 40);
    (void)new Zone_text(font2, inter, "Get it at http://quadra.googlecode.com",
                        10, y + 60);
  }
#endif
  (void)new Zone_text(inter, "Quadra, Universal Game Skelton and the "
                      "Ludus Design logo are trademarks", 10, 430);
  (void)new Zone_text(inter, "of Ludus Design, Copyright (C) 1998-2000. "
                      "All rights reserved.", 10, 450);
  once = false;
}

void Menu_intro::init() {
  Menu::init();
  call(new Setpalette(pal));
}

void Menu_intro::step() {
  Menu::step();
  ret();
  call(new Menu_main());
  call(new Setpalette(noir));
  call(new Wait_time(3000));
}

Menu_intro::~Menu_intro() {
  delete font2;
}

Menu_main::Menu_main():
	version_warning(false)
{
  {
    Res_doze res("debuto.png");
    Png png(res);
    pal.load(png);
  }
  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  redraw();
}

void Menu_main::redraw() {
  inter->flush();
  Bitmap *background;
  {
    Res_doze res("debuto.png");
    Png png(res);
    background = new Bitmap(png);
  }
  z_back = new Zone_bitmap(inter, background, 0, 0, true);

  b_help = NULL;
  b_single = new Zone_menu(inter, background, "debut0.png", 160, 99);
  b_multi = new Zone_menu(inter, background, "debut1.png", 166, 139);
  b_demo = new Zone_menu(inter, background, "debut2.png", 214, 183);
  b_tut = new Zone_menu(inter, background, "debut3.png", 235, 225);
  b_setup = new Zone_menu(inter, background, "debut4.png", 221, 267);
  b_option = new Zone_menu(inter, background, "debut5.png", 264, 310);
  b_help = new Zone_menu(inter, background, "debut6.png", 261, 351);
  b_quit = new Zone_menu(inter, background, "debut7.png", 295, 392);

  sprintf(st, "Quadra version %s", VERSION_STRING);
  new Zone_text(inter, st, 450, 430);
  
  b_logo = new Zone_menu(inter, background, "debut8.png", 0, 390);
}

void Menu_main::init() {
  Menu::init();
  // FIXME: The following line might not be necessary without the music.
  call(new Wait_time(6)); // to force the palette being set BEFORE the
                          // music starts
  call(new Setpalette(pal));
  reset_delay();
}

void Menu_main::reset_delay() {
	delay = 12000;
  
	if(!version_warning)
	{
		if(strcmp(VERSION_STRING, config.info3.latest_version) != 0)
		{
			(void)new Zone_text(inter, "New version available", 455, 445);
			(void)new Zone_text(inter, "Get it at http://quadra.googlecode.com", 330, 460);
			version_warning = true;
		}
	}
}

void Menu_main::step() {
  Menu::step();
  delay--;
  if(delay < 0) {
    call(new Fade_in(pal));
    if(1/*ugs_random.rnd(1) == 0*/) { // always pop a demo (highscore menu sucks)
      int i=ugs_random.rnd(3);
      char st[20];
      sprintf(st, "demo%02i.rec", i);
      call(new Call_setfont(pal, new Demo_multi_player(new Res_compress(st, RES_READ, true), true)));
    }
    else
      call(new Menu_highscore());
    call(new Fade_out(pal));
    reset_delay();
  }
  if(result == b_quit || input->last_key.sym == SDLK_ESCAPE || quitting) {
    input->last_key.sym = SDLK_UNKNOWN;
    exec(new Fade_out(pal));
  }
  if(result == b_tut) {
    call(new Fade_in(pal));
    call(new Menu_highscore(-1, NULL, true));
    call(new Fade_out(pal));
  }
#ifdef UGS_DIRECTX
  if(result == b_logo) {
    call(new Fade_in(pal));
    call(new Menu_internet("http://quadra.sourceforge.net/"));
    call(new Fade_to(Palette(), pal));
  }
#endif
  if(result == b_option) {
    call(new Fade_in(pal));
    call(new Menu_option());
    call(new Fade_out(pal));
  }
  if(result && result == b_help) {
    call(new Fade_in(pal));
    call(new Menu_help());
    call(new Fade_out(pal));
  }
  if(result == b_setup) {
    call(new Fade_in(pal));
    call(new Menu_setup());
    call(new Fade_out(pal));
  }
  if(result == b_multi) {
    call(new Fade_in(pal));
    call(new Menu_multi());
    call(new Fade_out(pal));
  }
  if(result == b_single) {
    //-roncli 4/29/01 This doesn't suck anymore. :-D
    call(new Fade_in(pal));
    call(new Menu_single());
    call(new Fade_out(pal));
    
  }
  if(result == b_demo) {
    call(new Fade_in(pal));
    call(new Menu_demo_central());
    call(new Fade_out(pal));
  }
//  if(result && result==z_back) {
//    Bulle::test(*z_back->actual);
//    video->need_paint=2;
//  }
  if(result)
    reset_delay();
}

Menu_stat::Colonne::Colonne() {
  sort_me = false;
}

void Menu_stat::Colonne::set_titre(const char *s) {
  strcpy(titre, s);
}

Menu_stat::Menu_stat():
c_start(105), c_gap(4) {
  int i;
  {
    Res_doze res("result.png");
    Png raw(res);
    bit = new Bitmap(raw);
    pal.load(raw);
  }
  pal.set_size(256);
  Palette temp;
  {
    Res_doze res("fond0.png");
    Png png(res);
    temp.load(png);
  }
  for(i=184; i<256; i++) // copie les couleurs des blocs de l'image fond0.png
    pal.setcolor(i, temp.r(i), temp.g(i), temp.b(i));

  set_fteam_color(pal);
  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  fcourrier[0] = new Font(*fonts.courrier, pal, 255,125,0);
  fcourrier[1] = new Font(*fonts.courrier, pal, 0,225,255);
  fcourrier[2] = new Font(*fonts.courrier, pal, 255,0,0);
  fcourrier[3] = new Font(*fonts.courrier, pal, 255,0,255);
  fcourrier[4] = new Font(*fonts.courrier, pal, 255,255,0);
  fcourrier[5] = new Font(*fonts.courrier, pal, 0,255,0);
  fcourrier[6] = new Font(*fonts.courrier, pal, 0,0,255);
  fcourrier[7] = new Font(*fonts.courrier, pal, 170,170,170);
  font2 = fteam[4];
  (void)new Zone_bitmap(inter, bit, 0, 0);
  b_quit = new Zone_text_button2(inter, bit, font2, "Quit ·0", 340, 455);
  Zone_text_button *z;
  int x(8), wid(105);
  z=new Zone_text_button2(inter, bit, font2, "Rank", x, 80, wid-8);
  b_page.add(z);
  x+=wid;
  z=new Zone_text_button2(inter, bit, font2, "Speed", x, 80, wid-8);
  b_page.add(z);
  x+=wid;
  z=new Zone_text_button2(inter, bit, font2, "Lines", x, 80, wid-8);
  b_page.add(z);
  x+=wid;
  z=new Zone_text_button2(inter, bit, font2, "Combos", x, 80, wid-8);
  b_page.add(z);
  x+=wid;

  b_restart = b_stop = NULL;

  init_columns(bit);

  active_sort = -1;
  active_page = -1;
  change_page(0);

  notify();
  (void)new Chat_interface(inter, pal, bit->surface, 427, 47, 212, 410);
  if(game)
    game->net_list.add_watch(this);
}

Menu_stat::~Menu_stat() {
  col.deleteall();
  if(game)
    game->net_list.remove_watch(this);
  for(int i=0; i<MAXTEAMS; i++)
    delete fcourrier[i];
  if(bit)
    delete bit;
}

void Menu_stat::init_columns(Bitmap *bit) {
  int i, page=0;
  Colonne *n;
  n=new Colonne();
  n->quel_stat = CS::FRAG;
  n->width = 70;
  n->set_titre("Frags");
  col.add(n);

  n=new Colonne();
  n->quel_stat = CS::DEATH;
  n->width = 70;
  n->set_titre("Deaths");
  col.add(n);

  n=new Colonne();
  n->quel_stat = CS::SCORE;
  n->width = 90;
  n->set_titre("Score");
  col.add(n);

  n=new Colonne();
  n->quel_stat = CS::LINESTOT;
  n->width = 90;
  n->set_titre("Lines");
  col.add(n);

  int px;
  px = c_start;
  for(i=0; i<4; i++) {
    add_title(*col[i], &px, bit);
    col[i]->page = page;
  }

  page++;
  px = c_start;
  n=new Colonne();
  n->quel_stat = CS::PPM;
  n->width = 150;
  n->set_titre("Points per min.");
  n->page = page;
  add_title(*n, &px, bit);
  col.add(n);

  n=new Colonne();
  n->quel_stat = CS::BPM;
  n->width = 150;
  n->set_titre("Blocks per min.");
  n->page = page;
  add_title(*n, &px, bit);
  col.add(n);

  page++;
  px = c_start;
  for(i=0; i<8; i++) {
    n=new Colonne();
    n->quel_stat = CS::clear_trans(i);
    n->width = 40;
    sprintf(st, "%i", i+1);
    n->set_titre(st);
    n->page = page;
    add_title(*n, &px, bit);
    col.add(n);
  }

  page++;
  px = c_start;
  for(i=8; i<16; i++) {
    n=new Colonne();
    n->quel_stat = CS::clear_trans(i);
    n->width = 40;
    sprintf(st, "%i", i+1);
    n->set_titre(st);
    n->page = page;
    add_title(*n, &px, bit);
    col.add(n);
  }

  page++;
  px = c_start;
  for(i=0; i<8; i++) {
    n=new Colonne();
    n->quel_stat = (CS::Stat_type) (CS::COMBO00+i);
    n->width = 40;
    sprintf(st, "%i", i+1);
    n->set_titre(st);
    n->page = page;
    add_title(*n, &px, bit);
    col.add(n);
  }

  page++;
  px = c_start;
  for(i=8; i<16; i++) {
    n=new Colonne();
    n->quel_stat = (CS::Stat_type) (CS::COMBO00+i);
    n->width = 40;
    sprintf(st, "%i", i+1);
    n->set_titre(st);
    n->page = page;
    add_title(*n, &px, bit);
    col.add(n);
  }
}

void Menu_stat::add_title(Colonne &col, int *px, Bitmap *bit) {
  col.z_titre = new Zone_text_button2(inter, bit, font2, col.titre, *px+3, 120, col.width-6);
  col.z_titre->disable();
  *px += col.width;
}

void Menu_stat::set_sort(int quel) {
  if(active_sort == quel)
    return;
  col[quel]->sort_me = true;
  col[quel]->z_titre->set_font(fteam[4]);

  if(active_sort != -1) {
    col[active_sort]->sort_me = false;
    col[active_sort]->z_titre->set_font(inter->font);
  }
  active_sort = quel;
}

void Menu_stat::change_page(int p) {
  int i, last_page=0;
  for(i=0; i<col.size(); i++) {
    if(col[i]->page == active_page) {
      col[i]->z_titre->disable();
    }
    if(col[i]->page>last_page)
      last_page=col[i]->page;
  }
  switch(active_page) {
    case 0:
    case 1:
      b_page[active_page]->set_font(inter->font);
      break;
    case 2:
    case 3:
      b_page[2]->set_font(inter->font);
      break;
    case 4:
    case 5:
      b_page[3]->set_font(inter->font);
      break;
  }
  switch(p) {
    case 0:
      active_page=0;
      b_page[0]->set_font(fteam[4]);
      break;
    case 1:
      active_page=1;
      b_page[1]->set_font(fteam[4]);
      break;
    case 2:
      if(active_page==2)
        active_page=3;
      else
        active_page=2;
      b_page[2]->set_font(fteam[4]);
      break;
    case 3:
      if(active_page==4)
        active_page=5;
      else
        active_page=4;
      b_page[3]->set_font(fteam[4]);
      break;
  }
  if(active_page > last_page)
    active_page = 0;
  for(i=0; i<col.size(); i++)
    if(col[i]->page == active_page) {
      col[i]->z_titre->enable();
    }
  notify();
}

void Menu_stat::calculate_total(bool force_blit) {
  if(!(overmind.framecount&127) || force_blit) {
    bool must_reblit = false;
    score.updateFromGame();
    if(score.team_order_changed || score.order_changed)
      must_reblit = true;
    if(active_sort!=-1)
      score.sort(col[active_sort]->quel_stat);
    if(score.team_order_changed || score.order_changed)
      must_reblit = true;
    if(must_reblit || force_blit)
      display();
  }
}

void Menu_stat::display() {
  zone.deleteall();
  int y = 145;
  for(int loo=0; loo<MAXTEAMS; loo++) {
    int team = score.team_order[loo];
    for(int loo2=0; loo2<MAXPLAYERS; loo2++) {
      int i=score.order[loo2];
      if(score.player_team[i]==team) {
        Canvas *c = game->net_list.get(i);
        if(c && c->color == team) {
          zone.add(new Zone_text(fteam[team], inter, c->long_name(), 2, y));

          Font *color = fcourrier[team];

          int px = c_start;
          for(int j=0; j<col.size(); j++) {
            if(col[j]->page == active_page) {
              zone.add(new Zone_text_numeric(color, inter, score.stats[i].stats[col[j]->quel_stat].get_address(), px, y, col[j]->width-c_gap));
              px += col[j]->width;
            }
          }
          y += 22;
        }
      }
    }
    if(score.player_count[team] > 1) {
      zone.add(new Zone_text(fteam[team], inter, "Total:", 15, y));
      int px = c_start;
      Font *color = fcourrier[team];
      for(int j=0; j<col.size(); j++) {
        if(col[j]->page == active_page) {
          zone.add(new Zone_text_numeric(color, inter, score.team_stats[team].stats[col[j]->quel_stat].get_address(), px, y, col[j]->width-c_gap));
          px += col[j]->width;
        }
      }
      y += 34;
    } else if(score.player_count[team] == 1) {
      y += 12; // leave space between the players/totals of each team
    }
  }
  video->need_paint = 2;
}

void Menu_stat::notify() {
  calculate_total(true);
}

void Menu_stat::init() {
  set_fteam_color(pal);
  set_sort(0);
  Menu_standard::init();
}

void Menu_stat::step() {
  Menu_standard::step();
  bool force_blit=false;
  //Add appropriate button(s)
  if(game->server && game->network && !game->terminated && !b_stop)
    b_stop = new Zone_text_button2(inter, bit, font2, "·2 End game", 8, 455);
  //Remove end-of-game button if already terminated
  if(b_stop && game->terminated) {
    delete b_stop;
    b_stop = NULL;
		// delete the rejoin button when deleting the stop button; it may
		//   be recreated right after but at least it will be in the correct place
		delete b_restart;
		b_restart = NULL;
    video->need_paint = 2;
  }
  if(net->active && !game->server && !net->connected()) {
    delete b_restart;
    b_restart=NULL;
		video->need_paint = 2;
  }
  if(!playback && !b_restart)
    if(game->server)
			if(game->terminated)
				b_restart = new Zone_text_button2(inter, bit, font2,
                                          "·2 Restart game", 8, 455);
			else
				b_restart = new Zone_text_button2(inter, bit, font2,
                                          "·2 Rejoin game",
                                          8+(b_stop? b_stop->w+4:0), 455);
    else
      if(net->active && net->connected())
        b_restart = new Zone_text_button2(inter, bit, font2, 
                                          "·2 Rejoin game", 8, 455);
  if(result) {
    if(result == b_quit)
      quit = true;
    if(result == b_stop) {
      if(!game->terminated)
        game->net_list.send_end_signal(false);
    }
    if(result == b_restart) {
      if(game->server) {
				if(game->terminated) {
					game->stop_stuff();
					game->restart();
				}
				else
					game->abort = false;
        exec(new Call_setfont(pal, new Multi_player_launcher()));
      }
      else {
        //We're not server but we want to restart, drop all players
        int i;
        for(i=0; i<MAXPLAYERS; i++) {
          if(game->net_list.get(i)) {
            Packet_dropplayer p;
            p.player=i;
            p.reason=DROP_AUTO;
            game->net_list.drop_player(&p, false);
          }
        }
        //... and delete the game
        delete game;
        exec(new Join_game(bit, inter->font, font2, pal, NULL, 0, 0, true));
      }
    }
    int i;
    for(i=0; i<col.size(); i++) {
      if(result == col[i]->z_titre) {
        set_sort(i);
        force_blit=true;
        break;
      }
    }
    for(i=0; i<b_page.size(); i++)
      if(result == b_page[i])
        change_page(i);
  }
  calculate_total(force_blit);
}

Menu_multi_checkip::Menu_multi_checkip(Bitmap *bit, Font *font, Font *font2, const Palette& p) {
  pal = p;
  bit_ = bit;
  inter->set_font(font, false);
  new Zone_bitmap(inter, bit, 0, 0);
  new Zone_text(inter, "IP configuration of this computer", 20);
  cancel = new Zone_text_button2(inter, bit, font2, "Back ·0", 560, 450);
  new Zone_text(inter, "Host name:", 170, 110);
  new Zone_text_field(inter, net->host_name, 310, 110, inter->font->width(net->host_name)+8);
  new Zone_text(inter, "IP address:", 170, 140);
  Zone_listbox *list;
  list = new Zone_listbox2(inter, bit, font2, NULL, 310, 140, 160, 200);
  for(int i=0; i<net->host_adr.size(); i++) {
    Net::stringaddress(st, net->host_adr[i]);
    list->add_item(st);
  }
}

void Menu_multi_checkip::step() {
  Menu::step();
  if(input->last_key.sym == SDLK_ESCAPE || result == cancel) {
    input->last_key.sym = SDLK_UNKNOWN;
    ret();
  }
}

Menu_multi_book::Menu_multi_book(Bitmap *bit, Font *font, Font *font2, const Palette& p, const char *adr) {
  pal = p;
  bit_ = bit;
  font2_ = font2;
  inter->set_font(font, false);
  address = adr;
  new Zone_bitmap(inter, bit, 0, 0);
  cancel = new Zone_text_button2(inter, bit, font2, "Back ·0", 560, 450);
  if(address) { // if connecting address already provided
    new Zone_text(inter, "Connect", 20);
  } else {
    new Zone_text(inter, "Address book", 20);
    for(int i=0; i<10; i++) {
      int y = 60 + i*36;
      new Zone_text_input(inter, pal, config.info.book[i], 255, 40, y, 460);
      b_connect[i] = new Zone_text_button2(inter, bit, font2, "Connect", 520, y);
    }
  }
  status = new Zone_text_field(inter, "", 6, 450, 540);
  looking = false;
  connect_failed = false;
}

void Menu_multi_book::init() {
  Menu::init();
  if(address)
    call(new Fade_in(pal));
}

void Menu_multi_book::step() {
  Menu::step();
  if(input->last_key.sym == SDLK_ESCAPE || result == cancel) {
    input->last_key.sym = SDLK_UNKNOWN;
    if(looking)
      net->gethostbyname_cancel();
    config.write();
    ret();
  }
  const char *connect_adr=NULL;

  if(address) {
    if(!looking && !connect_failed) {
      connect_adr = address;
    }
  } else {
    for(int i=0; i<10; i++) {
      if(result == b_connect[i] && config.info.book[i][0] != 0) {
        connect_adr = config.info.book[i];
        break;
      }
    }
  }
  if(connect_adr) {
    if(looking)
      net->gethostbyname_cancel();
    Dword adr = net->getaddress(connect_adr);
    if(adr > 0) {
      call(new Join_game(bit_, inter->font, font2_, pal, NULL, adr, net->port_resolve, false));
      if(address)
        ret();
    } else {
      sprintf(st, "Looking for %s...", connect_adr);
      status->set_val(st);
      looking = true;
    }
  }
  if(looking) {
    if(net->name_resolve != (unsigned int)-1) {
      if(net->name_resolve == 0) {
        status->set_val("Unable to locate host. Please verify and try again.");
        looking = false;
        connect_failed = true;
      } else {
        status->set_val("");
        looking = false;
        call(new Join_game(bit_, inter->font, font2_, pal, NULL, net->name_resolve, net->port_resolve, false));
        if(address)
          ret();
      }
      net->name_resolve = (Dword)-1;
    }
  }
}

Menu_internet::Menu_internet(const char *c): Menu() {
  command = c;
  new Zone_clear(inter);
}

void Menu_internet::init() {
  Menu::init();
}

void Menu_internet::step() {
  Menu::step();
#ifdef UGS_DIRECTX
  ShellExecute(0, "open", command, NULL, NULL, SW_SHOWDEFAULT);
#endif
  call(new Wait_time(200));
  ret();
}

Menu_startserver::Menu_startserver() {
  {
    Res_doze res("multi.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }
  set_fteam_color(pal);

  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  (void)new Zone_clear(inter);
}

void Menu_startserver::step() {
  exec(new Create_game_start(pal, bit, inter->font));
}

Menu_startserver::~Menu_startserver() {
  delete bit;
}

Menu_startconnect::Menu_startconnect(const char *adr, bool rejoin) {
  {
    Res_doze res("multi.png");
    Png img(res);
    bit = new Bitmap(img);
    pal.load(img);
  }
  set_fteam_color(pal);

  inter->set_font(new Font(*fonts.normal, pal, 255,255,255));
  font2 = new Font(*fonts.normal, pal, 255,255,0);
  (void)new Zone_clear(inter);
  if(rejoin)
    module = new Join_game(bit, inter->font, font2, pal, NULL, 0, 0, true);
  else
    module = new Menu_multi_book(bit, inter->font, font2, pal, adr);
}

void Menu_startconnect::step() {
  call(new Fade_out(pal));
  exec(module);
}

Menu_startconnect::~Menu_startconnect() {
  if(bit)
    delete bit;
  delete font2;
}
