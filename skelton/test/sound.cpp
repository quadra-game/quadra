/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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

