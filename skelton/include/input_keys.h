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

#ifndef _HEADER_INPUT_KEYS
#define _HEADER_INPUT_KEYS

#define PRESSED 1
#define RELEASED 2

#include "config.h"

#if defined(UGS_DIRECTX)

#undef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>

#define KEY_PADENTER    DIK_NUMPADENTER
#define KEY_LEFTARROW   DIK_LEFTARROW
#define KEY_RIGHTARROW  DIK_RIGHTARROW
#define KEY_UPARROW     DIK_UPARROW
#define KEY_DOWNARROW   DIK_DOWNARROW
#define KEY_LCTRL       DIK_LCONTROL
#define KEY_RCTRL       DIK_RCONTROL
#define KEY_LSHIFT      DIK_LSHIFT
#define KEY_RSHIFT      DIK_RSHIFT
#define KEY_LALT        DIK_LALT
#define KEY_RALT        DIK_RALT
#define KEY_ESCAPE      DIK_ESCAPE
#define KEY_ENTER       DIK_RETURN
#define KEY_SPACE       DIK_SPACE
#define KEY_TAB         DIK_TAB
#define KEY_F1          DIK_F1
#define KEY_F2          DIK_F2
#define KEY_F3          DIK_F3
#define KEY_F4          DIK_F4
#define KEY_F5          DIK_F5
#define KEY_F6          DIK_F6
#define KEY_F7          DIK_F7
#define KEY_F8          DIK_F8
#define KEY_F9          DIK_F9
#define KEY_F10         DIK_F10
#define KEY_F11         DIK_F11
#define KEY_F12         DIK_F12

#elif defined(UGS_LINUX)

/* I have no idea where these values come from. */
#define KEY_PADENTER    96
#define KEY_LEFTARROW   105
#define KEY_RIGHTARROW  106
#define KEY_UPARROW     103
#define KEY_DOWNARROW   108
#define KEY_LCTRL       29
#define KEY_RCTRL       97
#define KEY_LSHIFT      42
#define KEY_RSHIFT      54
#define KEY_LALT        56
#define KEY_RALT        100
#define KEY_ESCAPE      1
#define KEY_ENTER       28
#define KEY_SPACE       57
#define KEY_TAB         15
#define KEY_F1          59
#define KEY_F2          60
#define KEY_F3          61
#define KEY_F4          62
#define KEY_F5          63
#define KEY_F6          64
#define KEY_F7          65
#define KEY_F8          66
#define KEY_F9          67
#define KEY_F10         68
#define KEY_F11         87
#define KEY_F12         88

#endif

#endif /* _HEADER_INPUT_KEYS */
