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

#include "main.h"

#include "SDL.h"

#include "config.h"
#ifdef SOCKS
#include <stdio.h>
#include <socks.h>
extern "C" int SOCKSinit(char *);
#endif
#include <stdlib.h>
#include <signal.h>

#include "types.h"
#include "net.h"
#include "video.h"
#include "palette.h"
#include "input.h"
#include "sound.h"
#include "cursor.h"
#include "stringtable.h"
#include "overmind.h"
#include "resfile.h"
#include "command.h"

int ux_argc;
char** ux_argv;
bool alt_tab = false;
Time_mode time_control = TIME_NORMAL;
char cmd_line[1024];

void start_frame() {
  input->check();
  video->start_frame();
}

void end_frame() {
  video->end_frame();
}

char exe_directory[1024];

static bool ignore_sigpipe=false;

int main(int ARGC, char **ARGV) {
#ifdef HAVE_MCHECK_H
#ifndef NDEBUG
  mcheck(NULL);
#endif
#endif
#ifdef SOCKS
  SOCKSinit(ARGV[0]);
#endif

  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  atexit(delete_obj);
	struct sigaction signals;
	if(sigaction(SIGPIPE, NULL, &signals) < 0)
		skelton_msgbox("Can't get SIGPIPE signal handler, ignoring.\n");
	else
		if(signals.sa_handler == SIG_DFL) {
			signals.sa_handler = SIG_IGN;
			if(sigaction(SIGPIPE, &signals, NULL) < 0)
				skelton_msgbox("Can't set SIGPIPE signal handler, ignoring.\n");
			else
				ignore_sigpipe=true;
		}
		else
			skelton_msgbox("SIGPIPE handler isn't default, ignoring.\n");

  ux_argc = ARGC;
  ux_argv = ARGV;

	//Copy the whole thing
	strncpy(exe_directory, ARGV[0], sizeof(exe_directory));
	exe_directory[sizeof(exe_directory)-1]=0;
	//Remove file name and final last /
	char *temp=strrchr(exe_directory, '/');
	if(temp)
		*temp=0;

  for(int i=1; i<ARGC; i++) {
    command.add(ARGV[i]);
  }

  start_game();
  return 0;
}

void delete_obj() {
  msgbox("overmind clean_up\n");
  overmind.clean_up();
  if(net) {
    msgbox("deleting net...\n");
    delete net;
    net=NULL;
  }
  if(sound) {
    msgbox("deleting sound...\n");
    delete sound;
    sound=NULL;
  }
  if(input) {
    msgbox("deleting input...\n");
    delete input;
    input=NULL;
  }
  if(video) {
    msgbox("deleting video...\n");
    delete video;
    video=NULL;
  }
  if(cursor) {
    msgbox("deleting cursor..\n");
    delete cursor;
    cursor=NULL;
  }
	if(ignore_sigpipe) {
		msgbox("restoring default SIGPIPE handler...\n");
		struct sigaction sigs;
		if(sigaction(SIGPIPE, NULL, &sigs) < 0)
			msgbox("Can't get signal, whatever...\n");
		else {
			sigs.sa_handler = SIG_DFL;
			if(sigaction(SIGPIPE, &sigs, NULL) < 0)
				msgbox("Can't set signal, whatever...\n");
			ignore_sigpipe=false;
		}
	}

  SDL_Quit();

  msgbox("ending delete_obj...\n");
}

Dword getmsec() {
  return SDL_GetTicks();
}
