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
#include "video.h"
#include "cursor_self.h"

RCSID("$Id$")

Cursor_Self::Cursor_Self(Sprite* s) {
  change_cursor(s);
  set_pos(video->width/2, video->height/2);
  set_limit(0, 0, video->width, video->height);
  back[0].dirty = false;
  back[0].buf = NULL;
  back[1].dirty = false;
  back[1].buf = NULL;
  page = 0;
  set_speed(64);
  restore_back(true);
  mouse_rate=0;
  smooth_mode=true; // enable le smooth par defaut
  visible = true;
}

Cursor_Self::~Cursor_Self() {
  if(back[0].buf)
    delete back[0].buf;
  if(back[1].buf)
    delete back[1].buf;
}

void Cursor_Self::change_cursor(Sprite* s) {
  spr = s;
}

void Cursor_Self::restore_back(bool r) {
  _restore_back = r;
}

void Cursor_Self::set_pos(int px, int py) {
  x = sx = px;
  y = sy = py;
}

void Cursor_Self::set_limit(int x1, int y1, int x2, int y2) {
  lx1 = x1;
  ly1 = y1;
  lx2 = x2-1;
  ly2 = y2-1;
}

void Cursor_Self::set_speed(const Byte s) {
  speed = max(1, s);
  pool_x = pool_y = 0;
}

void Cursor_Self::move() {
  int nx, ny; // nouvelle pos x,y
  pool_x += input->mouse.dx * speed;
  pool_y += input->mouse.dy * speed;
  int temp_x, temp_y; // valeur ajuste de pool_x et pool_y
  temp_x = (pool_x) >> 6;
  temp_y = (pool_y) >> 6;
  pool_x -= (temp_x << 6);
  pool_y -= (temp_y << 6);
  nx = sx+temp_x;
  ny = sy+temp_y;
  if(nx > lx2)
    nx = lx2;
  if(nx < lx1)
    nx = lx1;
  if(ny > ly2)
    ny = ly2;
  if(ny < ly1)
    ny = ly1;
  if(smooth_mode) { // mode smooth: interpolation des mouvements (pour serial mouse a 35 update/seconde)
    if(sx != nx || sy != ny) {
      x = (sx+nx) >> 1;
      sx = nx;
      y = (sy+ny) >> 1;
      sy = ny;
      mouse_rate++;
      if(mouse_rate > 45) // si la souris s'update plus que 40 frame/seconde,
	smooth_mode = false; // desactive le smoothing
    } else {
      x = nx;
      y = ny;
      mouse_rate=0;
    }
  } else { // mode normal: pour bus mouse rapide
    x = sx = nx;
    y = sy = ny;
  }
}

void Cursor_Self::get_back() {
  if(_restore_back && spr && visible) {
    back[page].x = x-spr->hot_x;
    back[page].y = y-spr->hot_y;
    back[page].dirty = true;
    if(back[page].buf && back[page].buf->width != spr->width) {
      delete back[page].buf;
      back[page].buf = NULL;
    }
    if(back[page].buf == NULL)
      back[page].buf = new Bitmap(spr->width, spr->height, spr->width);
    video->vb->get_bitmap(back[page].buf, back[page].x, back[page].y, spr->width, spr->height);
  }
  page = 1-page;
}

void Cursor_Self::put_back() {
  if(back[page].dirty) {
    back[page].buf->draw(video->vb, back[page].x, back[page].y);
    back[page].dirty = false;
  }
}

void Cursor_Self::draw() const {
  if(spr && visible)
    spr->draw(video->vb, x, y);
}
