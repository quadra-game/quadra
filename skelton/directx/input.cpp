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

#include "error.h"
#include "main.h"
#include "input_dx.h"
#include "input_dumb.h"

RCSID("$Id$")

Input *input = NULL;

Input* Input::New(bool dumb) {
	if(dumb)
		return new Input_Dumb();
	else
  	return new Input_DX();
}

const char *keynames[256]={
	"", "Escape", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
	"Backspace", "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
	"[", "]", "Return", "Ctrl", "A", "S", "D", "F", "G", "H", "J", "K",
	"L", ";", "'", "`", "Left shift", "\\", "Z", "X", "C", "V", "B", "N", "M",
	",", ".", "/", "Right shift", "Pad *", "Alt", "Space", "Caps lock",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9",
	"F10", "Num lock", "Scrl lock", "Pad 7", "Pad 8", "Pad 9",
	"Pad -", "Pad 4", "Pad 5", "Pad 6", "Pad +", "Pad 1", "Pad 2",
	"Pad 3", "Pad 0", "Pad .", "Print scrn", "", "<",
	"F11", "F12", "", "", "", "", "", "", "",
	"", "", "", "", "F13", "F14", "F15", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
	"", "Pause 1", "", "", "", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "", "", "",
	"", "", "Pad Enter", "2nd Ctrl", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "", "", "", "",
	"", "", "Pad /", "", "Print scrn", "2nd Alt", "", "",
	"", "", "", "", "", "", "", "", "",
	"", "", "Pause 2", "Home", "Up", "Page up",
	"", "Left", "", "Right", "", "End", "Down",
	"Page down", "Insert", "Del", "", "", "",
	"", "", "", "", "Win left", "Win right", "Win popup", "Power", "Sleep",
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Macro",
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

Input_DX::Input_DX() {
	lpinput = NULL;
	lpinputdevice = NULL;
	lpinputmouse = NULL;
	calldx(DirectInputCreate(hinst, DIRECTINPUT_VERSION, &lpinput, NULL));
	calldx(lpinput->CreateDevice(GUID_SysKeyboard, &lpinputdevice, NULL));
	calldx(lpinputdevice->SetDataFormat(&c_dfDIKeyboard));
	calldx(lpinputdevice->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
	DIPROPDWORD dipdw = {
		{
			sizeof(DIPROPDWORD),
			sizeof(DIPROPHEADER),
			0,
			DIPH_DEVICE,
		},
		16,
	};
	calldx(lpinputdevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph));
	calldx(lpinputdevice->Acquire());

	calldx(lpinput->CreateDevice(GUID_SysMouse, &lpinputmouse, NULL));
	calldx(lpinputmouse->SetDataFormat(&c_dfDIMouse));
	calldx(lpinputmouse->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));

	mouse_handle = NULL;
	mouse.dx = mouse.dy = mouse.dz = 0;
	mouse.quel = -1;
	int i;
	for(i=0; i<4; i++)
		mouse.button[i] = 0;
/*  mouse_handle = CreateEvent(0, 0, 0, 0);
	if (mouse_handle == NULL)
		new Error("Could not create 'event' for the mouse");
	calldx(lpinputmouse->SetEventNotification(mouse_handle));
*/
	DIPROPDWORD dipdw2 = {
		{
			sizeof(DIPROPDWORD),
			sizeof(DIPROPHEADER),
			0,
			DIPH_DEVICE,
		},
		16,
	};

	calldx(lpinputmouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw2.diph));
	calldx(lpinputmouse->Acquire());
	quel_key = -1;
	pause = false;
	for(i=0; i<256; i++)
		keys[i] = 0;
	clear_key();
}

Input_DX::~Input_DX() {
	if(lpinputdevice) {
		lpinputdevice->Unacquire();
		lpinputdevice->Release();
	}
	if(lpinputmouse) {
		lpinputmouse->Unacquire();
		lpinputmouse->Release();
	}
	if(lpinput)
		lpinput->Release();
	if(mouse_handle)
		CloseHandle(mouse_handle);
}

void Input_DX::clear_key() {
	process_key(); // empties the key queue
	shift_key = 0;
	quel_key = -1;
	key_pending = 0;
	for(int i=0; i<256; i++)
		keys[i] = 0;
}

void Input_DX::process_key() {
	DIDEVICEOBJECTDATA od;
	DWORD dwElements = 1;
	Byte butt;
	HRESULT hr;
	for(;;) {
		hr = lpinputdevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
		if(hr == DI_BUFFEROVERFLOW) {
//      new Error("Catched a buffer overflow reading keyboard");
			calldx(lpinputdevice->GetDeviceState(sizeof(keys), keys));
			clear_key();
			break;
		}
		if(hr == DIERR_INPUTLOST) {
			restore();
			break;
		}
		calldx(hr);
		if (dwElements == 0)
			break;
		butt = (Byte) (od.dwData & 0x80);
		if(butt)
			keys[od.dwOfs] |= PRESSED;  // bit #1 -> 0:not pressed 1:pressed
		else
			keys[od.dwOfs] = RELEASED;  // bit #2 -> 0:rien        1:was released
		if(butt) { // if a key was pressed
			switch(od.dwOfs) {
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
				#ifndef NDEBUG
					case DIK_F10: // toggle copper (doesn't work on many video cards)
						COPPER(0,0,0);
						copper = !copper;
						break;
					case DIK_F11: // toggle msgbox() of skelton
						skelton_debug = !skelton_debug;
						break;
					case DIK_F1: // "eats" the F1 key for the screenshots
						break;
				#endif
				default:
					quel_key = od.dwOfs;
			}
		} else { // if a key was released
			switch(od.dwOfs) {
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
		}
	}
	if(keys[KEY_F4] & PRESSED && (keys[KEY_LALT] & PRESSED || keys[KEY_RALT] & PRESSED))
		quit_game();
	if(keys[KEY_LCTRL] && keys[DIK_NUMLOCK]) { // ignore control+num_lock since "pause" conflicts
		keys[KEY_LCTRL] = 0;
		keys[DIK_NUMLOCK] = 0;
	}
}

void Input_DX::process_mouse() {
	DIDEVICEOBJECTDATA od;
	DWORD dwElements = 1;
	mouse.dx = mouse.dy = mouse.dz = 0;
	Byte butt;
	for(;;) {
		HRESULT hr = lpinputmouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
		if(hr == DIERR_INPUTLOST) {
			restore();
			break;
		}
		calldx(hr);
		if(dwElements == 0)
			break;
		butt = (Byte) (od.dwData & 0x80);
		switch (od.dwOfs) {
			case DIMOFS_X:
				mouse.dx += (int) od.dwData;
				break;
			case DIMOFS_Y:
				mouse.dy += (int) od.dwData;
				break;
			case DIMOFS_Z:
				mouse.dz += (int) od.dwData;
				break;
			case DIMOFS_BUTTON0:
				mouse.button[0] = (Byte) (butt ? PRESSED:RELEASED);
				if(butt)
					mouse.quel = 0;
				break;
			case DIMOFS_BUTTON1:
				mouse.button[1] = (Byte) (butt ? PRESSED:RELEASED);
				if(butt)
					mouse.quel = 1;
				break;
			case DIMOFS_BUTTON2:
				mouse.button[2] = (Byte) (butt ? PRESSED:RELEASED);
				if(butt)
					mouse.quel = 2;
				break;
			case DIMOFS_BUTTON3:
				mouse.button[3] = (Byte) (butt ? PRESSED:RELEASED);
				if(butt)
					mouse.quel = 3;
				break;
		}
	}
}

void Input_DX::add_key_buf(char c, bool special) {
  if(key_pending < MAXKEY) {
    key_buf[key_pending].c = c;
    key_buf[key_pending].special = special;
    key_pending++;
  }
}

void Input_DX::restore() {
  calldx(lpinputdevice->Acquire());
  calldx(lpinputmouse->Acquire());
  clear_key();
}
