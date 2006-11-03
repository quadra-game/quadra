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
	static void getFilename(char* st, int i, int size_of);
	static void getGlobalFilename(char* st, int i, int size_of);
	static void load();
	static void freemem();
	static int update(Canvas* c);
};

#endif
