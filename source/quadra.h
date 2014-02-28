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

#ifndef _HEADER_QUADRA
#define _HEADER_QUADRA

#include <stdint.h>
#include <stdlib.h>

#include "overmind.h"
#include "inter.h"
#include "sprite.h"
#include "color.h"
#include "zone_list.h"

extern Inter* ecran;
extern Overmind overmind;

extern Color* color[];
extern Font *fteam[];

extern Sprite *cur;

void raw_draw_bloc(const Video_bitmap* bit, int x, int y, uint8_t side, Color* col);
void raw_draw_bloc_corner(const Video_bitmap* bit, int x, int y, uint8_t side, Color* col, uint8_t to[4]);
void raw_small_draw_bloc(const Video_bitmap* bit, int x, int y, uint8_t side, Color* col);
void set_fteam_color(const Palette& pal);
int start_game();

#endif
