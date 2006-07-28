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

#include "input.h"

#include <assert.h>
#include <stdlib.h>
#include "autoconf.h"
#if defined(HAVE_SDL_H)
#include "SDL.h"
#elif defined(HAVE_SDL_SDL_H)
#include "SDL/SDL.h"
#endif
#include "video.h"
#include "input_dumb.h"
#include "input_x11.h"
#include "cursor.h"

Input *input = NULL;

class Input_SDL: public Input {
public:
  Input_SDL();
  virtual ~Input_SDL();
  virtual void clear_key();
  virtual void check();
  virtual void deraw();
  virtual void reraw();
};

Input* Input::New(bool dumb) {
  if(dumb)
    return new Input_Dumb();

  return new Input_SDL;

#ifndef X_DISPLAY_MISSING
  if(video->xwindow)
    return new Input_X11;
  else
#endif
    return NULL;
}

Input_SDL::Input_SDL() {
  int i;

  pause = false;

  mouse.quel = -1;
  for(i = 0; i < 4; i++)
    mouse.button[i] = RELEASED;

  clear_key();
}

Input_SDL::~Input_SDL() {
}

void Input_SDL::clear_key() {
  int i;

  shift_key = 0;
  quel_key = -1;
  key_pending = 0;
  
  for(i = 0; i < 256; i++)
    keys[i] = 0;
}

void Input_SDL::check() {
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_QUIT:
      fprintf(stderr, "event: quit\n");
      exit(0);
      break;
    case SDL_MOUSEMOTION:
      if(cursor)
        cursor->set_pos(event.motion.x, event.motion.y);
      break;
    case SDL_MOUSEBUTTONDOWN:
      fprintf(stderr, "event: mousebuttondown\n");
      switch(event.button.button) {
      case SDL_BUTTON_LEFT:
        if(mouse.button[0] == RELEASED)
          mouse.quel = 0;
        mouse.button[0] = PRESSED;
        break;
      case SDL_BUTTON_MIDDLE:
        if(mouse.button[1] == RELEASED)
          mouse.quel = 1;
        mouse.button[1] = PRESSED;
        break;
      case SDL_BUTTON_RIGHT:
        if(mouse.button[2] == RELEASED)
          mouse.quel = 2;
        mouse.button[2] = PRESSED;
        break;
      }
      break;
    case SDL_MOUSEBUTTONUP:
      fprintf(stderr, "event: mousebuttonup\n");
      switch(event.button.button) {
      case SDL_BUTTON_LEFT:
        mouse.button[0] = RELEASED;
        break;
      case SDL_BUTTON_MIDDLE:
        mouse.button[1] = RELEASED;
        break;
      case SDL_BUTTON_RIGHT:
        mouse.button[2] = RELEASED;
        break;
      }
      break;
    case SDL_KEYDOWN:
      fprintf(stderr, "event: keydown\n");
      keys[event.key.keysym.scancode] |= PRESSED;

      switch(event.key.keysym.mod) {
      case SDLK_RSHIFT:
      case SDLK_LSHIFT:
        fprintf(stderr, "shift pressed\n");
        shift_key |= SHIFT;
        break;
      case SDLK_RALT:
      case SDLK_LALT:
        fprintf(stderr, "alt pressed\n");
        shift_key |= ALT;
        break;
      case SDLK_RCTRL:
      case SDLK_LCTRL:
        fprintf(stderr, "control pressed\n");
        shift_key |= CONTROL;
        break;
      case SDLK_PAUSE:
        pause = true;
        break;
      default:
        quel_key = event.key.keysym.scancode;
      }

      break;
    case SDL_KEYUP:
      fprintf(stderr, "event: keyup\n");
      keys[event.key.keysym.scancode] = RELEASED;
      break;
    default:
      fprintf(stderr, "event: unknown\n");
      break;
    }
  }
}

void Input_SDL::deraw() {
  assert(false);
}

void Input_SDL::reraw() {
  assert(false);
}

