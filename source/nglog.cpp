/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifdef UGS_LINUX
#define stricmp strcasecmp
#endif
#ifdef UGS_DIRECTX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdio.h>
#include "game.h"
#include "overmind.h"
#include "global.h"
#include "nglog.h"

static Log *local=NULL;

static char *__nglog_rel_path="NetGamesUSA.com";
static char *__nglog_ngstats_logdir="logs";
static char *__nglog_ngstats_cfg="ngStatsQD.cfg";
void ngLog_ngStatsCall(int server_quit);

Log::Log(const char *fname) {
	strcpy(filename, fname);
	file=new Res_dos(filename, RES_CREATE);
	exist=file->exist;
	if(!exist) {
		msgbox("Log::Log: failed to create %s\n", filename);
		delete file;
		file=NULL;
	}
}

Log::~Log() {
	delete file;
}

void Log::log_event(const char *st) {
	if(exist) {
		char buf[1024];
		Dword frame=overmind.framecount-game->frame_start;
		sprintf(buf, "%u.%02u\t%s\n", frame/100, frame%100, st);
		file->write(buf, strlen(buf));
	}
}

bool log_init(const char *filename) {
	char the_name[1024];
	sprintf(the_name, "%s/%s/ngStats/%s/%s", quadradir, __nglog_rel_path, __nglog_ngstats_logdir, filename);
	local=new Log(the_name);
	return local->exist;
}

void log_step(const char *st, ...) {
	char event[1024];
	va_list marker;
	va_start(marker, st);
	vsprintf(event, st, marker);
	va_end(marker);
	if(local)
		local->log_event(event);
}

void log_finalize(char *salt) {
	if(local) {
		log_step("game_end");
		delete local;
		local=NULL;
		ngLog_ngStatsCall(false);
	}
}

char *log_team(int t) {
	char *team="none";
	switch(t) {
		case 0: team="orange"; break;
		case 1: team="cyan"; break;
		case 2: team="red"; break;
		case 3: team="purple"; break;
		case 4: team="yellow"; break;
		case 5: team="green"; break;
		case 6: team="blue"; break;
		case 7: team="gray"; break;
	}
	return team;
}

char *log_handicap(int h) {
	char *handi="unknown";
	switch(h) {
		case 0: handi="beginner"; break;
		case 1: handi="apprentice"; break;
		case 2: handi="intermediate"; break;
		case 3: handi="master"; break;
		case 4: handi="grand_master"; break;
	}
	return handi;
}

//
// void ngLog_ngStatsCall(int server_quit):
// ----------------------------------------
//
void ngLog_ngStatsCall(int server_quit) {
#ifdef UGS_LINUX
	char cmd[2048];
#endif
#ifdef UGS_DIRECTX
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	char full_cmd[2048];
#endif
	char browse[] = "false";
	char binexe[1024], logpath[1024];
	char ngs_done[1024];

			
	if(server_quit)
		strcpy(browse, "true");

#ifdef UGS_LINUX
	strcpy(ngs_done, quadradir);
	sprintf(binexe, "%s/%s/ngStats/bin/ngStatsQuadra", ngs_done, __nglog_rel_path);
	sprintf(logpath, "%s/%s/ngStats/%s", ngs_done, __nglog_rel_path, __nglog_ngstats_logdir);
	sprintf(cmd, "%s -b %s -c %s %s &", binexe, browse, __nglog_ngstats_cfg, logpath);
	// Call to ngStats
	system(cmd);
#endif
#ifdef UGS_DIRECTX
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if(server_quit)
		si.wShowWindow = SW_SHOW;
	else
		si.wShowWindow = SW_HIDE;
	si.hStdInput = NULL;
	si.hStdOutput = NULL;
	si.hStdError = NULL; 

	strcpy(ngs_done, quadradir);
	sprintf(binexe, "%s\\%s\\ngStats\\ngStatsQD.exe", ngs_done, __nglog_rel_path);
	sprintf(logpath, "%s\\%s\\ngStats\\%s", ngs_done, __nglog_rel_path, __nglog_ngstats_logdir);
	sprintf(full_cmd, "%s -b %s -c %s\\%s\\ngStats\\%s %s", binexe, browse, ngs_done, __nglog_rel_path, __nglog_ngstats_cfg, logpath);
	// Call to ngStats
	if(!server_quit)
		CreateProcess(NULL, full_cmd, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);
	else
		CreateProcess(NULL, full_cmd, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);

#endif

	return;
}
