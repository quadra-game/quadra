/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_HIGHSCORES
#define _HEADER_HIGHSCORES

#include "stats.h"

class Canvas;
class Playback;

#define MAX_SCORE 5

class Highscores {
	static bool loaded;
	Highscores(); //statics only, don't instantiate
public:
	struct Best {
		char name[40];
		int score, lines, level;
		Playback* demo;
	};
	//You must call 'load' before using these. I know it sucks
	static int numLocal;
	static Best bestlocal[MAX_SCORE];
	static int numGlobal;
	static Best bestglobal[MAX_SCORE];
	static void getFilename(char* st, int i);
	static void getGlobalFilename(char* st, int i);
	static void load();
	static void free();
	static int update(Canvas* c);
};

#endif
