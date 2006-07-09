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

#include "autoconf.h"
#if defined(HAVE_SDL_H)
#include "SDL.h"
#elif defined(HAVE_SDL_SDL_H)
#include "SDL/SDL.h"
#endif
#ifndef X_DISPLAY_MISSING
#include <X11/keysym.h>
#include "cursor.h"
#include "main.h"
#include "input_x11.h"
#include "video_x11.h"
#include <X11/Xutil.h>

static KeyCode xlate[256];

Input_X11::Input_X11() {
  int i;
  Video_X11* videox11;
 
  videox11 = dynamic_cast<Video_X11*>(video);

  mouse.dx = mouse.dy = mouse.dz = 0;

  mouse.quel = -1;
  for(i = 0; i < 4; i++)
    mouse.button[i] = RELEASED;

  pause = false;

  clear_key();

  /* Here, I'm just about to go mad. */
  for(i = 0; i < 256; i++)
    xlate[i] = 0;

  for(i = 9; i < 92; i++)
    xlate[i] = i - 8;

  //xlate[] = SCANCODE_LESS;			// 86
  //xlate[] = SCANCODE_BREAK;		// 101
  
  for(i = 94; i < 97; i++)
    xlate[i] = i - 8;

  for(i = 97; i < 101; i++)
    xlate[i] = i + 5;

  for(i = 102; i < 108; i++)
    xlate[i] = i + 4;

  xlate[108] = 96; /* SCANCODE_KEYPADENTER */
  xlate[109] = 97; /* SCANCODE_RIGHTCONTROL */
  xlate[110] = 119; /* SCANCODE_BREAK_ALTERNATIVE */
  xlate[111] = 99; /* SCANCODE_PRINTSCREEN */
  xlate[112] = 98; /* SCANCODE_KEYPADDIVIDE */
  xlate[113] = 100; /* SCANCODE_RIGHTALT */

  xlate[115] = 219; /* Win left */
  xlate[116] = 220; /* Win right */
  xlate[117] = 221; /* Win popup */

  im = XOpenIM(videox11->display, NULL, NULL, NULL);
  if(im) {
    ic = XCreateIC(im,
		   XNInputStyle, XIMPreeditNothing|XIMStatusNothing,
		   XNClientWindow, videox11->window,
		   XNFocusWindow, videox11->window,
		   NULL);
    if(!ic) {
      msgbox("Warning: unsupported input method\n");
      XCloseIM(im);
      im = NULL;
    }
  } else {
    msgbox("Warning: error opening X input method\n");
    ic = NULL;
  }

  reraw();
}

Input_X11::~Input_X11() {
  if(ic)
    XDestroyIC(ic);

  if(im)
    XCloseIM(im);
}

void Input_X11::clear_key() {
  int i;

  shift_key = 0;
  quel_key = -1;
  key_pending = 0;
  
  for(i = 0; i < 256; i++)
    keys[i] = 0;
}

void Input_X11::check() {
  Video_X11* videox11;
  int pending;

  if(!video)
    return;

  videox11 = dynamic_cast<Video_X11*>(video);

  for(pending = XPending(videox11->display);
      pending > 0;
      pending--) {
    XEvent event;

    XNextEvent(videox11->display, &event);

    if(!XFilterEvent(&event, None))
      switch(event.type) {
      case KeyPress:
      case KeyRelease:
        process_key(event);
        break;

      case ButtonPress:
      case ButtonRelease:
      case LeaveNotify:
      case MotionNotify:
        process_mouse(event);
        break;

      case FocusIn:
        videox11->focus_in(event.xfocus.window);
        alt_tab = false;
        break;

      case FocusOut:
        videox11->focus_out(event.xfocus.window);
        alt_tab = true;
        break;

      case ClientMessage:
        if((Atom)event.xclient.data.l[0] == videox11->delete_win)
          exit(0);
        break;

      case Expose:
        if(videox11)
          videox11->dirty2(event.xexpose.x,
                           event.xexpose.y,
                           event.xexpose.x+event.xexpose.width,
                           event.xexpose.y+event.xexpose.height);
        break;

      default:
        skelton_msgbox("Unknown XEvent (%i)\n", event.type);
        break;
      }
  }
}

void Input_X11::process_key(XEvent event) {
  KeyCode key = xlate[event.xkey.keycode];
  char buf[20];
  KeySym keysym;
  Status status;
  int num;

  switch(event.type) {
  case KeyPress:
    keys[key] |= PRESSED;

    if(event.xkey.state == Mod1Mask && XLookupKeysym(&event.xkey, 0) == XK_Return) {
      video->toggle_fullscreen();
      return;
    }
    
    if(!key)
      skelton_msgbox("Unknown KeyCode: %i\n", event.xkey.keycode);

    if(israw) {
      switch(key) {
      case KEY_RSHIFT:
      case KEY_LSHIFT:
        shift_key |= SHIFT;
        break;
      case KEY_RALT:
      case KEY_LALT:
        shift_key |= ALT;
        break;
      case KEY_RCTRL:
      case KEY_LCTRL:
        shift_key |= CONTROL;
        break;
      case 101:
      case 119:
        pause = true;
        break;
      default:
        quel_key = key;
      }
    } else {
      if(ic) {
        num = XmbLookupString(ic, &event.xkey, buf, 20, &keysym, &status);
      } else {
        num = XLookupString(&event.xkey, buf, 20, &keysym, NULL);
      }
      if(num) {
        switch(buf[0]) {
        case 27:
          quel_key = KEY_ESCAPE;
          break;
        case 10:
        case 13:
          quel_key = KEY_ENTER;
          break;
        default:
          if(key_pending < MAXKEY) {
            key_buf[key_pending].c = buf[0];
            key_buf[key_pending].special = false;
            key_pending++;
          }
        }
      }
    }
    break;

  case KeyRelease:
    keys[key] = RELEASED;
    switch(key) {
    case KEY_RSHIFT:
    case KEY_LSHIFT:
      shift_key &= ~SHIFT;
      break;
    case KEY_RALT:
    case KEY_LALT:
      shift_key &= ~ALT;
      break;
    case KEY_RCTRL:
    case KEY_LCTRL:
      shift_key &= ~CONTROL;
      break;
    }
    break;

  default:
    skelton_msgbox("process_key got non-key event\n");
    break;
  }
}

void Input_X11::process_mouse(XEvent event) {
  switch(event.type) {
  case ButtonPress:
    switch(event.xbutton.button) {
    case 1:
      if(mouse.button[0] == RELEASED)
	mouse.quel = 0;
      mouse.button[0] = PRESSED;
      break;
    case 2:
      if(mouse.button[1] == RELEASED)
	mouse.quel = 1;
      mouse.button[1] = PRESSED;
      break;
    case 3:
      if(mouse.button[2] == RELEASED)
	mouse.quel = 2;
      mouse.button[2] = PRESSED;
      break;
    }
    break;
    
  case ButtonRelease:
    switch(event.xbutton.button) {
    case 1:
      mouse.button[0] = RELEASED;
      break;
    case 2:
      mouse.button[1] = RELEASED;
      break;
    case 3:
      mouse.button[2] = RELEASED;
      break;
    }
    break;
    
  case LeaveNotify:
    cursor->set_pos(-1, -1);
    break;
    
  case MotionNotify:
    if(cursor)
      cursor->set_pos(event.xmotion.x, event.xmotion.y);
    break;

  default:
    skelton_msgbox("process_mouse got non-mouse event\n");
    break;
  }
}

void Input_X11::deraw() {
  israw = false;
}

void Input_X11::reraw() {
  israw = true;
}

#endif /* X_DISPLAY_MISSING */
