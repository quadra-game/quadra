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

#ifndef _HEADER_SOUND
#define _HEADER_SOUND

#include "autoconf.h"
#if defined(HAVE_SDL_H)
#include "SDL.h"
#elif defined(HAVE_SDL_SDL_H)
#include "SDL/SDL.h"
#endif
#include "array.h"
#include "res.h"

class Sample;
class Sfx;
class Playing_sfx;

class Sound {
	friend class Sample;
	friend class Sfx;
  SDL_AudioSpec spec;
	Array<Playing_sfx*> plays;
	static void process(void *userdata, Uint8 *stream, int len);
public:
	bool active;
	Sound();
	void start(Playing_sfx* play);
	void delete_sample(Sample *sam);
	virtual ~Sound();
};

class Sample {
	friend class Sfx;
	void loadriff(const char *res, unsigned int size);
	void resample(char* sample, unsigned int size, unsigned int bps);
public:
	void *audio_data;
	unsigned int sampling;
	unsigned int length;
	Sample(Res& re, int nb);
	virtual ~Sample();
	int refcount;
};

class Playing_sfx {
public:
	Sfx* sfx;
	Sample *sam;
	Dword flags;
	unsigned int vo, f, pos;
	int pa;
	unsigned int delta_inc, delta_position, inc;
	Playing_sfx(Sfx* thesfx, Sample *thesam, Dword theflags=0);
	virtual ~Playing_sfx();
};

class Sfx {
	friend class Playing_sfx;
	Playing_sfx* playing;
public:
	Sfx(Sample *sam, Dword dwPlayFlags=0, int vo = -1, int pa = -1, int f = -1, int pos = -1);
	void pan(int pa);  //-4000=left 0=center 4000=right
	void freq(int pa); //200=low  60000=very high
	void volume(int pa);  //0=full .. -4000=nil
	void position(int pa);
	virtual ~Sfx();
};

extern Sound* sound;

#endif
