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

#ifndef _HEADER_BLOC
#define _HEADER_BLOC

#include "types.h"

class Video_bitmap;

/* pieces of Tetris from 0 to 6 :
   0 = Cube
	 ...
*/

class Bloc {
public:
	int quel, x, y, rot, bx,by;
	int col;
	Bloc(int q, int c=-1, int px=0, int py=0);
	void draw(const Video_bitmap* b, int tx=-1, int ty=-1) const;
	void small_draw(const Video_bitmap* b, int tx=-1, int ty=-1) const;
	const static Byte bloc[7][4][4][4];
	void calc_xy() {
		x=(bx-4)*18<<4;
		y=(by-12)*18<<4;
	}
};

#endif
