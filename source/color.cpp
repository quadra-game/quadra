/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
