/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_NGLOG
#define _HEADER_NGLOG

#include "res.h"

class Log {
	Res_dos *file;
	char filename[1024];
public:
	bool exist;
	Log(const char *fname);
	virtual ~Log();
	void log_event(const char *st);
};

bool log_init(const char *filename);
void log_step(const char *st, ...);
void log_finalize(char *salt);

char *log_team(int t);
char *log_handicap(int h);

#endif
