/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_INPUT_KEYS
#define _HEADER_INPUT_KEYS

#define PRESSED 1
#define RELEASED 2

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

#if 0
#include <vgakeyboard.h>

#define KEY_PADENTER    SCANCODE_KEYPADENTER
#define KEY_LEFTARROW   SCANCODE_CURSORBLOCKLEFT
#define KEY_RIGHTARROW  SCANCODE_CURSORBLOCKRIGHT
#define KEY_UPARROW     SCANCODE_CURSORBLOCKUP
#define KEY_DOWNARROW   SCANCODE_CURSORBLOCKDOWN
#define KEY_LCTRL       SCANCODE_LEFTCONTROL
#define KEY_RCTRL       SCANCODE_RIGHTCONTROL
#define KEY_LSHIFT      SCANCODE_LEFTSHIFT
#define KEY_RSHIFT      SCANCODE_RIGHTSHIFT
#define KEY_LALT        SCANCODE_LEFTALT
#define KEY_RALT        SCANCODE_RIGHTALT
#define KEY_ESCAPE      SCANCODE_ESCAPE
#define KEY_ENTER       SCANCODE_ENTER
#define KEY_SPACE       SCANCODE_SPACE
#define KEY_TAB         SCANCODE_TAB
#define KEY_F1          SCANCODE_F1
#define KEY_F2          SCANCODE_F2
#define KEY_F3          SCANCODE_F3
#define KEY_F4          SCANCODE_F4
#define KEY_F5          SCANCODE_F5
#define KEY_F6          SCANCODE_F6
#define KEY_F7          SCANCODE_F7
#define KEY_F8          SCANCODE_F8
#define KEY_F9          SCANCODE_F9
#define KEY_F10         SCANCODE_F10
#define KEY_F11         SCANCODE_F11
#define KEY_F12         SCANCODE_F12

#else

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

#endif

#endif /* _HEADER_INPUT_KEYS */
