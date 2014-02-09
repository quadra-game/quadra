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

#ifdef HAVE_CONFIG_H
#include "autoconf.h"
#endif

#include "SDL.h"

#include "video.h"
#include "cursor.h"

Input *input = NULL;

const char *keynames[256] = {
  "", "Escape", "1", "2", "3", "4", "5", "6",
  "7", "8", "9", "0", "-", "=", "Backspace", "Tab",
  "Q", "W", "E", "R", "T", "Y", "U", "I",
  "O", "P", "[", "]", "Enter", "Ctrl", "A", "S",
  "D", "F", "G", "H", "J", "K", "L", ";",
  "'", "`", "Left shift", "\\", "Z", "X", "C", "V",
  "B", "N", "M", ",", ".", "/", "Right shift", "Pad *",
  "Alt", "Space", "Caps lock", "F1", "F2", "F3", "F4", "F5",
  "F6", "F7", "F8", "F9", "F10", "Num lock", "Scrl lock", "Pad 7",
  "Pad 8", "Pad 9", "Pad -", "Pad 4", "Pad 5", "Pad 6", "Pad +",
  "Pad 1", "Pad 2", "Pad 3", "Pad 0", "Pad .", "Print scrn", "",
  "<", "F11", "F12", "", "", "", "", "", "", "","Pad Enter",
  "Right Ctrl", "Pad /", "PrintScrn", "Alt Char", "Pause",
  "Home", "Up", "Page Up", "Left", "Right", "End", "Down",
  "Page Down", "Insert", "Delete",
  "", "", "", "", "", "", "", "Pause",
  "", "", "", "", "", "Win left", "Win right", "Win popup",
  "", "Pause", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "Pad Enter", "2nd Ctrl", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "Pad /", "", "",
  "2nd Alt", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "Home",
  "Up", "Page up", "", "Left", "", "Right", "", "End",
  "Down", "Page down", "Insert", "Del", "", "", "", "",
  "", "", "", "Win left", "Win right", "Win popup", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "Macro",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", ""
};

Input::Input()
  : pause(false),
    key_pending(0) {
  mouse.quel = -1;
  for (int i = 0; i < 4; ++i)
    mouse.button[i] = RELEASED;
  clear_key();
}

void Input::clear_key() {
  clear_last_keysym();
  key_pending = 0;
  memset(keys, 0, sizeof(keys));
}

void Input::clear_last_keysym() {
  last_keysym.scancode = SDL_SCANCODE_UNKNOWN;
  last_keysym.sym = SDLK_UNKNOWN;
  last_keysym.mod = 0;
}

class Input_SDL : public Input {
public:
  virtual void check() {
    SDL_Event event;

    while (SDL_PollEvent(&event) > 0) {
      switch (event.type) {
        case SDL_QUIT:
          exit(0);

        case SDL_MOUSEMOTION:
          if (cursor)
            cursor->set_pos(event.motion.x, event.motion.y);
          break;

        case SDL_MOUSEBUTTONDOWN:
          switch (event.button.button) {
            case 1:
              if (mouse.button[event.button.button - 1] == RELEASED)
                mouse.quel = event.button.button - 1;
              mouse.button[event.button.button - 1] = PRESSED;
              break;

            default:
              SDL_Log("unknown button down: %i", event.button.button);
          }
          break;

        case SDL_MOUSEBUTTONUP:
          switch (event.button.button) {
            case 1:
              mouse.button[event.button.button - 1] = RELEASED;
              break;

            default:
              SDL_Log("unknown button up: %i", event.button.button);
          }
          break;

        case SDL_KEYDOWN:
          keys[event.key.keysym.scancode] |= PRESSED;
          last_keysym = event.key.keysym;
          break;

        case SDL_KEYUP:
          keys[event.key.keysym.scancode] = RELEASED;
          break;

        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_LEAVE:
              if (cursor)
                cursor->set_pos(-1, -1);
              break;

            default:
              SDL_Log("unknown window event: %i", event.window.event);
          }
          break;

        // Ignore these events.
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION:
        case SDL_MULTIGESTURE:
          break;

        default:
          SDL_Log("unknown event: 0x%x", event.type);
      }
    }
  }

  virtual void deraw() {
    SDL_assert_release(false);
  }

  virtual void reraw() {
    SDL_assert_release(false);
  }
};

class Input_Dumb: public Input {
public:
  virtual void check() {}
  virtual void deraw() {}
  virtual void reraw() {}
};

Input* Input::New(bool dumb) {
  if (dumb)
    return new Input_Dumb;
  else
    return new Input_SDL;
}

