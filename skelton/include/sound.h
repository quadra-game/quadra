/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
	Sfx(Sample *sam, Dword dwPlayFlags=0, int vo=-1, int pa=-1, int f=-1, int pos=-1);
	void stop();
	void pan(int pa);  //-4000=gauche 0=centre 4000=droite
	void freq(int pa); //200=bas  60000=tres haute
	void volume(int pa);  //0=full .. -4000=absent
	void position(int pa);
#ifdef UGS_LINUX
	virtual ~Sfx();
#endif
};

extern Sound* sound;

#endif
