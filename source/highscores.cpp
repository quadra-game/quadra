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

#include <stdlib.h>
#include "res_compress.h"
#include "quadra.h"
#include "config.h"
#include "recording.h"
#include "canvas.h"
#include "global.h"
#include "highscores.h"

int Highscores::numLocal=0;
int Highscores::numGlobal=0;
Highscores::Best Highscores::bestlocal[MAX_SCORE];
Highscores::Best Highscores::bestglobal[MAX_SCORE];
bool Highscores::loaded = false;

void Highscores::getFilename(char* st, int i) {
	sprintf(st, "%s/local%i.rec", quadradir, i);
}

void Highscores::getGlobalFilename(char* st, int i) {
	sprintf(st, "%s/global%i.rec", quadradir, i);
}

void Highscores::load() {
	msgbox("Highscores::load\n");
	if(loaded)
		return;
	loaded=true;
	char st[1024], st2[1024];
	int i;
	numLocal=0;
	for(i=0; i<MAX_SCORE; i++) {
		getFilename(st, i);
		Res_compress* res=new Res_compress(st, RES_TRY);
		bestlocal[i].demo = NULL;
		if(res->exist) {
			Playback* tmp=new Playback(res);
			if(!tmp->valid || !tmp->single()) {
				delete tmp;
				delete res;
				continue;
			}
			strcpy(bestlocal[numLocal].name, tmp->player[0].name);
			bestlocal[numLocal].score = tmp->score;
			bestlocal[numLocal].level = tmp->level;
			bestlocal[numLocal].lines = tmp->lines;
			bestlocal[numLocal].demo = tmp;
			delete res; //  must delete to allow rename (Win95)
			if(numLocal!=i) {
				getFilename(st2, numLocal);
				if(rename(st, st2))
					msgbox("Warning: Highscore: could not rename '%s' to '%s'\n", st, st2);
			}
			numLocal++;
		}
		else
			delete res;
	}
	numGlobal=0;
	for(i=0; i<MAX_SCORE; i++) {
		getGlobalFilename(st, i);
		Res_compress* res=new Res_compress(st, RES_TRY);
		bestglobal[i].demo = NULL;
		if(res->exist) {
			Playback* tmp=new Playback(res);
			if(!tmp->valid || !tmp->single()) {
				delete tmp;
				delete res;
				continue;
			}
			strcpy(bestglobal[numGlobal].name, tmp->player[0].name);
			bestglobal[numGlobal].score = tmp->score;
			bestglobal[numGlobal].level = tmp->level;
			bestglobal[numGlobal].lines = tmp->lines;
			bestglobal[numGlobal].demo = tmp;
			delete res; //  must delete to allow rename (Win95)
			if(numGlobal!=i) {
				getGlobalFilename(st2, numGlobal);
				if(rename(st, st2))
					msgbox("Warning: Highscore: could not rename '%s' to '%s'\n", st, st2);
			}
			numGlobal++;
		}
		else
			delete res;
	}
}

void Highscores::free() {
	int i;

	if(!loaded)
		return;

	for(i = 0; i < MAX_SCORE; i++) {
		if(bestlocal[i].demo)
			delete bestlocal[i].demo;
		bestlocal[i].demo = NULL;
		if(bestglobal[i].demo)
			delete bestglobal[i].demo;
		bestglobal[i].demo = NULL;
	}
	
	loaded = false;
}

int Highscores::update(Canvas *c) {
	msgbox("Highscores::update\n");
	char st[1024], st2[1024];
	int i;
	load();
	int ret = -1;
	for(i=0; i<numLocal; i++)
		if(c->stats[CS::SCORE].get_value() >= bestlocal[i].score) {
			ret=i;
			break;
		}
	if(numLocal<MAX_SCORE) {
		if(ret==-1)
			ret=numLocal;
		numLocal++;
	}
	if(ret!=-1) {
		int j;
		for(j=MAX_SCORE-1; j>ret; j--) {
			strcpy(bestlocal[j].name, bestlocal[j-1].name);
			bestlocal[j].score = bestlocal[j-1].score;
			bestlocal[j].lines = bestlocal[j-1].lines;
			bestlocal[j].level = bestlocal[j-1].level;
			if(bestlocal[j].demo)
				delete bestlocal[j].demo;
			bestlocal[j].demo = bestlocal[j-1].demo;
			bestlocal[j-1].demo = NULL;
			getFilename(st, j);
			if(remove(st) != 0)
				msgbox("Warning: Highscore: could not delete '%s'\n", st);
			getFilename(st2, j-1);
			if(rename(st2, st) != 0)
				msgbox("Warning: Highscore: could not rename '%s' to '%s'\n", st2, st);
		}
		sprintf(st2, "%s/last.rec", quadradir);
		Playback* demo=NULL;
		{
			Res_compress res(st2, RES_TRY);
			demo=new Playback(&res);
		}
		getFilename(st, ret);
		if(remove(st) == 0)
			msgbox("Warning: Highscore: '%s' is in the way! Deleting it.\n", st);
		if(rename(st2, st) != 0)
			msgbox("Warning: Highscore: could not rename 'last.rec' to '%s'\n", st);
		msgbox("Setting hscore %i: %s, %i, %i, %i\n", ret, c->name, c->stats[CS::SCORE].get_value(), c->lines, c->level);
		strcpy(bestlocal[ret].name, c->name);
		bestlocal[ret].score = c->stats[CS::SCORE].get_value();
		bestlocal[ret].lines = c->lines;
		bestlocal[ret].level = c->level;
		if(bestlocal[ret].demo)
			delete bestlocal[ret].demo;
		bestlocal[ret].demo = demo;
	}
	return ret;
}
