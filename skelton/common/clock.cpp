/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include "clock.h"

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
