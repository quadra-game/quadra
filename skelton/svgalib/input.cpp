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

#include "video.h"
#include "input_dumb.h"
#ifdef UGS_LINUX_X11
#include "input_x11.h"
#endif

RCSID("$Id$")

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

Input* Input::New(bool dumb) {
  if(dumb)
    return new Input_Dumb();
#ifdef UGS_LINUX_X11
  if(video->xwindow)
    return new Input_X11;
  else
#endif
  return NULL;
}

