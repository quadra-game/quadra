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

/* -*- c++ -*- */
#include <stdio.h>
#include <mcheck.h>
//#include <stdlib.h>
#include "sound.h"
//#include "main.h"

/* Ceci est un test */

void start_game() {
  Sample* thesample;
  Res_dos* foo;

//  video = Video::New(640, 480, 8, "Quadra");
//  input = Input::New();
//  music = Music::alloc();
  mcheck(NULL);
  sound = new Sound();
  if(sound->active == false)
    printf("Sound NOT active!\n");
  printf("Loading ../sons/glissup.wav\n");
  foo = new Res_dos("../sons/glissup.wav", RES_READ);
  thesample = new Sample(*foo, 2);
  int pa = 0;
  int freq = 22050;
  Sfx stmp(thesample, 0, 0, pa, freq);
  do {
    sound->process();
    //start_frame();
    //video->vb->rect(0, 0, 640, 480, 15);
    //end_frame();
  } while(1);
  
  sound->delete_sample(thesample);
  delete thesample;
  delete foo;

#if 0
  if(video)
    delete video;
#endif

  printf("Terminated, sample deleted.\nPress ESC to quit.\n");
}

