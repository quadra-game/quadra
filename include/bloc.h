/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_BLOC
#define _HEADER_BLOC

#include "bitmap.h"

/* morceaux de Tetris de 0 a 6 :
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
