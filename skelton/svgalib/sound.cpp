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

#include <stdio.h>

#include <mcheck.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>

#include "error.h"
#include "types.h"

#include "main.h"
#include "sound.h"

RCSID("$Id$")

#define SOUNDDEV "/dev/dsp"
#define CHUNKSIZEBITS 13
#define CHANNELNUMBER 2
#define SAMPLINGRATE 44100
#define BITSPERSAMPLE 16
#define MAXVOICES 8
#define VOLUMESHIFT 2

/* Infos sur la technique:
 *
 * Ça consiste à loader les sons en les normalisant au bit/sample et
 * frequence du device. En faisant ceci, ça nous permet de faire
 * toutes les interpolations qu'on veut d'avance.
 */

struct riff_header {
  unsigned int signature;
  unsigned int length;
  unsigned int type;
};

struct chunk_header {
  unsigned int type;
  unsigned int size;
};

struct fmt_chunk {
  unsigned short format;        /* format type */
  unsigned short channels;      /* number of channels
				   (i.e. mono, stereo...) */
  unsigned int sampling;        /* sample rate */
  unsigned int bytespersec;     /* for buffer estimation */
  unsigned short blockalign;    /* block size of data */
  unsigned short bitspersample; /* Number of bits per sample of mono data */
};

Sound *sound = NULL;

Sound::Sound(): dspfd(-1), fragbuf(NULL), active(false) {
  int i;

  dspfd = open(SOUNDDEV, O_WRONLY);
  if(dspfd == -1) {
    perror(SOUNDDEV);
    return;
  }

  i = 0x00020000 | CHUNKSIZEBITS;
  if(ioctl(dspfd, SNDCTL_DSP_SETFRAGMENT, &i) == -1) {
    perror(SOUNDDEV);
    return;
  }

  if(i != (0x00020000 | CHUNKSIZEBITS)) {
    skelton_msgbox("sound: warning, could not set fragment size\n");
  }

  if(ioctl(dspfd, SNDCTL_DSP_GETBLKSIZE, &fragsize) == -1) {
    perror(SOUNDDEV);
    return;
  }

  fragbuf = malloc(fragsize);

  bps = BITSPERSAMPLE;
  if(ioctl(dspfd, SNDCTL_DSP_SAMPLESIZE, &bps) == -1) {
    perror(SOUNDDEV);
    return;
  }

  if(bps != BITSPERSAMPLE) {
    skelton_msgbox("sound: warning, could not set sample size to %i bits\n", BITSPERSAMPLE);
  }

  channels = CHANNELNUMBER;
  if(ioctl(dspfd, SNDCTL_DSP_CHANNELS, &channels) == -1) {
    perror(SOUNDDEV);
    return;
  }

  if(channels != CHANNELNUMBER) {
    skelton_msgbox("sound: warning, could not set number of channels\n");
  }

  if(channels != 1 && channels != 2) {
    skelton_msgbox("  Sound card doesn't support neither mono or stereo output. Sound disabled.\n");
    return;
  }

  sampling = SAMPLINGRATE;
  if(ioctl(dspfd, SNDCTL_DSP_SPEED, &sampling) == -1) {
    perror(SOUNDDEV);
    return;
  }

  if(sampling != SAMPLINGRATE) {
    skelton_msgbox("sound: warning, could not set sampling rate\n");
  }

  active = true;
  skelton_msgbox("Sound::Sound: opened succesfully.\n");
}

void Sound::process() {
  unsigned int i;
  audio_buf_info info;

  if(ioctl(dspfd, SNDCTL_DSP_GETOSPACE, &info) == -1)
    return;

  if(info.fragments == 0)
    return;

  memset(fragbuf, 0, fragsize);
  unsigned int frag_temp = fragsize;

  if(bps == 16)
    frag_temp = frag_temp >> 1;

  if(channels == 2)
    frag_temp = frag_temp >> 1;

  for(i = 0; i < (unsigned int)plays.size(); i++) {
    Playing_sfx* p = plays[i];

    if(bps == 8) { // si output 8-bit
      Byte* output = (Byte*)fragbuf;
      Byte* input = (Byte*)p->sam->audio_data;

      for(unsigned int j=0; j<frag_temp; j++) {
	Byte tmpl = *(input + p->pos);
	Byte tmpr;

	tmpl = ((int) tmpl * p->vo) >> 8;

	if(channels == 2) { // stereo output
	  tmpr = tmpl;
	  if(p->pa > 0)
	    tmpl = ((int) tmpl * (p->pa)) >> 8;
	  else if(p->pa < 0)
	    tmpr = ((int) tmpr * (-p->pa)) >> 8;

	  *output++ += tmpr;
	}

	*output++ += tmpl;

	if(p->delta_position + p->delta_inc < p->delta_position)
	  p->pos++; /* si overflow du delta */

	p->delta_position += p->delta_inc;
	p->pos += p->inc;
	if((unsigned int)p->pos >= p->sam->length)
	  break;
      }
    } else { // si output 16-bit
      signed short* output = (signed short*)fragbuf;
      signed short* input = (signed short*)p->sam->audio_data;

      for(unsigned int j=0; j<frag_temp; j++) {
	signed short tmpl = *(input + p->pos);
	tmpl = (tmpl * p->vo) >> 8;
	if(channels == 2) { // stereo output
	  signed short tmpr = tmpl;
	  if(p->pa > 0)
	    tmpl = (tmpl * (p->pa)) >> 8;
	  else if(p->pa < 0)
	    tmpr = (tmpr * (-p->pa)) >> 8;

	  *output++ += tmpr;
	}
	*output++ += tmpl;
	if(p->delta_position + p->delta_inc < p->delta_position) {
	  p->pos++; /* si overflow du delta */
	}

	p->delta_position += p->delta_inc;
	p->pos += p->inc;
	if((unsigned int)p->pos >= p->sam->length)
	  break;
      }
    }

    if((unsigned int)p->pos >= p->sam->length) {
      plays.remove(i);
      i--;
      delete p;
    }
  }

  write(dspfd, fragbuf, fragsize);
}

void Sound::delete_sample(Sample *sam) {
  // il faut detruire tout les sons qui utilisent un Sample avant de
  // detruire ce dernier!
  for(int i=0; i<plays.size(); i++) {
    Playing_sfx *p = plays[i];
    if(p->sam == sam) {
      plays.remove(i);
      i--;
      delete p;
    }
  }
}

void Sound::start(Playing_sfx* play) {
  plays.add(play);
}

Sound::~Sound() {
  if(dspfd != -1)
    close(dspfd);

  if(fragbuf)
    free(fragbuf);
}

Sample::Sample(Res& re, int nb): audio_data(NULL), sampling(0), length(0) {
  char *buffer;
  Dword size;

  if(!sound)
    return;

  buffer = (char *)re.buf();
  size = re.size();

  loadriff(buffer, size);
}

void Sample::loadriff(const char *res, unsigned int len) {
  bool seenfmt = false;

  char *sample=NULL;
  unsigned int size=0, bps=0;

  if(((struct riff_header *)res)->signature != 0x46464952) /* 'RIFF' */
    (void)new Error("Bad RIFF signature");

  if(((struct riff_header *)res)->type != 0x45564157) /* 'WAVE' */
    (void)new Error("RIFF is not a WAVE");

  char *ptr = (char *)res + sizeof(struct riff_header);
  char *endptr = (char *)res + len - 3;

  while((ptr < endptr) && (ptr >= res)) {
    char *data = ptr + sizeof(struct chunk_header);
    struct chunk_header *header_ptr = (struct chunk_header *)ptr;

    switch(header_ptr->type) {
    case 0x20746d66: /* 'fmt ' */
      seenfmt = true;

      if(((struct fmt_chunk *)data)->channels != 1)
	(void)new Error("RIFF/WAVE: unsupported number of channels");

      sampling = ((struct fmt_chunk *)data)->sampling;
      bps = ((struct fmt_chunk *)data)->bitspersample;
      size = 0;
      break;
    case 0x61746164: /* 'data' */
      if(!seenfmt)
	(void)new Error("RIFF/WAVE: 'data' subchunk seen before 'fmt ' subchunk");

      unsigned int chunksize;
      chunksize = header_ptr->size;
      sample = (char*)realloc(sample, size+chunksize);
      memcpy(sample+size, data, chunksize);
      size += chunksize;

      break;
    default:
      /* ignore unknown chunks/subchunks */
      break;
    }
    ptr += sizeof(struct chunk_header) + header_ptr->size;
  }

  if(!sample)
    (void)new Error("Error loading sample");

  resample(sample, size, bps);

  free(sample);
}

void Sample::resample(char* sample, unsigned int size, unsigned int bps) {
  unsigned int i;

  length = (size * (sound->sampling >> 7)) / (sampling >> 7);
  length = (length * sound->bps) / bps;

  audio_data = malloc(length); // length est en 'byte' ici

  if(!audio_data)
    (void)new Error("Couldn't allocate sample");

  if(bps == 8) {
    if(sound->bps == 16)
      length = length >> 1; // transforme length en 'short'

    unsigned int pos, inc, delta, delta_pos, old_pos;
    pos = delta_pos = 0;
    old_pos = 1;
    inc = size / length;
    delta = (unsigned int) (4294967295U / length) * (size % length);

    for(i = 0; i < length; i++) {
      int tube;
      if(pos == old_pos && ((bps == 8 && pos < size-1) || (bps == 16 && pos < size - 1))) {
	if(sound->bps == 8) {
	  tube = (Byte)sample[pos+1] >> VOLUMESHIFT;
	  // interpolation cheap
	  tube = (tube+((Byte *)audio_data)[i-1]) >> 1;
	} else {
	  tube = (128 - (Byte)sample[pos+1]) << (8-VOLUMESHIFT);
	  // interpolation cheap
	  tube = (tube+((signed short*)audio_data)[i-1]) >> 1;
	}
      } else {
	if(sound->bps == 8)
	  tube = (Byte)sample[pos] >> VOLUMESHIFT;
	else
	  tube = (128 - (Byte)sample[pos]) << (8-VOLUMESHIFT);
	old_pos = pos;
      }
      if(sound->bps == 8)
	((Byte *)audio_data)[i] = tube;
      else
	((signed short*)audio_data)[i] = tube;

      pos += inc;
      if(delta_pos + delta < delta_pos) // si overflow du delta
	pos++;
      delta_pos += delta;
    }
  } else {
    (void)new Error("Sound: wave 16-bit non supporte presentement");
  }
}

Sample::~Sample() {
  if(audio_data)
    free(audio_data);
}

void Sample::stop() {
}

Playing_sfx::Playing_sfx(Sfx* thesfx, Sample *thesam, Dword theflags):
  sfx(thesfx), sam(thesam), flags(theflags), vo(0), f(0), pos(0),
  pa(0), delta_inc(0), delta_position(0), inc(0) {
}

Playing_sfx::~Playing_sfx() {
  if(sfx)
    sfx->playing = NULL;
}

Sfx::Sfx(Sample *sam, Dword dwPlayFlags, int vo, int pa, int f, int pos):
  playing(NULL) {
  if(!sound || !sam || !sound->active)
    return;

  if(sound->plays.size() == MAXVOICES)
    return;

  playing = new Playing_sfx(this, sam, dwPlayFlags);

  volume(vo);
  pan(pa);
  freq(f);
  position(pos);

  sound->start(playing);
}

void Sfx::stop() {
  /* TODO: fu fu fu */
}

void Sfx::pan(int pa) {
  if(!playing) // le son est deja fini!!
    return;
  if(pa < -4096)
    pa = -4096;
  if(pa > 4096)
    pa = 4096;
  pa = pa >> 4;
  if(pa > 0)
    playing->pa = 256-pa;
  else if(pa < 0)
    playing->pa = -pa-256;
  else if(pa == 0)
    playing->pa = 0;
}

void Sfx::freq(int pa) {
  if(!playing) // le son est deja fini!!
    return;
  pa = pa * sound->sampling / playing->sam->sampling;
  // il faut ajuster la frequence demande selon la freq original du sample!
  playing->f = pa;
  playing->inc = pa / sound->sampling; // calcul l'increment 'entier'
  // puis calcul l'increment 'delta' qui overflowera a 2^32
  playing->delta_inc =  (unsigned int) (4294967295U / sound->sampling) * (pa % sound->sampling);
}

void Sfx::volume(int pa) {
  if(!playing) // le son est deja fini!!
    return;
  if(pa < -4096)
    pa = -4096;
  playing->vo = (pa+4096) >> 4;
}

void Sfx::position(int pa) {
  if(!playing) // le son est deja fini!!
    return;
  if(pa == -1)
    pa = 0;

  pa = pa * sound->sampling / playing->sam->sampling;
  // il faut ajuster la position demande selon la freq original du sample!
  playing->pos = pa;
  playing->delta_position = 0;
}

Sfx::~Sfx() {
  if(playing)
    playing->sfx = NULL;
}
