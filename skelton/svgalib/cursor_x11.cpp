/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "video.h"
#include "cursor_x11.h"

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
