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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "types.h"
#include "net.h"
#include "video.h"
#include "palette.h"
#include "input_dx.h"
#include "sound.h"
#include "cursor.h"
#include "music.h"
#include "stringtable.h"
#include "overmind.h"
#include "command.h"
#include "main.h"

RCSID("$Id$")

bool alt_tab=false;
Time_mode time_control=TIME_NORMAL;
void quit_game();
HINSTANCE hinst;
HWND hwnd;

void start_frame() {
	bool quit=false;
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
COPPER(0,0,30);
		if(msg.message == WM_QUIT)
			alt_tab=quit=true;
		TranslateMessage(&msg);
		if(msg.message == WM_TIMER) {
			COPPER(0,30,0);
			//msgbox("WM_TIMER on frame %i\n",overmind.framecount);
		}
		DispatchMessage(&msg);
COPPER(0,0,30);
	}
COPPER(0,0,0);
	if(quit)
		quit_game();
	Sleep(1);
	if(!alt_tab) {
		if(input)
			input->check();
		if(video)
			video->start_frame();
	}
}

void end_frame() {
	if(!alt_tab) {
		if(video)
			video->end_frame();
	}
}

char exe_directory[_MAX_DRIVE+_MAX_DIR+1];

void set_path() {
	char drive_buf[_MAX_DRIVE];
  char dir_buf[_MAX_DIR];
	//+3 in case they "forgot" to count the stupid double quotes
	char temp_path[_MAX_PATH+3];
	strcpy(temp_path, GetCommandLine()+1);
	char *find_fin = strchr(temp_path, '"');
	if(find_fin)
		*find_fin = 0;
	_splitpath(temp_path, drive_buf, dir_buf, NULL, NULL);
	exe_directory[0] = 0;
	strcat(exe_directory, drive_buf);
	strcat(exe_directory, dir_buf);
	//Remove useless ending \ or /
	char *c=&exe_directory[strlen(exe_directory)-1];
	if(*c=='\\' || *c=='/')
		*c=0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	hinst = hInstance;
	command.add(lpCmdLine);
	set_path();
	start_game();
	quit_game();
	return 0;
}

void delete_obj() {
	skelton_msgbox("overmind clean_up\n");
	overmind.clean_up();
	if(net) {
		skelton_msgbox("deleting net...\n");
		delete net;
		net=NULL;
	}
	if(sound) {
		skelton_msgbox("deleting sound...\n");
		delete sound;
		sound=NULL;
	}
	if(input) {
		skelton_msgbox("deleting input...\n");
		delete input;
		input=NULL;
	}
	if(video) {
		skelton_msgbox("deleting video...\n");
		delete video;
		video=NULL;
	}
	if(music) {
		skelton_msgbox("stopping and deleting music...\n");
		music->stop();
		music->close();
		delete music;
		music=NULL;
	}
	if(stringtable) {
		skelton_msgbox("deleting stringtable...\n");
		delete stringtable;
		stringtable=NULL;
	}
	if(cursor) {
		skelton_msgbox("deleting cursor..\n");
		delete cursor;
		cursor=NULL;
	}
	skelton_msgbox("ending delete_obj...\n");
}

void quit_game() {
	if(video)
		video->clean_up();
	delete_obj();
	exit(0);
}

LRESULT CALLBACK windowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
COPPER(30,0,0);
//skelton_msgbox("Frame overmind=%i, receive msg=%i\n",overmind.framecount, msg);
	switch(msg) {
		case WM_ACTIVATEAPP:
			if(wparam && alt_tab) {
				alt_tab = false;
				if(input)
					((Input_DX *) input)->restore();
				if(video)
					video->restore();
				ShowCursor(FALSE);
			}
			if(!wparam && !alt_tab) {
				alt_tab = true;
				if(input)
					input->clear_key(); // pour eviter qu'une touche reste coller
				ShowCursor(TRUE);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SYSKEYDOWN:
			if(!alt_tab)
				return 0;
			break;
		case WM_SYSCOMMAND:
			if(!alt_tab)
				return 0;
			break;
		case WM_CHAR:
			if(input)
				((Input_DX *) input)->add_key_buf((char) wparam);
			return 0;
		case WM_KEYDOWN:
			if(input) {
				if(wparam == 19) // touche 'pause'
					input->pause = true;
				if(wparam >= 16 && wparam <= 46)
					((Input_DX *) input)->add_key_buf((char) wparam, true);
			}
			return 0;
/*		case MM_MCINOTIFY:
			if(wparam == MCI_NOTIFY_SUCCESSFUL && music)
				music->replay();
			return 0;*/
		case WM_USER:
			if(net) {
				int err = WSAGETASYNCERROR(lparam);
				net->gethostbyname_completed(err == 0);
			}
			return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

Dword getmsec() {
	return timeGetTime();
}
