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

#include "SDL.h"

/* version Linux */
#include "config.h"
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
#include "main.h"
#include "quadra.h"

Time_mode time_control = TIME_NORMAL;
char cmd_line[1024];

void delete_obj();

char exe_directory[1024];

extern "C" int main(int ARGC, char **ARGV) {
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  atexit(delete_obj);

#ifndef WIN32
	struct sigaction signals;
	if(sigaction(SIGPIPE, NULL, &signals) < 0)
		skelton_msgbox("Can't get SIGPIPE signal handler, ignoring.\n");
	else
		if(signals.sa_handler == SIG_DFL) {
			signals.sa_handler = SIG_IGN;
			if(sigaction(SIGPIPE, &signals, NULL) < 0)
				skelton_msgbox("Can't set SIGPIPE signal handler, ignoring.\n");
		}
		else
			skelton_msgbox("SIGPIPE handler isn't default, ignoring.\n");
#endif

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

  return start_game();
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
    msgbox("deleting sound...");
    delete sound;
    msgbox(" done\n");
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
  if(stringtable) {
    msgbox("deleting stringtable...\n");
    delete stringtable;
    stringtable=NULL;
  }
  if(cursor) {
    msgbox("deleting cursor..\n");
    delete cursor;
    cursor=NULL;
  }
  SDL_Quit();
  msgbox("ending delete_obj...\n");
}
