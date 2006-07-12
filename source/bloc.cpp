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

// "bloc" is simply "block" in french.
#include "bloc.h"

#include "color.h"
#include "quadra.h"

Bloc::Bloc(int q, int c, int px, int py) {
	quel=q;
	if(c != -1)
		col=c;
	else
		col=quel;
	bx=px;
	by=py;
	calc_xy();
	y -= 17<<4;  // to force the block to start at the beginning of its initiall position
	rot=0;
}

void Bloc::draw(const Video_bitmap* b, int tx, int ty) const {
	int i,j;
	Byte t, to[4];
	if(tx == -1)
		tx = x>>4;
	if(ty == -1)
		ty = y>>4;
	for(j=0; j<4; j++)
		for(i=0; i<4; i++) {
			t = bloc[quel][rot][j][i];
			if(t) {
				if(i > 0)
					to[0] = bloc[quel][rot][j][i-1];
				else
					to[0] = 0;
				if(j > 0)
					to[1] = bloc[quel][rot][j-1][i];
				else
					to[1] = 0;
				if(i < 3)
					to[2] = bloc[quel][rot][j][i+1];
				else
					to[2] = 0;
				if(j < 3)
					to[3] = bloc[quel][rot][j+1][i];
				else
					to[3] = 0;
				raw_draw_bloc_corner(b, tx+i*18, ty+j*18, t&15, color[col], to);
			}
		}
}

void Bloc::small_draw(const Video_bitmap* b, int tx, int ty) const {
	int i,j;
	Byte t;
	if(tx == -1)
		tx = x>>4;
	if(ty == -1)
		ty = y>>4;
	for(j=0; j<4; j++)
		for(i=0; i<4; i++) {
			t = bloc[quel][rot][j][i];
			if(t)
				raw_small_draw_bloc(b, tx+i*6, ty+j*6, t&15, color[col]);
		}
}

const Byte Bloc::bloc[7][4][4][4]={{{{0,0,0,0},{0,3,6,0},{0,9,12,0},{0,0,0,0}},
																		{{0,0,0,0},{0,3,6,0},{0,9,12,0},{0,0,0,0}},
																		{{0,0,0,0},{0,3,6,0},{0,9,12,0},{0,0,0,0}},
																		{{0,0,0,0},{0,3,6,0},{0,9,12,0},{0,0,0,0}}},
																	 {{{0,0,0,0},{0,3,14,0},{11,12,0,0},{0,0,0,0}},
																		{{7,0,0,0},{9,6,0,0},{0,13,0,0},{0,0,0,0}},
																		{{0,0,0,0},{0,3,14,0},{11,12,0,0},{0,0,0,0}},
																		{{7,0,0,0},{9,6,0,0},{0,13,0,0},{0,0,0,0}}},
																	 {{{0,0,0,0},{11,6,0,0},{0,9,14,0},{0,0,0,0}},
																		{{0,7,0,0},{3,12,0,0},{13,0,0,0},{0,0,0,0}},
																		{{0,0,0,0},{11,6,0,0},{0,9,14,0},{0,0,0,0}},
																		{{0,7,0,0},{3,12,0,0},{13,0,0,0},{0,0,0,0}}},
																	 {{{0,0,0,0},{3,10,14,0},{13,0,0,0},{0,0,0,0}},
																		{{11,6,0,0},{0,5,0,0},{0,13,0,0},{0,0,0,0}},
																		{{0,0,7,0},{11,10,12,0},{0,0,0,0},{0,0,0,0}},
																		{{0,7,0,0},{0,5,0,0},{0,9,14,0},{0,0,0,0}}},
																	 {{{0,0,0,0},{11,10,6,0},{0,0,13,0},{0,0,0,0}},
																		{{0,7,0,0},{0,5,0,0},{11,12,0,0},{0,0,0,0}},
																		{{7,0,0,0},{9,10,14,0},{0,0,0,0},{0,0,0,0}},
																		{{0,3,14,0},{0,5,0,0},{0,13,0,0},{0,0,0,0}}},
																	 {{{0,0,0,0},{11,10,10,14},{0,0,0,0},{0,0,0,0}},
																		{{0,7,0,0},{0,5,0,0},{0,5,0,0},{0,13,0,0}},
																		{{0,0,0,0},{11,10,10,14},{0,0,0,0},{0,0,0,0}},
																		{{0,7,0,0},{0,5,0,0},{0,5,0,0},{0,13,0,0}}},
																	 {{{0,0,0,0},{11,2,14,0},{0,13,0,0},{0,0,0,0}},
																		{{0,7,0,0},{11,4,0,0},{0,13,0,0},{0,0,0,0}},
																		{{0,7,0,0},{11,8,14,0},{0,0,0,0},{0,0,0,0}},
																		{{0,7,0,0},{0,1,14,0},{0,13,0,0},{0,0,0,0}}}};
