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

#ifdef UGS_DIRECTX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#endif

#include "array.h"
#include "res.h"

class Sample;
class Sfx;
#ifdef UGS_LINUX
class Playing_sfx;
#endif

class Sound {
	friend class Sample;
	friend class Sfx;
#ifdef UGS_DIRECTX
	LPDIRECTSOUND lpds;
#endif
#ifdef UGS_LINUX
	int dspfd;
	unsigned int channels;
	unsigned int sampling;
	unsigned int bps;
	unsigned int fragsize;
	void *fragbuf;
	Array<Playing_sfx*> plays;
#endif
public:
	bool active;
	Sound();
#ifdef UGS_LINUX
	void process();
	void start(Playing_sfx* play);
#endif
	void delete_sample(Sample *sam);
	virtual ~Sound();
};

class Sample {
	friend class Sfx;
#ifdef UGS_DIRECTX
	BYTE *pbWaveData;
	DWORD cbWaveSize;
	int iAlloc;
	int iCurrent;
	IDirectSoundBuffer* buffers[64];

	IDirectSoundBuffer *DSLoadSoundBuffer(void *res);
	void DSFillSoundBuffer(IDirectSoundBuffer *pDSB, BYTE *pbWaveData, DWORD cbWaveSize);
	void DSParseWaveResource(void *res, WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData,DWORD *pcbWaveSize);
	//void DSReloadSoundBuffer(IDirectSoundBuffer *pDSB, LPCTSTR lpName);
	IDirectSoundBuffer *getfreebuffer();
#endif
#ifdef UGS_LINUX
	void loadriff(const char *res, unsigned int size);
	void resample(char* sample, unsigned int size, unsigned int bps);
#endif
public:
#ifdef UGS_LINUX
	void *audio_data;
	unsigned int sampling;
	unsigned int length;
#endif
	Sample(Res& re, int nb);
	virtual ~Sample();
	void stop();
};

#ifdef UGS_LINUX

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
#endif

class Sfx {
#ifdef UGS_DIRECTX
	IDirectSoundBuffer *buf;
#endif
#ifdef UGS_LINUX
	friend class Playing_sfx;
	Playing_sfx* playing;
#endif
public:
	Sfx(Sample *sam, Dword dwPlayFlags=0, int vo = -1, int pa = -1, int f = -1, int pos = -1);
	void stop();
	void pan(int pa);  //-4000=left 0=center 4000=right
	void freq(int pa); //200=low  60000=very high
	void volume(int pa);  //0=full .. -4000=nil
	void position(int pa);
#ifdef UGS_LINUX
	virtual ~Sfx();
#endif
};

extern Sound* sound;

#endif
