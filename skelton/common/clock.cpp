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

#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include "clock.h"

RCSID("$Id$")

Clock::Clock() {
}

int Clock::get_time() {
	time_t t;
	time(&t);
	return (int) t;
}

char *Clock::time2char(int time) {
	static char st[20];
	const time_t ti=time;
	tm *t = localtime(&ti);
	if(t) {
		sprintf(st, "%04i-%02i-%02i %2i:%02i:%02i", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	}
	else {
		st[0]=0;
	}
	return st;
}

char *Clock::absolute_time() {
	/* FIXME: shouldn't we rather use asctime(3)? */
	static char st[64];
#ifdef UGS_LINUX
	struct timeb time_info;
	ftime(&time_info);
	int tz=time_info.timezone;
#endif
#ifdef UGS_DIRECTX
	struct _timeb time_info;
	_ftime(&time_info);
	int tz=_timezone;
#endif
	const time_t ti=get_time();
	tm *t = localtime(&ti);
	if(t) {
		sprintf(st, "%04i.%02i.%02i.%2i.%02i.%02i.%03i.%+2.1f",
			t->tm_year+1900,
			t->tm_mon+1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec,
			time_info.millitm,
			-tz/3600.0
		);
	}
	else {
		st[0]=0;
	}
	return st;
}
