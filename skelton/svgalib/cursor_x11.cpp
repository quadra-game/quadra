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

#ifdef UGS_LINUX_X11

#include "video.h"
#include "cursor_x11.h"

RCSID("$Id$")

Cursor_X11::Cursor_X11() {
  visible = true;
}

void Cursor_X11::change_cursor(Sprite* s) {
}

void Cursor_X11::restore_back(bool r) {
}

void Cursor_X11::set_pos(int px, int py) {
  x = px;
  y = py;
}

void Cursor_X11::set_limit(int x1, int y1, int x2, int y2) {
}

void Cursor_X11::set_speed(const Byte s) {
}

void Cursor_X11::move() {
}

void Cursor_X11::get_back() {
}

void Cursor_X11::put_back() {
}

void Cursor_X11::draw() const {
}

#endif /* UGS_LINUX_X11 */

