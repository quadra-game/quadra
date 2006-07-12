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

#ifndef _HEADER_ATTACK
#define _HEADER_ATTACK

enum Attack_type {
	ATTACK_LINES,
	ATTACK_NONE,
	ATTACK_BLIND,
	ATTACK_FULLBLIND,
	ATTACK_LAST
};

class Attack {
public:
	Attack_type type;
	int param;
	Attack() {
		type=ATTACK_LINES;
		param=0;
	}
	char *log_type() {
		switch(type) {
			case ATTACK_LINES: return "lines";
			case ATTACK_NONE: return "none";
			case ATTACK_BLIND: return "blind";
			case ATTACK_FULLBLIND: return "fullblind";
			default: return "unknown";
		}
		return "unknown";
	}
};

enum End_type {
	END_NEVER,
	END_FRAG,
	END_TIME,
	END_POINTS,
	END_LINES,
	END_LAST
};

#endif
