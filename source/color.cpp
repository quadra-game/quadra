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

#include "color.h"

Color::Color(Byte q, Palette& p):	pal(p) {
	if(q==8)
		q=7;
	else
		if(q==7)
			q=8;
	base = q*8 + 184;
}

void Color::set(int r, int g, int b, int r2, int g2, int b2) {
	int i;
	for(i=0; i<5; i++) {
		pal.setcolor(i+base, (r*i)/4,(g*i)/4,(b*i)/4);
	}
	for(i=5; i<8; i++) {
		pal.setcolor(i+base, (r*(7-i)+r2*(i-4))/3, (g*(7-i)+g2*(i-4))/3, (b*(7-i)+b2*(i-4))/3);
	}
}
